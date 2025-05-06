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
| M Integer multiplication and division. | \u2705 |
| A Atomic instructions. | SUPPORTED |
| F Single-precision floating-point instructions. | SUPPORTED |
| D Double-precision floating-point instructions. | SUPPORTED |
| C Compressed instructions. | SUPPORTED |
| B Bit-manipulation instructions. | SUPPORTED |
| Zicsr CSR instructions. These are implied by presence of F. | SUPPORTED |
| Zicntr Base counters and timers. | UNSUPPORTED |
| Zihpm Hardware performance counters. | UNSUPPORTED |
| Ziccif Main memory regions with both the cacheability and coherence PMAs must support instruction fetch, and any instruction fetches of naturally aligned power-of-2 sizes up to min(ILEN,XLEN) (i.e., 32 bits for RVA23) are atomic. | UNSUPPORTED |
| Ziccrse Main memory regions with both the cacheability and coherence PMAs must support RsrvEventual. | UNSUPPORTED |
| Ziccamoa Main memory regions with both the cacheability and coherence PMAs must support all atomics in A. | UNSUPPORTED |
| Zicclsm Misaligned loads and stores to main memory regions with both the cacheability and coherence PMAs must be supported. | UNSUPPORTED |
| Za64rs Reservation sets are contiguous, naturally aligned, and a maximum of 64 bytes. | UNSUPPORTED |
| Zihintpause Pause hint. | SUPPORTED |
| Zic64b Cache blocks must be 64 bytes in size, naturally aligned in the address space. | UNSUPPORTED |
| Zicbom Cache-block management instructions. | SUPPORTED |
| Zicbop Cache-block prefetch instructions. | SUPPORTED |
| Zicboz Cache-Block Zero Instructions. | SUPPORTED |
| Zfhmin Half-precision floating-point. | SUPPORTED |
| Zkt Data-independent execution latency. | UNSUPPORTED |
| V Vector extension. | SUPPORTED |
| Zvfhmin Vector minimal half-precision floating-point. | SUPPORTED |
| Zvbb Vector basic bit-manipulation instructions. | SUPPORTED |
| Zvkt Vector data-independent execution latency. | UNSUPPORTED |
| Zihintntl Non-temporal locality hints. | SUPPORTED |
| Zicond Integer conditional operations. | SUPPORTED |
| Zimop may-be-operations. | UNSUPPORTED |
| Zcmop Compressed may-be-operations. | UNSUPPORTED |
| Zcb Additional compressed instructions. | SUPPORTED |
| Zfa Additional floating-Point instructions. | SUPPORTED |
| Zawrs Wait-on-reservation-set instructions. | SUPPORTED |
| Supm Pointer masking, with the execution environment providing a means to select PMLEN=0 and PMLEN=7 at minimum. | UNSUPPORTED |
