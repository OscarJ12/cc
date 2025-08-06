# Contributing to 1972 C Compiler

Thank you for your interest in contributing to this archaeological restoration of the 1972 C compiler! This document provides guidelines for contributing to the project.

## 🎯 Project Goals

This project aims to:
- Preserve the original 1972 C compiler algorithms and design
- Make the compiler buildable and runnable on modern systems
- Provide educational value for studying compiler design and C language history
- Maintain historical accuracy while ensuring modern compatibility

## 🤝 How to Contribute

### 1. Code Contributions

We welcome contributions in these areas:

#### Parser Enhancements
- Extend parsing for more 1972 C language constructs
- Improve error recovery in the parser
- Add support for additional 1972-era C features

#### Code Generation
- Improve assembly output generation
- Add more sophisticated register allocation
- Enhance intermediate code representation

#### Testing
- Add more test cases for edge conditions
- Create stress tests for large programs
- Add historical C programs as test cases

#### Documentation
- Add more historical context and explanations
- Create tutorials for using the compiler
- Document the differences between 1972 C and modern C

### 2. Bug Reports

When reporting bugs:
- Use the GitHub issue tracker
- Provide a minimal reproducible example
- Include your operating system and compiler version
- Describe the expected vs. actual behavior

### 3. Feature Requests

For new features:
- Check if the feature existed in 1972 C
- Explain the historical justification
- Provide examples of how it would be used

## 📋 Development Guidelines

### Code Style

Follow the existing code style:
- Use the original 1972 C style where possible
- Preserve original variable names and algorithms
- Add modern safety features (bounds checking, etc.) carefully
- Use clear, descriptive comments for modern additions

### Testing Requirements

All contributions must:
- Pass existing tests (`make test`)
- Include appropriate test cases
- Not break backward compatibility
- Maintain the historical character of the compiler

### Commit Guidelines

Use clear commit messages:
```
[category]: Brief description

Longer explanation if needed, explaining what changed
and why the change was made.

Fixes #123
```

Categories:
- `parser`: Changes to parsing logic
- `lexer`: Changes to lexical analysis
- `codegen`: Changes to code generation
- `tests`: Changes to test suite
- `docs`: Documentation changes
- `build`: Build system changes
- `fix`: Bug fixes

## 🧪 Testing

Before submitting a pull request:

```bash
# Run all tests
make test

# Check syntax
make check-syntax

# Clean build
make clean && make

# Test with example programs
./simple_c_compiler test_1972.c
```

## 📚 Resources

### Historical References
- "The C Programming Language" (1978) - Kernighan & Ritchie
- Unix Programmer's Manual (1972-1973)
- Bell Labs C Compiler documentation
- PDP-11 architecture documentation

### Understanding the Original
- Study the original algorithms in each module
- Understand the 1972 C language features
- Learn about PDP-11 assembly and calling conventions
- Read about early Unix development

## 🎓 Educational Value

Remember that this project serves educational purposes:
- Preserve historical programming techniques
- Demonstrate evolution of compiler technology
- Show the elegance of early C design
- Provide hands-on experience with compiler internals

## 📝 Pull Request Process

1. **Fork** the repository
2. **Create** a feature branch (`git checkout -b feature/amazing-feature`)
3. **Make** your changes
4. **Add** tests if applicable
5. **Ensure** all tests pass
6. **Commit** your changes (`git commit -m 'Add amazing feature'`)
7. **Push** to the branch (`git push origin feature/amazing-feature`)
8. **Open** a Pull Request

### Pull Request Checklist

- [ ] Tests pass (`make test`)
- [ ] Code follows project style guidelines
- [ ] Changes are documented
- [ ] Historical accuracy is maintained
- [ ] No breaking changes to existing functionality
- [ ] Commit messages are clear and descriptive

## 🚫 What Not to Contribute

Please avoid:
- Adding modern C features that didn't exist in 1972
- Changing the fundamental algorithms without historical justification
- Breaking compatibility with existing test cases
- Removing historical comments or code structure
- Adding dependencies on modern libraries

## ❓ Questions?

If you have questions:
- Check existing issues and documentation
- Ask in GitHub Discussions
- Reference historical documentation when possible
- Remember the educational and historical goals

## 🎖️ Recognition

Contributors will be acknowledged in:
- The project README
- Release notes
- Git commit history

Thank you for helping preserve this important piece of computing history!

---

*"The best way to understand how something works is to try to build it yourself."* - This project embodies that principle for compiler design and C language history.