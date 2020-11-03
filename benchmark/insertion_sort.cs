using static System.Console;

class Sort {
    public static void Main(string[] args)
    {
        int count = int.Parse(args[0]);
        int[] a = new int[count];

        // fill array with pseudo-random numbers
        int rand = int.Parse(args[1]);

        for (int i = 0 ; i < count ; i = i + 1) {
            rand = (rand * 1103515 + 12345) % 2147483;
            a[i] = rand % 1000;
        }

        WriteLine("before sorting:");
        for (int i = 0 ; i < count ; i = i + 1)
            WriteLine(a[i]);


        // insertion sort
        for (int i = 1 ; i < count ; i = i + 1) {
            int t = a[i];
            int j = i;
            while (j > 0 && a[j - 1] > t) {
                a[j] = a[j - 1];
                j = j - 1;
            }
            a[j] = t;
        }


        WriteLine("after sorting:");
        for (int i = 0 ; i < count ; i = i + 1)
            WriteLine(a[i]);
    }
}