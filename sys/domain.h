#ifndef SYS_DOMAIN_H
#define SYS_DOMAIN_H

#include "../sys/types.h"
#include "protosw.h"


struct domain
{
    int dom_family; // defined in socket.h
    char *dom_name;
    void (*dom_init)(void);
    int (*dom_externalize)(struct mbuf*);
    int (*dom_dispose)(struct mbuf*);
    struct protosw *dom_protosw;
    struct protosw *dom_protoswNPROTOSW;
    struct domain *dom_next;
    int (*dom_rtattach)(void **, int);
    int dom_rtoffset;
    int dom_maxrtkey;
};

struct domain *domains;

//struct protosw inetsw[] =
//{
    //{
        //0, &inetdomain, 0, 0,
        //0, ip_output, 0, 0, 0,
        //ip_init, 0, ip_slowtimo, ip_drain, ip_sysctl
    //},
    //{
        //SOCK_DGRAM, &inetdomain, IPPROTO_UDP, PR_ATOMIC | PR_ADDR,
        //udp_input, 0, udp_ctlinput, ip_ctloutput, udp_usrreq,
        //udp_init, 0, 0, 0, udp_sysctl
    //},
    //{
        //SOCK_STREAM, &inetdomain, IPPROTO_TCP, PR_CONNREQUIRED | PR_WANTRCVD,
        //tcp_input, 0, tcp_ctlinput, tc_ctloutput, tcp_usrreq,
        //tcp_init, 0, 0, 0, 0, tcp_sysctl
    //},
    //{
        //SOCK_RAW, &inetdomain, IPPROTO_RAW, PR_ATOMIC | PR_ADDR,
        //rip_input, ripctloutput, 0, rip_usrreq,
        //0, 0, 0, 0
    //},
    //{
        //SOCK_RAW, &inetdomain, IPPROTO_ICMP, PR_ATOMIC | PR_ADDR,
        //icmp_input, rip_output, 0, rip_ctloutput, rip_usrreq,
        //0, 0, 0, 0, icmp_sysctl
    //},
    //{
        //SOCK_RAW, &inetdomain, IPPROTO_IGMP, PR_ATOMIC | PR_ADDR,
        //igmp_input, rip_output, 0, rip_ctloutput, rip_usrreq,
        //igmp_init, igmp_fasttimo, 0, 0
    //},
    //{
        //SOCK_RAW, &inetdomain, 0, PR_ATOMIC | PR_ADDR,
        //rip_input, rip_output, 0, rip_ctloutput, rip_usrreq,
        //rip_init, 0, 0, 0
    //},
//};

//struct domain inetdomain = 
//{
    //af_inet, "internet", 0, 0, 0,
    //inetsw, inetsw+7, 0, 
    //rn_inithead, 32, sizeof(struct sockaddr_in)
//};

//void domaininit();
//void pfslowtimo(void *arg);
//void pffasttimo(void *arg);

//struct protosw *pffindtype(int family, int type);
//struct protosw *pffindproto(int family, int protocol, int type);

//void pfctlinput(int cmd, struct sockaddr *sa);
//void ip_init();
//void net_sysctl(int *name, u_int namelen, void *oldp,
        //size_t *oldlenp, void *newp, size_t newlen, struct proc *p);


#endif  // SYS_DOMAIN_H
