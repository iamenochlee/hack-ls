# Hack (Nand2Tetris) Language Server Implementation

A minimal Language Server (LS) implementation in C++ for the Hack assembly language from the Nand2Tetris course.

## Architecture

```
src/
├── protocol/           # LS protocol layer
│   ├── types.hpp      # Position, Range, TextDocument types
│   ├── messages.hpp   # RequestMessage, NotificationMessage
│   ├── params.hpp     # Method parameters (Initialize, DidOpen, DidChange)
│   ├── responses.hpp  # Response structures and ServerCapabilities
│   └── lsp.hpp        # Main protocol header
├── core/              # Server Implementation
│   ├── handlers/      
│   │   └── MessageHandler  # Routes and processes LS messages
│   └── structures/    
│       └── TextDocument    # Document state and incremental updates
├── common/
│   └── GlobalState.hpp     # Server state (documents, initialized flag)
└── main.cpp                # Entry point: LS message loop over stdin/stdout
```


## Current Capabilities

- ✅ Initialize handshake
- ✅ Document synchronization (open/change)
- ✅ Incremental text updates
- 🚧 Definition provider (advertised, not implemented)
- ❌ Hover, completion, diagnostics (not yet)

## Build & Run

```bash
cmake -B build && cmake --build build
./build/bin/hack-language-server
```
