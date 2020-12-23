#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
    int prime_count = 0;
    int result = 0;
    int i = 2;
    int n = std::stoi(argv[1]);
    while (prime_count < n) {
        bool prime = true;
        int j = 2;
        while (j < i && prime) {
            if (i % j == 0)
                prime = false;
            j = j + 1;
        }
        if (prime) {
            prime_count = prime_count + 1;
            result = result + i;
        }
        i = i + 1;
    }
    std::cout << "Result: " << result << std::endl;
}