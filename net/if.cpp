#include "if_if.h"

int ifconf(int cmd, caddr_t data)
{
	return 0;
}

void if_down(struct ifnet *ifp)
{
}

void if_up(struct ifnet *ifp)
{
}


int ifioctl(struct socket *so, int cmd,
	caddr_t data, struct proc *p)
{
	return 0;
}

void if_attach(struct ifnet *ifp)
{
    static int i = 0;

    struct ifnet *iflast = ifnet;
    while (iflast && iflast->if_next)
    {
        iflast = iflast->if_next;
    }

    if (if_indexlim == 0)
        if_indexlim = 1;
    if (if_indexlim == i)
        if_indexlim <= 1;
    ifnet_addrs = (struct ifaddr **)realloc(ifnet_addrs,
            sizeof(struct ifaddr *) * if_indexlim);

    ifp->if_index = ++if_index;

    ifp->if_addrlist = (struct ifaddr *)calloc(1, sizeof (struct ifaddr));

    // set the member of struct ifaddr pointed by if_addrlist
    auto ifa = ifp->if_addrlist;
    ifa->ifa_next = nullptr;
    ifa->ifa_ifp = ifp;
    ifa->ifa_addr = (struct sockaddr *)calloc(2, sizeof (struct sockaddr_dl));
    //ifa->ifa_rtrequest = ;
    //ifa->ifa_flags = ;
    ifa->ifa_refcnt++;
    ifa->ifa_metric = ifp->if_metric;

    // set the member of struct sockaddr_dl pointed by ifa_addr && ifa_netmask
    struct sockaddr_dl *dl_addr = (struct sockaddr_dl *)ifa->ifa_addr;
    dl_addr->sdl_len = sizeof (sockaddr_dl);
    dl_addr->sdl_family = AF_LINK;
    dl_addr->sdl_index = ifp->if_index;
    dl_addr->sdl_type = ifp->if_type;
    strcpy(dl_addr->sdl_data, ifp->if_name);
    strcat(dl_addr->sdl_data, std::to_string(ifp->if_unit).c_str());
    dl_addr->sdl_nlen = strlen(dl_addr->sdl_data);
    dl_addr->sdl_alen = 6;
    //dl_addr->sdl_slen = ;

    struct sockaddr_dl *dl_netmask = dl_addr + 1;
    dl_netmask->sdl_len = offsetof(struct sockaddr_dl, sdl_data[0]) + dl_addr->sdl_nlen;
    for (int j = 0; j < dl_addr->sdl_nlen; j++)
        dl_netmask->sdl_data[j] = 0xff;

    ifa->ifa_netmask = (struct sockaddr *)dl_netmask;

    //add if_addrlist to the global variable "ifnext_addrs"
    ifnet_addrs[if_index - 1] = ifa;
    //add ifp to the global struct ifnet list pointed by ifnet;
    ifp->if_next = iflast->if_next;
    iflast->if_next = ifp;

    i++;
}

void ifinit()
{
}

void if_slowtimo(void *arg)
{
}
