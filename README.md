# Hack (Nand2Tetris) Language Server

A Language Server Protocol (LSP) server implementation in C++ for the Hack assembly language from the Nand2Tetris course.

## Architecture

![Architecture Diagram](docs/architecture.png)

The codebase is organized into three main layers:
- **`lsp/`** - LSP protocol layer (types, messages, params, responses, errors)
- **`core/`** - Server implementation (handlers, document structures, transport I/O)
- **`hack/`** - Hack-specific functionality (assembler integration, diagnostics, completion, hover)

## Features

- [x] Document synchronization (open, change, close)
- [x] Incremental text updates
- [x] Code completion (`@`, `=`, `;` triggers)
- [x] Hover information for symbols
- [x] Real-time diagnostics


## Getting Started

### Prerequisites
- C++20 compatible compiler (clang++ or g++)
- CMake 3.16 or higher
- Git (for submodules)
- Dependencies (managed as git submodules):
  - nlohmann/json (included in `external/nlohmann_json/`)
  - HackAssembler frontend (included in `external/HackAssembler/`)

## Build & Run

### Initial Setup
First, initialize and update the git submodules:
```bash
git submodule update --init --recursive
```

### Build
```bash
cmake -B build && cmake --build build
```

### Run
```bash
./build/bin/hack-ls --stdio
```
The server communicates via stdin/stdout using the LSP protocol.

## Testing

The project includes a test script (`test.sh`) that exercises the LSP server with multiple Hack assembly files.

### Basic Usage

```bash
./test.sh --shutdown tests/ | ./build/bin/hack-ls --stdio
```

### Options

- `-h, --help` - Show help message and exit
- `--shutdown` - Send proper LSP shutdown sequence before exiting

### What the Test Script Does

The test script will:
- Initialize the LSP server
- Open all `.asm` files from the specified folder (excluding files ending with "2")
- Send `didChange` notifications for files ending with "2" (e.g., `Add2.asm` updates `Add.asm`)
- Test completion requests at random positions
- Test hover requests at random positions
- Optionally shutdown the server properly (when using `--shutdown` flag)

### Examples

```bash
# Test with proper shutdown
./test.sh --shutdown tests/ | ./build/bin/hack-ls --stdio

# Test without shutdown (server will detect EOF)
./test.sh tests/ | ./build/bin/hack-ls --stdio

# Show help
./test.sh -h
```

### Requirements

The test script requires `jq` to be installed and available in your PATH.

## Development

See [TODO.txt](TODO.txt) for current development priorities and known issues.

## License

MIT
