#include "../sys/domain.h"
#include <stdlib.h>
#include "../sys/types.h"
#include "../sys/mbuf.h"
#include "../sys/socket.h"
#include "../netinet/in.h"
#include "..\sys\cdefs.h"
#include "..\sys\errno.h"

extern int hz;

#define ADDDOMAIN(x) {  \
    extern struct domain __CONCAT(x,domain);  \
    __CONCAT(x,domain).dom_next = domains; \
    domains = &__CONCAT(x,domain);}

void pfslowtimo(void *arg);
void pffasttimo(void *arg);
void timeout();
void domaininit()
{
    //     ADDDOMAIN(unix);
    //     ADDDOMAIN(route);
    ADDDOMAIN(inet);
    //     ADDDOMAIN(iso);

    for (struct domain *d = domains; d; d = d->dom_next)
    {
        if (d->dom_init)
            d->dom_init();

        for (struct protosw *pro = d->dom_protosw;
                pro < d->dom_protoswNPROTOSW;
                pro++)
        {
            if (pro->pr_init)
                (pro->pr_init)();
        }
    }

    max_protohdr = 40;

    if (max_linkhdr < 16)
        max_linkhdr = 16;
    max_hdr = max_linkhdr + max_protohdr;
    max_datalen = MHLEN - max_hdr;

    timeout(pffasttimo, (void *)0, 1);
    timeout(pfslowtimo, (void *)0, 1);
}

void pfslowtimo(void *arg)
{
    for (struct domain *d = domains; d; d = d->dom_next)
    {
        for (struct protosw *pro = d->dom_protosw;
                pro != d->dom_protoswNPROTOSW;
                pro++)
        {
            if (pro->pr_slowtimo)
                (pro->pr_slowtimo)();
        }
    }
    timeout(pfslowtimo, (void*)0, hz / 2);
}

void pffasttimo(void *arg)
{
    for (struct domain *d = domains; d; d = d->dom_next)
    {
        for (struct protosw *pro = d->dom_protosw;
                pro != d->dom_protoswNPROTOSW;
                pro++)
        {
            if (pro->pr_fasttimo)
                (pro->pr_fasttimo)();
        }
    }
    timeout(pffasttimo, (void*)0, hz / 2);
}

struct protosw *pffindtype(int family, int type)
{
    struct domain *d = domains;
    for (; d; d = d->dom_next)
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
    if (family == 0)
        return NULL;

    struct domain *d = domains;
    for (; d; d = d->dom_next)
    {
        if (d->dom_family == family)
            break;
    }

    if (d == NULL)
        return NULL;

    struct protosw *pro = d->dom_protosw;
    for (; pro < d->dom_protoswNPROTOSW; pro++)
    {
        if ((protocol == pro->pr_protocol) && (type == pro->pr_type))
            return pro;
        
        if (type == SOCK_RAW && pro->pr_type == SOCK_RAW
            && pro->pr_protocol == 0)
            return pro;
    }

    return NULL;
}

void pfctlinput(int cmd, struct sockaddr *sa)
{
    for (struct domain *d = domains; d; d = d->dom_next)
    {
        for (struct protosw *pro = d->dom_protosw;
                pro != d->dom_protoswNPROTOSW;
                pro++)
        {
            if (pro->pr_ctlinput)
                (pro->pr_ctlinput)(cmd, sa, (caddr_t)0);
        }
    }
}

int net_sysctl(int *name, u_int namelen, void *oldp,
        size_t *oldlenp, void *newp, size_t newlen, struct proc *p)
{
    if (namelen < 2)
        return;

    struct domain *d;
    int family = name[0];
    int protocol = name[1];

    for (d = domains; d; d = d->dom_next)
        if (d->dom_family == family)
            break;
    if (d == NULL)
        return ENOPROTOOPT;

    for (struct protosw *pr = d->dom_protosw; pr < d->dom_protoswNPROTOSW; pr++)
    {
        if (pr->pr_protocol == protocol && pr->pr_sysctl)
            return (pr->pr_sysctl)(name+2, namelen-2, oldp, oldlenp, newp, newlen);
    }

    return ENOPROTOOPT;
}

