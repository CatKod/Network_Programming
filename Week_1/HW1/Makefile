# IT4062 - Homework 1: user account management
#
# Project layout:
#   bin/      final executable (hw1 / hw1.exe)
#   build/    object files (.o) and dependency files (.d)
#   core/inc/ headers (.h)
#   core/src/ sources (.c)
#   data/     input data (account.txt, test_input.txt)
#   logs/     activity log output
#   docs/     assignment description
#
# Run the program from the project root (HW1) so the relative paths
# data/ and logs/ resolve correctly.

CC       = gcc
CFLAGS   = -Wall -Wextra -g
CPPFLAGS = -Icore/inc

CORE_DIR  = core
SRC_DIR   = $(CORE_DIR)/src
INC_DIR   = $(CORE_DIR)/inc
BUILD_DIR = build
BIN_DIR   = bin

ifeq ($(OS),Windows_NT)
TARGET = $(BIN_DIR)/hw1.exe
MKDIR  = if not exist "$(1)" mkdir "$(1)"
RM     = del /q
else
TARGET = $(BIN_DIR)/hw1
MKDIR  = mkdir -p "$(1)"
RM     = rm -f
endif

SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/account.c $(SRC_DIR)/logger.c $(SRC_DIR)/utils.c
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
DEPS = $(OBJS:.o=.d)

# Default target: build the executable
all: $(TARGET)

$(BIN_DIR):
	$(call MKDIR,$@)

$(BUILD_DIR):
	$(call MKDIR,$@)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# Compile each source into build/ (-MMD tracks header dependencies)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

# Convenience target: run the program from the project root
run: $(TARGET)
	$(TARGET)

# Remove the executable, object files and dependency files
clean:
ifeq ($(OS),Windows_NT)
	-$(RM) $(BIN_DIR)\hw1.exe $(BUILD_DIR)\*.o $(BUILD_DIR)\*.d 2>NUL
else
	$(RM) $(BIN_DIR)/hw1 $(BIN_DIR)/hw1.exe $(BUILD_DIR)/*.o $(BUILD_DIR)/*.d
endif

.PHONY: all run clean
