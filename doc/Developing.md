## Developing on Mavis

Adding new instructions to Mavis involves the updates/changes to a few
aspects of the code base.

- Update/addition to json files (see json directory)
- Creation of or reuse of an existing Extractor form
- Creation of or reuse of a new type
- Updating the RISC-V ISA spec
- Updating Mavis' testers

This document will illustrate the effort to add the `zimop` extension.

### Create JSON file

In the `json` directory, the file `isa_rv64zimop.json` is created and
populated with basic stencil for the `mop.r` instruction.

```
TBD
```

For RV32, since the instruction is available in 32-bit form, a
symbolic link is created:

```
ln -s isa_rv64zimop.json isa_rv32zimop.json
```

> **_NOTE_** This might not be the case for all 32-bit instructions.

For the `form` entry, since there are no RISC-V forms that match
`mop.r`'s form, a new form is required, called `mop` (can be anything).

For `mop.r` and `mop.r.r` operations are actually 32 instructions and
8 instructions respectively based on the `n` field of the opcode, TBD...

### Adding New Types

Edit `impl/InstMetaData.cpp` and a new `type` to the maps.  In the
case of `mop`, which is just a `nop`, using `type` of `int` is
appropriate.

### Extractor Forms

For the `mop` instructions a new Extractor form is required.
