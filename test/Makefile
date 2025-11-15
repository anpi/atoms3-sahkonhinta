# Test Makefile for atoms3-sahkonhinta

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I/opt/homebrew/include -I../src
LDFLAGS = -L/opt/homebrew/lib -lgtest -lgtest_main -pthread

BUILD_DIR = ../build/test
TEST_SOURCES = test_price_analyzer.cpp
TEST_TARGETS = $(patsubst %.cpp,$(BUILD_DIR)/%,$(TEST_SOURCES))

.PHONY: all clean run

all: $(BUILD_DIR) $(TEST_TARGETS)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/test_price_analyzer: test_price_analyzer.cpp ../src/PriceData.h test_data_helpers.h mocks/MockString.h | $(BUILD_DIR)
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
