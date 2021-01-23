Each test in this file is a separate program whose compilation should fail.
Tests are separated by one or more blank lines.
void main() {
int x = "hello";
}

void main() {
string s = "yoyo";
s = 77;
}

void main() {
for (...)
  print(1);
}


void main() {
print(hello);
}

void main() {
char c = 97;
}

void main() {
while (1)
print(1);
}

void main() {
bool b = 12 == "hello";
}

void main() {
int[][] arr = new int[][];
}

void main() {
int[][] arr = new int[10][];
arr[1] = new string[10];
}

void main() {
string[] arr = new string[10];
arr[0]++;
}

void main() {
char[] arr = new char[10];
arr[0]++;
}

void main() {
bool[] arr = new bool[10];
arr[0]++;
}

void main() {
bool b = true;
b++;
}

void main() {
char c = 'a';
c++;
}

void main() {
int x = 1;
x++++;
}

void main() {
int[] a = new int[10];
a++;  // should be an error: you can't increment an int[]
}

void main() {
for (int i=0;i<5;int n = 1)
  print(2);
}

void main() {
print(1, 2, 3);
}

void main() {
print(1, "Hello");
}

void main() {
int x = int.Parse("123")++;
}

void main() {
int x = (2 + 2)++;
}

void test_void(void a) {}
void main() {
  test_void();
}

char test() { return 1; }
void main() {
  test();
}

int[] test() { return 0; }
int main() { 
  test();
}