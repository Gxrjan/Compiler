void main(int argc, string[] args) {
    int[] arr = new int[1];
    print(arr[1]);
}



int[] ret_arr() { return new int[10]; }
void main(int argc, string[] args) { 
    int[] arr = ret_arr();
    print(arr[20]);
}



int[] ret_arr() { return new int[10]; }
void main(int argc, string[] args) { 
    print((ret_arr())[20]);
}


void main(int argc, string[] args) { 
    print((new int[10])[20]);
}


int[] global_arr = new int[10];
void main(int argc, string[] args) { 
    print(global_arr[20]);
}



int[] global_arr = new int[10];
void foo() { print(global_arr[10]); }
void main(int argc, string[] args) { 
    foo();
}


int[] global_arr = new int[10];
void main(int argc, string[] args) { 
    print(global_arr[-1]);
}