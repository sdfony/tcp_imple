#include "..\sys\mbuf.h"
#include <string.h>
#include <algorithm>

static inline int m_total_len(struct mbuf *m)
{
    int len = 0;
    while (m != nullptr)
    {
        len += m->m_len;
        m = m->m_next;
    }

    return len;
}

// len > 0, remove data from end to start
// len < 0, remove data from start to end
void m_adj(struct mbuf *m, int len)
{
    if (m == nullptr)
        return ;

    if (len > 0)
    {
        if (m->m_flags & M_PKTHDR)
           m->m_pkthdr.len -= len;

         while (m != nullptr)
        {
            if (m->m_len < len)
            {
                len -= m->m_len;

                m->m_len = 0;
                m = m->m_next;
            }
            else
                break;
        }

        m->m_len -= len;
        m->m_data += len;
    }
    else
    {
        len = -len;

        //int total = 0;
        //if (m->m_flags & M_PKTHDR)
        //{
            //total = m->m_pkthdr.len -= len;
        //}
        //else
        //{
            //struct mbuf *old = m;
            //total = m_total_len(m);
			//total -= len;

            //m = old;
        //}

        while (m != nullptr)
        {
            if (m->m_len < len)
            {
                len -= m->m_len;
            }
            else
            {
                m->m_len -= len;
                break;
            }
            m = m->m_next;
        }
    }
}

// 1 2 3 4 5 6 7 8 9 
void m_cat(struct mbuf *m, struct mbuf *n)
{
    struct mbuf *old = m;

    while (m->m_next)
    {
        m = m->m_next;
    }

    m->m_next = n;

    m = old;

    if (m->m_flags & M_PKTHDR && n->m_flags & M_PKTHDR)
        m->m_pkthdr.len += n->m_pkthdr.len;
    else if (m->m_flags & M_PKTHDR && !(n->m_flags & M_PKTHDR))
    {
        int len = m_total_len(n);
        m->m_pkthdr.len += len;
    }
    // m->m_flags has no M_PKTHDR, indicates that m has no m_pkthdr
    // so there is no way to set m_pkthdr.len
    //
    //else if (!(m->m_flags & M_PKTHDR) && n->m_flags & M_PKTHDR)
    //{
        //int len = m_total_len(m);
    //}
    //else if (!(m->m_flags & M_PKTHDR) && !(n->m_flags & M_PKTHDR))
    //{
        //int mlen = m_total_len(m);
        //int nlen = m_total_len(n);
    //}
}

struct mbuf *m_copy(struct mbuf *m, int offset, int len)
{
    return nullptr;
}

static inline int get_total_len(struct mbuf *m)
{
	int total_len = 0;
	if (m->m_flags == M_PKTHDR)
		total_len = MHLEN;
	else if (m->m_flags == M_EXT)
		total_len = MCLBYTES;
	else
		total_len = MLEN;

	return total_len;
}

// copy len bytes data from cp to mbuf's offset position
void m_copyback(struct mbuf *m, int offset, int len, caddr_t cp)
{
    struct mbuf *old = m;

    while (m)
    {
		int total_len = 0;

        if (m == nullptr)
        {
            int nowait = 0;
            m = m_get(nowait, 0);
            m->m_len = MLEN;
            memset(mtod(m, caddr_t), 0, m->m_len * sizeof(char));
            m_cat(old, m);
        }

		total_len = get_total_len(m);
		if (total_len < offset)
        {
            offset -= total_len;
            m = m->m_next;
        }
        else
            break;
    }

    while (len > 0)
    {
		int total_len = 0;
		
		if (m == nullptr)
        {
            int nowait = 0;
            m = m_get(nowait, 0);
            m->m_len = MLEN;
            memset(mtod(m, caddr_t), 0, m->m_len * sizeof(char));
            m_cat(old, m);
        }
		if (m->m_len == 0)
		{
			memset(mtod(m, caddr_t), 0, offset * sizeof(char));
			m->m_len += offset;
		}

		total_len = get_total_len(m);
        memcpy(mtod(m, caddr_t)+offset, cp, std::min(total_len-offset, len));

		if (m->m_len < offset + len)
			m->m_len = std::min(total_len, len + offset);

        len -= std::min(total_len - offset, len);
        offset = 0;

        m = m->m_next;
    }
}


// copy data from mbuf list to cp buffer
void m_copydata(struct mbuf *m, int offset, int len, caddr_t cp)
{
    while (m)
    {
        if (m->m_len < offset)
        {
            offset -= m->m_len;
            m = m->m_next;
        }
        else
            break;
    }

    while (len > 0)
    {
        memcpy(cp, mtod(m, caddr_t) + offset, std::min(m->m_len-offset, len) * sizeof (char));

        cp += std::min(m->m_len-offset, len);
        len -= std::min(m->m_len-offset, len);
        offset = 0;

        m = m->m_next;
    }
}

// copy data of an exsiting mbuf to create a new mbuf
struct mbuf *m_copym(struct mbuf *m, int offset, int len, int nowait)
{
    struct mbuf *n = nullptr;
    MGETHDR(n, nowait, m->m_flags);
    struct mbuf *old_n = n;

    if (len == M_COPYFLAGS)
    {
        // 
    }

    struct mbuf *old = m;
    while (m)
    {
        if (m == nullptr)
        {
            int nowait = 0;
            m = m_get(nowait, 0);
            m->m_len = MLEN;
            memset(mtod(m, caddr_t), 0, m->m_len * sizeof(char));

            m_cat(old, m);
        }

        if (m->m_len < offset)
        {
            offset -= m->m_len;
            m = m->m_next;
        }
        else
            break;
    }

    while (len > 0)
    {
        if (m == nullptr)
        {
            int nowait = 0;
            m = m_get(nowait, 0);
            m->m_len = MLEN;
            memset(mtod(m, caddr_t), 0, m->m_len * sizeof(char));

            m_cat(old, m);
        }

        int copy_len = std::min(len, m->m_len - offset);

        if (n == nullptr)
        {
            MGET(n, nowait, m->m_flags);
            m_cat(old_n, n);
        }
        memcpy(mtod(n, caddr_t), mtod(m, caddr_t) + offset, copy_len * sizeof (char));
        n->m_len = copy_len;

        offset = 0;
        len -= copy_len;

        m = m->m_next;
        n = n->m_next;
    }

	return old_n;
}

// copy data from buf to a created new mbuf with M_PKTHDR flag.
struct mbuf *m_devget(char *buf, int len, int off, struct ifnet *ifp,
        void (*copy)(const void *, void *, u_int))
{
    struct mbuf *m = nullptr;
	int type = 0;
	int nowait = 0;
    char *p = buf + off;

    MGETHDR(m, nowait, M_PKTHDR);
    struct mbuf *old = m;
    bool has_cluster = len >= 209;

    while (len > 0)
    {
        if (m == nullptr)
        {
            int nowait = 0;
            if (!has_cluster)
            {
                m = m_get(nowait, 0);
                m->m_len = MLEN;
            }
            else if (has_cluster)
            {
                m = m_getclr(nowait, 0);
                m->m_len = MCLBYTES;
            }

            memset(mtod(m, caddr_t), 0, m->m_len * sizeof(char));
            m_cat(old, m);
        }

        int copy_len = std::min(len, m->m_len);
        memcpy(mtod(m, caddr_t), p, copy_len * sizeof (char));
        m->m_len = copy_len;
        len -= copy_len;
        p += copy_len;
        m = m->m_next;
    }

    return old;
}

static int ref(caddr_t p)
{
    return 0;
}

struct mbuf *m_free(struct mbuf *m)
{
    struct mbuf *n = nullptr;

    if (m == nullptr)
        return n;

    if (m->m_flags & M_EXT)
    {
        if (ref(m->m_ext.ext_buf) == 0)
        {
            free(m->m_ext.ext_buf);
        }

    }
    n->m_nextpkt = m->m_nextpkt;
    n->m_type = m->m_type;
    n->m_flags |= M_PKTHDR;

    n->m_pkthdr.len = m->m_pkthdr.len - m->m_len;
    n->m_pkthdr.rcvif = m->m_pkthdr.rcvif;

    free(m);
    return n;
}

void m_freem(struct mbuf *m)
{
    struct mbuf *n;

    while (m)
    {
        n = m->m_next;

        if (m->m_flags & M_EXT)
        {
            free(m->m_ext.ext_buf);
        }

        free(m);

        m = n;
    }
}

struct mbuf *m_get(int nowait, int type)
{
    struct mbuf *m = nullptr;

    MGET(m, nowait, type);
    memset(mtod(m, caddr_t), 0, sizeof (char) * MLEN);

    return m;
}

struct mbuf *m_getclr(int nowait, int type)
{
    struct mbuf *m = nullptr;

    MGET(m, nowait, type);
	mtod(m, caddr_t) = (caddr_t)malloc(sizeof(char) * MCLBYTES);
    memset(mtod(m, caddr_t), 0, sizeof (char) * MCLBYTES);

    return m;
}

struct mbuf *m_gethdr(int nowait, int type)
{
    struct mbuf *m = nullptr;

    MGETHDR(m, nowait, type);
    memset(mtod(m, caddr_t), 0, sizeof (char) * MHLEN);

    return m;
}

struct mbuf *m_pullup(struct mbuf *m, int len)
{
    struct mbuf *n = m;
	int nowait = 0;
    if (m->m_flags & M_EXT)
    {
        n = m_gethdr(nowait, m->m_type);
        n->m_pkthdr = m->m_pkthdr;
    }

    while (n->m_len < len)
    {
        int copy_len = std::min(m->m_len, len - m->m_len);

        memcpy(mtod(n, caddr_t) + n->m_len, mtod(m, caddr_t), copy_len);
        n->m_len += copy_len;
        len -= copy_len;

        if (m->m_next)
            m = m->m_next;
        else
            break;
    }

    if (n->m_len == len)
        return n;
    else
    {
        m_free(n);
        return nullptr;
    }
}

void m_reclaim()
{
}

struct mbuf *m_retry(int i, int t)
{
	struct mbuf *m = nullptr;

	m_reclaim();

#define m_retry(i, t)   (struct mbuf *)0
	MGET(m, i, t);
#undef  m_retry

	return m;
}

