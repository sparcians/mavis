## Developing on Mavis

Adding new extensions and instructions to Mavis involves updating/changing
a few aspects of the code base.

- Update/addition to json files (see `json` directory)
- Creation of or reuse of new types
- Creation of or reuse of an existing Extractor form
- Updating the RISC-V ISA spec (`riscv_isa_spec.json`)
- Updating Mavis' testers

This document will give general guidance on these topics and then illustrate
the effort to add the `Zimop`, `Zcmop`, and `Zicfiss` extension from PR #104
as an example of putting these instructions into practice.

### General Instructions

#### Create JSON file

Each new extension needs a corresponding file in the `json` directory. The
name of the file should follow this basic convention:
`isa_[rv32|rv64]<extension_name>.json`

For example, the RV64 version of `zicond` would look like: `isa_rv64zicond.json`.

If the instructions are also available in 32-bit form, the RV32 file can be
a symbolic link to the RV64 version as long as the instruction encodings are the
same. The `zbb` extension, for example, has different instruction encodings
for RV64 and RV32, so separate files are needed.

There is minimal documentation for the format of the json file in:
[doc/mavis_docs/json.txt](https://github.com/sparcians/mavis/blob/main/doc/mavis_docs/json.txt)

Some additional notes:
- If it is a new extension, the tag field for each instruction definition in the
  json should include the new extension name. For example, for the `zicond`
  extension: `"tags" : ["zicond"]`.
- The `stencil` field can be thought of as “any bits that have to be set to 1 in
  the instruction.” For example, if it is a `func` field with specific bits set
  to `1` in the encoding for that instruction, then those corresponding bits in
  the stencil should be `1`. If it is a register field (ex. `rs2`), which can
  vary depending on which register might be used for the instruction, then the
  corresponding bits for that field in the stencil should be `0`.
- If you had to add a new `Extractor` class in `impl/forms/ExtractorDerived.h`
  (more on this below), you will need to add an `xform` field to the
  instruction definition.
- Although not a required field, the operand type is useful metadata to have
  for properly identifying the source and destination register types. A
  couple of examples:
  - An instruction with single-precision `rs1` and `rs2` operands and word
    operand type for `rd`:
    ```
    "s-oper" : ["rs1","rs2"],
    "w-oper" : ["rd"]
    ```
  - An instruction with long operand types for all operands:
    ```
    `"l-oper" : "all"`
    ```

### Creation or Reuse of Instruction Form (or format)

There are common forms for decode "types" as dscribed in the RISC-V
standard opcode types (R-type, I-type, S-type, B-type, etc) and those
are defined in `impl/forms/CommonForms.h`.  Most RV instructions fall
into one of these types.

If, however, you need to add a new form:
- Declare a new Form class in `impl/forms/CommonForms.h`. The form and
  field names are arbitrary, but try to follow existing naming
  conventions and pick something meaningful.
- Define the static members for the class in `impl/forms/CommonForms.cpp`.
  The `opcode_fields` member needs careful handling - this defines the
  order of the fields Mavis uses to decode an opcode, so it should only
  contain fields with fixed values and go in order from least to most specific.
- Add the Form to the registry in `impl/FormRegistry.cpp`.
- Be sure to set the `form` field in the JSON you create to point to your
  new form.

### Creation or Reuse of Extraction Form

To enable proper disassembly of instructions, an extraction form is use.
This form _does not_ contribute to the decoding of the instruction, just
extraction and disassembly.

If you had to add a new Form class, you will also need to add a
specialization of `Extractor` to `impl/forms/ExtractorForms.h`.

If you were able to re-use an existing Form, but need to make some
modifications to the way the fields of the instruction are extracted,
you will need to add a derivative class in `impl/forms/ExtractorDerived.h`.

If you added a new `Extractor`:
- Add it to the registry in `impl/ExtractorRegistry.cpp`
- Add a stub in `mavis/FormStub.h`
- Be sure to set the `xform` field in the JSON you create to point to your
  new extractor

### Updating the RISC-V ISA Spec

Mavis has an ExtensionsManager that can be used to create a Mavis
facade or even validate an ISA string. This manager uses as input the
`json/riscv_isa_spec.json` file to build relationships between
extensions.

**Adding any new extensions to this file is required.**

Instructions for adding a new extension to `json/riscv_isa_spec.json`:
- Normal extensions should go under the extensions array
- Required fields
  - `xlen`: a single integer or an array of integers indicating which XLENs
    support this extension
  - `extension`: a string containing the name of the extension (e.g. i, m, zba, etc.)
  - `json`: The name of the JSON file you added in the previous step
- Optional fields
  - `enabled_by`: Array of arrays of strings. Use this to indicate that
    the extension should automatically be enabled if some combination of
    other extensions are enabled. The inner arrays constitute AND statements
    while the outer array is equivalent to an OR statement.
    - Example: `"enabled_by": [["c", "f"], ["zce", "f"]]` will enable the
      extension if `c` AND `f` are enabled OR if `zce` AND `f` are enabled
  - `enables`: A single extension name or an array of extensions that
    should be automatically enabled if this extension is enabled
  - `requires`: An array of extension names that must all be enabled for
    this extension to be enabled. Causes an exception to be thrown if the
    extension is present in the ISA string but the required extensions are
    not enabled.
  - `conflicts`: An array of extension names that must not be enabled if
    this extension is enabled.
  - `meta_extension`: A meta extension name (defined in the `meta_extensions`
    array). This extension will be automatically enabled if the meta extension
    is enabled.
  - `is_base_extension`: Boolean value. If true, this extension is a RISC-V
    base extension.

#### Update Mavis' Testers
- Directed test: Add your new `.json` file to `test/directed/main.cpp`. Use
  this tester to confirm the instruction extraction/disassembly matches what
  you expected.
- Basic test: Add your new `.json` file to `test/basic/main.cpp` and add any
  desired tests. You may also need to update `test/basic/golden.out`.

### Case Study of Adding `Zimop`, `Zcmop` and `Zicfiss` Extensions

#### Create JSON file

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

For `Zimop`, the `mop.r,N` instructions do not easily fit or
fall into the standard RISC-V forms. Specifically, the instruction
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

#### Adding Zicfiss; Using Overlays

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

#### Update the RISC-V ISA JSON Spec File

Next, the new extensions must be added to `json/riscv_isa_spec.json`.

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

