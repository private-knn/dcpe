# DCPE Scheme

This is an implementation of the DCPE algorithm.
This implementation has the following features:
- it's written in C++ and is compilable into a standalone shared library (see [usage example](./scheme/test/test-shared-lib.cpp))
- PRG and encryption are done with OpenSSL, encryption is AES-CBC-256 (or AES-CTR-256), random IV every time # TODO
- the solution is tested, the coverage is 100% # TODO
- the solution is benchmarked
- the solution is documented, the documentation is [online](https://dcpe.dbogatov.org/)
- Makefile is sophisticated - with simple commands one can compile and run tests and benchmarks
- the code is formatted according to [clang file](./.clang-format)
- there are VS Code configs that let one debug the code with breakpoints

## How to compile and run

Dependencies:
- for building a shared library
	- `g++` that supports `--std=c++17`
	- `make`
	- these libs `-l boost_system -l ssl -l crypto`
- for testing and benchmarking
	- all of the above
	- these libs `-l gtest -l pthread -l benchmark`
	- `gcovr` for coverage
	- `doxygen` for generating docs
- or, use this Docker image: `dbogatov/docker-images:pbc-latest`

[Makefile](./scheme/Makefile) is used for everything.

```bash
# to only compile shared library
make clean shared

# to compile the shared library (libdcpe.so in ./bin) and run example code against it
make clean run-shared

# to compile and run all unit tests
make clean run-tests

# to compile and run all benchmarks
make clean run-benchmarks

# to compute unit test coverage
make clean coverage

# to generate documentation
make clean docs
```

### Notes for M1 mac

Use this to compile libraries from source (e.g. `gtest`, google `benchmark`)

```bash
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/homebrew/ -DCMAKE_CXX_COMPILER=/opt/homebrew/bin/g++
```
