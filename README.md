# Hack LSP - Language Server Implementation

A minimal Language Server Protocol (LSP) implementation in C++ for the Hack programming language.

## Architecture

```
src/
├── protocol/           # LSP Protocol Layer
│   ├── types.hpp      # Position, Range, TextDocument types
│   ├── messages.hpp   # RequestMessage, NotificationMessage
│   ├── params.hpp     # Method parameters (Initialize, DidOpen, DidChange)
│   ├── responses.hpp  # Response structures and ServerCapabilities
│   └── lsp.hpp        # Main protocol header
├── core/              # Server Implementation
│   ├── handlers/      
│   │   └── MessageHandler  # Routes and processes LSP messages
│   └── structures/    
│       └── TextDocument    # Document state and incremental updates
├── common/
│   └── GlobalState.hpp     # Server state (documents, initialized flag)
└── main.cpp                # Entry point: LSP message loop over stdin/stdout
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
