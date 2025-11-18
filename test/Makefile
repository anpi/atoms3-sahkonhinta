# Test Makefile for atoms3-sahkonhinta

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
LDFLAGS = -pthread

# Coverage flags
COVERAGE_CXXFLAGS = --coverage -O0
COVERAGE_LDFLAGS = --coverage

# GoogleTest configuration
GTEST_VERSION = 1.15.2
GTEST_URL = https://github.com/google/googletest/archive/refs/tags/v$(GTEST_VERSION).tar.gz
VENDOR_DIR = ./vendor
GTEST_LOCAL = $(VENDOR_DIR)/googletest

# Search for GoogleTest in multiple locations
GTEST_SEARCH_PATHS = \
	$(GTEST_LOCAL) \
	/opt/homebrew \
	/usr/local \
	/usr

GTEST_PATH = $(firstword $(foreach path,$(GTEST_SEARCH_PATHS),$(wildcard $(path)/include/gtest)))

# Set include and lib paths based on found GoogleTest
ifneq ($(GTEST_PATH),)
    GTEST_BASE = $(patsubst %/include/gtest,%,$(GTEST_PATH))
    CXXFLAGS += -I$(GTEST_BASE)/include
    LDFLAGS += -L$(GTEST_BASE)/lib -lgtest -lgtest_main
else
    # Will trigger auto-install
    CXXFLAGS += -I$(GTEST_LOCAL)/googletest/include -I$(GTEST_LOCAL)/googlemock/include
    LDFLAGS += -L$(GTEST_LOCAL)/build/lib -lgtest -lgtest_main
endif

# ArduinoJson configuration
ARDUINO_JSON_VERSION = 7.4.2
ARDUINO_JSON_URL = https://github.com/bblanchon/ArduinoJson/archive/refs/tags/v$(ARDUINO_JSON_VERSION).tar.gz
ARDUINO_JSON_LOCAL = $(VENDOR_DIR)/ArduinoJson

# Search for ArduinoJson in multiple locations
# Priority: project local > user Arduino libraries > system Arduino libraries
ARDUINO_JSON_SEARCH_PATHS = \
	$(ARDUINO_JSON_LOCAL) \
	$(HOME)/Documents/Arduino/libraries/ArduinoJson \
	$(HOME)/Arduino/libraries/ArduinoJson \
	/usr/local/share/arduino/libraries/ArduinoJson

ARDUINO_JSON_PATH = $(firstword $(foreach path,$(ARDUINO_JSON_SEARCH_PATHS),$(wildcard $(path))))

# Add ArduinoJson to include path if found
ifneq ($(ARDUINO_JSON_PATH),)
    CXXFLAGS += -I$(ARDUINO_JSON_PATH)/src
endif

BUILD_DIR = ../build/test

# Automatically find all test files
TEST_SOURCES = $(wildcard test_*.cpp)
TEST_TARGETS = $(patsubst %.cpp,$(BUILD_DIR)/%,$(TEST_SOURCES))

.PHONY: all clean run test deps install-deps install-gtest install-arduinojson clean-deps clean-all help coverage clean-coverage

# Auto-install dependencies if needed
all: deps $(BUILD_DIR) $(TEST_TARGETS)

# Install dependencies locally if not found
deps:
ifeq ($(GTEST_PATH),)
	@echo "GoogleTest not found. Installing to $(GTEST_LOCAL)..."
	@$(MAKE) install-gtest
endif
ifeq ($(ARDUINO_JSON_PATH),)
	@echo "ArduinoJson not found. Installing to $(ARDUINO_JSON_LOCAL)..."
	@$(MAKE) install-arduinojson
endif

install-gtest:
	@mkdir -p $(VENDOR_DIR)
	@echo "Downloading GoogleTest v$(GTEST_VERSION)..."
	@curl -L $(GTEST_URL) -o $(VENDOR_DIR)/googletest.tar.gz
	@echo "Extracting..."
	@tar -xzf $(VENDOR_DIR)/googletest.tar.gz -C $(VENDOR_DIR)
	@mv $(VENDOR_DIR)/googletest-$(GTEST_VERSION) $(GTEST_LOCAL)
	@rm $(VENDOR_DIR)/googletest.tar.gz
	@echo "Building GoogleTest..."
	@mkdir -p $(GTEST_LOCAL)/build
	@cd $(GTEST_LOCAL)/build && cmake -DCMAKE_BUILD_TYPE=Release .. && $(MAKE) -s
	@echo "✓ GoogleTest v$(GTEST_VERSION) installed to $(GTEST_LOCAL)"

install-arduinojson:
	@mkdir -p $(VENDOR_DIR)
	@echo "Downloading ArduinoJson v$(ARDUINO_JSON_VERSION)..."
	@curl -L $(ARDUINO_JSON_URL) -o $(VENDOR_DIR)/arduinojson.tar.gz
	@echo "Extracting..."
	@tar -xzf $(VENDOR_DIR)/arduinojson.tar.gz -C $(VENDOR_DIR)
	@mv $(VENDOR_DIR)/ArduinoJson-$(ARDUINO_JSON_VERSION) $(ARDUINO_JSON_LOCAL)
	@rm $(VENDOR_DIR)/arduinojson.tar.gz
	@echo "✓ ArduinoJson v$(ARDUINO_JSON_VERSION) installed to $(ARDUINO_JSON_LOCAL)"

install-deps: install-gtest install-arduinojson

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Generic pattern rule to build all test files
$(BUILD_DIR)/test_%: test_%.cpp TestStringAdapter.h | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

test: run

run: all
	@echo "Running tests..."
	@for test in $(TEST_TARGETS); do \
		echo "\n=== Running $$(basename $$test) ==="; \
		$$test; \
	done

clean:
	rm -rf $(BUILD_DIR)

clean-coverage:
	rm -f *.gcda *.gcno *.gcov coverage.info
	rm -rf coverage_html

clean-deps:
	rm -rf $(VENDOR_DIR)

clean-all: clean clean-coverage clean-deps

coverage: clean-coverage clean
	@echo "Building tests with coverage instrumentation..."
	@$(MAKE) CXXFLAGS="$(CXXFLAGS) $(COVERAGE_CXXFLAGS)" LDFLAGS="$(LDFLAGS) $(COVERAGE_LDFLAGS)" all
	@echo ""
	@echo "Running tests..."
	@for test in $(TEST_TARGETS); do \
		echo "Running $$(basename $$test)..."; \
		$$test > /dev/null 2>&1 || true; \
	done
	@echo ""
	@echo "Generating coverage report..."
	@cd $(BUILD_DIR) && lcov --capture --directory . --output-file ../../test/coverage.info --rc lcov_branch_coverage=0 --ignore-errors source,inconsistent,format,empty
	@lcov --extract coverage.info '*/atoms3-sahkonhinta/src/*' --output-file coverage.info --rc lcov_branch_coverage=0 --ignore-errors inconsistent
	@genhtml coverage.info --output-directory coverage_html --title "atoms3-sahkonhinta Test Coverage" --quiet --rc genhtml_branch_coverage=0 --ignore-errors inconsistent,category
	@echo ""
	@echo "✓ Coverage report generated: test/coverage_html/index.html"
	@echo ""
	@lcov --summary coverage.info --ignore-errors inconsistent 2>&1 | grep -E "lines\.\.\.\.\.\.|functions\.\.\.\.\.\."

help:
	@echo "Test targets:"
	@echo "  make all         - Build all tests (auto-installs deps)"
	@echo "  make run         - Build and run all tests"
	@echo "  make test        - Alias for 'make run'"
	@echo "  make coverage    - Generate coverage report (requires lcov)"
	@echo "  make clean       - Remove built test binaries"
	@echo "  make install-deps - Force install all dependencies locally"
	@echo "  make clean-deps  - Remove vendored dependencies"
	@echo "  make clean-all   - Remove builds, coverage, and dependencies"
	@echo ""
	@echo "Dependencies:"
	@echo "  - GoogleTest v$(GTEST_VERSION) (auto-installed if missing)"
	@echo "  - ArduinoJson v$(ARDUINO_JSON_VERSION) (auto-installed if missing)"
	@echo "  - lcov (for coverage: brew install lcov)"
	@echo ""
	@echo "Current GoogleTest: $(if $(GTEST_PATH),$(GTEST_BASE),not found - will auto-install)"
	@echo "Current ArduinoJson: $(if $(ARDUINO_JSON_PATH),$(ARDUINO_JSON_PATH),not found - will auto-install)"
