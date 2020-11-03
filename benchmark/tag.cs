using static System.Console;

class Tag {
    static string[] rules = { "bc", "a", "aaa" };
    
    static int iter(string s) {
        int count = 0;
        
        while (s != "a") {
            s = s.Substring(2) + rules[s[0] - 'a'];
            count += 1;
        }
        return count;
    }
    
    static void Main(string[] args) {
        int n = int.Parse(args[0]);
        int total = 0;
        for (int i = 1 ; i <= n ; ++i)
            total += iter(new string('a', i));
        WriteLine("the total is " + total);
    }
}
