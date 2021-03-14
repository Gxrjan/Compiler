#include <iostream>
#include <string>

int SieveOfEratosthenes(int n) {
    // Create a boolean array 
    // "prime[0..n]" and
    // initialize all entries
    // it as true. A value in
    // prime[i] will finally be 
    // false if i is Not a
    // prime, else true.
    int result = 0;

    bool *prime = new bool[n + 1];

    for (int i = 0; i < n; i++)
        prime[i] = true;

    for (int p = 2; p * p <= n; p++) {
        // If prime[p] is not changed,
        // then it is a prime
        if (prime[p] == true) {
            // Update all multiples of p
            for (int i = p * p; i <= n; i = i+p)
                prime[i] = false;
        }
    }

    // Print all prime numbers
    for (int i = 2; i <= n; i++) {
        if (prime[i] == true)
            result++;
    }
    delete[] prime;
    return result;
}


int main(int argc, char *argv[]) {
    int n = std::stoi(argv[1]);
    std::cout << SieveOfEratosthenes(n) << std::endl;
}