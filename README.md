# Mavis

Mavis is a framework that allows decoding of the RISC-V ISA into
custom instruction class types as well as custom extensions to those
class types.

Mavis is not a massive switch-case statement that most RISC-V decoders
are.  Instead, Mavis intelligently builds a decode table or DTable
with a digital tree or [Trie](https://en.wikipedia.org/wiki/Trie) to
decode the instructions _at runtime_.

## Basics of the Design

Mavis' design is based on the notion there are two parts to every
decoded instruction:

1. Static information like instruction type, execution target units,
   data width, etc.  This information is the same regardless of an
   opcode's specifics.  For example, an `lw` is always loading 32-bits
   of data regardless of the source/destination registers

1. Dynamic information like which registers are being sourced/written,
   immediate field information, instruction modes, etc.

On initialization of the Mavis decoder, Mavis instantiates the static
information immediately for each instruction in the ISA.  The dynamic
information is generated based on the specific opcode encountered.

To instantiate the tree with static information, Mavis uses [JSON
files](https://github.com/sparcians/mavis/tree/main/json) populated
with instruction definitions to build the Trie at runtime.  Each
instruction definition contains at minimum a stencil that defines how
that instruction is decoded:

```
 {
    "mnemonic" : "amoxor.d",
    "stencil" : "0x2000302f",
    ...
 }
```

The Trie is built using these stencils.  The data structure at each
node of the tree is a factory or `IFactory` that Mavis uses to build
the dynamic information or more specifically the modeler's instruction
type.   This is what's returned to the user.

The user can supply "micro-architectural" information to extend the
instruction as it is being created using micro-architectural JSON files.

Build the doxygen found in
[mavis_docs](https://github.com/sparcians/mavis/tree/main/doc) to
learn more or take a peek at
[doc/Developing.md](https://github.com/sparcians/mavis/tree/main/doc/Developing.md)

## Using Mavis

### Build Mavis/Add to Project

To add Mavis to a project, if using `cmake`, make Mavis a submodule or
provide a local clone.  Then add the following to the project's CMakeLists.txt:

```
# (OPTIONAL) Build mavis with C++23 instead of C++20
set(MAVIS_CXX_STANDARD 23)

# Bring in the mavis project/CMakeLists.txt to build Mavis
add_subdirectory (<path to mavis>)

# Setup Mavis include paths
get_target_property(MAVIS_INCLUDES mavis INCLUDE_DIRECTORIES)
include_directories(${MAVIS_INCLUDES})

# Needed for Mavis to find the JSON files (path to be changed by the user)
file(CREATE_LINK <path to mavis>/json ${CMAKE_CURRENT_BINARY_DIR}/mavis_isa_files SYMBOLIC)

# Link Mavis with project
target_link_libraries(<project> mavis)
```

### Instantiating the Decoder

Mavis needs class definitions for the Static Information and the
Dynamic Information as described in [Basics of the Design](#Basics-of-the-Design).

Example Static information class: [uArchInfo](https://github.com/sparcians/mavis/blob/main/test/basic/uArchInfo.h)

Example Dynamic information class: [Instruction](https://github.com/sparcians/mavis/blob/main/test/basic/Inst.h)

Mavis **requires** that the user classes (both Static and Dynamic classes) define a type called `PtrType`:

```c++
   class MyDynamicInstructionType
   {
   public:
       typedef typename std::shared_ptr<MyDynamicinstructiontype> PtrType;
       // ...
   };
```
Mavis only keeps track of heap-allocated objects.  The
allocation/deallocation of these objects can be managed/changed via
the template parameters to Mavis.

> **_NOTE_** Mavis' allocator works well with GNU allocators as well
> as Sparta's
> [SpartaSharedPointerAllocator](https://sparcians.github.io/map/classsparta_1_1SpartaSharedPointerAllocator.html)
> class.

In addition, the constructor for the Dynamic instruction is required
to take Mavis specific opcode information as well as the Static
information created by the modeler.  This is how the Dynamic part of
the instruction is connected to Mavis' detailed/static information.

```c++
   class MyDynamicInstructionType
   {
   public:
       typedef typename std::shared_ptr<MyDynamicinstructiontype> PtrType;

       MyDynamicInstructionType(const typename mavis::OpcodeInfo::PtrType & opcode_info,
                                const typename MyStaticInstructionType::PtrType & my_static_info,
                                ... other construction arguments pass through by makeInst);

   };
```

Instantiate a Mavis instance:
```c++
#include "mavis/Mavis.h"

#include "MyStaticInstructionType.hpp"
#include "MyDynamicInstructionType.hpp"

using MavisType = mavis::Mavis<MyDynamicinstructiontype, MyStaticInstructionType>;

...

{
    // Create a mavis decoder that can only decode rv64imf
    MavisType mavis_decoder({"mavis_isa_files/isa_rv64i.json",
                             "mavis_isa_files/isa_rv64m.json",
                             "mavis_isa_files/isa_rv64f.json"},
                             {"my_uarch_extensions.json"});

    // Decode add x1,x2,x3
    MyDynamicInstructionType::PtrType inst = mavis_decoder.makeInst(0x003100b3,
                               /* other construction info for MyDynamicInstructionType */);

    // do stuff with the instruction
}
```

### Using the Extension Manager

The ExtensionManager is a powerful set of classes that allow the parsing
of an ISA string and helps build a Mavis instance that adheres to the
given ISA.  If there are conflicts in that ISA string (extensions that
conflict with other extensions) the manager will throw an exception.

Example:
```c++

    const std::string rv_isa = "rv64imac_zicond_zicsr_zifencei_zawrs_zihintpause";

    mavis::extension_manager::riscv::RISCVExtensionManager extension_manager =
        mavis::extension_manager::riscv::RISCVExtensionManager::fromISA(
            rv_isa, "json/riscv_isa_spec.json", "json");

    std::unique_ptr<MavisType> mavis_decoder
        = std::make_unique<MavisType>(
            extension_manager.constructMavis<MyDynamicinstructiontype,
                                             MyStaticinstructiontype>({"my_uarch_extensions.json"}));

```

## Contributing

### Run regression

Regression is mostly manual and entails the developer to build a basic
tester application, run it, and compare the output against a golden
reference.

* Build library, output will be `libmavis.a`
```
mkdir build
cd build
# To build in C++20 mode:
cmake ..
# To build in C++23 mode:
cmake .. -DMAVIS_CXX_STANDARD=23
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
./mavis_decode -a rv32g -o 0x0001a283
./mavis_decode -a rv64g -o 0x0001b283
```

## Supported Extensions

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
| **Zvkt** Vector data-independent execution latency. | :white_check_mark: |
| **Zihintntl** Non-temporal locality hints. | :white_check_mark: |
| **Zicond** Integer conditional operations. | :white_check_mark: |
| **Zimop** may-be-operations. | :white_check_mark: |
| **Zcmop** Compressed may-be-operations. | :white_check_mark: |
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
| **Zbkx** Crosssbar permutation instructions (xperm4, xperm8). | :white_check_mark: |
| **Zknd** Accelerating instructions for decryption and key-schedule functions of the AES block cipher. | :white_check_mark: |
| **Zama16b** Misaligned loads, stores, and AMOs to main memory regions that do not cross a naturally aligned 16-byte boundary are atomic. | |
| **Zfh** Scalar half-precision floating-point. | :x: |
| **Zbc** Scalar carryless multiply. | :x: |
| **Zicfilp** Landing Pads. | :x: |
| **Zicfiss** Shadow Stack. | :white_check_mark: |
| **Zvfh** Vector half-precision floating-point. | :x: |
| **Zfbfmin** Scalar BF16 converts. | :x: |
| **Zvfbfmin** Vector BF16 converts. | :white_check_mark: |
| **Zvfbfwma** Vector BF16 widening mul-add. | :white_check_mark: |


| RVA23S64 Mandatory Extensions | Status |
| :---------------------------- | :----: |
| **Zifencei** Instruction-Fetch Fence. | :white_check_mark: |
| **Svbare** The satp mode Bare must be supported. | |
| **Sv39** Page-based 39-bit virtual-Memory system. | |
| **Svade** Page-fault exceptions are raised when a page is accessed when A bit is clear, or written when D bit is clear. | |
| **Ssccptr** Main memory regions with both the cacheability and coherence PMAs must support hardware page-table reads. | |
| **Sstvecd** stvec.MODE must be capable of holding the value 0 (Direct). When stvec.MODE=Direct, stvec.BASE must be capable of holding any valid four-byte-aligned address. | |
| **Sstvala** stval must be written with the faulting virtual address for load, store, and instruction page-fault, access-fault, and misaligned exceptions, and for breakpoint exceptions other than those caused by execution of the EBREAK or C.EBREAK instructions. For virtual-instruction and illegal-instruction exceptions, stval must be written with the faulting instruction. | |
| **Sscounterenw** For any hpmcounter that is not read-only zero, the corresponding bit in scounteren must be writable. | |
| **Svpbmt** Page-based memory types |  :white_check_mark: |
| **Svinval** Fine-grained address-translation cache invalidation. | :white_check_mark: |
| **Svnapot** NAPOT translation contiguity. | :white_check_mark: |
| **Sstc** supervisor-mode timer interrupts. | |
| **Sscofpmf** count overflow and mode-based filtering. | :white_check_mark: |
| **Ssnpm** Pointer masking, with senvcfg.PME and henvcfg.PME supporting, at minimum, settings PMLEN=0 and PMLEN=7. | |
| **Ssu64xl** sstatus.UXL must be capable of holding the value 2 (i.e., UXLEN=64 must be supported). | |
| **H** The hypervisor extension. | :white_check_mark: |
| **Ssstateen** Supervisor-mode view of the state-enable extension. The supervisor-mode (sstateen0-3 and hypervisor-mode (hstateen0-3) state-enable registers must be provided. |  :white_check_mark: |
| **Shcounterenw** For any hpmcounter that is not read-only zero, the corresponding bit in hcounteren
must be writable. | |
| **Shvstvala** vstval must be written in all cases described above for stval. | |
| **Shtvala** htval must be written with the faulting guest physical address in all circumstances permitted by the ISA. | |
| **Shvstvecd** vstvec.MODE must be capable of holding the value 0 (Direct). When vstvec.MODE=Direct, vstvec.BASE must be capable of holding any valid four-byte-aligned address. | |
| **Shvsatpa** All translation modes supported in satp must be supported in vsatp. | |
| **Shgatpa** For each supported virtual memory scheme SvNN supported in satp, the corresponding hgatp SvNNx4 mode must be supported. The hgatp mode Bare must also be supported. | |

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
