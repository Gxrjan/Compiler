
string[] rules = new string[3];
void assign_rules() {
    rules[0] = "bc";
    rules[1] = "a";
    rules[2] = "aaa";
}
int iter(string s) {
    int count = 0;
    
    while (s != "a") {
        s = s.Substring(2) + rules[s[0] - 'a'];
        count = count + 1;
    }
    return count;
}

void main(int argc, string[] argv) {
    // Use a tag system to compute Collatz sequences.
    assign_rules();
    int n = int.Parse(argv[1]);
    int total = 0;
    for (int i = 1 ; i <= n ; i++)
        total = total +  iter(new string('a', i));
    print("the total is: " + total);
}