# Project mcu_emu
Project on [github.com](https://github.com/drollik/mcu_emu)


## Links

**Building multiple binaries within one Eclipse project**

- [Answer at Stack overflow](https://stackoverflow.com/questions/2424795/building-multiple-binaries-within-one-eclipse-project)

**C Links**
- [C library](https://cplusplus.com/reference/clibrary/) at [cplusplus.com](http://cplusplus.com)

** Unit testing**
- [tst](https://github.com/rdentato/tst/tree/main)
- [MinUnit](https://jera.com/techinfo/jtns/jtn002)

## Todos

### Assembler

Labels
- implement two-pass assembly to deal with forward-referenced labels (or use fixups )

Misc
- in find_instruction(): return error codes for different kind of errors, e.g.:
    - unknown instruction mnemonic
    - operand type unknown (already done in tokenize_line()?)
    - operand type mismach: the found operand type does not match the instruction mnemonic
    

Unit tests 
- Unit tests for assemble_instruction() for instructions
    - ADD, SUB, AND, CMP, BRN, BRZ, BRP
- Unit tests for find_instruction() for instructions
    - ADD, SUB, AND, CMP, BRN, BRZ, BRP
  
### Emulator

### Disassembler
- pretty much everything?!