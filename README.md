# Mavis

Mavis is a framework that allows decoding of the RISC-V ISA into
custom instruction class types as well as custom extensions to those
class types.

## Run regression

Mavis is a header-only library. Regression needs to build a tester.

* Build library for tester, output will be `libmavis.a`
```
cmake .
make -j8
```
* Build tester, output will be `Mavis`
```
cmake .
make -j8
```
* Build tester, output will be `Mavis`
```
mkdir build
cd build
cmake ..
cd test
make
```
* Execute the tester and compare golden `out`
```
./Mavis > test.out
diff -s test.out golden.out
```
