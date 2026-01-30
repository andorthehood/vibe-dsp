# Load environment variables from .env file (optional)
-include .env
export

# Project Name
TARGET = bypass

# Serial port for speech prompter
SERIAL_PORT ?= /dev/cu.usbmodem2102

# Sources
CPP_SOURCES = src/main.cpp

# Library Locations
DAISYSP_DIR ?= libs/DaisySP
LIBDAISY_DIR ?= libs/libDaisy

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

.PHONY: build_libs
# Build libraries (libDaisy and DaisySP)
build_libs:
	$(MAKE) clean -C $(LIBDAISY_DIR)
	$(MAKE) -C $(LIBDAISY_DIR)
	$(MAKE) clean -C $(DAISYSP_DIR)
	$(MAKE) -C $(DAISYSP_DIR)

.PHONY: build_and_program
# Build and program the firmware to the device
build_and_program: clean all program

.PHONY: build_and_program_dfu
# Build and program the firmware to the device via DFU (Device Firmware Upgrade)
build_and_program_dfu: clean all program-dfu

.PHONY: start_speech_prompter_claude
# Start a speech prompter for Claude, interacting with the specified serial port
start_speech_prompter_claude:
	speech2shell \
		--serial-port $(SERIAL_PORT) \
		--api-key ${ASSEMBLYAI_API_KEY} \
		--action-template "claude -p \"Read AGENTS.md first, then: {text}. You MUST run make build_and_program after writing code.\" \
			--allowedTools \"Read,Edit,Bash,Write\" \
			--output-format stream-json \
			--verbose "

.PHONY: start_speech_prompter_gemini
# Start a speech prompter for Gemini, interacting with the specified serial port
start_speech_prompter_gemini:
	speech2shell \
		--serial-port $(SERIAL_PORT) \
		--api-key ${ASSEMBLYAI_API_KEY} \
		--action-template "gemini -p \"Read AGENTS.md first, then: {text}. You MUST run make build_and_program after writing code.\" \
			--yolo \
			--output-format stream-json"

.PHONY: list_serial_ports
# List available serial ports (short version)
list_serial_ports:
	# Lists the device names of available serial ports
	ls /dev/cu.*
