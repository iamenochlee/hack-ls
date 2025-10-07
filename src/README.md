# Hack Language Server Architecture

This document describes the architecture and organization of the Hack Language Server codebase.

## Directory Structure

```
src/
├── protocol/              # LSP Protocol Definitions
│   ├── lsp.hpp           # Main protocol header with version info
│   ├── types.hpp         # Basic LSP types (Position, Range, TextDocument*, etc.)
│   ├── messages.hpp      # Request/Response message structures
│   ├── params.hpp        # Method parameter structures
│   └── responses.hpp     # Server response structures
├── server/               # Server Implementation
│   ├── server.hpp        # Main server class and configuration
│   └── handlers/         # Message handlers
│       ├── MessageHandler.hpp
│       └── MessageHandler.cpp
├── language/             # Language-Specific Support
│   ├── LanguageSupport.hpp    # Base language support interface
│   └── hack/             # Hack language implementation
│       └── HackSupport.hpp    # Hack-specific language features
├── common/               # Shared Utilities
│   └── GlobalState.hpp   # Global application state
├── main.hpp              # Common includes and base types
└── main.cpp              # Application entry point
```

## Architecture Overview

### Protocol Layer (`protocol/`)
Contains all Language Server Protocol (LSP) definitions and structures. This layer is language-agnostic and implements the standard LSP specification.

- **`types.hpp`**: Core LSP data types like `Position`, `Range`, `TextDocumentIdentifier`
- **`messages.hpp`**: LSP message protocol structures (`RequestMessage`, `NotificationMessage`)
- **`params.hpp`**: Parameter structures for LSP methods (`DidOpenParams`, `DidChangeParams`)
- **`responses.hpp`**: Server response structures (`InitializeResult`, `ServerCapabilities`)
- **`lsp.hpp`**: Main protocol header that includes everything and defines protocol constants

### Server Layer (`server/`)
Implements the core server functionality and request handling.

- **`server.hpp`**: Main `LanguageServer` class with configuration and lifecycle management
- **`handlers/`**: Message handlers that process incoming LSP requests and notifications
  - `MessageHandler`: Routes and processes LSP messages

### Language Layer (`language/`)
Provides language-specific implementations and features.

- **`LanguageSupport.hpp`**: Abstract interface defining language support capabilities
- **`hack/`**: Hack language-specific implementation
  - `HackSupport`: Implements language features like completion, hover, diagnostics

### Common Layer (`common/`)
Shared utilities and global state management.

- **`GlobalState.hpp`**: Manages global application state like document cache

## Design Principles

### 1. Separation of Concerns
- **Protocol**: Pure LSP specification implementation
- **Server**: Generic server functionality
- **Language**: Language-specific logic
- **Common**: Shared utilities

### 2. Extensibility
- New languages can be added by implementing the `LanguageSupport` interface
- New LSP features can be added to the protocol layer
- Server capabilities can be extended independently

### 3. Type Safety
- Strong typing throughout the codebase
- Namespace isolation (`lsp::`, `protocol::`, `server::`, `language::`)
- Clear interfaces between layers

### 4. Maintainability
- Logical file organization
- Clear dependencies between modules
- Comprehensive documentation

## Key Classes

### MessageHandler
Routes incoming LSP messages to appropriate handlers and manages the request/response lifecycle.

### LanguageSupport (Abstract)
Defines the interface for language-specific functionality:
- Document lifecycle management
- Language features (completion, hover, definition, etc.)
- Diagnostics and validation

### HackSupport
Concrete implementation of `LanguageSupport` for the Hack programming language.

## Adding New Languages

To add support for a new language:

1. Create a new directory under `language/` (e.g., `language/python/`)
2. Implement the `LanguageSupport` interface
3. Register the new language in `LanguageSupportFactory`
4. Add language-specific parsers, analyzers, and feature implementations

## Adding New LSP Features

To add new LSP capabilities:

1. Add new types/structures to the appropriate `protocol/` files
2. Update `ServerCapabilities` in `responses.hpp`
3. Add handler methods to `MessageHandler`
4. Implement language-specific support in relevant `LanguageSupport` implementations

## Dependencies

- **nlohmann/json**: JSON parsing and serialization
- **Standard Library**: STL containers, algorithms, and utilities

## Buil