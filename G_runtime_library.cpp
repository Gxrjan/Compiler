#include <iostream>
#include <cstring>
typedef char16_t *gstring;
using namespace std;

extern "C" {

char16_t get(gstring s, int i)
{
    long long slen = *((long long *)(s-4));
    if (i<0 || i>=slen)
        throw runtime_error("index out of bounds");
    return s[i];
}

gstring concat(gstring s, gstring t)
{
    long long slen = *((long long *)(s-4));
    long long tlen = *((long long *)(t-4));
    gstring u = (gstring)malloc(2*(slen+tlen)+8);
    long long *ulen = (long long *)u;
    u = u + 4;
    *ulen = slen + tlen;
    memcpy(u, s, 2*slen);
    memcpy(u+slen, t, 2*tlen);
    return u;
}

void printg(gstring s)
{
    long long slen = *((long long *)(s-4));
    for (int i = 0;i < slen;i++) {
        char c = *((char *)(s+i));
        printf("%c", c);
    }
    printf("\n");
}

}
