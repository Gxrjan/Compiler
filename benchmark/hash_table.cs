using static System.Console;


class Prog {
    static int rand = 0;

    static int random(int seed) {
        return seed * 1103515 + 12345;
    }

    static string random_string(int len) {
        string result = "";
        for (int i=0;i<len;i++) {
            rand = random(rand);
            int next = rand % 95;
            if (next < 0)
                next = next * -1;
            next = next + 33;
            char ch = (char)next;
            result = result + ch;
        }
        return result;
    }

    static int hash_(string s) {
        int hash = 7;
        for (int i = 0; i < s.Length; i++) {
            hash = hash*31 + s[i];
        }
        return hash;
    }

    static void insert(string[] table, string s) {
        int hash = hash_(s);
        if (hash < 0)
            hash = hash * -1;
        hash = hash % table.Length;
        while (table[hash] != null) {
            hash = (hash + 1) % table.Length;
        }
        table[hash] = s;
    }

    static bool contains(string[] table, string s) {
        int hash = hash_(s);
        if (hash < 0)
            hash = hash * -1;
        hash = hash % table.Length;
        int count = 0;
        while (count < table.Length) {
            //print("Comparing " + table[hash] + " and "+s);
            if (table[hash] == null)
                return false;
            if (table[hash] == s)
                return true;
            hash = (hash+1) % table.Length;
            count++;

        }
        return false;
    }


    public static void Main(string[] argv) {
        rand = int.Parse(argv[1]);
        int N = int.Parse(argv[0]);
        string[] table = new string[2*N];
        for (int i=0;i<N;i++) {
            rand = random(rand);
            string s = random_string(7);
            //print("Adding "+s);
            insert(table, s);
        }


        int count = 0;

        for (int i=0;i<N;i++) {
            rand = random(rand);
            string s = random_string(7);
            //print("Checking "+s);
            if (contains(table, s))
                count++;
        }
        WriteLine(count);
                
    }

}