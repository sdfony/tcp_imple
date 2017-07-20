#include "test.h"
#include "sys\mbuf.h"
#include "net\if.h"
#include "net\if_slvar.h"

#define N 1024

#define CHAPTER1
#define CHAPTER2
#define CHAPTER3
#define CHAPTER3
#define CHAPTER4

int main(void)
{
    char buf[N] = {'\0'};

#ifdef CHAPTER2
    struct mbuf *m1;
    struct mbuf *m2;

    MGET(m1, 0, MT_HEADER);
    MGET(m2, 0, MT_DATA);

    char *p = "it's a test of mbuf on T460s, end...";

    m_copyback(m1, 0, strlen(p), p);
    print_mbuf(m1, 0, 0);
	print_mbuf_content(m1);

    m_copyback(m2, 4, strlen(p), p);
    print_mbuf(m2, 4, 0);
	print_mbuf_content(m2);

    m_adj(m1, -5);
	print_mbuf(m1, 0, 0);
	print_mbuf_content(m1);

    m_cat(m1, m2);
	print_mbuf(m1, 0, 0);
	print_mbuf_content(m1);

    m_copydata(m1, 4, 5, buf);
	print_mbuf(m1, 0, 0);
	print_mbuf_content(m1);
	printf("%s\n", buf);

    struct mbuf *m3 = m_copym(m2, 15, 40, 0);
	print_mbuf(m3, 0, 0);
	print_mbuf_content(m3);

	struct ifnet ifnet1;
    struct mbuf *m4 = m_devget(buf, 20, 5, &ifnet1, NULL);
	print_mbuf(m4, 0, 0);
	print_mbuf_content(m4);
#endif  // CHAPTER2

#ifdef CHAPTER3
loopattach(6);
slattach();
#endif  // CHAPTER3

#ifdef CHAPTER4
#endif  // CHAPTER4

    return 0;
}
