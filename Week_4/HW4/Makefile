# IT4062 - Homework 4: TCP socket applications
#
# Project layout:
#   bin/      final executables (server / client / server_b2 / client_b2)
#   build/    object files (.o) and dependency files (.d)
#   core/inc/ headers (.h)
#   core/src/ sources (.c)
#   logs/     activity log output
#   server_storage/    files uploaded through Bai 2
#   docs/     assignment description
#
# Run the programs from the project root (HW4) so the relative paths
# `logs/` and `server_storage/` resolve correctly.

CC       = gcc
CFLAGS   = -Wall -Wextra -g
CPPFLAGS = -Icore/inc

CORE_DIR  = core
SRC_DIR   = $(CORE_DIR)/src
INC_DIR   = $(CORE_DIR)/inc
BUILD_DIR = build
BIN_DIR   = bin

ifeq ($(OS),Windows_NT)
BIN_SRV1  = $(BIN_DIR)/server.exe
BIN_CLI1  = $(BIN_DIR)/client.exe
BIN_SRV2  = $(BIN_DIR)/server_b2.exe
BIN_CLI2  = $(BIN_DIR)/client_b2.exe
LDLIBS    = -lws2_32
MKDIR     = if not exist "$(1)" mkdir "$(1)"
RM        = del /q
else
BIN_SRV1  = $(BIN_DIR)/server
BIN_CLI1  = $(BIN_DIR)/client
BIN_SRV2  = $(BIN_DIR)/server_b2
BIN_CLI2  = $(BIN_DIR)/client_b2
LDLIBS    =
MKDIR     = mkdir -p "$(1)"
RM        = rm -f
endif

COMMON_SRCS = $(SRC_DIR)/logger.c $(SRC_DIR)/utils.c

SRCS_SRV1 = $(SRC_DIR)/server_b1.c $(COMMON_SRCS) $(SRC_DIR)/splitter.c
SRCS_CLI1 = $(SRC_DIR)/client_b1.c $(COMMON_SRCS) $(SRC_DIR)/splitter.c
SRCS_SRV2 = $(SRC_DIR)/server_b2.c $(COMMON_SRCS)
SRCS_CLI2 = $(SRC_DIR)/client_b2.c $(COMMON_SRCS)

OBJS_SRV1 = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS_SRV1))
OBJS_CLI1 = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS_CLI1))
OBJS_SRV2 = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS_SRV2))
OBJS_CLI2 = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS_CLI2))

DEPS_SRV1 = $(OBJS_SRV1:.o=.d)
DEPS_CLI1 = $(OBJS_CLI1:.o=.d)
DEPS_SRV2 = $(OBJS_SRV2:.o=.d)
DEPS_CLI2 = $(OBJS_CLI2:.o=.d)

# Default target: build all four executables
all: $(BIN_SRV1) $(BIN_CLI1) $(BIN_SRV2) $(BIN_CLI2)

$(BIN_DIR):
	$(call MKDIR,$@)

$(BUILD_DIR):
	$(call MKDIR,$@)

$(BIN_SRV1): $(OBJS_SRV1) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJS_SRV1) $(LDLIBS)

$(BIN_CLI1): $(OBJS_CLI1) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJS_CLI1) $(LDLIBS)

$(BIN_SRV2): $(OBJS_SRV2) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJS_SRV2) $(LDLIBS)

$(BIN_CLI2): $(OBJS_CLI2) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJS_CLI2) $(LDLIBS)

# Compile each source into build/ (-MMD tracks header dependencies)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS_SRV1) $(DEPS_CLI1) $(DEPS_SRV2) $(DEPS_CLI2)

# Convenience target: build and start Bai 1 server on port 5500
run-server1: $(BIN_SRV1)
	$(BIN_SRV1) 5500

# Convenience target: build and start Bai 1 client
run-client1: $(BIN_CLI1)
	$(BIN_CLI1) 127.0.0.1 5500

# Convenience target: build and start Bai 2 server on port 5600
run-server2: $(BIN_SRV2)
	$(BIN_SRV2) 5600

# Convenience target: build and start Bai 2 client
run-client2: $(BIN_CLI2)
	$(BIN_CLI2) 127.0.0.1 5600

# Remove the executables, object files and dependency files
clean:
ifeq ($(OS),Windows_NT)
	-$(RM) $(BIN_DIR)\server.exe $(BIN_DIR)\client.exe \
            $(BIN_DIR)\server_b2.exe $(BIN_DIR)\client_b2.exe \
            $(BUILD_DIR)\*.o $(BUILD_DIR)\*.d 2>NUL
else
	$(RM) $(BIN_SRV1) $(BIN_CLI1) $(BIN_SRV2) $(BIN_CLI2) \
        $(BIN_DIR)/server.exe $(BIN_DIR)/client.exe \
        $(BIN_DIR)/server_b2.exe $(BIN_DIR)/client_b2.exe \
        $(BUILD_DIR)/*.o $(BUILD_DIR)/*.d
endif

.PHONY: all run-server1 run-client1 run-server2 run-client2 clean