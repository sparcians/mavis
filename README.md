# Mavis

Mavis is a framework that allows decoding of the RISC-V ISA into
custom instruction class types as well as custom extensions to those
class types.

## Run regression

Mavis is a header-only library. Regression needs to build a tester.

* Build library, output will be `libmavis.a`
```
mkdir build
cd build
cmake ..
make -j8
```
* Build the basic tester; output program is `Mavis`
```
cd build/test/basic
make -j8
```
* Execute the basic tester and compare golden `out`
```
./Mavis > test.out
diff -s test.out golden.out
```

* Build and test with a directed program; output program is
  `mavis_decode`.  This program allows a user to test the validity of
  an opcode or a mnemonic.  This program requires boost program
  options to be installed on the host machine.

```
cd build/test/directed
make
./mavis_decode --help
./mavis_decode -a rv32 -o 0x0001a283
./mavis_decode -a rv64 -o 0x0001b283
./mavis_decode -a rv32 -z -o 0x6008

```
