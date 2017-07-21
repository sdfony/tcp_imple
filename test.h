struct mbuf;
struct ifnet;
struct ifaddr;
struct sockaddr_dl;

// chapter2
void  print_mbuf(struct mbuf *m, int offset, int len);
void print_mbuf_content(struct mbuf *m);

// chapter3
void print_ifnet(struct ifnet *ifp);
void print_global_ifnet();
void print_i_global_ifnet(int i);
void print_ifaddr(struct ifaddr *addrp);
void print_global_ifaddr();
void print_i_global_ifaddr(int i);
void print_sockaddr_dl(struct sockaddr_dl *sip);
