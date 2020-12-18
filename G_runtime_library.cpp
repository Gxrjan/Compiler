#include <iostream>
#include <cstring>
typedef char16_t *gstring;
using namespace std;


u16string ascii_to_u16(string s) {
    u16string u;
    for (char c : s)
        u += c;
    return u;
}

extern "C" {

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


char16_t get16(gstring s, int i)
{
    if (!s)
        throw runtime_error("null pointer exception");
    int slen = gstring_len(s);
    if (i<0 || i>=slen)
        throw runtime_error("index out of bounds get16");
    return s[i];
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

int *new_int_arr_expr(int len)
{
    int *p = (int *)malloc(len*4+4);
    *(p) = len;
    p += 1;
    memset(p, 0, len*4);
    return p;
}

char16_t *new_char_arr_expr(int len)
{
    char16_t *p = (char16_t *)malloc(len*2+4);
    *((int *)p) = len;
    p += 2;
    memset(p, 0, len*2);
    return p;
}

gstring *new_string_arr_expr(int len)
{
    gstring *p = (gstring *)malloc(len*8+4);
    *((int *)p) = len;
    p += 1;
    memset(p, 0, len*8);
    return p;
}

char *new_bool_arr_expr(int len)
{
    char *p = (char *)malloc(len+4);
    *((int *)p) = len;
    p += 4;
    memset(p, 0, len);
    return p;
}

void *new_arr_expr(int size, int len)
{
    void *p = malloc(len*size+4);
    *((int *)p) = len;
    p = static_cast<char *>(p) + 4;
    memset(p, 0, len*size);
    return p;
}


int arr_len(void *p)
{
    return gstring_len(static_cast<gstring>(p));
}

int get32(int *arr, int index)
{
    if (!arr)
        throw runtime_error("null pointer exception");
    int arr_l = arr_len(arr);
    if (index < 0 || index >= arr_l)
        throw runtime_error("index out of bounds get32");
    return arr[index];
}

void *get64(void **arr, int index)
{
    if (!arr)
        throw runtime_error("null pointer exception");
    int arr_l = arr_len(arr);
    if (index < 0 || index >= arr_l)
        throw runtime_error("index out of bounds get64");
    return arr[index];
}

bool get8(bool *arr, int index)
{
    if (!arr)
        throw runtime_error("null pointer exception");
    int arr_l = arr_len(arr);
    if (index < 0 || index >= arr_l)
        throw runtime_error("index out of bounds get8");
    return arr[index];
}


void set16(gstring arr, int index, char16_t val)
{
    if (!arr)
        throw runtime_error("null pointer exception");
    int arr_l = arr_len(arr);
    if (index < 0 || index >= arr_l)
        throw runtime_error("index out of bounds set16");
    arr[index] = val;
}

void set32(int *arr, int index, int val)
{
    if (!arr)
        throw runtime_error("null pointer exception");
    int arr_l = arr_len(arr);
    if (index < 0 || index >= arr_l)
        throw runtime_error("index out of bounds set32");
    arr[index] = val;
}

void set64(void **arr, int index, void *val)
{
    if (!arr)
        throw runtime_error("null pointer exception");
    int arr_l = arr_len(arr);
    if (index < 0 || index >= arr_l) {
        printf("%d\n", arr_l);
        throw runtime_error("index out of bounds set64");
    }
    arr[index] = val;
}

void set8(bool *arr, int index, bool val)
{
    if (!arr)
        throw runtime_error("null pointer exception");
    int arr_l = arr_len(arr);
    if (index < 0 || index >= arr_l)
        throw runtime_error("index out of bounds set8");
    arr[index] = val;
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
