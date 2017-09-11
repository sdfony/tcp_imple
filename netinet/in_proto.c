#include "../sys/param.h"
#include "../sys/socket.h"
#include "../sys/protosw.h"
#include "../sys/domain.h"
#include "../sys/mbuf.h"

#include "../net/if.h"
#include "../net/radix.h"
#include "../net/route.h"

#include "in.h"
#include "in_systm.h"
#include "ip.h"
#include "ip_var.h"
/*#include "ip_icmp.h"*/
/*#include "in_pcb.h"*/
/*#include "igmp_var.h"*/
/*#include "tcp.h"*/
/*#include "tcp_fsm.h"*/
/*#include "tcp_seq.h"*/
/*#include "tcp_timer.h"*/
/*#include "tcp_var.h"*/
/*#include "tcpip.h"*/
/*#include "tcp_debug.h"*/
/*#include "udp.h"*/
/*#include "udp_var.h"*/
/*
 * TCP/IP protocol family: IP, ICMP, UDP, TCP.
 */

#ifdef NSIP
void	idpip_input(), nsip_ctlinput();
#endif

#ifdef TPIP
void	tpip_input(), tpip_ctlinput(), tp_ctloutput();
int	tp_init(), tp_slowtimo(), tp_drain(), tp_usrreq();
#endif

#ifdef EON
void	eoninput(), eonctlinput(), eonprotoinit();
#endif /* EON */

extern	struct domain inetdomain;

int ip_output();
void ip_init();
void ip_slowtimo();
void ip_drain();
int ip_sysctl();

void udp_input();
void udp_ctlinput();
int ip_ctloutput();
int udp_usrreq();
void udp_init();
int udp_sysctl();

void tcp_input();
void tcp_ctlinput();
int tcp_ctloutput();
int tcp_usrreq();
void tcp_init();
void tcp_fasttimo();
void tcp_slowtimo();
void tcp_drain();

void rip_input();
int rip_output();
int rip_ctloutput();
int rip_usrreq();

void icmp_input();
int icmp_sysctl();

void igmp_input();
void igmp_init();
void igmp_fasttimo();

void tpip_input();
int tpip_ctlinput();
int tp_ctloutput();
int tp_usrreq();
void tp_init();
void tp_slowtimo();
void tp_drain();

void eoninput();
int eonctlinput();
void eonprotoinit();

void idpip_input();
void rip_init();
int nsip_ctlinput();

struct protosw inetsw[] = {
{ 0,		&inetdomain,	0,		0,
  0,		ip_output,	0,		0,
  0,
  ip_init,	0,		ip_slowtimo,	ip_drain,	ip_sysctl
},
{ SOCK_DGRAM,	&inetdomain,	IPPROTO_UDP,	PR_ATOMIC|PR_ADDR,
  udp_input,	0,		udp_ctlinput,	ip_ctloutput,
  udp_usrreq,
  udp_init,	0,		0,		0,		udp_sysctl
},
{ SOCK_STREAM,	&inetdomain,	IPPROTO_TCP,	PR_CONNREQUIRED|PR_WANTRCVD,
  tcp_input,	0,		tcp_ctlinput,	tcp_ctloutput,
  tcp_usrreq,
  tcp_init,	tcp_fasttimo,	tcp_slowtimo,	tcp_drain,
},
{ SOCK_RAW,	&inetdomain,	IPPROTO_RAW,	PR_ATOMIC|PR_ADDR,
  rip_input,	rip_output,	0,		rip_ctloutput,
  rip_usrreq,
  0,		0,		0,		0,
},
{ SOCK_RAW,	&inetdomain,	IPPROTO_ICMP,	PR_ATOMIC|PR_ADDR,
  icmp_input,	rip_output,	0,		rip_ctloutput,
  rip_usrreq,
  0,		0,		0,		0,		icmp_sysctl
},
{ SOCK_RAW,	&inetdomain,	IPPROTO_IGMP,	PR_ATOMIC|PR_ADDR,
  igmp_input,	rip_output,	0,		rip_ctloutput,
  rip_usrreq,
  igmp_init,	igmp_fasttimo,	0,		0,
},
#ifdef TPIP
{ SOCK_SEQPACKET,&inetdomain,	IPPROTO_TP,	PR_CONNREQUIRED|PR_WANTRCVD,
  tpip_input,	0,		tpip_ctlinput,	tp_ctloutput,
  tp_usrreq,
  tp_init,	0,		tp_slowtimo,	tp_drain,
},
#endif
/* EON (ISO CLNL over IP) */
#ifdef EON
{ SOCK_RAW,	&inetdomain,	IPPROTO_EON,	0,
  eoninput,	0,		eonctlinput,		0,
  0,
  eonprotoinit,	0,		0,		0,
},
#endif
#ifdef NSIP
{ SOCK_RAW,	&inetdomain,	IPPROTO_IDP,	PR_ATOMIC|PR_ADDR,
  idpip_input,	rip_output,	nsip_ctlinput,	0,
  rip_usrreq,
  0,		0,		0,		0,
},
#endif
	/* raw wildcard */
{ SOCK_RAW,	&inetdomain,	0,		PR_ATOMIC|PR_ADDR,
  rip_input,	rip_output,	0,		rip_ctloutput,
  rip_usrreq,
  rip_init,	0,		0,		0,
},
};

struct domain inetdomain =
    { AF_INET, "internet", 0, 0, 0, 
      inetsw, &inetsw[sizeof(inetsw)/sizeof(inetsw[0])], 0,
      rn_inithead, 32, sizeof(struct sockaddr_in) };

//#include "imp.h"
//#if NIMP > 0
//extern	struct domain impdomain;
//int	rimp_output(), hostslowtimo();
//
//struct protosw impsw[] = {
//{ SOCK_RAW,	&impdomain,	0,		PR_ATOMIC|PR_ADDR,
//  0,		rimp_output,	0,		0,
//  rip_usrreq,
//  0,		0,		hostslowtimo,	0,
//},
//};
//
//struct domain impdomain =
//    { AF_IMPLINK, "imp", 0, 0, 0,
//      impsw, &impsw[sizeof (impsw)/sizeof(impsw[0])] };
//#endif
//
//#include "hy.h"
//#if NHY > 0
///*
// * HYPERchannel protocol family: raw interface.
// */
//int	rhy_output();
//extern	struct domain hydomain;
//
//struct protosw hysw[] = {
//{ SOCK_RAW,	&hydomain,	0,		PR_ATOMIC|PR_ADDR,
//  0,		rhy_output,	0,		0,
//  rip_usrreq,
//  0,		0,		0,		0,
//},
//};
//
//struct domain hydomain =
//    { AF_HYLINK, "hy", 0, 0, 0, hysw, &hysw[sizeof (hysw)/sizeof(hysw[0])] };
//#endif
