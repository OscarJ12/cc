# Makefile for 1972 C Compiler Test Suite
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -g
SRCDIR = mod
SOURCES = $(SRCDIR)/c00-mod.c
COMPILER = c00-mod
TEST_SUITE = test_suite
UNIT_TESTS = unit_tests
FUNCTIONAL_TESTS = functional_test
OBJECTS = $(SOURCES:.c=.o)

# Default target
all: $(UNIT_TESTS) $(FUNCTIONAL_TESTS)

# Build the 1972 C compiler
$(COMPILER): $(SOURCES)
	@echo "Building 1972 C Compiler..."
	$(CC) $(CFLAGS) -o $@ $^
	@echo "Compiler built successfully!"

# Build individual object files for debugging
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Build the test suite
$(TEST_SUITE): test_suite.c
	@echo "Building test suite..."
	$(CC) $(CFLAGS) -o $@ $<
	@echo "Test suite built successfully!"

# Build the unit tests
$(UNIT_TESTS): unit_tests.c
	@echo "Building unit tests..."
	$(CC) $(CFLAGS) -o $@ $<
	@echo "Unit tests built successfully!"

# Build the functional tests
$(FUNCTIONAL_TESTS): functional_test.c
	@echo "Building functional tests..."
	$(CC) $(CFLAGS) -o $@ $<
	@echo "Functional tests built successfully!"

# Run tests
test: $(UNIT_TESTS) $(FUNCTIONAL_TESTS)
	@echo "Running unit tests..."
	./$(UNIT_TESTS)
	@echo ""
	@echo "Running functional tests..."
	./$(FUNCTIONAL_TESTS)

# Run old comprehensive test suite (if needed)
test-full: $(COMPILER) $(TEST_SUITE)
	@echo "Running comprehensive test suite..."
	./$(TEST_SUITE)

# Run specific test categories
test-modules: $(TEST_SUITE)
	@echo "Testing compiler module compilation..."
	./$(TEST_SUITE) | grep -A 20 "Compiler modules"

test-lexer: $(COMPILER) $(TEST_SUITE)
	@echo "Testing lexical analyzer..."
	./$(TEST_SUITE) | grep -A 10 "Lexical analyzer"

test-parser: $(COMPILER) $(TEST_SUITE)
	@echo "Testing parsers..."
	./$(TEST_SUITE) | grep -A 10 "parser"

test-integration: $(COMPILER) $(TEST_SUITE)
	@echo "Testing integration..."
	./$(TEST_SUITE) | grep -A 10 "Integration"

# Test the compiler manually with the provided test file
test-manual: $(COMPILER)
	@echo "Testing compiler with mod/test.c..."
	./$(COMPILER) mod/test.c output.s temp.tmp
	@if [ -f output.s ]; then \
		echo "✓ Compilation successful - output.s created"; \
		wc -l output.s; \
	else \
		echo "✗ Compilation failed"; \
	fi

# Debug build
debug: CFLAGS += -DDEBUG -g3
debug: $(COMPILER)

# Check for compilation errors in individual modules
check-syntax:
	@echo "Checking syntax of all modules..."
	@for file in $(SOURCES); do \
		echo "Checking $$file..."; \
		$(CC) $(CFLAGS) -fsyntax-only $$file || exit 1; \
	done
	@echo "All modules pass syntax check!"

# Static analysis
analyze:
	@echo "Running static analysis..."
	@for file in $(SOURCES); do \
		echo "Analyzing $$file..."; \
		$(CC) $(CFLAGS) -Wall -Wextra -Wpedantic -fsyntax-only $$file; \
	done

# Memory check with valgrind (if available)
memcheck: $(COMPILER)
	@if command -v valgrind >/dev/null 2>&1; then \
		echo "Running memory check with valgrind..."; \
		valgrind --leak-check=full --show-leak-kinds=all ./$(COMPILER) mod/test.c output.s temp.tmp; \
	else \
		echo "Valgrind not available, skipping memory check"; \
	fi

# Performance test
perf-test: $(COMPILER)
	@echo "Running performance test..."
	@time ./$(COMPILER) mod/test.c perf_output.s perf_temp.tmp
	@rm -f perf_output.s perf_temp.tmp

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -f $(COMPILER) $(TEST_SUITE) $(UNIT_TESTS) $(FUNCTIONAL_TESTS) $(OBJECTS)
	rm -f *.o *.s *.tmp
	rm -f test_*.c test_*.s test_*.tmp
	rm -f output.s temp.tmp mod_test_output.s mod_test.tmp
	rm -f perf_output.s perf_temp.tmp
	@echo "Clean complete!"

# Install (copy to /usr/local/bin)
install: $(COMPILER)
	@echo "Installing $(COMPILER) to /usr/local/bin..."
	@sudo cp $(COMPILER) /usr/local/bin/
	@echo "Installation complete!"

# Uninstall
uninstall:
	@echo "Removing $(COMPILER) from /usr/local/bin..."
	@sudo rm -f /usr/local/bin/$(COMPILER)
	@echo "Uninstall complete!"

# Show help
help:
	@echo "1972 C Compiler Makefile"
	@echo "========================"
	@echo ""
	@echo "Targets:"
	@echo "  all          - Build compiler and test suite (default)"
	@echo "  $(COMPILER)       - Build the 1972 C compiler only"
	@echo "  $(TEST_SUITE)    - Build the test suite only"
	@echo "  test         - Run comprehensive test suite"
	@echo "  test-modules - Test compiler module compilation"
	@echo "  test-lexer   - Test lexical analyzer"
	@echo "  test-parser  - Test parsers"
	@echo "  test-integration - Test integration"
	@echo "  test-manual  - Test compiler manually with mod/test.c"
	@echo "  debug        - Build with debug flags"
	@echo "  check-syntax - Check syntax of all modules"
	@echo "  analyze      - Run static analysis"
	@echo "  memcheck     - Run memory check with valgrind"
	@echo "  perf-test    - Run performance test"
	@echo "  clean        - Clean build artifacts"
	@echo "  install      - Install compiler to /usr/local/bin"
	@echo "  uninstall    - Remove compiler from /usr/local/bin"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "Usage Examples:"
	@echo "  make                 # Build everything"
	@echo "  make test            # Run all tests"
	@echo "  make test-manual     # Quick manual test"
	@echo "  make clean           # Clean up"

# Continuous Integration test
ci: clean all test
	@echo "CI pipeline completed successfully!"

# Show compiler version and info
info: $(COMPILER)
	@echo "1972 C Compiler Information"
	@echo "==========================="
	@echo "Compiler executable: $(COMPILER)"
	@echo "Source files:"
	@for file in $(SOURCES); do echo "  - $$file"; done
	@echo "Build date: $$(date)"
	@if [ -f $(COMPILER) ]; then \
		echo "Binary size: $$(du -h $(COMPILER) | cut -f1)"; \
		echo "Binary info: $$(file $(COMPILER))"; \
	fi

.PHONY: all test test-modules test-lexer test-parser test-integration test-manual debug check-syntax analyze memcheck perf-test clean install uninstall help ci info