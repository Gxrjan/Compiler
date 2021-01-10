int dim = 10;


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