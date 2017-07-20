struct mbuf;
struct ifnet;
struct ifaddr;
struct sockaddr_dl;

void  print_mbuf(struct mbuf *m, int offset, int len);
void print_mbuf_content(struct mbuf *m);

void print_ifnet(struct ifnet *ifp);
void print_global_ifnet();
void print_ifaddr(struct ifaddr *addrp);
void print_global_ifaddr();
void print_sockaddr_dl(struct sockaddr_dl *sip);
