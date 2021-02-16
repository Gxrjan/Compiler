int test(int[] arr) {  
  print("Entered");
  arr[0] = 99;
  print("Assigned");
  return arr[0]; 
  
}

void test_ret(int[] a) {
  return;
  print("After return");
}
int fact(int n) {
  if (n==0)
    return 1;
  return n * fact(n-1);
}
int another(int m) {
  int n = 0;
  return 0;
}
char ret_char() { return 'a';}
bool ret_bool() { return true; }
string ret_str() { return "hello world"; } 

int str_len(string s) {
  return s.Length;
}


int[] ret_arr() { return new int[10]; }


// Function overloading
void print_arr(int[] arr) {
  for (int i=0;i<arr.Length;i++)
    print(arr[i]);
}

void print_arr(string[] arr) {
  for (int i=0;i<arr.Length;i++)
    print(arr[i]);
}
void assign_arr(int[] arr, int value) {
  for (int i=0;i<arr.Length;i++)
    arr[i] = value;
}

void assign_arr(string[] arr, string s) {
  for (int i=0;i<arr.Length;i++)
    arr[i] = s;
}

bool flip_bool(bool b) {
  if (b)
    return false;
  else
    return true;
}




// Globals
int global_int = 12;
bool global_bool = false;
string global_string = "This string is global";
int[] global_arr_int = new int[5];
string[] global_arr_str = new string[5];
int[][] global_arr_int_2d = new int[5][];

void assign_2d_arr(int[][] arr, int[] asgn) {
  for (int i=0;i<arr.Length;i++)
    arr[i] = asgn;
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

// rest

int[] global_a = new int[10];
int[] global_a2 = new int[10];

int foo() {
    global_a = null;
    return 0;
}

int fooo() {
    global_a2 = null;
    return 7;
}

int bar(int[] b, int c) {
    return b[0] + c;
}

int[][] global_arr = new int[10][];


int first(int[][] arr) {
   return global_arr[0][second()];
}

int second() {
    global_arr = null;
    int x = 1 + 1;
    return 7;
}

//



void main() { 
  // test operators
  print(2 + 2);  // expect: 4

  print(10 - 5); // expect: 5

  print(3 * 3); // expect: 9

  print(10 / 2); // expect: 5

  // test if

  if (5 > 2)
    print(10); // expect: 10
  else
    print(5);

  if (2 > 5)
    print(10);
  else
    print(5);  // expect: 5


  // bool operators
  if (true && true)
    print(20); // expect: 20
  else
    print(10);

  if (true || false)
    print(30); // expect: 30
  else
    print(1);


  // %

  print(129 % 13); // expect: 12


  // conditional operators

  if (2==2)
    print(2); // expect: 2
  else
    print(1);

  if (0 != 1)
    print(0); // expect: 0
  else
    print(-1);

  if (10 >= 10)
    print(10); // expect: 10
  else
    print(29);

  if (10 > 10)
    print(99);
  else
    print(100); // expect: 100

  if (10 <= 10)
    print(10); // expect: 10
  else
    print(99);

  if (3 < 2)
    print(3);
  else
    print(2); // expect: 2


  // declarations

  int n = 10;
  char c = 'a';
  string s = "Hello world!";
  string t = "foo";
  bool b = true;
  bool d = false;
  int x = 'a';

  // assignments
  n = 1;
  c = 'b';
  s = "Bye";
  b = false;
  d = true;


  // while

  while (n < 5) {
    print(n);
    n = n + 1;
  }
  // expect: 1
  // expect: 2
  // expect: 3
  // expect: 4


  // for

  for (int j=5;j>-1;j=j-1)
    print(j);
  // expect: 5
  // expect: 4
  // expect: 3
  // expect: 2
  // expect: 1
  // expect: 0


  // break

  while (n < 1000) {
    n = n + 1;
    if (n == 500)
      break;
  }
  print(n); // expect: 500


  // [] operator

  print(s[0]); // expect: B


  // concat

  s = "Hello ";
  t = "world!";
  print(s + t); // expect: Hello world!
  print("Hell" + 'o'); // expect: Hello
  print('A' + "loha"); // expect: Aloha
  print("number: " + 12); // expect: number: 12
  print(300 + " spartans"); // expect: 300 spartans


  // Length call

  print(s.Length); // expect: 6
  print("abc".Length); // expect: 3


  // Type cast

  print((int)'a'); // expect: 97
  print((char)97); // expect: a
  print((int)(char)(65536 + 65)); // expect: 65

  // Substring

  print("abcdef".Substring(1)); // expect: bcdef
  print("abcdef".Substring(2, 4)); // expect: cdef

  print("xyzabc".Substring((char) 0, (char) 3)); // expect: xyz


  // int.Parse

  print(int.Parse("15123")); // expect: 15123
  print(int.Parse("-1")); // expect: -1

  // new String()

  print(new string('a', 3)); // expect: aaa

  // null

  string null_string = null;
  print(null_string + 11); // expect: 11
  print(null_string + 'a'); // expect: a
  print(null_string + "hell"); // expect: hell

  // arrays

  int[] arr = null;
  arr = new int[1];
  string [][][] str_arr = new string[100][][];
  bool[][] b_arr = new bool[100][];
  char[] c_arr = new char[10];

  // arr.Length

  print(b_arr.Length); // expect: 100


  // array types 

  int[][][] arr_3d = new int[100][][];
  arr_3d[1] = new int[200][];
  arr_3d[1][2] = new int[300];


  // assignment of multi dim arrays
  arr_3d[1][2][3] = 999;
  print(arr_3d[1][2][3]); // expect: 999

  bool[][][][] b_arr_4d = new bool[10][][][];
  b_arr_4d[0] = new bool[10][][];
  b_arr_4d[0][1] = new bool[10][];
  b_arr_4d[0][1][2] = new bool[10];
  b_arr_4d[0][1][2][3] = true;
  if (b_arr_4d[0][1][2][3])
    print("Yay");  // expect: Yay
  else
    print("Nay");


  str_arr[10] = new string[200][];
  str_arr[10][20] = new string[300];
  str_arr[10][20][30] = "Hello world";
  print(str_arr[10][20][30]); // expect: Hello world

  c_arr[9] = 'x';
  print(c_arr[9]); // expect: x

  print(str_arr[10][20].Length); // expect: 300


  int[] a = new int[100];
  int index = 3;
  a[index] = 99;
  print(a[index]); // expect: 99


  if ("hi" == "hi")
    print("same");  // expect: same
  else
    print("different");

  string new_s = null;
  string new_t = "null";

  if (new_s == new_t)
    print("same again");
  else
    print("different"); // expect: different

  if ("a" != "a")
    print("diff");
  else
    print("same"); // expect: same

  // inc expr

  x = -1;
  x++;
  print(x); // expect: 0
  int[][][] new_arr = new int[10][][];
  new_arr[0] = new int[10][];
  new_arr[0][1] = new int[10];
  new_arr[x++][x++][x++] = x++;
  print(new_arr[0][1][2]); // expect: 3

  x = 0;
  print(x++); // expect: 0
  for (int i=0;i<5;i++)
    print(i);
  // expect: 0
  // expect: 1
  // expect: 2
  // expect: 3
  // expect: 4

  n = 0;
  x = 0;
  n = 1 + x++;
  print(n); // expect: 1


  n = 0;
  x = 1;
  n = x++ * 20 - 10;
  print(n); // expect: 10


  arr = new int[10];
  x = 0;
  arr[x++]++;
  print(x); // expect: 1

  arr = new int[10];
  for (int i=0;i<arr.Length;i++)
    print(arr[i]++);
  // expect: 0
  // expect: 0
  // expect: 0
  // expect: 0
  // expect: 0
  // expect: 0
  // expect: 0
  // expect: 0
  // expect: 0
  // expect: 0
  for (int i=0;i<arr.Length;i++)
    print(arr[i]);
  // expect: 1
  // expect: 1
  // expect: 1
  // expect: 1
  // expect: 1
  // expect: 1
  // expect: 1
  // expect: 1
  // expect: 1
  // expect: 1

  for (int i = 0;arr[0]<arr.Length;arr[0]++)
    print(arr[0]);
  // expect: 1
  // expect: 2
  // expect: 3
  // expect: 4
  // expect: 5
  // expect: 6
  // expect: 7
  // expect: 8
  // expect: 9

  x = 1;
  x--;
  print(x); // expect: 0


  for (int i = 5;i>-1;i--)
    print(i);
  // expect: 5
  // expect: 4
  // expect: 3
  // expect: 2
  // expect: 1
  // expect: 0

  s = "abc";
  t = (s + "d").Substring(1);


  a = new int[5];
  x = (a)[3];  

  // Functions
  int[] func_arr = ret_arr();
  test(func_arr);
  // expect: Entered
  // expect: Assigned
  test_ret(func_arr);
  print(func_arr[0]); // expect: 99
  print(ret_arr()[0]); // expect: 0
  print(fact(5)); // expect: 120
  print(ret_char()); // expect: a
  if (ret_bool())
    print("yes"); // expect: yes
  else
    print("no");
  print(ret_str()); // expect: hello world
  print(str_len("Hello".Substring(0))); // expect: 5


  print_arr(global_arr_int);
  // expect: 0
  // expect: 0
  // expect: 0
  // expect: 0
  // expect: 0
  assign_arr(global_arr_int, 34);
  print_arr(global_arr_int);
  // expect: 34
  // expect: 34
  // expect: 34
  // expect: 34
  // expect: 34
  assign_arr(global_arr_str, global_string);
  print_arr(global_arr_str);
  // expect: This string is global
  // expect: This string is global
  // expect: This string is global
  // expect: This string is global
  // expect: This string is global
  if (global_bool)
    print("global bool doesn't work");
  else
    print("global bool works"); // expect: global bool works
  global_bool = flip_bool(global_bool);
  if (global_bool)
    print("global bool works"); // expect: global bool works
  else
    print("global bool doesn't work");
  assign_2d_arr(global_arr_int_2d, new int[10]);

  s1 = "ab" + "cd";
  string t1 = s1 + foo1();
  print(t1); // expect: abcdef

  s2 = "ab" + "cd";
  string t2 = s2.Substring(foo2(), 2);
  print(t2); // expect: ab

  s3 = "ab" + "cd";
  if (s3 == foo3())
      print("equal"); // expect: equal
  else
      print("not equal");

  int dim = 10;
  global_a[0] = 100;
  
  print(global_a[foo()]); // expect: 100

  global_a2[0] = 100;

  print(bar(global_a2, fooo())); // expect: 107

  global_arr[0] = new int[10];
  global_arr[0][7] = 99;
  print(first(global_arr)); // expect: 99

}