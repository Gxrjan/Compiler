
// Merges two subarrays of []arr.
// First subarray is arr[l..m]
// Second subarray is arr[m+1..r]
void merge(int[] arr, int l, int m, int r)
{
    // Find sizes of two
    // subarrays to be merged
    int n1 = m - l + 1;
    int n2 = r - m;

    // Create temp arrays
    int[] L = new int[n1];
    int[] R = new int[n2];

    // Copy data to temp arrays
    for (int i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

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
void sort(int[] arr, int l, int r)
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
void printArray(int[] arr)
{
    int n = arr.Length;
    for (int i = 0; i < n; i++)
        print(arr[i]);
}

void main(string[] args)
{
    int count = int.Parse(args[1]);
    int[] arr = new int[count];

    // fill array with pseudo-random numbers
    int rand = int.Parse(args[2]);

    for (int i = 0 ; i < count ; i = i + 1) {
        rand = (rand * 1103515 + 12345) % 2147483;
        arr[i] = rand % 1000;
    }
    // print("Given Array");
    // printArray(arr);
    sort(arr, 0, arr.Length - 1);
    // print("Sorted array");
    // printArray(arr);
}