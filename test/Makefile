# Test Makefile for atoms3-sahkonhinta

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/lib -lgtest -lgtest_main -pthread

BUILD_DIR = ../build/test

# Automatically find all test files
TEST_SOURCES = $(wildcard test_price_analyzer_*.cpp)
TEST_TARGETS = $(patsubst %.cpp,$(BUILD_DIR)/%,$(TEST_SOURCES))

.PHONY: all clean run

all: $(BUILD_DIR) $(TEST_TARGETS)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Pattern rule to build any test file
$(BUILD_DIR)/test_price_analyzer_%: test_price_analyzer_%.cpp TestStringAdapter.h ../src/PriceData.h ../src/PriceAnalyzer.h ../src/PriceAnalyzer.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

run: all
	@echo "Running tests..."
	@for test in $(TEST_TARGETS); do \
		echo "\n=== Running $$(basename $$test) ==="; \
		$$test; \
	done

clean:
	rm -rf $(BUILD_DIR)

help:
	@echo "Test targets:"
	@echo "  make all    - Build all tests"
	@echo "  make run    - Build and run all tests"
	@echo "  make clean  - Remove built test binaries"
