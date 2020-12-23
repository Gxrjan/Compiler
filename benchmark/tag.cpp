#include <string>
#include <iostream>
using namespace std;

static string rules[] = { "bc", "a", "aaa"};


static int iter(string s) {
    int count = 0;
    
    while (s != "a") {
        s = s.substr(2) + rules[s[0] - 'a'];
        count += 1;
    }
    return count;
}

int main(int argc, char *argv[]) {
    int n = stoi(argv[1]);
    int total = 0;
    for (int i = 1 ; i <= n ; ++i)
        total += iter(string(i , 'a'));
    cout << "the total is: " << total << endl;
}