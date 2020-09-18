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


gstring concat_str_int(gstring s, int i)
{
    string num = std::to_string(i);
    const char *num_p = num.c_str();
    long long num_len = num.length();
    long long s_len = gstring_len(s);
    gstring u = (gstring)malloc(2*(s_len+num_len)+8);
    long long *u_len = (long long *)u;
    u = u + 4;
    *u_len = s_len + num_len;
    memcpy(u, s, 2*s_len);
    for (int j=0;j<num_len;j++)
        memcpy(u+s_len+j, num_p+j, 1);
    return u;
}

gstring concat_int_str(int i, gstring s)
{
    string num = std::to_string(i);
    const char *num_p = num.c_str();
    long long num_len = num.length();
    long long s_len = gstring_len(s);
    gstring u = (gstring)malloc(2*(s_len+num_len)+8);
    long long *u_len = (long long *)u;
    u = u + 4;
    *u_len = s_len + num_len;
    for (int j=0;j<num_len;j++)
        memcpy(u+j, num_p+j, 1);
    memcpy(u+num_len, s, 2*s_len);
    return u;

}

gstring concat_str_chr(gstring s, char16_t c) 
{
    long long s_len = gstring_len(s);
    gstring u = (gstring)malloc(2*(s_len+1)+8);
    long long *u_len = (long long *)u;
    u = u + 4;
    *u_len = s_len + 1;
    memcpy(u, s, 2*s_len);
    u[s_len] = c;
    return u;
}

gstring concat_chr_str(char16_t c, gstring s)
{
    long long s_len = gstring_len(s);
    gstring u = (gstring)malloc(2*(s_len+1)+8);
    long long *u_len = (long long *)u;
    u = u + 4;
    *u_len = s_len + 1;
    *u = c;
    memcpy(u+1, s, 2*s_len);
    return u;
}


}
