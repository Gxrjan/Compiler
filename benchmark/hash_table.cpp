#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
using namespace std;

int rand_seed = 0;

int randomf(int seed) {
    return seed * 1103515 + 12345;
}

u16string random_string(int len) {
    u16string result;
    for (int i=0;i<len;i++) {
        rand_seed = randomf(rand_seed);
        int next = rand_seed % 95;
        if (next < 0)
            next = next * -1;
        next = next + 33;
        char ch = (char)next;
        result += ch;
    }
    return result;
}

int hashf(u16string s) {
    int hash = 7;
    for (int i = 0; i < s.size(); i++) {
        hash = hash*31 + s[i];
    }
    return hash;
}

void insert(u16string table[], int len, u16string s) {
    int hash_ = hashf(s);
    if (hash_ < 0)
        hash_ = hash_ * -1;
    hash_ = hash_ % len;
    while (!table[hash_].empty()) {
        hash_ = (hash_ + 1) % len;
    }
    table[hash_] = s;
}

bool contains(u16string table[], int len, u16string s) {
    int hash_ = hashf(s);
    if (hash_ < 0)
        hash_ = hash_ * -1;
    hash_ = hash_ % len;
    int count = 0;
    while (count < len) {
        //print("Comparing " + table[hash] + " and "+s);
        if (table[hash_] == s)
            return true;
        hash_ = (hash_+1) % len;
        count++;

    }
    return false;
}


int main(int argc, char *argv[]) {
    rand_seed = stoi(argv[2]);
    int N = stoi(argv[1]);
    u16string table[N];
    for (int i=0;i<N;i++) {
        rand_seed = randomf(rand_seed);
        u16string s = random_string(7);
        //print("Adding "+s);
        insert(table, N, s);
    }


    int count = 0;

    for (int i=0;i<N;i++) {
        rand_seed = randomf(rand_seed);
        u16string s = random_string(7);
        //print("Checking "+s);
        if (contains(table, N, s))
            count++;
    }
    cout << count << endl;

            
}