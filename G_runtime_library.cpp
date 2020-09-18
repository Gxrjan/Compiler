#include <iostream>
#include <cstring>
typedef char16_t *gstring;
using namespace std;

extern "C" {
inline long long gstring_len(gstring s) {
  return *((long long *)(s-4));
}


char16_t get(gstring s, int i)
{
    long long slen = gstring_len(s);
    if (i<0 || i>=slen)
        throw runtime_error("index out of bounds");
    return s[i];
}

gstring concat(gstring s, gstring t)
{
    long long slen = gstring_len(s);
    long long tlen = gstring_len(t);
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
    long long slen = gstring_len(s);
    for (int i = 0;i < slen;i++) {
        printf("%lc", s[i]);
    }
    printf("\n");
}

}
