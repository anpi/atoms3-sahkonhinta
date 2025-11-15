# Makefile for M5AtomS3 Sahkonhinta Project

# Configuration
SKETCH = atoms3-sahkonhinta.ino
FQBN = m5stack:esp32:m5stack_atoms3
PORT = /dev/cu.usbmodem1101

# arduino-cli command
ARDUINO_CLI = arduino-cli

# Targets
.PHONY: all compile upload clean monitor help test

# Default target
all: compile

# Run unit tests
test:
	@echo "Running unit tests..."
	@$(MAKE) -C test run

# Compile the sketch
compile:
	@echo "Compiling $(SKETCH)..."
	$(ARDUINO_CLI) compile --fqbn $(FQBN) $(SKETCH)

# Upload to board
upload:
	@echo "Uploading $(SKETCH) to $(PORT)..."
	$(ARDUINO_CLI) upload --fqbn $(FQBN) -p $(PORT) $(SKETCH)

# Compile and upload
flash: compile upload

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf build/
	@$(MAKE) -C test clean

# Open serial monitor
monitor:
	@echo "Opening serial monitor on $(PORT)..."
	$(ARDUINO_CLI) monitor -p $(PORT)

# Auto-detect port
detect:
	@echo "Detecting connected boards..."
	$(ARDUINO_CLI) board list

# Show help
help:
	@echo "M5AtomS3 Sahkonhinta Project - Makefile Commands"
	@echo ""
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@echo "  compile    - Compile the sketch"
	@echo "  upload     - Upload to the board"
	@echo "  flash      - Compile and upload"
	@echo "  test       - Run unit tests"
	@echo "  clean      - Remove build artifacts"
	@echo "  monitor    - Open serial monitor"
	@echo "  detect     - Detect connected boards"
	@echo "  help       - Show this help message"
	@echo ""
	@echo "Configuration:"
	@echo "  SKETCH = $(SKETCH)"
	@echo "  FQBN   = $(FQBN)"
	@echo "  PORT   = $(PORT)"
	@echo ""
	@echo "Example:"
	@echo "  make compile         # Compile only"
	@echo "  make upload          # Upload only"
	@echo "  make flash           # Compile and upload"
	@echo "  make PORT=/dev/cu... # Override port"
