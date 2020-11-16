// Use a tag system to compute Collatz sequences.

int n = int.Parse(argv[1]);

string[] rules = new string[3];
rules[0] = "bc";
rules[1] = "a";
rules[2] = "aaa";
    
int count = 0;
for (int i = 1 ; i <= n ; i = i + 1) {
    string s = new string('a', i);
    while (s != "a") {
        s = s.Substring(2) + rules[s[0] - 'a'];
        count = count + 1;
    }
}
print("the total is: " + count);
