# Lox in C++

An implementation of [Crafting Interpreters](https://craftinginterpreters.com/)'s Lox in C++20.

```javascript
// fibonacci.lox
fun fib(n) {
  if (n <= 1) return n;
  return fib(n - 2) + fib(n - 1);
}

print fib(7); // prints 13
```

Extensions:

- Properly nesting C-style `/*...*/` block comments.
- Added support for the ternary operator and the [comma operator](https://en.wikipedia.org/wiki/Comma_operator).
- Implemented break statements inside loops.

## Building and running

Prerequisites:
- `cmake` v3.25
- `llvm` v15 (for `clangd` and `clang-format`)
- `ninja` (optional)
- `rlwrap` (optional)

```sh
mkdir build
cd build

cmake -GNinja ..
ninja
```

This will produce two binaries in `bin/` which you can run with
```sh
# Run interpreter
bin/lox
# Use rlwrap to scroll through history
rlwrap bin/lox

# Run tests (expects to be called from the build/ dir)
(cd bin && ./tests)
```

So far, I've been able to build and run on WSL and my M1 MacBook using `clang 15.0.6`.

## Todos

- [ ] Write about the visitor pattern.
- [ ] Try using `mpark/patterns` and refactor code with variants.
- [ ] Try to merge literals and values (don't need both) - make everything a literal
