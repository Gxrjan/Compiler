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


// Substring

print("abcdef".Substring(1)); // expect: bcdef
print("abcdef".Substring(2, 4)); // expect: cdef



