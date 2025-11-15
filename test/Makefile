# Test Makefile for atoms3-sahkonhinta

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I/opt/homebrew/include -I..
LDFLAGS = -L/opt/homebrew/lib -lgtest -lgtest_main -pthread

TEST_SOURCES = test_price_analyzer.cpp
TEST_TARGETS = $(TEST_SOURCES:.cpp=)

.PHONY: all clean run

all: $(TEST_TARGETS)

test_price_analyzer: test_price_analyzer.cpp ../PriceData.h test_data_helpers.h mocks/MockString.h
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

run: all
	@echo "Running tests..."
	@for test in $(TEST_TARGETS); do \
		echo "\n=== Running $$test ==="; \
		./$$test; \
	done

clean:
	rm -f $(TEST_TARGETS)
	rm -f *.o

help:
	@echo "Test targets:"
	@echo "  make all    - Build all tests"
	@echo "  make run    - Build and run all tests"
	@echo "  make clean  - Remove built test binaries"
