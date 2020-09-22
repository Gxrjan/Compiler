#include <iostream>
#include <cstring>
typedef char16_t *gstring;
using namespace std;

extern "C" {
inline long long gstring_len(gstring s) {
    if (s == 0)
        throw runtime_error("null pointer exception");
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
    if (s == 0)
        throw runtime_error("null pointer exception");
    long long slen = gstring_len(s);
    if (i<0 || i>=slen)
        throw runtime_error("index out of bounds");
    return s[i];
}

gstring concat(gstring s, gstring t)
{
    if (t == 0 && s == 0)
        return 0;
    if (t == 0)
        return s;
    if (s == 0)
        return t;

    return concat_chars(s, gstring_len(s), t, gstring_len(t));
}

void printg(gstring s)
{
    if (s == 0) {
        printf("\n");
        return;
    }
    long long slen = gstring_len(s);
    for (int i = 0;i < slen;i++) {
        printf("%lc", s[i]);
    }
    printf("\n");
}


gstring concat_str_int(gstring s, int i)
{
    u16string num = ascii_to_u16(std::to_string(i));
    if (s == 0)
        return concat_chars(s, 0, num.c_str(), num.length());
    return concat_chars(s, gstring_len(s), num.c_str(), num.length());
}

gstring concat_int_str(int i, gstring s)
{
    u16string num = ascii_to_u16(std::to_string(i));
    if (s == 0)
        return concat_chars(num.c_str(), num.length(), s, 0);
    return concat_chars(num.c_str(), num.length(), s, gstring_len(s));
}

gstring concat_str_chr(gstring s, char16_t c) 
{
    if (s == 0)
        return concat_chars(s, 0, &c, 1);
    return concat_chars(s, gstring_len(s), &c, 1);
}

gstring concat_chr_str(char16_t c, gstring s)
{
    if (s == 0)
        return concat_chars(&c, 1, s, 0);
    return concat_chars(&c, 1, s, gstring_len(s));
}

gstring substr_int_int(gstring s, int from, int len) 
{
    if (s == 0)
        throw runtime_error("null pointer exception");
    long long s_len = gstring_len(s);
    if (len < 0)
        throw runtime_error("length of the substring must be >=0");
    if ((from < 0 || from > s_len) || 
        ((from + len) > s_len))
        throw runtime_error("index out of bounds");
    return concat_chars(s + from, len, s, 0);
}

gstring substr_int(gstring s, int from)
{
    if (s == 0)
        throw runtime_error("null pointer exception");
    return substr_int_int(s, from, gstring_len(s)-from);
}


long long int_parse(gstring s)
{
    if (s == 0)
        throw runtime_error("null pointer exception");
    long long s_len = gstring_len(s);
    if (s_len == 0)
        throw runtime_error("Trying to convert empty string to int");
    wstring result;
    int i = 0;
    wchar_t c = s[i];
    if (c == '-') {
        result += c;
        i++;
    }
    for (;i<s_len;i++) {
        wchar_t c = s[i];
        if (!isdigit(c))
            throw runtime_error("String contains non-digit characters");
        else
            result += c;
    }
    return std::stoll(result);
}


gstring new_str_expr(char16_t c, long long len)
{
    u16string s(len, c);
    return concat_chars(s.c_str(), len, &c, 0);
}




}
