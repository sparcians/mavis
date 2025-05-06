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

| RVA23U64 Mandatory Extensions | Status |
| :---------------------------- | :----: |
| **M** Integer multiplication and division. | :white_check_mark: |
| **A** Atomic instructions. | :white_check_mark: |
| **F** Single-precision floating-point instructions. | :white_check_mark: |
| **D** Double-precision floating-point instructions. | :white_check_mark: |
| **C** Compressed instructions. | :white_check_mark: |
| **B** Bit-manipulation instructions. | :white_check_mark: |
| **Zicsr** CSR instructions. These are implied by presence of F. | :white_check_mark: |
| **Zicntr** Base counters and timers. | :x: |
| **Zihpm** Hardware performance counters. | :x: |
| **Ziccif** Main memory regions with both the cacheability and coherence PMAs must support instruction fetch, and any instruction fetches of naturally aligned power-of-2 sizes up to min(ILEN,XLEN) (i.e., 32 bits for RVA23) are atomic. | :x: |
| **Ziccrse** Main memory regions with both the cacheability and coherence PMAs must support RsrvEventual. | :x: |
| **Ziccamoa** Main memory regions with both the cacheability and coherence PMAs must support all atomics in A. | :x: |
| **Zicclsm** Misaligned loads and stores to main memory regions with both the cacheability and coherence PMAs must be supported. | :x: |
| **Za64rs** Reservation sets are contiguous, naturally aligned, and a maximum of 64 bytes. | :x: |
| **Zihintpause** Pause hint. | :white_check_mark: |
| **Zic64b** Cache blocks must be 64 bytes in size, naturally aligned in the address space. | :x: |
| **Zicbom** Cache-block management instructions. | :white_check_mark: |
| **Zicbop** Cache-block prefetch instructions. | :white_check_mark: |
| **Zicboz** Cache-Block Zero Instructions. | :white_check_mark: |
| **Zfhmin** Half-precision floating-point. | :white_check_mark: |
| **Zkt** Data-independent execution latency. | :x: |
| **V** Vector extension. | :white_check_mark: |
| **Zvfhmin** Vector minimal half-precision floating-point. | :white_check_mark: |
| **Zvbb** Vector basic bit-manipulation instructions. | :white_check_mark: |
| **Zvkt** Vector data-independent execution latency. | :x: |
| **Zihintntl** Non-temporal locality hints. | :white_check_mark: |
| **Zicond** Integer conditional operations. | :white_check_mark: |
| **Zimop** may-be-operations. | :x: |
| **Zcmop** Compressed may-be-operations. | :x: |
| **Zcb** Additional compressed instructions. | :white_check_mark: |
| **Zfa** Additional floating-Point instructions. | :white_check_mark: |
| **Zawrs** Wait-on-reservation-set instructions. | :white_check_mark: |
| **Supm** Pointer masking, with the execution environment providing a means to select PMLEN=0 and PMLEN=7 at minimum. | :x: |
