# CalcCpp

A small scientific calculator written in 100% C++ with cross-platform frontends.

## Scope

This first implementation focuses on a simple scientific calculator:

- basic arithmetic: `+`, `-`, `*`, `/`
- parentheses and operator precedence
- power with `^`
- constants: `pi`, `e`
- scientific functions: `sin`, `cos`, `tan`, `sqrt`, `ln`, `log`, `abs`
- clear, backspace, and evaluate actions

## Structure

- `src/main.cpp` - Windows Win32 GUI entrypoint (optional)
- `src/gui/` - Win32 window and button layout (Windows-only)
- `src/gui_sdl/` - cross-platform SDL2-based GUI (recommended for Linux/macOS)
- `src/operator/` - expression parsing and evaluation

## Build

This project uses CMake and supports cross-platform builds. The Win32 GUI target is optional — use the SDL2 GUI for a single cross-platform binary.

```bash
# build CLI and SDL GUI (recommended)
cmake -S . -B build -DBUILD_CLI=ON -DBUILD_SDL_GUI=ON
cmake --build build --parallel 2

# build only CLI
cmake -S . -B build -DBUILD_CLI=ON
cmake --build build --parallel 2

# On Windows (MSVC) build Win32 GUI target (optional)
cmake -S . -B build -A x64
cmake --build build --config Release --parallel 2
```

## Notes

- The project uses only C++ source code.
- The repository contains two GUIs:
	- a Win32 GUI implementation (`src/gui/`) that is Windows-only and provided for native integration;
	- a cross-platform SDL2 GUI (`src/gui_sdl/`) which is the recommended frontend for Linux/macOS and CI builds.
- Trigonometric functions use radians.

## Download binaries

- Tag a release with `v*` (for example `v0.1.0`) to trigger `.github/workflows/release.yml`.
- The workflow publishes downloadable release assets for:
  - Linux: `calc_cli-linux`, `CalcCppSDL-linux`
  - Windows: `calc_cli-windows.exe`, `CalcCpp-windows.exe`, `calc_cli-windows-cross.exe`

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

(Note: tests were removed in this branch; evaluator validated via CI and manual smoke tests.)

- The GUI is implemented with the Win32 API and requires building on Windows (MSVC or a MinGW toolchain). To build the GUI on Windows use:

```bash
cmake -S . -B build
cmake --build build --config Release
```

Note: This repository's `CMakeLists.txt` includes an optional `calc_tests` target so the evaluator can be validated on non-Windows platforms.

## Linux and Cross-platform GUI

This project now includes:

- A command-line frontend: `calc_cli` (built with `BUILD_CLI=ON`, default `ON`).
- An optional SDL2-based GUI: `CalcCppSDL` (enabled with `-DBUILD_SDL_GUI=ON`). The SDL2 GUI depends on `SDL2` and `SDL2_ttf` development packages.

Build CLI on Linux (WSL):

```bash
cmake -S . -B build -DBUILD_CLI=ON
cmake --build build --target calc_cli --parallel 2
./build/calc_cli
```

Build SDL GUI (if SDL2 and SDL2_ttf are installed):

```bash
# install dependencies on Debian/Ubuntu
sudo apt update && sudo apt install libsdl2-dev libsdl2-ttf-dev

cmake -S . -B build -DBUILD_SDL_GUI=ON
cmake --build build --target CalcCppSDL
./build/CalcCppSDL
```

Cross-compile a Windows `.exe` from WSL using `mingw-w64` (example):

```bash
sudo apt install mingw-w64
cmake -S . -B build-windows -DCMAKE_TOOLCHAIN_FILE=/usr/share/mingw/toolchain.cmake -G "Unix Makefiles"
cmake --build build-windows --config Release
```

If you prefer a native GTK/Qt/wxWidgets GUI instead of SDL2, tell me and I will implement it.
