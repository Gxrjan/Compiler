#include <iostream>
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

}
