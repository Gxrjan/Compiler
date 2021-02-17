#include "head.h"


int main(int argc, char *argv[])
{
    int c;
    int bounds = 1;
    int ref = 1;
    int free = 1;
    string infile, outfile;

    while (1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"nobounds",     no_argument, &bounds,  0 },
            {"noref",  no_argument,       &ref,  0 },
            {"nofree",  no_argument, &free,  0 },
            {"help", no_argument,       0,  'h' },
            {0,         0,                 0,  0 }
        };

        c = getopt_long(argc, argv, "o:h",
                long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            case 0:
                break;
            case 'o':
                outfile = string(optarg);
                break;

            case 'h':
                cout << "Usage: " << argv[0] << " [--nobounds] [--noref] [--nofree] [-o outfile] infile" << endl;
                exit(EXIT_SUCCESS);
                break;

            default:
                cerr << "Usage: " << argv[0] <<" [--nobounds] [--noref] [--nofree] [-o outfile] infile\n";
                exit(EXIT_FAILURE);
        }
    }

    if (optind != argc-1) {
        cerr << "Usage: " << argv[0] <<" [--nobounds] [--noref] [--nofree] [-o outfile] infile\n";
        exit(EXIT_FAILURE);
    } else {
        infile = string(argv[optind]);
        size_t pos;
        if ((pos=infile.rfind(".g"))==string::npos ||
            infile.length() - pos != 2) {
            cerr << "infile should have g extension" << endl;
            exit(EXIT_FAILURE);
        }
        infile.resize(pos);
        if (!outfile.size())
            outfile = infile;
    }

    ofstream file{ (infile+".ll").c_str() , ios::out };
    Scanner scan(argv[optind]);
    Translator_LLVM tran{bounds, ref, free};
    unique_ptr<Program> prog = Parser(&scan).parse_program();
    Checker checker;
    checker.check_program(prog.get());
    string asm_code = tran.translate_program(prog.get());
    file << asm_code;
    file.close();
    string cmd = "clang++ -Wno-override-module -O2 "+infile+".ll G_runtime_library.o Types.o -o "+outfile;
    system(cmd.c_str());
}
