CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Iinclude
CFLAGS_BUILD = -O3
CFLAGS_DEBUG = -g -DDEBUG -O0
LDFLAGS = -lncurses #-lsqlite3

BUILD_DIR = build
SRC_DIR = src
OBJ_DIR = obj

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS_RELEASE := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/release/%.o, $(SRCS))
OBJS_DEBUG := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/debug/%.o, $(SRCS))

TARGET_RELEASE = $(BUILD_DIR)/release/finance-cli
TARGET_DEBUG = $(BUILD_DIR)/debug/finance-cli

all: release

release: CFLAGS += $(CFLAGS_BUILD)
release: $(TARGET_RELEASE)

$(TARGET_RELEASE): $(OBJ_DIR)/release/finance-cli.o $(OBJS_RELEASE) | $(BUILD_DIR)/release
	$(CC) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/release/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)/release
	$(CC) $(CFLAGS) -c $< -o $@

debug: CFLAGS += $(CFLAGS_DEBUG)
debug: $(TARGET_DEBUG)

$(TARGET_DEBUG): $(OBJ_DIR)/debug/finance-cli.o $(OBJS_DEBUG) | $(BUILD_DIR)/debug
	$(CC) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/debug/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)/debug
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/release/finance-cli.o: main.c | $(OBJ_DIR)/release
	$(CC) $(CFLAGS) -c main.c -o $@

$(OBJ_DIR)/debug/finance-cli.o: main.c | $(OBJ_DIR)/debug
	$(CC) $(CFLAGS) -c main.c -o $@

$(OBJ_DIR)/release $(OBJ_DIR)/debug $(BUILD_DIR)/release $(BUILD_DIR)/debug:
	mkdir -p $@

clean:
	rm -rf obj build

.PHONY: all release debug clean