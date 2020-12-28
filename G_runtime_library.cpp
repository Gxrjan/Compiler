#include <iostream>
#include <cstring>
typedef char16_t *gstring;
typedef char byte;
using namespace std;


u16string ascii_to_u16(string s) {
    u16string u;
    for (char c : s)
        u += c;
    return u;
}

extern "C" {

void decrement_reference_count(void *ptr) {
    int *p = (int*)ptr;
    int ref_count = *(p-2);
    printf("%d\n", ref_count);
}

void report_error(char *msg) {
    throw runtime_error(string(msg));
}

gstring to_gstring(char *str) {
    if (!str)
        throw runtime_error("null pointer exception");
    gstring u = (gstring) malloc(2 * strlen(str) + 4);
    *((int *)u) = strlen(str);
    u += 2;
    for (int i=0;i<strlen(str);i++)
        u[i] = str[i];
    return u;
}

gstring* to_argv(int argc, char **args) {
    char *ptr = (char *)malloc(argc*sizeof(gstring *) + 4);
    *((int *)ptr) = argc;
    ptr += 4;
    gstring *argv = (gstring *)ptr;
    for (int i=0;i<argc;i++)
        argv[i] = to_gstring(args[i]);
    return argv;
}

inline int gstring_len(gstring s) {
    if (!s)
        throw runtime_error("null pointer exception");
    return *((int *)(s-2));
}


gstring concat_chars(const char16_t *s, int slen, const char16_t *t, int tlen) {
    gstring u = (gstring) malloc(2 * (slen + tlen) + 4) + 2;
    *((int *)(u - 2)) = slen + tlen;
    memcpy(u, s, 2*slen);
    memcpy(u+slen, t, 2*tlen);
    return u;
}




gstring concat(gstring s, gstring t)
{
    if (!t && !s)
        return 0;
    if (!t)
        return s;
    if (!s)
        return t;

    return concat_chars(s, gstring_len(s), t, gstring_len(t));
}

void printg(gstring s)
{
    if (!s) {
        printf("\n");
        return;
    }
    int slen = gstring_len(s);
    for (int i = 0;i < slen;i++) {
        printf("%lc", s[i]);
    }
    printf("\n");
}


gstring concat_str_int(gstring s, int i)
{
    u16string num = ascii_to_u16(std::to_string(i));
    if (!s)
        return concat_chars(s, 0, num.c_str(), num.length());
    return concat_chars(s, gstring_len(s), num.c_str(), num.length());
}

gstring concat_int_str(int i, gstring s)
{
    u16string num = ascii_to_u16(std::to_string(i));
    if (!s)
        return concat_chars(num.c_str(), num.length(), s, 0);
    return concat_chars(num.c_str(), num.length(), s, gstring_len(s));
}

gstring concat_str_chr(gstring s, char16_t c) 
{
    if (!s)
        return concat_chars(s, 0, &c, 1);
    return concat_chars(s, gstring_len(s), &c, 1);
}

gstring concat_chr_str(char16_t c, gstring s)
{
    if (!s)
        return concat_chars(&c, 1, s, 0);
    return concat_chars(&c, 1, s, gstring_len(s));
}

gstring substr_int_int(gstring s, int from, int len) 
{
    if (!s)
        throw runtime_error("null pointer exception");
    int s_len = gstring_len(s);
    if (len < 0)
        throw runtime_error("length of the substring must be >=0");
    if ((from < 0 || from > s_len) || 
        ((from + len) > s_len))
        throw runtime_error("index out of bounds substr");
    return concat_chars(s + from, len, s, 0);
}

gstring substr_int(gstring s, int from)
{
    if (!s)
        throw runtime_error("null pointer exception");
    return substr_int_int(s, from, gstring_len(s)-from);
}


int int_parse(gstring s)
{
    if (!s)
        throw runtime_error("null pointer exception");
    int s_len = gstring_len(s);
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
    return std::stoi(result);
}


gstring new_str_expr(char16_t c, int len)
{
    u16string s(len, c);
    return concat_chars(s.c_str(), len, &c, 0);
}

void *new_arr_expr(int size, int len)
{
    byte *p = (byte*)malloc(len*size+4+4); // 4 for ref count, 4 for length
    *((int *)p) = 199; // initialize ref count
    p += 4;
    *((int *)p) = len; // initialize length
    p += 4;
    memset(p, 0, len*size);
    return p;
}

void free_if_zero_ref(void *p) {
    int *ptr = (int*)p;
    ptr = ptr-2;
    int ref_count = *(ptr);
    if (ref_count == 0) 
        free(ptr);
}


int arr_len(void *p)
{
    return gstring_len(static_cast<gstring>(p));
}


int cmp_str(gstring s, gstring t)
{
    if (!s && !t)
        return 1;
    else if (!s || !t)
        return 0;
    int s_len = gstring_len(s);
    int t_len = gstring_len(t);
    if (s_len != t_len)
        return 0;
    for (int i=0;i<s_len;i++)
        if (s[i] != t[i])
            return 0;
    return 1;
}

}
