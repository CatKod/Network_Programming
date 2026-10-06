# IT4062 - Homework 2: domain name / IP address resolver
#
# Project layout:
#   build/    object files (.o) and dependency files (.d)
#   core/inc/ headers (.h)
#   core/src/ sources (.c)
#   docs/     assignment description
#
# `make` builds the executable `resolver` at the project root, so the
# program is run exactly as required by the assignment:
#   ./resolver <domain name | IPv4 address>

CC       = gcc
CFLAGS   = -Wall -Wextra -g
CPPFLAGS = -Icore/inc

CORE_DIR  = core
SRC_DIR   = $(CORE_DIR)/src
INC_DIR   = $(CORE_DIR)/inc
BUILD_DIR = build

# On Windows the executable is resolver.exe and needs the Winsock
# library. Recipes must match the shell make runs them with: MSYS2 make
# uses a POSIX shell even on Windows, while mingw32-make uses cmd.exe.
ifeq ($(OS),Windows_NT)
TARGET = resolver.exe
LDLIBS = -lws2_32
ifeq ($(findstring sh,$(notdir $(SHELL))),sh)
POSIX_SHELL = 1
else
POSIX_SHELL = 0
endif
else
TARGET = resolver
LDLIBS =
POSIX_SHELL = 1
endif

ifeq ($(POSIX_SHELL),1)
MKDIR = mkdir -p "$(1)"
CLEAN = rm -f resolver resolver.exe $(BUILD_DIR)/*.o $(BUILD_DIR)/*.d
else
MKDIR = if not exist "$(1)" mkdir "$(1)"
CLEAN = del /q resolver.exe $(BUILD_DIR)\*.o $(BUILD_DIR)\*.d 2>NUL
endif

SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/resolver.c
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
DEPS = $(OBJS:.o=.d)

# Default target: build the executable
all: $(TARGET)

$(BUILD_DIR):
	$(call MKDIR,$@)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDLIBS)

# Compile each source into build/ (-MMD tracks header dependencies)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

# Remove the executable, object files and dependency files
clean:
	-$(CLEAN)

.PHONY: all clean
