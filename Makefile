# Project Name
TARGET = bypass

# Sources
CPP_SOURCES = src/main.cpp

# Library Locations
DAISYSP_DIR ?= libs/DaisySP
LIBDAISY_DIR ?= libs/libDaisy

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

# Build libraries
.PHONY: build_libs
build_libs:
	$(MAKE) clean -C $(LIBDAISY_DIR)
	$(MAKE) -C $(LIBDAISY_DIR)
	$(MAKE) clean -C $(DAISYSP_DIR)
	$(MAKE) -C $(DAISYSP_DIR)

# Build and program
.PHONY: build_and_program
build_and_program:
	make clean
	make
	make program

# Build and program via DFU
.PHONY: build_and_program_dfu
build_and_program_dfu:
	make clean
	make
	make program-dfu
