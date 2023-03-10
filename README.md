# Lox in C++

An implementation of [Crafting Interpreters](https://craftinginterpreters.com/)'s Lox in C++20.

Extensions:

- Properly nesting C-style `/*...*/` block comments.
- Added support for the [comma operator](https://en.wikipedia.org/wiki/Comma_operator) from C.
- Added support for the ternarny operator.
- Implemented break statements inside loops.

## Todos

- [ ] Read through the CGold documentation on CMake and restructure project. 
  - [ ] Divide into tree-walking interpreter and bytecode VM.
- [ ] Write about the visitor pattern.
- [ ] Try using `mpark/patterns` and refactor code (mainly with variants).
