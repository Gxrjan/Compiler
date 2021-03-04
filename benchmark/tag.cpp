#include <string>
#include <iostream>
using namespace std;

static u16string rules[] = { u"bc", u"a", u"aaa"};


static int iter(u16string s) {
    int count = 0;
    
    while (s != u16string(u"a")) {
        s = s.substr(2) + rules[s[0] - u'a'];
        count += 1;
    }
    return count;
}

int main(int argc, char *argv[]) {
    int n = stoi(argv[1]);
    int total = 0;
    for (int i = 1 ; i <= n ; ++i)
        total += iter(u16string(i , u'a'));
    cout << "the total is: " << total << endl;
}