#include "../sys/param.h"
#include "../sys/malloc.h"
#include "../sys/mbuf.h"
#include "../sys/errno.h"
#include "../sys/protosw.h"
#include "../sys/socket.h"
#include "../sys/socketvar.h"

#include "../net/if.h"
#include "../net/route.h"

#include "in.h"
#include "in_systm.h"
#include "ip.h"
#include "in_pcb.h"
#include "in_var.h"
#include "ip_var.h"

#ifdef vax
#include <machine/mtpr.h>
#endif

static struct mbuf *ip_insertoptions(struct mbuf *, struct mbuf *, int *);
static void ip_mloopback
	(struct ifnet *, struct mbuf *, struct sockaddr_in *);

/*
 * IP output.  The packet in mbuf chain m contains a skeletal IP
 * header (with len, off, ttl, proto, tos, src, dst).
 * The mbuf chain containing the packet will be freed.
 * The mbuf opt, if present, will not be freed.
 */
int
ip_output(m0, opt, ro, flags, imo)
	struct mbuf *m0;
	struct mbuf *opt;
	struct route *ro;
	int flags;
	struct ip_moptions *imo;
{
    int hlen = 0;
    if (opt)
    {
        m0 = ip_insertoptions(m0, opt, &hlen);
    }

    struct ip *ip = mtod(m0, struct ip*);

    if (flags & (IP_FORWARDING | IP_RAWOUTPUT) == 0)
    {
        ip->ip_v = IPVERSION;
        ip->ip_off &= IP_DF;
        ip->ip_id = ip_id++;
        ip->ip_hl = hlen >> 2;
    }
    else 
        hlen = ip->ip_hl << 2;

    struct sockaddr_in dst;
    struct route iproute;
#define SATOSIN(sa) ((struct sockaddr_in *)(sa))
#define SINTOSA(sin) ((struct sockaddr_in *)(sin))
    if (ro == NULL
        || SATOSIN(&ro->ro_dst)->sin_addr.s_addr != ip->ip_dst.s_addr)
    {
        if (ro != NULL)
            RTFREE(ro->ro_rt);

        dst.sin_addr = ip->ip_dst;
        dst.sin_family = AF_INET;
        dst.sin_len = sizeof dst;
    }

    struct ifnet  *ifp = NULL;
    struct ifaddr *ia = NULL;
    struct in_ifaddr *ifa = NULL;
#define IFATOINA(ifa) ((struct in_ifaddr *)(ifa))
    if (flags & IP_ROUTETOIF)
    {
        if ((ifa = IFATOINA(ifa_ifwithdstaddr(SINTOSA(&dst))) == NULL
            && (ifa = IFATOINA(ifa_ifwithnet(SINTOSA(&dst)))) == NULL))
            return ENETUNREACH;
        else
            ifp = ifa->ia_ifa.ifa_ifp;
    }
    else
    {
        if (ro == NULL)
            ro = &iproute;
        ro->ro_dst = *(struct sockaddr *)&dst;
        rtalloc(ro);

        if (ro->ro_rt == NULL)
            return EHOSTUNREACH;

        ia = ro->ro_rt->rt_ifa;
        ifp = ro->ro_rt->rt_ifp;

        if (IFATOINA(ia)->ia_addr.sin_addr.s_addr != ip->ip_dst.s_addr)
        {
            dst.sin_addr = IFATOINA(ia)->ia_addr.sin_addr;
        }
    }

    if (ip->ip_src.s_addr == 0)
        ip->ip_src = SATOSIN(ia->ifa_dstaddr)->sin_addr;
// or     ip->ip_src = IFATOINA(ia)->ia_addr.sin_addr;

    if (in_broadcast(ip->ip_dst, ifp))
    {
        ifp->if_flags |= IFF_BROADCAST;
    }

    if (m0->m_pkthdr.len > ifp->if_baudrate)
        return EMSGSIZE;
    if (dst.sin_addr.s_addr == 0)
        return EADDRNOTAVAIL;

    m0->m_flags |= M_BCAST;
    if (m0->m_pkthdr.len < ifp->if_baudrate)
    {
        HTONS(ip->ip_len);
        HTONS(ip->ip_off);

        in_cksum(m0, hlen);
        if (ifp->if_output)
            (ifp->if_output)(ifp, m0, NULL, ro->ro_rt);
    }
    else
    {

    }

bad:
    m_free(m0);
    return 0;
}

/*
 * Insert IP options into preformed packet.
 * Adjust IP destination as required for IP source routing,
 * as indicated by a non-zero in_addr at the start of the options.
 */
static struct mbuf *
ip_insertoptions(m, opt, phlen)
	register struct mbuf *m;
	struct mbuf *opt;
	int *phlen;
{
    *phlen = m->m_len;
    
    if (opt->m_pkthdr.len > IP_MAXPACKET)
        return m;

    struct ipoption *ipoption = mtod(opt, struct ipoption*);
    struct ip* ip = mtod(m, struct ip*);

    if (ipoption->ipopt_dst.s_addr)
        ip->ip_dst = ipoption->ipopt_dst;

    struct mbuf *m0 = NULL;
    if (opt->m_len > 16 || m->m_flags & M_EXT != 0)
    {
        MGETHDR(m0, 0, MT_HEADER);
        m0->m_data += 20 + 8 + max_linkhdr;
        memcpy(m0->m_data, m->m_data, sizeof(struct ip));
        m0->m_len += sizeof(struct ip);

        m->m_data += sizeof(struct ip);
        m->m_len -= sizeof(struct ip);

        m0->m_next = m->m_next;
        m = m0;
    }
    else
    {
        memmove(m->m_data+ m->m_len + opt->m_len, m->m_data + m->m_len - sizeof (struct tcp_hdr));
    }
    memcpy(m->m_data + 20 + 8 + max_linkhdr + sizeof(struct ip), ipoption->ipopt_list, sizeof ipoption->ipopt_list);
    mtod(m, struct ip*)->ip_len += sizeof ipoption->ipopt_list;
    *phlen = m->m_len + sizeof ipoption->ipopt_list;

    return m;
}

/*
 * Copy options from ip to jp,
 * omitting those not copied during fragmentation.
 */
int
ip_optcopy(ip, jp)
	struct ip *ip, *jp;
{
    return 0;
}

/*
 * IP socket option processing.
 */
int
ip_ctloutput(op, so, level, optname, mp)
	int op;
	struct socket *so;
	int level, optname;
	struct mbuf **mp;
{
    struct mbuf *m = *mp;
    switch (op)
    {
    case PRCO_SETOPT:
        break;
    case PRCO_GETOPT:
        break;
    default:
        m_free(m);
        return EINVAL;
    }
    return 0;
}

/*
 * Set up IP options in pcb for insertion in output packets.
 * Store in mbuf with pointer in pcbopt, adding pseudo-option
 * with destination address if source routed.
 */
int
#ifdef notyet
ip_pcbopts(optname, pcbopt, m)
	int optname;
#else
ip_pcbopts(pcbopt, m)
#endif
	struct mbuf **pcbopt;
	register struct mbuf *m;
{
    if (pcbopt && *pcbopt)
    {
        m_free(*pcbopt);
        *pcbopt = NULL;
    }
    if (m == NULL || m->m_len == 0)
        return 0;

    if (mtod(m, char*) + m->m_len + sizeof(struct in_addr) > (char*)m + MLEN)
    {
        goto bad;
    }

    memmove(mtod(m, char*) + sizeof(struct in_addr), mtod(m, char*), m->m_len);
    memset(mtod(m, char*), 0, sizeof(struct in_addr));
   
    m->m_len += sizeof(struct in_addr);

    int optlen = 0;
    int len = m->m_len - sizeof(struct in_addr);
    char *p = mtod(m, char*) + sizeof(struct in_addr);
    for (; len > 0; len -= optlen)
    {
        int type = p[IPOPT_OPTVAL];
        if (type == IPOPT_NOP)
            optlen = 1;
        else if (type == IPOPT_NOP)
            break;
        else
        {
            switch (type)
            {
            case IPOPT_LSRR:
            case IPOPT_SSRR:
                memmove(mtod(m, char*), p + p[IPOPT_OFFSET] - 1, sizeof(struct in_addr));
                memmove(p + p[IPOPT_OFFSET] - 1, p + p[IPOPT_OFFSET] - 1 + sizeof(struct in_addr), p[IPOPT_OLEN] - p[IPOPT_OFFSET] - sizeof(struct in_addr) - 1);
                p[IPOPT_OLEN] -= sizeof(struct in_addr);
                m->m_len -= sizeof(struct in_addr);
                break;
            default:
                break;
            }
        }
    }
    IP_OPTIONS;
    IPOPT_OLEN;
    for (struct mbuf **opt = pcbopt; opt; opt++)
    {
        m_free(*opt);
    }

    struct ipoption ipoption;
    //if ()
    {
    }

    return 0;
bad:
    m_free(m);
    return EINVAL;
}

/*
 * Set the IP multicast options in response to user setsockopt().
 */
int
ip_setmoptions(optname, imop, m)
	int optname;
	struct ip_moptions **imop;
	struct mbuf *m;
{
    return 0;
}

/*
 * Return the IP multicast options in response to user getsockopt().
 */
int
ip_getmoptions(optname, imo, mp)
	int optname;
	register struct ip_moptions *imo;
	register struct mbuf **mp;
{
    return 0;
}

/*
 * Discard the IP multicast options.
 */
void
ip_freemoptions(imo)
	register struct ip_moptions *imo;
{
    return 0;
}

/*
 * Routine called from ip_output() to loop back a copy of an IP multicast
 * packet to the input queue of a specified interface.  Note that this
 * calls the output routine of the loopback "driver", but with an interface
 * pointer that might NOT be &loif -- easier than replicating that code here.
 */
static void
ip_mloopback(ifp, m, dst)
	struct ifnet *ifp;
	register struct mbuf *m;
	register struct sockaddr_in *dst;
{
    return 0;
}
