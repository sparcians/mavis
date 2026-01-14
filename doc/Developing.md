## Developing on Mavis

Adding new instructions to Mavis involves the updates/changes to a few
aspects of the code base.

- Update/addition to json files (see json directory)
- Creation of or reuse of an existing Extractor form
- Creation of or reuse of new types
- Updating the RISC-V ISA spec (`riscv_isa_spec.json`)
- Updating Mavis' testers

This document will illustrate the effort to add the `zimop` extension.

### Create JSON file

In the `json` directory, the file `isa_rv64zimop.json` is created and
populated with basic stencil for the `mop.r` instruction.

```
```

For RV32, since the instruction is available in 32-bit form, a
symbolic link is created:

```
ln -s isa_rv64zimop.json isa_rv32zimop.json
```

> **_NOTE_** This might not be the case for all 32-bit instructions.

For decoding, Mavis will use the `stencil` portion of the json
listing.  To determine which fields of the stencil are used for
decoding, the form is used, specifically the form's defined fields.

There are common forms, based on the RISC-V standard opcode types
(R-type, I-type, S-type, B-type) and those are defined in
impl/forms/CommonForms.h.  Most RV instructions fall into one of these
types.

For `mop.r` there are no RISC-V forms that match `mop.r`'s form or any
of the RISC-V standards. Specifically, the instruction has functional
bits 31, 29-28, and 25-22 that are part of the opcode.

In this case, a custom form must be created.

> Why not use "xform"?  xform is for extraction of the dynamic portions
> of the opcode, not for use in decoding

For `mop.r` and `mop.r.r` operations are actually 32 instructions and
8 instructions respectively based on the `n` field of the opcode, TBD...

### Adding New Types

Edit `impl/InstMetaData.cpp` and a new `type` to the maps.  In the
case of `mop`, which is just a `nop`, using `type` of `int` is
appropriate.

### Extractor Forms

For the `mop` instructions a new Extractor form is required.
