Each test in this file is a separate program whose compilation should fail.
Tests are separated by one or more blank lines.

int x = "hello";

string s = "yoyo";
s = 77;


for (...)
  print(1);


print(hello);


char c = 97;


while (1)
print(1);


bool b = 12 == "hello";

int[][] arr = new int[][];

int[][] arr = new int[10][];
arr[1] = new string[10];


string[] arr = new string[10];
arr[0]++;

char[] arr = new char[10];
arr[0]++;

bool[] arr = new bool[10];
arr[0]++;

bool b = true;
b++;

char c = 'a';
c++;

int x = 1;
x++++;

int[] a = new int[10];
a++;  // should be an error: you can't increment an int[]

for (int i=0;i<5;int n = 1)
  print(2);

for (int i=0;i<5;print(2))
  print(2);

int[] arr = new int[10];
print(arr[10]);
print(arr[0]);


int x = int.Parse("123")++;

int x = (2 + 2)++;