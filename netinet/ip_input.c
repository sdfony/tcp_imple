#include "../sys/socket.h"
#include "../sys/errno.h"
#include "../sys/time.h"
#include "../sys/kernel.h"
#include "../sys/protosw.h"
#include "../sys/domain.h"
#include "../sys/mbuf.h"

#include "../net/if.h"
#include "../net/route.h"

#include "in.h"
#include "in_systm.h"
#include "ip.h"
#include "in_var.h"
#include "ip_var.h"
#include "ip_icmp.h"
#include "in_pcb.h"

#ifndef	IPFORWARDING
#ifdef GATEWAY
#define	IPFORWARDING	1	/* forward IP packets not for us */
#else /* GATEWAY */
#define	IPFORWARDING	0	/* don't forward IP packets not for us */
#endif /* GATEWAY */
#endif /* IPFORWARDING */
#ifndef	IPSENDREDIRECTS
#define	IPSENDREDIRECTS	1
#endif

int	ipforwarding = IPFORWARDING;
int	ipsendredirects = IPSENDREDIRECTS;
int	ip_defttl = IPDEFTTL;
#ifdef DIAGNOSTIC
int	ipprintfs = 0;
#endif

extern  struct ipq	ipq;			/* ip reass. queue */
extern	struct domain inetdomain;
extern	struct protosw inetsw[];

u_char	ip_protox[IPPROTO_MAX];
int	ipqmaxlen = IFQ_MAXLEN;
struct	in_ifaddr *in_ifaddr;			/* first inet address */
struct	ifqueue ipintrq;
/*
 * We need to save the IP options in case a protocol wants to respond
 * to an incoming packet over the same route if the packet got here
 * using IP source routing.  This allows connection establishment and
 * maintenance when the remote end is on a network that is not known
 * to us.
 */
int	ip_nhops = 0;

static	struct ip_srcrt {
	struct	in_addr dst;			/* final destination */
	char	nop;				/* one NOP to align */
	char	srcopt[IPOPT_OFFSET + 1];	/* OPTVAL, OLEN and OFFSET */
	struct	in_addr route[MAX_IPOPTLEN/sizeof(struct in_addr)];
} ip_srcrt;

#ifndef GATEWAY
extern	int if_index;
u_long	*ip_ifmatrix;
#endif

#define RSA_SIN(ro_dst) ((struct sockaddr_in *)&ro_dst)

static void save_rte (u_char *, struct in_addr);
/*
 * IP initialization: fill in IP protocol switch table.
 * All protocols not implemented in kernel go to raw IP protocol handler.
 */
void
ip_init()
{
    struct protosw *pr = pffindproto(PF_INET, IPPROTO_RAW, SOCK_RAW);
    if (pr == NULL)
        perror("canot find the protosw\n");

    for (int i = 0; i < IPPROTO_MAX; i++)
        ip_protox[i] = pr - inetdomain.dom_protosw;

    for (struct protosw *p = inetdomain.dom_protosw;
        p < inetdomain.dom_protoswNPROTOSW; p++)
    {
        if (p->pr_protocol != IPPROTO_RAW)
            ip_protox[p->pr_protocol] = p - inetdomain.dom_protosw;
    }

    ipq.ipq_prev = ipq.ipq_next = &ipq;
    ip_id = time.tv_sec & 0xffff;
    int i = (if_index + 1) * (if_index + 1) * sizeof(u_long);
    ip_ifmatrix = malloc(i);
    memset(ip_ifmatrix, 0, i);
  
    ipintrq.ifq_maxlen = ipqmaxlen;
}

struct	sockaddr_in ipaddr = { sizeof(ipaddr), AF_INET };
struct	route ipforward_rt;

/*
 * Ip input routine.  Checksum and byte swap header.  If fragmented
 * try to reassemble.  Process options.  Pass to next level.
 */
void
ipintr()
{
    struct ip *ip;
    struct mbuf *m;
    int cksum;
    int hlen;

next:
    IF_DEQUEUE(&ipintrq, m);
    if (!m)
        goto bad;
    if (!in_ifaddr)
        goto bad;
    ip = mtod(m, struct ip*);

    if (ip->ip_v != IPVERSION)
        goto bad;
    if (m->m_flags & M_PKTHDR == 0)
        goto bad;

    if (m->m_len < sizeof *ip)
        m = m_pullup(m, sizeof *ip);
    if (!m)
        goto bad;

    hlen = ip->ip_hl * 4;
    if (hlen < sizeof (struct ip))
        goto bad;
    if (m->m_len < hlen)
        m = m_pullup(m, hlen);
  
    ip->ip_sum = 0;
    ip->ip_sum = in_cksum(m, hlen);
    if (ip->ip_sum)
        goto bad;

    NTOHS(ip->ip_len);
    NTOHS(ip->ip_id);
    NTOHS(ip->ip_off);

    if (ip->ip_len > m->m_pkthdr.len)
    {
        goto bad;
    }
    else
    {
        m_adj(m, m->m_pkthdr.len - ip->ip_len);
    }

    ip_nhops = 0;
    if (hlen > sizeof (struct ip))
    {
        if (ip_dooptions(m) != 0)
            goto next;
    }

    // match with the one of the broadcast addresses associated with
    // the receiving interface 
    struct sockaddr_in *sin = NULL;
    for (struct in_ifaddr *ia = in_ifaddr; ia; ia = ia->ia_next)
    {
        u_long src_addr = ntohl(ip->ip_dst.s_addr);
        if (ia->ia_addr.sin_addr.s_addr == src_addr)
        {

        }
        if (ia->ia_broadaddr.sin_addr.s_addr == src_addr)
        {
        }
        if (ia->ia_netbroadcast.s_addr == src_addr)
        {

        }
        if (ia->ia_subnet = src_addr)
        {

        }
        if (ia->ia_net == src_addr)
        {

        }
        for (struct in_multi *im = ia->ia_multiaddrs;
            im; im = im->inm_next)
            if (im->inm_addr.s_addr == src_addr)
            {

            }
        if (INADDR_ANY == src_addr 
            || INADDR_BROADCAST == src_addr)
        {

        }

        if (ipforwarding)
            ip_forward(m, 0);
        else
            goto bad;
    }

    // reassembly and demultiplexing
    // in chapter 10

    struct protosw *pr = &inetsw[ip_protox[ip->ip_p]];
    (pr->pr_input)(m, hlen);
    goto next;
bad:
    m_freem(m);
    goto next;
}

/*
 * Take incoming datagram fragment and try to
 * reassemble it into whole datagram.  If a chain for
 * reassembly of this datagram already exists, then it
 * is given as fp; otherwise have to make a chain.
 */
struct ip *
ip_reass(ip, fp)
	register struct ipasfrag *ip;
	register struct ipq *fp;
{
    return NULL;
}

/*
 * Free a fragment reassembly header and all
 * associated datagrams.
 */
void
ip_freef(fp)
	struct ipq *fp;
{
}

/*
 * Put an ip fragment on a reassembly chain.
 * Like insque, but pointers in middle of structure.
 */
void
ip_enq(p, prev)
	register struct ipasfrag *p, *prev;
{
}

/*
 * To ip_enq as remque is to insque.
 */
void
ip_deq(p)
	register struct ipasfrag *p;
{
}

/*
 * IP timer processing;
 * if a timer expires on a reassembly
 * queue, discard it.
 */
void
ip_slowtimo()
{

}

/*
 * Drain off all datagram fragments.
 */
void
ip_drain()
{

}

/*
 * Do option processing on a datagram,
 * possibly discarding it if bad options are encountered,
 * or forwarding it if source-routed.
 * Returns 1 if packet has been forwarded/freed,
 * 0 if the packet should be processed further.
 */
int
ip_dooptions(m)
	struct mbuf *m;
{
    struct ip *ip = mtod(m, struct ip*);
    u_char *cp = (u_char *)(ip + 1);
    int type, code;
    int totallen = m->m_len;
    int optlen = cp[IPOPT_OLEN];
    int offset = 0;
    type = ICMP_PARAMPROB;
    code = ICMP_PARAMPROB;

    for (int i = optlen; i > 0; i -= optlen)
    {
        if (cp[IPOPT_OLEN] > MAX_IPOPTLEN)
        {
            code = cp - (u_char *)m;
            icmp_error(m, type, code, 0, NULL);
            return 1;
        }

        int opt_id = cp[IPOPT_OPTVAL];

        if (opt_id == IPOPT_EOL)
            return 0;
        else if (opt_id == IPOPT_NOP)
        {
            optlen = 1;
            continue;
        }
        else
        {
            struct in_ifaddr *ia = NULL;
            switch (opt_id)
            {
            case IPOPT_RR:
                if (cp[IPOPT_OFFSET] < IPOPT_MINOFF)
                {
                    code = cp - (u_char *)mtod(m, struct ip*);
                    goto bad;
                }
                if (cp[IPOPT_OFFSET] >= MAX_IPOPTLEN - sizeof (struct in_addr))
                {
                    return 0;
                }
                // if ip_dst is one of the systems addresses, the address of the receiving 
                // interface is recorded in the option.
                // otherwise the address of the outgoing interface as provided
                // by ip_rtaddr is recorded.
                for (ia = m->m_pkthdr.rcvif->if_addrlist;
                    ia != NULL; ia = ia->ia_next)
                {
                    if (ia->ia_addr.sin_addr.s_addr == ip->ip_dst.s_addr)
                    {
                        break;
                    }
                }
                if (ia == NULL)
                {
                    ia = ip_rtaddr(ip->ip_dst);
                }
                if (ia == NULL)
                {
                    code = ICMP_UNREACH_HOST;
                    goto bad;
                }

                memcpy(cp + cp[IPOPT_OFFSET]-1, ia->ia_addr.sin_addr, sizeof(struct in_addr));
                cp[IPOPT_OFFSET] += sizeof(struct in_addr);

                return 0;		/* record packet route */
            case IPOPT_TS:
                if (cp[IPOPT_OLEN] < 5)
                {
                    code = cp - (u_char *)(mtod(m, struct ip*) + 1);
                    goto bad;
                }
                struct ip_timestamp *ipts = (struct ip_timestamp *)(cp + IPOPT_OFFSET);
                if (code < 0)
                {
                    if (++ipts->ipt_oflw == 16)
                    {
                        ipts->ipt_oflw = 0;
                        code = ICMP_PARAMPROB;
                        goto bad;
                    }
                }

                switch (ipts->ipt_flg)
                {
                case IPOPT_TS_TSONLY:
                    break;
                case IPOPT_TS_TSANDADDR:
                    ia = ifaof_ifpforaddr(NULL, NULL);
                    if (ia == NULL)
                        break;

                   //  if room remains in the data area
                    ipts->ipt_timestamp.ipt_ta.ipt_addr = ia->ia_addr;
                    ipts->ipt_len += sizeof(struct in_addr);
                    ipts->ipt_ptr += sizeof(struct in_addr);

                    break;
                case IPOPT_TS_PRESPEC:
                    ia = (struct in_ifaddr*)ifa_ifwithaddr(NULL);
                    if (ia == NULL)
                        continue;
                    else
                    {
                        ipts->ipt_len += sizeof(struct in_addr);
                        ipts->ipt_ptr += sizeof(struct in_addr);
                    }
                    break;
                default:
                    goto bad;
                    break;
                }

                n_time nt = iptime();
                ipts->ipt_len += sizeof(n_time);
                ipts->ipt_ptr += sizeof(n_time);

                return 0;		/* timestamp */

            case IPOPT_LSRR:
            case IPOPT_SSRR:
                if (cp[IPOPT_OFFSET] < IPOPT_MINOFF)
                {
                    code = cp - (u_char *)mtod(m, struct ip *);
                    goto bad;
                }

                // if the destination address of the packet 
                // does not match one of the local addresses 
                // and the option is a strict source route
                // an ICMP source route failure error is sent
                // ??? how to get the local addresses
                struct sockaddr *sa;
                struct in_ifaddr *ifa = NULL;
                for (ia = (struct in_ifaddr *)(m->m_pkthdr.rcvif->if_addrlist);
                    ia != NULL; ia = ia->ia_next)
                {
                    if (ia->ia_addr.sin_addr.s_addr == ip->ip_dst.s_addr)
                    {
                        break;
                    }
                }
                if (ia == NULL)
                {
                    if (opt_id == IPOPT_SSRR)
                    {
                        code = ICMP_UNREACH_HOST;
                        goto bad;
                    }
                    if (opt_id == IPOPT_LSRR)
                    {
                        ipforwarding = 1;
                        return 1;
                    }
                }
                int off = cp[IPOPT_OFFSET];
                off--;
                if (off > MAX_IPOPTLEN - sizeof(struct in_addr))
                {
                    save_rte();
                }
                ip_nhops++;

                if (opt_id == IPOPT_SSRR)
                {
                    ia = ifa_ifwithdstaddr();
                    if (ia == NULL)
                        ia = ifa_ifwithnet();
                }
                else
                {
                    ia = ip_rtaddr();
                }
                if (ia == NULL)
                {
                    code = cp - (u_char *)(mtod(m, struct ip *) + 1);
                    goto bad;
                }
                ip->ip_dst = *(struct in_addr*)(cp + off);
                *(struct in_addr*)(cp + off) = ia->ia_addr.sin_addr;
                cp[IPOPT_OFFSET] += sizeof(struct in_addr);

                if (in_broadcast(ip->ip_dst, NULL))
                {
                    return 1;
                }

            default:
                return 0;
            }
        }
    }
bad:
    code = cp - (u_char *)mtod(m, struct ip*);
    icmp_error(m, type, code, 0, NULL);
    return 1;
}

/*
 * Given address of next destination (final or next hop),
 * return internet address info of interface to be used to get there.
 */
struct in_ifaddr *
ip_rtaddr(dst)
	 struct in_addr dst;
{
    struct sockaddr_in sin;

    if (ipforward_rt.ro_rt == NULL
        || RSA_SIN(ipforward_rt.ro_dst)->sin_addr.s_addr != dst.s_addr)
    {
        if (ipforward_rt.ro_rt != NULL)
            rtfree(ipforward_rt.ro_rt);
    }
    sin.sin_addr = dst;
    sin.sin_family = AF_INET;
    sin.sin_len = sizeof sin;

    ipforward_rt.ro_dst = *(struct sockaddr *)&sin;
    rtalloc(&ipforward_rt);
  
    if (ipforward_rt.ro_rt == NULL)
        return NULL;

    return (struct in_ifaddr *)(ipforward_rt.ro_rt->rt_ifa);
}

/*
 * Save incoming source route for use in replies,
 * to be picked up later by ip_srcroute if the receiver is interested.
 */
void
save_rte(option, dst)
	u_char *option;
	struct in_addr dst;
{
    if (option[IPOPT_OLEN] > sizeof ip_srcrt)
        return;

    ip_srcrt.dst = dst;
    ip_srcrt.nop = ip_nhops;  // do nothing, i think this expression can be skip.
    memcpy(ip_srcrt.srcopt, option, option[IPOPT_OLEN]);

    ip_nhops = (option[IPOPT_OLEN] - IPOPT_MINOFF) / sizeof(struct in_addr);
}

/*
 * Retrieve incoming source route for use in replies,
 * in the same form used by setsockopt.
 * The first hop is placed before the options, will be removed later.
 */
struct mbuf *
ip_srcroute()
{
    struct mbuf *m = NULL;

    if (ip_nhops == 0)
        return NULL;
    m = m_gethdr(0, MT_SOOPTS);
    if (m == NULL)
        return NULL;

#define OPTSIZ IPOPT_MINOFF
    m->m_len = sizeof(struct in_addr) + OPTSIZ
            + sizeof(struct in_addr) * ip_nhops;
    struct in_addr *p = ip_srcrt.route + ip_nhops;
    mtod(m, struct ip*)->ip_dst = *--p;
    u_char *option = (u_char *)(mtod(m, struct ip*) + 1);

    *option++ = IPOPT_NOP;
    memcpy(option, ip_srcrt.srcopt, sizeof ip_srcrt.srcopt);
    option += sizeof ip_srcrt.srcopt;

    while (p)
    {
        *(struct in_addr *)option = *--p;
        option += sizeof(struct in_addr);
    }
    *(struct in_addr *)option = ip_srcrt.dst;

    return m;
}

/*
 * Strip out IP options, at higher
 * level protocol in the kernel.
 * Second argument is buffer to which options
 * will be moved, and return value is their length.
 * XXX should be deleted; last arg currently ignored.
 */
void
ip_stripoptions(m, mopt)
	register struct mbuf *m;
	struct mbuf *mopt;
{
    ;
}

u_char inetctlerrmap[PRC_NCMDS] = {
	0,		0,		0,		0,
	0,		EMSGSIZE,	EHOSTDOWN,	EHOSTUNREACH,
	EHOSTUNREACH,	EHOSTUNREACH,	ECONNREFUSED,	ECONNREFUSED,
	EMSGSIZE,	EHOSTUNREACH,	0,		0,
	0,		0,		0,		0,
	ENOPROTOOPT
};

/*
 * Forward a packet.  If some error occurs return the sender
 * an icmp packet.  Note we can't always generate a meaningful
 * icmp message because icmp doesn't have a large enough repertoire
 * of codes and types.
 *
 * If not forwarding, just drop the packet.  This could be confusing
 * if ipforwarding was zero but some routing protocol was advancing
 * us as a gateway to somewhere.  However, we must let the routing
 * protocol deal with that.
 *
 * The srcrt parameter indicates whether the packet is being forwarded
 * via a source route.
 */
void
ip_forward(m, srcrt)
	struct mbuf *m;
	int srcrt;
{
    int type, code;
    struct ip *ip = mtod(m, struct ip*);
    struct ifnet *ifp = m->m_pkthdr.rcvif;
    if (srcrt)
        ;

    if (ifp->if_flags & IFF_BROADCAST)
        m->m_flags |= M_BCAST;

    if (in_canforward(ip->ip_src) == 0)
        return;

    HTONS(ip->ip_id);
    HTONS(ip->ip_len);

    if (ip->ip_ttl <= IPTTLDEC)
    {
        type = ICMP_TIMXCEED_INTRANS;
        code = ICMP_TIMXCEED;

        icmp_error(m, type, code, 0, ifp);
        m_free(m);
    }
    
    ip->ip_ttl -= IPTTLDEC;

    struct sockaddr_in sin;
    if (ipforward_rt.ro_rt == NULL
        || RSA_SIN(ipforward_rt.ro_dst)->sin_addr.s_addr != ip->ip_dst.s_addr)
    {
        if (ipforward_rt.ro_rt != NULL)
            rtfree(ipforward_rt.ro_rt);
    }
    sin.sin_addr = ip->ip_dst;
    sin.sin_family = AF_INET;
    sin.sin_len = sizeof sin;
    
    ipforward_rt.ro_dst = *(struct sockaddr *)&sin;
    rtalloc(&ipforward_rt);
    if (ipforward_rt.ro_rt == NULL)
    {
        code = ICMP_UNREACH;
        icmp_error(m, type, code, 0, 0);
        m_free(m);
    }

    struct mbuf *mc = m_copy(m, 0, min(m->m_len, 64));
 
    ip_ifmatrix[ifp->if_index]++;
    ip_ifmatrix[ipforward_rt.ro_rt->rt_ifp->if_index]++;

    struct in_addr dst;
    // only routers send redirect messages and that hosts must update their
    // routing tables when receiving ICMP redirect messages
    if (m->m_pkthdr.rcvif == ipforward_rt.ro_rt->rt_ifp
        && ipforward_rt.ro_rt->rt_flags & (RTF_DYNAMIC | RTF_MODIFIED) == 0
        && RSA_SIN(ipforward_rt.ro_dst)->sin_addr.s_addr != 0
        && ipsendredirects && !srcrt)
    {
        //if the subnet mask bits of the source address and the outgoing interface's
        //address are the same, the addressed are on the same IP network
        if (ip->ip_src.s_addr == RSA_SIN(ipforward_rt.ro_dst)->sin_addr.s_addr)
            ;
        if (ip->ip_src.s_addr != in_ifaddr)
        {
            if (ipforward_rt.ro_rt->rt_flags & RTF_GATEWAY)
                dst.s_addr = ((struct sockaddr_in *)ipforward_rt.ro_rt->rt_gateway)->sin_addr.s_addr;
            else
                dst.s_addr = RSA_SIN(ipforward_rt.ro_dst)->sin_addr.s_addr;
        }
    }

    IP_ALLOWBROADCAST;

    //int	 ip_output __P((struct mbuf *,
    //    struct mbuf *, struct route *, int, struct ip_moptions *));
    int error = ip_output(m, NULL, ipforward_rt.ro_rt, IP_FORWARDING, NULL);
    if (error)
    {
        m_free(m);

        switch (error)
        {
        case ENOBUFS:
            code = ICMP_SOURCEQUENCH;
            break;
        case EMSGSIZE:
            code = ICMP_UNREACH_NEEDFRAG;
            break;
        case ENETDOWN:
            code = ICMP_UNREACH_HOST;
            break;
        case EHOSTUNREACH:
            code = ICMP_UNREACH_HOST;
            break;
        default:
            code = ICMP_UNREACH_HOST;
            break;
        }

        type = ICMP_UNREACH;
        if (mc)
            icmp_error(mc, type, code, 0, ifp);
    }
    else
    {
        m_free(mc);
    }

    return 0;
}

int
ip_sysctl(name, namelen, oldp, oldlenp, newp, newlen)
	int *name;
	u_int namelen;
	void *oldp;
	size_t *oldlenp;
	void *newp;
	size_t newlen;
{
        ipforwarding;
    return 0;
}
