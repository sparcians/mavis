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
| **Zicntr** Base counters and timers. | |
| **Zihpm** Hardware performance counters. | |
| **Ziccif** Main memory regions with both the cacheability and coherence PMAs must support instruction fetch, and any instruction fetches of naturally aligned power-of-2 sizes up to min(ILEN,XLEN) (i.e., 32 bits for RVA23) are atomic. | |
| **Ziccrse** Main memory regions with both the cacheability and coherence PMAs must support RsrvEventual. | |
| **Ziccamoa** Main memory regions with both the cacheability and coherence PMAs must support all atomics in A. | |
| **Zicclsm** Misaligned loads and stores to main memory regions with both the cacheability and coherence PMAs must be supported. | |
| **Za64rs** Reservation sets are contiguous, naturally aligned, and a maximum of 64 bytes. | |
| **Zihintpause** Pause hint. | :white_check_mark: |
| **Zic64b** Cache blocks must be 64 bytes in size, naturally aligned in the address space. | |
| **Zicbom** Cache-block management instructions. | :white_check_mark: |
| **Zicbop** Cache-block prefetch instructions. | :white_check_mark: |
| **Zicboz** Cache-Block Zero Instructions. | :white_check_mark: |
| **Zfhmin** Half-precision floating-point. | :white_check_mark: |
| **Zkt** Data-independent execution latency. | |
| **V** Vector extension. | :white_check_mark: |
| **Zvfhmin** Vector minimal half-precision floating-point. | :white_check_mark: |
| **Zvbb** Vector basic bit-manipulation instructions. | :white_check_mark: |
| **Zvkt** Vector data-independent execution latency. | |
| **Zihintntl** Non-temporal locality hints. | :white_check_mark: |
| **Zicond** Integer conditional operations. | :white_check_mark: |
| **Zimop** may-be-operations. | :x: |
| **Zcmop** Compressed may-be-operations. | :x: |
| **Zcb** Additional compressed instructions. | :white_check_mark: |
| **Zfa** Additional floating-Point instructions. | :white_check_mark: |
| **Zawrs** Wait-on-reservation-set instructions. | :white_check_mark: |
| **Supm** Pointer masking, with the execution environment providing a means to select PMLEN=0 and PMLEN=7 at minimum. | |


| RVA23U64 Optional Extensions | Status |
| :---------------------------- | :----: |
| **Zvkng** Vector crypto NIST algorithms with GCM. | :white_check_mark: |
| **Zvksg** Vector crypto ShangMi algorithms with GCM. | :white_check_mark: |
| **Zabha** Byte and halfword atomic memory operations. | :x: |
| **Zacas** Compare-and-Swap instructions. | :x: |
| **Ziccamoc** Main memory regions with both the cacheability and coherence PMAs must provide AMOCASQ level PMA support. | |
| **Zvbc** Vector carryless multiplication. | :white_check_mark: |
| **Zama16b** Misaligned loads, stores, and AMOs to main memory regions that do not cross a naturally aligned 16-byte boundary are atomic. | |
| **Zfh** Scalar half-precision floating-point. | :x: |
| **Zbc** Scalar carryless multiply. | :x: |
| **Zicfilp** Landing Pads. | :x: |
| **Zicfiss** Shadow Stack. | :x: |
| **Zvfh** Vector half-precision floating-point. | :x: |
| **Zfbfmin** Scalar BF16 converts. | :x: |
| **Zvfbfmin** Vector BF16 converts. | :white_check_mark: |
| **Zvfbfwma** Vector BF16 widening mul-add. | :white_check_mark: |


| RVA23S64 Mandatory Extensions | Status |
| :---------------------------- | :----: |
| **Zifencei** Instruction-Fetch Fence. | :x: |
| **Svbare** The satp mode Bare must be supported. | |
| **Sv39** Page-based 39-bit virtual-Memory system. | |
| **Svade** Page-fault exceptions are raised when a page is accessed when A bit is clear, or written when D bit is clear. | |
| **Ssccptr** Main memory regions with both the cacheability and coherence PMAs must support hardware page-table reads. | |
| **Sstvecd** stvec.MODE must be capable of holding the value 0 (Direct). When stvec.MODE=Direct, stvec.BASE must be capable of holding any valid four-byte-aligned address. | |
| **Sstvala** stval must be written with the faulting virtual address for load, store, and instruction page-fault, access-fault, and misaligned exceptions, and for breakpoint exceptions other than those caused by execution of the EBREAK or C.EBREAK instructions. For virtual-instruction and illegal-instruction exceptions, stval must be written with the faulting instruction. | |
| **Sscounterenw** For any hpmcounter that is not read-only zero, the corresponding bit in scounteren must be writable. | |
| **Svpbmt** Page-based memory types | |
| **Svinval** Fine-grained address-translation cache invalidation. | :x: |
| **Svnapot** NAPOT translation contiguity. | |
| **Sstc** supervisor-mode timer interrupts. | |
| **Sscofpmf** count overflow and mode-based filtering. | |
| **Ssnpm** Pointer masking, with senvcfg.PME and henvcfg.PME supporting, at minimum, settings PMLEN=0 and PMLEN=7. | |
| **Ssu64xl** sstatus.UXL must be capable of holding the value 2 (i.e., UXLEN=64 must be supported). | |
| **H** The hypervisor extension. | :white_check_mark: |
| **Ssstateen** Supervisor-mode view of the state-enable extension. The supervisor-mode (sstateen0-3 and hypervisor-mode (hstateen0-3) state-enable registers must be provided. | |
| **Shcounterenw** For any hpmcounter that is not read-only zero, the corresponding bit in hcounteren
must be writable. | |
| **Shvstvala** vstval must be written in all cases described above for stval. | |
| **Shtvala** htval must be written with the faulting guest physical address in all circumstances permitted by the ISA. | |
| **Shvstvecd** vstvec.MODE must be capable of holding the value 0 (Direct). When vstvec.MODE=Direct, vstvec.BASE must be capable of holding any valid four-byte-aligned address. | :x: |
| **Shvsatpa** All translation modes supported in satp must be supported in vsatp. | :x: |
| **Shgatpa** For each supported virtual memory scheme SvNN supported in satp, the corresponding hgatp SvNNx4 mode must be supported. The hgatp mode Bare must also be supported. | :x: |

| RVA23S64 Optional Extensions | Status |
| :--------------------------- | :----: |
| **Sv48** Page-based 48-bit virtual-memory system. | |
| **Sv57** Page-based 57-bit virtual-memory system. | |
| **Zkr** Entropy CSR. | |
| **Svadu** Hardware A/D bit updates. | |
| **Sdtrig** Debug triggers. | |
| **Ssstrict** No non-conforming extensions are present. Attempts to execute unimplemented opcodes or access unimplemented CSRs in the standard or reserved encoding spaces raises an illegal instruction exception that results in a contained trap to the supervisor-mode trap handler. | |
| **Svvptc** Transitions from invalid to valid PTEs will be visible in bounded time without an explicit memory-management fence. | |
| **Sspm** Supervisor-mode pointer masking, with the supervisor execution environment providing a
means to select PMLEN=0 and PMLEN=7 at minimum. | |

