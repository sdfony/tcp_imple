struct if_data
{
	u_char ifi_type;
	u_char ifi_addrlen;
	u_char ifi_hdrlen;
	u_long ifi_mtu;
	u_long ifi_metric;
	u_long ifi_baudrate;

	// other ifnet members;

	u_long ifi_ipackets;
	u_long ifi_ierrors;
	u_long ifi_opackets;
	u_long ifi_oerrors;
	u_long ifi_collisions;
	u_long ifi_ibytes;
	u_long ifi_obytes;
	u_long ifi_imcasts;
	u_long ifi_omcasts;
	u_long ifi_iqdrops;
	u_long ifi_noproto;

	struct timeval ifi_lastchange;
};

#define	IFF_UP		0x1		/* (n) interface is up */
#define	IFF_BROADCAST	0x2		/* (i) broadcast address valid */
#define	IFF_DEBUG	0x4		/* (n) turn on debugging */
#define	IFF_LOOPBACK	0x8		/* (i) is a loopback net */
#define	IFF_POINTOPOINT	0x10		/* (i) is a point-to-point link */
/*			0x20		   was IFF_SMART */
#define	IFF_DRV_RUNNING	0x40		/* (d) resources allocated */
#define	IFF_NOARP	0x80		/* (n) no address resolution protocol */
#define	IFF_PROMISC	0x100		/* (n) receive all packets */
#define	IFF_ALLMULTI	0x200		/* (n) receive all multicast packets */
#define	IFF_DRV_OACTIVE	0x400		/* (d) tx hardware queue is full */
#define	IFF_SIMPLEX	0x800		/* (i) can't hear own transmissions */
#define	IFF_LINK0	0x1000		/* per link layer defined bit */
#define	IFF_LINK1	0x2000		/* per link layer defined bit */
#define	IFF_LINK2	0x4000		/* per link layer defined bit */
#define	IFF_ALTPHYS	IFF_LINK2	/* use alternate physical connection */
#define	IFF_MULTICAST	0x8000		/* (i) supports multicast */
#define	IFF_CANTCONFIG	0x10000		/* (i) unconfigurable using ioctl(2) */
#define	IFF_PPROMISC	0x20000		/* (n) user-requested promisc mode */
#define	IFF_MONITOR	0x40000		/* (n) user-requested monitor mode */
#define	IFF_STATICARP	0x80000		/* (n) static ARP */
#define	IFF_DYING	0x200000	/* (n) interface is winding down */
#define	IFF_RENAMING	0x400000	/* (n) interface is being renamed */

/*
 * Values for if_link_state.
 */
#define	LINK_STATE_UNKNOWN	0	/* link invalid/unknown */
#define	LINK_STATE_DOWN		1	/* link is down */
#define	LINK_STATE_UP		2	/* link is up */

struct ifreq
{
#define IFNAMSIZ 16

    char ifr_name[IFNAMSIZ];
    union
    {
        struct sockaddr ifru_addr;
        struct sockaddr ifru_dstaddr;
        struct sockaddr ifru_broadaddr;
        short ifru_flags;
        int ifru_metric;
        caddr_t ifru_data;
    } ifr_ifru;

#define ifr_addr        ifr_ifru.ifru_addr
#define ifr_dstaddr     ifr_ifru.ifru_dstaddr
#define ifr_broadaddr   ifr_ifru.ifru_broadaddr
#define ifr_flags       ifr_ifru.ifru_flags
#define ifr_metric      ifr_ifru.ifru_metric
#define ifr_data        ifr_ifru.ifru_data
};

struct in_aliasreq
{
    char    ifra_name[IFNAMESIZE];
    struct sockaddr_in  ifra_addr;
    struct sockaddr_in  ifra_broadaddr;
#define ifra_dstaddr    ifra_broadaddr
    struct sockaddr_in  ifra_mask;
};

struct ifconf
{
    int ifc_len;
    union
    {
        caddr_t ifcu_buf;
        struct ifreq *ifcu_req;
    } ifc_ifcu;

#define ifc_buf ifc_ifcu.ifcu_buf
#define ifc_req ifc_ifcu.ifcu_req
};

struct ifnet
{
    struct ifnet *if_next;
    struct ifaddr *if_addrlist;
    char *if_name;
    short if_unit;
    u_short if_index;
    short if_flags;
    short if_timer;
    int if_pcount;
    caddr_t if_bpf;

    struct ifqueue if_snd;
	struct if_data if_data;

    int (*if_init)(int);
    int (*if_output)(struct ifnet *, struct mbuf *,
            struct sockaddr*, struct rtentry *);
    int (*if_start)(struct ifnet *);
    int (*if_done)(struct ifnet *);
    int (*if_ioctl)(struct ifnet *, int, caddr_t);
    int (*if_reset)(int);
    int (*if_watchdog)(int);

#define if_mtu if_data.ifi_mtu
#define if_type if_data.ifi_type
#define if_addrlen if_data.ifi_addrlen
#define if_hdrlen if_data.ifi_hdrlen
#define if_metric if_data.ifi_metric
#define if_baudrate if_data.ifi_baudrate
#define if_ipackets if_data.ifi_ipackets
#define if_ierrors if_data.ifi_ierrors
#define if_opackets if_data.ifi_opackets
#define if_oerrors if_data.ifi_oerrors
#define if_collisions if_data.ifi_collisions
#define if_ibytes if_data.ifi_ibytes
#define if_obytes if_data.ifi_obytes
#define if_imcasts if_data.ifi_imcasts
#define if_omcasts if_data.ifi_omcasts
#define if_iqdrops if_data.ifi_iqdrops
#define if_noproto if_data.ifi_noproto
#define if_lastchange if_data.ifi_lastchange
};

struct ifqueue
{
    struct mbuf *ifq_head;
    struct mbuf *ifq_tail;
    int ifq_len;
    int ifq_maxlen;
    int ifq_drops;
};

