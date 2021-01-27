
using static System.Console;
class Prog {
    // Merges two subarrays of []arr.
    // First subarray is arr[l..m]
    // Second subarray is arr[m+1..r]
    static void merge(int[] arr, int l, int m, int r)
    {
        // Find sizes of two
        // subarrays to be merged
        int n1 = m - l + 1;
        int n2 = r - m;

        // Create temp arrays
        int[] L = new int[n1];
        int[] R = new int[n2];

        // Copy data to temp arrays
        for (int ii = 0; ii < n1; ii++)
            L[ii] = arr[l + ii];
        for (int jj = 0; jj < n2; jj++)
            R[jj] = arr[m + 1 + jj];

        // Merge the temp arrays

        // Initial indexes of first
        // and second subarrays
        int i = 0;
        int j = 0;

        // Initial index of merged
        // subarry array
        int k = l;
        while (i < n1 && j < n2) {
            if (L[i] <= R[j]) {
                arr[k] = L[i];
                i++;
            }
            else {
                arr[k] = R[j];
                j++;
            }
            k++;
        }

        // Copy remaining elements
        // of L[] if any
        while (i < n1) {
            arr[k] = L[i];
            i++;
            k++;
        }

        // Copy remaining elements
        // of R[] if any
        while (j < n2) {
            arr[k] = R[j];
            j++;
            k++;
        }
    }

    // Main function that
    // sorts arr[l..r] using
    // merge()
    static void sort(int[] arr, int l, int r)
    {
        if (l < r) {
            // Find the middle
            // point
            int m = (l + r) / 2;

            // Sort first and
            // second halves
            sort(arr, l, m);
            sort(arr, m + 1, r);

            // Merge the sorted halves
            merge(arr, l, m, r);
        }
    }

    // A utility function to
    // print array of size n */
    static void printArray(int[] arr)
    {
        int n = arr.Length;
        for (int i = 0; i < n; i++)
            WriteLine(arr[i]);
    }

    static void Main(string[] args)
    {
        int count = int.Parse(args[0]);
        int[] arr = new int[count];

        // fill array with pseudo-random numbers
        int rand = int.Parse(args[1]);

        for (int i = 0 ; i < count ; i = i + 1) {
            rand = (rand * 1103515 + 12345) % 2147483;
            arr[i] = rand % 1000;
        }
        // WriteLine("Given Array");
        // printArray(arr);
        sort(arr, 0, arr.Length - 1);
        // WriteLine("Sorted array");
        // printArray(arr);
    }
}