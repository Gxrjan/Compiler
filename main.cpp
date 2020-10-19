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
    ofstream file{ argc == 1 ? "a.ll" : (file_name+".ll").c_str() , ios::out };
    Scanner scan(argc == 2 ? argv[1] : nullptr);
    Translator_LLVM tran;
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
    string cmd = "clang++ -Wno-override-module -O2 "+file_name+".ll G_runtime_library.o -o "+program;
    system(cmd.c_str());
}
