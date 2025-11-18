# Test Suite

Unit tests for the AtomS3 Sahkonhinta project.

## Quick Start

```bash
# Build and run tests (auto-installs dependencies if missing)
make test
```

## Prerequisites

Dependencies are installed automatically:

### GoogleTest
The Makefile:
1. Searches for existing installations (Homebrew, system, local)
2. Downloads and builds v1.15.2 if not found
3. Uses the local version for builds

### ArduinoJson
The Makefile:
1. Searches for existing installations (local, user, system)
2. Downloads and installs v7.4.2 if not found
3. Uses the local version for builds

If you prefer to install manually, see [Manual Installation](#manual-installation).

## Building Tests

```bash
# Build and run all tests (auto-installs dependencies if needed)
make test

# Build only
make all

# Clean and rebuild
make clean && make test

# Force reinstall all dependencies
make install-deps

# Remove all dependencies
make clean-deps

# Show help
make help
```

## Test Organization

- `test_price_analyzer_*.cpp` - PriceAnalyzer component tests (58 tests)
- `test_price_monitor_*.cpp` - PriceMonitor component tests (25 tests)

**Total: 83 tests**

## Test Coverage

### PriceAnalyzer
- ✅ Basic calculations (90-min averages)
- ✅ Cheapest period finding
- ✅ Time constraints (7:00-23:00 window)
- ✅ Tomorrow detection
- ✅ DateTime parsing
- ✅ Edge cases

### PriceMonitor
- ✅ JSON parsing (valid/invalid formats)
- ✅ Field validation
- ✅ Data type handling
- ✅ Error handling

## Troubleshooting

### Check Dependencies
```bash
./check-deps.sh
```

Or check what the Makefile sees:
```bash
make help  # Shows current GoogleTest and ArduinoJson paths
```

### Build errors
```bash
# Clean and rebuild everything
make clean-all && make test
```

### Force reinstall dependencies
```bash
make clean-deps && make install-deps
```

## Manual Installation

If you prefer to install dependencies manually:

### GoogleTest
```bash
# macOS
brew install googletest

# Linux (Ubuntu/Debian)
sudo apt-get install libgtest-dev

# Linux (Fedora)
sudo dnf install gtest-devel
```

### ArduinoJson

#### Option A: Arduino Library Manager
1. Open Arduino IDE
2. Go to `Sketch > Include Library > Manage Libraries`
3. Search for "ArduinoJson"
4. Install version 7.0 or higher

#### Option B: arduino-cli
```bash
arduino-cli lib install ArduinoJson
```

The Makefile will automatically find and use these installations.

## CI/CD Setup

The test suite is CI/CD ready with zero configuration:

```yaml
# GitHub Actions example
- name: Run tests
  run: make test  # Auto-installs GoogleTest and ArduinoJson
```

The Makefile handles everything:
- Auto-downloads GoogleTest v1.15.2 if not found
- Auto-downloads ArduinoJson v7.4.2 if not found
- Caches in `test/vendor/` (gitignored)
- No manual setup required

### Search Priority

**GoogleTest** search order:
1. `test/vendor/googletest/` (project-local, auto-installed)
2. `/opt/homebrew/` (Homebrew on Apple Silicon)
3. `/usr/local/` (Homebrew on Intel, manual installs)
4. `/usr/` (system packages)

**ArduinoJson** search order:
1. `test/vendor/ArduinoJson/` (project-local, auto-installed)
2. `~/Documents/Arduino/libraries/ArduinoJson/` (macOS Arduino IDE)
3. `~/Arduino/libraries/ArduinoJson/` (Linux/Windows Arduino IDE)
4. `/usr/local/share/arduino/libraries/ArduinoJson/` (system-wide)

### Manual CI Setup (Optional)
If you prefer to cache dependencies in CI:

```yaml
- name: Cache dependencies
  uses: actions/cache@v3
  with:
    path: test/vendor
    key: test-deps-gtest-1.15.2-arduinojson-7.4.2

- name: Run tests
  run: make test
```
