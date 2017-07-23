#include "../sys/domain.h"
#include <stdlib.h>
#include "../sys/types.h"
#include "../sys/mbuf.h"
#include "../sys/socket.h"
#include "../netinet/in.h"

#define ADDDOMAIN(x) {  \
    extern struct domains x##domain  \
    (x##domain).dom_next = domains; \
    domains = (x##domain);}


void domaininit()
{
//     ADDDOMAIN(iso);
//     ADDDOMAIN(inet);
//     ADDDOMAIN(route);
//     ADDDOMAIN(unix);

    for (struct domain *d = domains ; d != NULL; d = d->dom_next)
    {
        if (d->dom_init)
            d->dom_init();

        for (struct protosw *pro = d->dom_protosw;
                pro != d->dom_protoswNPROTOSW;
                pro++)
        {
            if (pro->pr_init)
                (pro->pr_init)();
        }
    }

    if (max_linkhdr < 16)
        max_linkhdr = 16;
    max_hdr = max_linkhdr + max_protohdr;
    max_datalen = MHLEN - max_hdr;

//     timeout(pffasttimo, (void *)0, 1);
//     timeout(pfslowtimo, (void *)0, 1);
}

void pfslowtimo(void *arg)
{
    for (struct domain *d = domains ; d != NULL; d = d->dom_next)
    {
        for (struct protosw *pro = d->dom_protosw;
                pro != d->dom_protoswNPROTOSW;
                pro++)
        {
            if (pro->pr_slowtimo)
                (pro->pr_slowtimo)();
        }
    }
}

void pffasttimo(void *arg)
{
    for (struct domain *d = domains ; d != NULL; d = d->dom_next)
    {
        for (struct protosw *pro = d->dom_protosw;
                pro != d->dom_protoswNPROTOSW;
                pro++)
        {
            if (pro->pr_fasttimo)
                (pro->pr_fasttimo)();
        }
    }
}

struct protosw *pffindtype(int family, int type)
{
    struct domain *d = domains;
    for (; d != NULL; d = d->dom_next)
    {
        if (d->dom_family == family)
            break;
    }

    if (d == NULL)
        return NULL;

    for (struct protosw *pro = d->dom_protosw;
            pro != d->dom_protoswNPROTOSW;
            pro++)
    {
        if (type == pro->pr_type)
            return pro;
    }

    return NULL;
}

struct protosw *pffindproto(int family, int protocol, int type)
{
    struct domain *d = domains;
    for (; d != NULL; d = d->dom_next)
    {
        if (d->dom_family == family)
            break;
    }

    if (d == NULL)
        return NULL;

    struct protosw *pro = d->dom_protosw;
    for (; pro != d->dom_protoswNPROTOSW; pro++)
    {
        if (type == pro->pr_type)
            return pro;
    }

    if (pro == d->dom_protoswNPROTOSW)
    {
        if (type == SOCK_RAW && protocol == 0)
            return d->dom_protoswNPROTOSW - 1;
    }

    return NULL;
}

void pfctlinput(int cmd, struct sockaddr *sa)
{
    for (struct domain *d = domains; d != NULL; d = d->dom_next)
    {
        for (struct protosw *pro = d->dom_protosw;
                pro != d->dom_protoswNPROTOSW;
                pro++)
        {
            if (pro->pr_ctlinput)
                (pro->pr_ctlinput)(cmd, sa, NULL);
        }
    }
}

void ip_init()
{
//     int ar[1024];
// 
//     struct protosw *pf = pffindproto(PF_INET, IPPROTO_RAW, SOCK_RAW);
//     if (pf == NULL)
//         return ;
// 
//     std::fill(ar, ar+1024, pf-inetsw);
// 
//     for (struct domain *d = domains; d != NULL; d = d->dom_next)
//     {
//         if (d->dom_family == PF_INET)
//             break;
//     }
// 
//     for (struct protosw *pro = domains->dom_protosw;
//             pro != domains->dom_protoswNPROTOSW;
//             pro++)
//     {
//         if (pro->pr_protocol && pro->pr_type != SOCK_RAW)
//             ar[pro->pr_protocol] = pro - domains->dom_protosw;
//     }
}

void net_sysctl(int *name, u_int namelen, void *oldp,
        size_t *oldlenp, void *newp, size_t newlen, struct proc *p)
{
}

