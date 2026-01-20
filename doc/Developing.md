## Developing on Mavis

Adding new instructions to Mavis involves the updates/changes to a few
aspects of the code base.

- Update/addition to json files (see json directory)
- Creation of or reuse of an existing Extractor form
- Creation of or reuse of new types
- Updating the RISC-V ISA spec (`riscv_isa_spec.json`)
- Updating Mavis' testers

This document will illustrate the effort to add the `Zimop`, `Zcmop`,
and `Zicfiss` extension from PR #104.

### Create JSON file

In the `json` directory, files to represent the instructions are
created and populated with basic stencil information. For RV32, since
the instruction is available in 32-bit form, a symbolic link is
created.

> **_NOTE_** This might not be the case for all 32-bit instructions.

```
json/isa_rv32zcmop.json -> json/isa_rv64zcmop.json
json/isa_rv32zimop.json -> json/isa_rv64zimop.json
json/isa_rv64zcmop.json
json/isa_rv64zimop.json
```

The files for `Zicfiss` extension are broken up into three files (more
information as to why below).

```
json/isa_rv64zicfiss_c.json
json/isa_rv64zicfiss_common.json
json/isa_rv64zicfiss.json
```

There are common forms for decode "types" as dscribed in the RISC-V
standard opcode types (R-type, I-type, S-type, B-type, etc) and those
are defined in `impl/forms/CommonForms.h`.  Most RV instructions fall
into one of these types.

However. for `Zimop` the `mop.r,N` instructions do not easily fit or
fall into these standard RISC-V forms. Specifically, the instruction
has functional bits 31, 29-28, and 25-22 that are part of the opcode,
which are "hard coded" parts of the R-form that overlap `rs2`.  To
allow these instructions to use `rs2` as part of the opcode, Mavis
supports a "fixed" section that tells Mavis that a field in the image
is part of the opcode's encoding.  These bits are refected as "set" in
the stencil.  For `mop.r.0`, the entry starts like so:

```
{
    "mnemonic" : "mop.r.0",
    "form" : "R",
    "stencil" : "0x81c04073",   // <-- bits 24 -> 20, shared with rs2 are fixed
    "fixed" : ["rs2"]
}
```

Next, to enable proper disassembly of these instructions, an
extraction form or `xform` is added called `MOP`.  This form _does
not_ contribute to the decoding of the instruction, just extraction
and disassembly.

```
{
    "mnemonic" : "mop.r.0",
    "form" : "R",
    "stencil" : "0x81c04073",   // <-- bits 24 -> 20, shared with rs2 are fixed
    "fixed" : ["rs2"],
    "xform" : "MOP"
}
```

The new xform will need to added to `impl/forms/ExtractorDerived.h` and
registered in `impl/ExtractorRegistry.cpp`.

Other meta information for the instruction is optional.

```
{
    "mnemonic" : "mop.r.0",
    "form" : "R",
    "stencil" : "0x81c04073",
    "fixed" : ["rs2"],
    "xform" : "MOP",
    "tags" : ["mop"],  // optional
    "type" : ["int"],  // optional
    "l-oper" : "all"   // long-operation types, etc.  See doc/mavis_docs/json.txt
}
```

For the compressed forms (`c.mop`) of maybe-ops, a different tactic is
employed since these instructions share the opcode space with `c.lui`
and `c.addi16sp`.  Specifically, these maybe-ops take advantage of the
`imm` field of `c.lui`, value 0 (which is illegal), to steer the
decoder to a different instruction.

In this case, the `imm` field _is part_ of the decoding, but only to
tell Mavis to decode to maybe ops.  Since the `imm` field is variable
for `c.lui` instructions, the fields cannot be part of the opcode.
But for `c.mop.N`, it _must be 0_ to decode.

To solve this, adding the `imm` field as part of the decoding is
required.  The `C1` form was changed to add the `imm` field (`imm5`)
as part of the opcode.  For the `c.mop` instructions, this is
**fixed** to the value 0.

```
{
    "mnemonic" : "c.mop.1",
    "stencil" : "0x6081",
    "fixed" : [ "rs1", "imm5" ],
    "tags" : ["mop"],
    "form" : "C1",
    "xform" : "MOPC"
},
```

But adding `imm5` as part of the opcode causes a problem with `c.lui`.
To resolve this, Mavis is instructed to ignore this new field in `json/isa_rv64zca.json`:

```
  {
    "mnemonic" : "c.lui",
    "ignore" : ["func2", "func1", "func2b", "imm5"],
     ...
  }
```

### Adding Zicfiss; Using Overlays

The instructions in the `Zicfiss` extension "remaps" or defines a
handful of the maybe ops.  The maybe-ops _must_ be enabled and added
to Mavis' internal decoding structures.  See "Update the RISC-V ISA
JSON Spec File" below to understand how that's accompished.

`sspush` instruction is actually `mop.rr.7`.  From Mavis' point of
view, if the `sspush` instruction were added without indicating the
`sspush` instruction is an overlays to `mop.rr.7`, Mavis will indicate
a collision or a "ALREADY A LEAF for stencil: ..." error.

To indicate the "replacement" add an `overlay` section to the JSON:
```
{
    "mnemonic" : "sspush",
    "overlay" : {
        "base" : "mop.rr.7",
        "match" : ["0x00700F80", "0x00100000"]
    },
    "ignore" : ["rd"],
    "xform" : "shadow",
    "tags" : ["shadow"],
    "type" : ["int"]
},
```

The syntax to the overlay is `[<mask of common bits>, <value of those
bits>]`.  In this example, bit[20] must be a 1 and the rest of the
`rd` field must be 0.  Also, to indicate to Mavis that the `rd` should
be considered as part of the opcode, the `ignore` field indicates to
Mavis that the `rd` field is part of the eventual decoding mask.

### Update the RISC-V ISA JSON Spec File

Mavis has an ExtensionsManager that can be used to create a Mavis
facade or even validate an ISA string.  This manager uses as input the
`json/riscv_isa_spec.json` file to build relationships between
extensions.

Adding the new extensions to this file is required.

Adding `Zimop` requires nothing else and can just be included:
```
{
  "xlen": [32, 64],
  "extension": "zimop",
  "json": "isa_rv32zimop.json"
},
```

Adding `Zcmop` requires _both_ `Zca` **and** `Zimop` to be provided:
```
{
  "xlen": [32, 64],
  "extension": "zcmop",
  "json": "isa_rv32zcmop.json",
  "requires": [["zca", "zimop"]]
},
```

Adding `Zicfiss` requires `Zimop` to be provided:
```
{
  "xlen": [32,64],
  "extension": "zicfiss",
  "requires": ["zimop"],
  "json": "isa_rv64zicfiss.json"
}
```

### Add Some Testing

It's not fancy, but add the new instructions and contexts to `mavis/test/basic/main.cpp`
