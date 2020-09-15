#include "head.h"


int main(int argc, char *argv[])
{
    if (argc > 2) {
        cerr << "Usage: gc [file_path]" << endl;
        return -1;
    }
    ofstream file{ "a.asm", ios::out };
    Scanner scan;
    if (argc == 2)
        scan = Scanner(argv[1]);
    int len = strlen(argv[1]);
    Translator tran;
    unique_ptr<Program> prog = Parser(&scan).parse_program();
    Checker c;
    c.check_program(prog.get());
    string asm_code = tran.translate_program(prog.get());
    file << asm_code;
    file.close();
    system("make prog");
    if (argc == 2) {
        argv[1][len-2] = 0;
        char cmd[256];
        strcat(cmd, "mv prog ");
        strcat(cmd, argv[1]);
        system(cmd);
    }
}
