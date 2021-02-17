#include <string>
#include <iostream>
#include <cstring>
using namespace std;

// String permutations
int main(int argc, char *argv[]) {
    // Print initial string, as only the alterations will be printed later
    u16string str;
    for (int i=0;i<strlen(argv[1]);i++)
        str += argv[1][i];
    int count = 1;
    char16_t *a = new char16_t[str.size()];
    for (int i=0;i<str.size();i++)
        a[i] = argv[1][i];
    int a_len = str.size();
    int *p = new int[a_len];  // Weight index control array initially all zeros. Of course, same size of the char array.
    int i = 1;            //Upper bound index. i.e: if string is "abc" then index i could be at "c"
    while (i < a_len) {
        if (p[i] < i) { //if the weight index is bigger or the same it means that we have already switched between these i,j (one iteration before).
            int j = 0;
            if (i%2!=0)
            j = p[i]; //Lower bound index. i.e: if string is "abc" then j index will always be 0.

            char16_t temp = a[i];
            a[i] = a[j];
            a[j] = temp;

            u16string combine;
            
            for (int k=0;k<str.size();k++)
                combine = combine + a[k];
            // Print current
            u16string result = combine;
            count++;
            // cout << result << endl;
            p[i]++; //Adding 1 to the specific weight that relates to the char array.
            i = 1; //if i was 2 (for example), after the swap we now need to swap for i=1

        }
        else { 
            p[i] = 0;//Weight index will be zero because one iteration before, it was 1 (for example) to indicate that char array a[i] swapped.
            i++;//i index will have the option to go forward in the char array for "longer swaps"
        }
    }
    delete[] a;
    delete[] p;
    cout << "Number of permutations: " << count << endl;
}