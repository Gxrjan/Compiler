#include <iostream>
#include <cstring>
typedef char16_t *gstring;
using namespace std;

extern "C" {
inline long long gstring_len(gstring s) {
  return *((long long *)(s-4));
}


gstring concat_chars(const char16_t *s, long long slen, const char16_t *t, long long tlen) {
    gstring u = (gstring) malloc(2 * (slen + tlen) + 8) + 4;
    *((long long *)(u - 4)) = slen + tlen;
    memcpy(u, s, 2*slen);
    memcpy(u+slen, t, 2*tlen);
    return u;
}


u16string ascii_to_u16(string s) {
    u16string u;
    for (char c : s)
        u += c;
    return u;
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
    return concat_chars(s, gstring_len(s), t, gstring_len(t));
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
    u16string num = ascii_to_u16(std::to_string(i));
    return concat_chars(s, gstring_len(s), num.c_str(), num.length());
}

gstring concat_int_str(int i, gstring s)
{
    u16string num = ascii_to_u16(std::to_string(i));
    return concat_chars(num.c_str(), num.length(), s, gstring_len(s));
}

gstring concat_str_chr(gstring s, char16_t c) 
{
    return concat_chars(s, gstring_len(s), &c, 1);
}

gstring concat_chr_str(char16_t c, gstring s)
{
    return concat_chars(&c, 1, s, gstring_len(s));
}


gstring substr_int(gstring s, int from)
{
    long long s_len = gstring_len(s);
    if (from < 0 || from > s_len)
        throw runtime_error("index out of bounds");
    gstring u = (gstring)malloc(2*(s_len-from));
    long long *u_len = (long long *)u;
    u = u + 4;
    *u_len = s_len - from;
    for (int i=0;i<*u_len;i++)
        u[i] = s[from+i];
    return u;
}

gstring substr_int_int(gstring s, int from, int len) 
{
    long long s_len = gstring_len(s);
    if (len < 0)
        throw runtime_error("length of the substring must be >=0");
    if ((from < 0 || from > s_len) || 
        ((from + len) > s_len))
        throw runtime_error("index out of bounds");
    gstring u = (gstring)malloc(2*(len));
    long long *u_len = (long long *)u;
    u = u + 4;
    *u_len = len;
    for (int i=0;i<len;i++)
        u[i] = s[from+i];
    return u;
}


}
