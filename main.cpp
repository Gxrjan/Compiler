#include "head.h"


int main(int argc, char *argv[])
{
    string file_name;
    string program;
    if (argc > 2) {
        cerr << "Usage: gc [file_path]" << endl;
        return -1;
    }
    if (argc == 2) {
        file_name = string(argv[1]);
        size_t pos;
        if ((pos=file_name.rfind(".g"))==string::npos ||
            file_name.length() - pos != 2) {
            cerr << "file should have g extension" << endl;
            return -1;
        }
        file_name.resize(pos);
        program = file_name;
    }
    ofstream file{ argc == 1 ? "a.asm" : (file_name+".asm").c_str() , ios::out };
    Scanner scan(argc == 2 ? argv[1] : nullptr);
    Translator tran;
    unique_ptr<Program> prog = Parser(&scan).parse_program();
    Checker c;
    c.check_program(prog.get());
    string asm_code = tran.translate_program(prog.get());
    file << asm_code;
    file.close();
    if (argc == 1) {
        file_name = "a";
        program = "prog";
    } 
    string asm_cmd = "nasm -Werror -f elf64 -g -F dwarf "+file_name+".asm -l "+file_name+".lst";
    string gcc_cmd = "gcc -o "+program+" "+file_name+".o -no-pie";
    system(asm_cmd.c_str());
    system(gcc_cmd.c_str());
}
