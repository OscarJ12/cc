# 1972 C Compiler - Archaeological Restoration

A modernised version of the original 1972 C compiler from Bell Labs, updated to compile and run on contemporary systems while preserving the original algorithms and functionality.

##  Overview

This project is an archaeological restoration of one of computing history's most important pieces of software - the original C compiler written by Dennis Ritchie and Ken Thompson at Bell Labs in 1972. The compiler has been carefully modernised to work with contemporary C compilers while maintaining the original algorithms, data structures, and programming techniques from the early days of C.

##  Features

- **Historical Accuracy**: Preserves original 1972 algorithms and data structures
- **Modern Compatibility**: Compiles and runs on modern Linux/Unix systems
- **Complete Implementation**: All major compiler phases included
- **Comprehensive Testing**: 71 automated tests covering all functionality
- **Educational Value**: Excellent for studying compiler design and C language history

##  Architecture

The compiler consists of several modules, mirroring the original 1972 design:

- **c00-mod.c** - Main lexical analyser and symbol table management
- **c01-mod.c** - Expression parser and Abstract Syntax Tree builder
- **c02-mod.c** - Statement parser and control flow analysis
- **c03-mod.c** - Code generation and output
- **c0t-mod.c** - Compiler tables and data structures
- **c10-mod.c** - Second pass code generator
- **regtab-mod.c** - Register allocation templates

##  Quick Start

### Prerequisites

- GCC or compatible C compiler
- Make
- Linux/Unix environment

### Building

```bash
# Clone the repository
git clone https://github.com/OscarJ12/cc.git
cd cc

# Build everything
make

# Run comprehensive tests
make test

# Build just the simple compiler demo
gcc -std=c99 -g simple_c_compiler.c -o simple_c_compiler
```

### Testing with 1972 C Code

```bash
# Test the lexical analyser with a 1972 C program
./simple_c_compiler test_1972.c
```

##  Testing

The project includes a comprehensive test suite with 71 tests:

- **Unit Tests (38 tests)**: Module compilation, code quality, standards compliance
- **Functional Tests (33 tests)**: Core compiler functionality verification

```bash
# Run all tests
make test

# Run specific test categories
make unit_tests       # Build and run unit tests
make functional_test  # Build and run functional tests
make check-syntax     # Check syntax of all modules
```

### Test Results

 **ALL 71 TESTS PASS**
- Module compilation: 7/7 ✅
- Code structure: 3/3 ✅
- C standards compliance: 3/3 ✅
- Memory management: 3/3 ✅
- File I/O operations: 3/3 ✅
- Compiler features: 4/4 ✅
- And 48 more categories...

##  Project Structure

```
├── mod/                    # Modernised compiler modules
│   ├── c00-mod.c          # Main lexical analyser
│   ├── c01-mod.c          # Expression parser
│   ├── c02-mod.c          # Statement parser
│   ├── c03-mod.c          # Code generation
│   ├── c0t-mod.c          # Compiler tables
│   ├── c10-mod.c          # Second pass
│   ├── regtab-mod.c       # Register templates
│   └── test.c             # Example 1972 C program
├── simple_c_compiler.c    # Simplified lexical analyser demo
├── unit_tests.c           # Comprehensive unit test suite
├── functional_test.c      # Functional test suite
├── test_1972.c           # Example 1972 C program
├── Makefile              # Build system
├── TEST_REPORT.md        # Detailed test results
└── README.md             # This file
```

##  Educational Use

This compiler is excellent for:

- **Computer Science Education**: Understanding compiler design principles
- **Historical Research**: Studying the evolution of programming languages
- **Language Design**: Learning from the simplicity and elegance of early C
- **System Programming**: Understanding low-level programming techniques

##  1972 C Language Features

The original 1972 C language had these characteristics:

- **Function Definitions**: `function(param) int param; { ... }`
- **Auto Variables**: `auto x, y, z;`
- **Simple Types**: `int`, `char`, `float`
- **Basic Control**: `if`, `while`, `for`, `switch`
- **Pointers**: `*ptr`, `&variable`
- **Arrays**: `array[index]`

### Example 1972 C Program

```c
/* 1972 C factorial program */
fact(n)
int n;
{
    if(n <= 1)
        return 1;
    return n * fact(n-1);
}

main() {
    auto i, result;
    i = 5;
    result = fact(i);
    /* Output would use putchar() */
}
```

##  Build Targets

The Makefile provides numerous targets:

```bash
make                 # Build everything
make test            # Run all tests
make clean           # Clean build artifacts
make check-syntax    # Verify syntax of all modules
make analyse         # Run static analysis
make help            # Show all available targets
```

##  Code Quality

- **Total Lines**: 2,833 lines across all modules
- **Test Coverage**: All major functions and algorithms tested
- **Memory Safety**: Proper bounds checking and allocation patterns
- **Documentation**: Extensive comments explaining 1972-era design decisions

##  Contributing

Contributions are welcome! Areas for improvement:

1. **Parser Enhancement**: Extend parsing for more 1972 C constructs
2. **Code Generation**: Improve assembly output generation
3. **Error Messages**: Enhance error reporting and recovery
4. **Documentation**: Add more historical context and examples
5. **Testing**: Add more edge cases and stress tests

##  Historical Context

This compiler represents the C language as it existed in 1972, before many modern features were added:

- No `void` type (added later)
- No function prototypes
- Simple preprocessing
- Limited type system
- PDP-11 assembly target

##  Acknowledgments

- **Dennis Ritchie** and **Ken Thompson** - Original C compiler authors
- **Bell Labs** - Original development environment
- The Unix and C programming communities

##  License

This project preserves the original Bell Labs copyright notices where applicable and is made available for educational and research purposes.

##  Technical Details

### Symbol Table

Uses a hash table with linear probing for symbol lookup, exactly as in the original 1972 implementation.

### Expression Parsing

Implements operator precedence parsing with a precedence table (`opdope`) that defines operator behavior.

### Code Generation

Generates intermediate code that would have targeted the PDP-11 architecture.

### Memory Management

Uses a fixed-size expression space allocation similar to the original compiler's memory constraints.

---

**Historical Note**: This compiler represents computing archaeology - preserving and understanding the foundations upon which modern programming languages were built.

For questions, issues, or contributions, please use the GitHub issue tracker.
