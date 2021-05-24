Dependencies: LLVM 11.1.9, Clang 11.1.0, .NET 5.0.202, Mono 6.12.0, Python 3.9.4
The G compiler was build with these versions, however it might possibly work with earlier versions as well but that's untested.  
To build the compiler: `make`

To install: `make install`

To uninstall: `make uninstall`

After installation, invoke the compiler: `g_compiler ...`
 
Or you can invoke the compiler from the root of the repository: `./gc ...`

To build and run the benchmarks: `make benchmark`

To run positive and negative tests: `make test`

To run memory tests: `make mem_leak`
