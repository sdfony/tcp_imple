#include "sys\mbuf.h"
#include <cstring>
#include <stdio.h>
#include "net\if.h"

#define N 1024

static void inline print_mbuf(struct mbuf *m, int offset, int len = 0);
static void inline print_mbuf_content(struct mbuf *m);

int main(void)
{
    char buf[N] = {'\0'};
    struct mbuf *m1;
    struct mbuf *m2;

    MGET(m1, 0, MT_HEADER);
    MGET(m2, 0, MT_DATA);

    char *p = "it's a test of mbuf on T460s, end...";

    m_copyback(m1, 0, std::strlen(p), p);
    print_mbuf(m1, 0);
	print_mbuf_content(m1);

    m_copyback(m2, 4, std::strlen(p), p);
    print_mbuf(m2, 4);
	print_mbuf_content(m2);

    m_adj(m1, -5);
	print_mbuf(m1, 0);
	print_mbuf_content(m1);

    m_cat(m1, m2);
	print_mbuf(m1, 0);
	print_mbuf_content(m1);

    m_copydata(m1, 4, 5, buf);
	print_mbuf(m1, 0);
	print_mbuf_content(m1);
	printf("%s\n", buf);

    struct mbuf *m3 = m_copym(m2, 15, 40, 0);
	print_mbuf(m3, 0);
	print_mbuf_content(m3);

	struct ifnet ifnet1;
    struct mbuf *m4 = m_devget(buf, 20, 5, &ifnet1, nullptr);
	print_mbuf(m4, 0);
	print_mbuf_content(m4);

    return 0;
}

static void inline print_mbuf(struct mbuf *m, int offset, int len)
{
    while (m->m_len < offset)
    {
        m = m->m_next;
    }

    auto p = mtod(m, caddr_t) + offset;
    while (m->m_len < len)
    {
        printf("%s\n", p);

        offset = 0;
        len -= m->m_len;
        m = m->m_next;
    }
    printf("%s\n", p);
    offset = 0;
}

static void inline print_mbuf_content(struct mbuf *m)
{
    int i = 0, total_len = 0;
    while (m)
    {
        printf("%d mbuf: p=0x%p, m_len=%d\n", ++i, mtod(m, caddr_t), m->m_len);
        total_len += m->m_len;
		m = m->m_next;
    }
    printf("total_len=%d\n\n", total_len);
}
