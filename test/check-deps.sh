#!/bin/bash
# Test dependency checker for CI/CD environments

set -e

echo "Checking test dependencies..."

# Check GoogleTest
GTEST_PATHS=(
    "./vendor/googletest"
    "/opt/homebrew/include/gtest"
    "/usr/local/include/gtest"
    "/usr/include/gtest"
)

GTEST_FOUND=0
for path in "${GTEST_PATHS[@]}"; do
    if [ -d "$path" ]; then
        echo "✓ GoogleTest found at: $path"
        GTEST_FOUND=1
        break
    fi
done

if [ $GTEST_FOUND -eq 0 ]; then
    echo "✗ GoogleTest not found"
    echo "  Will auto-install on 'make test'"
    echo "  Or install manually:"
    echo "  - macOS: brew install googletest"
    echo "  - Linux: sudo apt-get install libgtest-dev"
fi

# Check ArduinoJson
ARDUINO_JSON_PATHS=(
    "./vendor/ArduinoJson"
    "$HOME/Documents/Arduino/libraries/ArduinoJson"
    "$HOME/Arduino/libraries/ArduinoJson"
    "/usr/local/share/arduino/libraries/ArduinoJson"
)

ARDUINO_JSON_FOUND=0
for path in "${ARDUINO_JSON_PATHS[@]}"; do
    if [ -d "$path" ]; then
        echo "✓ ArduinoJson found at: $path"
        ARDUINO_JSON_FOUND=1
        break
    fi
done

if [ $ARDUINO_JSON_FOUND -eq 0 ]; then
    echo "✗ ArduinoJson not found"
    echo "  Will auto-install on 'make test'"
    echo "  Or install manually:"
    echo "  - Arduino IDE: Sketch > Include Library > Manage Libraries"
    echo "  - arduino-cli: arduino-cli lib install ArduinoJson"
fi

echo ""
if [ $GTEST_FOUND -eq 1 ] && [ $ARDUINO_JSON_FOUND -eq 1 ]; then
    echo "All dependencies satisfied!"
else
    echo "Missing dependencies will be auto-installed on 'make test'"
fi
