# CalcCpp

A small scientific calculator written in 100% C++ with a native Windows GUI.

## Scope

This first implementation focuses on a simple scientific calculator:

- basic arithmetic: `+`, `-`, `*`, `/`
- parentheses and operator precedence
- power with `^`
- constants: `pi`, `e`
- scientific functions: `sin`, `cos`, `tan`, `sqrt`, `ln`, `log`, `abs`
- clear, backspace, and evaluate actions

## Structure

- `src/main.cpp` - application entry point
- `src/gui/` - Win32 window and button layout
- `src/operator/` - expression parsing and evaluation

## Build

This project uses CMake and the native Windows toolchain.

```bash
cmake -S . -B build
cmake --build build
```

## Notes

- The project uses only C++ source code.
- The GUI is implemented with the Win32 API, so no extra GUI runtime is required.
- Trigonometric functions use radians.

## Remaining Plan

The first implementation chunk is in place. The remaining work will stay small and reviewable:

1. Add keyboard input so the display can accept typed expressions as well as button clicks.
2. Improve expression handling for edge cases such as repeated operators, empty input, and malformed parentheses.
3. Refine the button layout and add a few usability touches such as resizing behavior and better result formatting.
4. Split the parser and evaluator into smaller units if more scientific features are added later.
5. Verify the calculator with a short test set covering arithmetic, trig functions, powers, constants, and error cases.

## Build Order

The implementation should continue in small chunks:

1. Core input and validation.
2. GUI interaction and keyboard support.
3. Layout and polish.
4. Final behavior checks and cleanup.

## Testing and Notes

- Unit tests for the expression evaluator are provided in `tests/expression_evaluator_tests.cpp` and can be built/run on Linux (or any platform with a C++ toolchain) without the Win32 GUI.

- To build and run the tests (Linux example):

```bash
cmake -S . -B build -DBUILD_TESTS=ON
cmake --build build --target calc_tests
./build/calc_tests
```

- The GUI is implemented with the Win32 API and requires building on Windows (MSVC or a MinGW toolchain). To build the GUI on Windows use:

```bash
cmake -S . -B build
cmake --build build --config Release
```

Note: This repository's `CMakeLists.txt` includes an optional `calc_tests` target so the evaluator can be validated on non-Windows platforms.
