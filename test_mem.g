
int[] gl_arr = new int[100];
int[][] global_arr_int_2d = new int[5][];
string[] rules = new string[3];
string[][] global_arr_str_2d = new string[10][];
string null_str = null;
int[] global_1 = new int[10];

void assign_rules() {
    rules[0] = "bc";
    rules[1] = "a";
    rules[2] = "aaa";
}

void assign_2d_arr(int[][] arr, int[] asgn) {
  for (int i=0;i<arr.Length;i++)
    arr[i] = asgn;
}

void assign_2d_arr(string[][] arr, string[] asgn) {
  for (int i=0;i<arr.Length;i++)
    arr[i] = asgn;
}

// function calls
int[] a = new int[10];
int[] a2 = new int[10];

int foo() {
    a = null;
    return 0;
}

int fooo() {
    a2 = null;
    return 7;
}

int bar(int[] b, int c) {
    return b[0] + c;
}

int[][] arr = new int[10][];


int first(int[][] arr) {
   return arr[0][second()];
}

int second() {
    print("Assigning null");
    arr = null;
    print("Null assigned");
    int x = 1 + 1;
    return 7;
}

// 1

string s1 = "x";

string foo1() {
    s1 = null;
    return "ef";
}

// 2
string s2 = "x";

int foo2() {
    s2 = null;
    return 0;
}



// 3

string s3 = "x";

string foo3() {
    s3 = null;
    return "abcd";
}


string ret_substr() { return "Hello".Substring(0); }
string ret_var() { string var = "Hello"; return var; }
//


string retstrlit() { return "Hello"; }
string retstrvar() { 
    string var = "Hello".Substring(0); 
    print("here");
    return var; }
string retstrelemacess() {
    string[] arr1 = new string[10];
    arr1[0] = "test";
    return arr1[0];
}
string retstrop() { return "Hello" + "wolrd"; }
string retstrsub() { return "Hello".Substring(0);}

int[] retnewarr() { return new int[10]; }
int[] retarrvar() { 
    int[] arrret = new int[10]; 
    return arrret;
}
int[] retarrelem(){ 
    int[][] arr_two_dim = new int[10][];
    arr_two_dim[0] = new int[10];
    return arr_two_dim[0];
}

string[] retstrarr() { return new string[10];}

string[] retstrarrvar() { 
    string[] arrret = new string[10]; 
    return arrret;
}
string[] retstrarrelem(){ 
    string[][] arr_two_dim = new string[10][];
    arr_two_dim[0] = new string[10];
    return arr_two_dim[0];
}

int foo_global_1() { global_1 = null; return 1; }

int main(string[] argv) {

    s1 = "ab" + "cd";
    string t1 = s1 + foo1();
    print(t1);

    s2 = "ab" + "cd";
    string t2 = s2.Substring(foo2(), 2);
    print(t2);

    s3 = "ab" + "cd";
    if (s3 == foo3())
        print("equal");
    else
        print("not equal");

    int dim = 10;
    a[0] = 100;
   
    print(a[foo()]);

    a2[0] = 100;

    print(bar(a2, fooo()));

    arr[0] = new int[10];
    arr[0][7] = 99;
    print(first(arr));



    // Int
    int[][][] int_arr_3d = new int[dim][][];
    for (int i=0;i<dim;i++) {
        int_arr_3d[i] = new int[dim][];
        for (int ii=0;ii<dim;ii++) {
            int_arr_3d[i][ii] = new int[dim];
            for (int iii=0;iii<dim;iii++) {
                int_arr_3d[i][ii][iii] = 123;
            }
        }
    }

    // Bool
    bool[][][] bool_arr_3d = new bool[dim][][];
    for (int i=0;i<dim;i++) {
        bool_arr_3d[i] = new bool[dim][];
        for (int ii=0;ii<dim;ii++) {
            bool_arr_3d[i][ii] = new bool[dim];
            for (int iii=0;iii<dim;iii++) {
                bool_arr_3d[i][ii][iii] = true;
            }
        }
    }


    // Char
    char[][][] char_arr_3d = new char[dim][][];
    for (int i=0;i<dim;i++) {
        char_arr_3d[i] = new char[dim][];
        for (int ii=0;ii<dim;ii++) {
            char_arr_3d[i][ii] = new char[dim];
            for (int iii=0;iii<dim;iii++) {
                char_arr_3d[i][ii][iii] = 'a';
            }
        }
    }


    // String

    string[][][] string_arr_3d = new string[dim][][];
    for (int i=0;i<dim;i++) {
        string_arr_3d[i] = new string[dim][];
        for (int ii=0;ii<dim;ii++) {
            string_arr_3d[i][ii] = new string[dim];
            for (int iii=0;iii<dim;iii++) {
                string_arr_3d[i][ii][iii] = "Hello";
            }
        }
    }

    for (int i=0;i<dim;i++) {
        string_arr_3d[i] = new string[dim][];
        for (int ii=0;ii<dim;ii++) {
            string_arr_3d[i][ii] = new string[dim];
            for (int iii=0;iii<dim;iii++) {
                string_arr_3d[i][ii][iii] = "Hello";
            }
        }
    }

    string s = "Hello world!";
    string t = s.Substring(0);

    { 
        t = s.Substring(1);
        {
            t = t.Substring(2, 4);
        }
    }

    for (int j=0;j<s.Length;j++) {
        print(s.Substring(j, s.Length-j));
    }

    {
        string test = s.Substring(0);
    }


    {
        string test = s.Substring(0);
    }


    {
        string test = s.Substring(0);
    }

    string tes = null;
    tes = "Wow";

    assign_2d_arr(global_arr_int_2d, new int[10]);
    assign_rules();
    assign_2d_arr(global_arr_str_2d, string_arr_3d[0][0]);

    string str_1  = "";
    for (int i=0;i<10;i++)
        str_1 = str_1 + i;
    for (int i=0;i<10;i++) {
        str_1 = str_1 + i;
    }

    string str_2 = ret_substr();

    string ss1 = retstrlit();
    string ss2 = retstrvar();
    string ss3 = retstrelemacess();
    string ss4 = retstrop();
    string ss5 = retstrsub();
    retstrlit();
    retstrvar();
    retstrelemacess();
    retstrop();
    retstrsub();

    int[] arr1 = retnewarr();
    int[] arr2 = retarrvar();
    int[] arr3 = retarrelem();
    retnewarr();
    retarrvar();
    retarrelem();

    string[] strarr1 = retstrarr();
    string[] strarr2 = retstrarrvar();
    string[] strarr3 = retstrarrelem();
    retstrarr();
    retstrarrvar();
    retstrarrelem();
    

    global_1[foo_global_1()] = foo_global_1();
}