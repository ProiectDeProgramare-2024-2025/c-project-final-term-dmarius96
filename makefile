CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Iinclude
CFLAGS_BUILD = -O3
CFLAGS_DEBUG = -g -DDEBUG -O0
LDFLAGS = -lncurses -lsqlite3

BUILD_DIR = build
SRC_DIR = src
OBJ_DIR = obj

TARGET = finance-cli

SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS_RELEASE := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/release/%.o, $(SRCS))
OBJS_DEBUG := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/debug/%.o, $(SRCS))

TARGET_RELEASE = $(BUILD_DIR)/release/$(TARGET)
TARGET_DEBUG = $(BUILD_DIR)/debug/$(TARGET)

all: release

release: CFLAGS += $(CFLAGS_BUILD)
release: $(TARGET_RELEASE)

debug: CFLAGS += $(CFLAGS_DEBUG)
debug: $(TARGET_DEBUG)

$(TARGET_RELEASE): $(OBJS_RELEASE) | $(BUILD_DIR)/release
	$(CC) $^ -o $@ $(LDFLAGS)

$(TARGET_DEBUG): $(OBJS_DEBUG) | $(BUILD_DIR)/debug
	$(CC) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/release/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/debug/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/release $(BUILD_DIR)/debug:
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR) $(BUILD_DIR)

.PHONY: all release debug clean
