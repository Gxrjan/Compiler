
int rand = 0;

int random(int seed) {
    return seed * 1103515 + 12345;
}

string random_string(int len) {
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

int hash(string s) {
    int hash = 7;
    for (int i = 0; i < s.Length; i++) {
        hash = hash*31 + s[i];
    }
    return hash;
}

void insert(string[] table, string s) {
    int hash = hash(s);
    if (hash < 0)
        hash = hash * -1;
    hash = hash % table.Length;
    while (table[hash] != null) {
        hash = (hash + 1) % table.Length;
    }
    table[hash] = s;
}

bool contains(string[] table, string s) {
    int hash = hash(s);
    if (hash < 0)
        hash = hash * -1;
    hash = hash % table.Length;
    int count = 0;
    while (count < table.Length) {
        if (table[hash] == null)
            return false;
        //print("Comparing " + table[hash] + " and "+s);
        if (table[hash] == s)
            return true;
        hash = (hash+1) % table.Length;
        count++;

    }
    return false;
}


void main(int argc, string[] argv) {
    rand = int.Parse(argv[2]);
    int N = int.Parse(argv[1]);
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
    print(count);
            
}