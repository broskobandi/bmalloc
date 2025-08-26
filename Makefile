# Project
PROJECT := alloc
CC := $(shell command -v clang || command -v gcc)
CFLAGS := -Wall -Wextra -Werror -Wunused-result -Wconversion
CPPFLAGS := -Isrc
LDFLAGS := -pthread

# Dirs
BUILD_DIR := build
SRC_DIR := src
INC_DIR := include
TEST_DIR := test
OBJ_DIR := $(BUILD_DIR)/obj
TEST_OBJ_DIR := $(BUILD_DIR)/test-obj
LIB_INSTALL_DIR := /usr/local/lib
INC_INSTALL_DIR := /usr/local/include
DOC_DIR := doc

# Files
SRC := $(wildcard $(SRC_DIR)/*.c)
INC := $(INC_DIR)/$(PROJECT).h
INC_PRIV := $(wildcard $(SRC_DIR)/*.h)
TEST_SRC := $(wildcard $(TEST_DIR)/*.c)
TEST_INC_PRIV := $(wildcard $(TEST_DIR)/*.h)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TEST_OBJ := $(TEST_SRC:$(TEST_DIR)/%.c=$(TEST_OBJ_DIR)/%.o)
LIB_A := $(BUILD_DIR)/lib$(PROJECT).a
LIB_SO := $(BUILD_DIR)/lib$(PROJECT).so
TEST_EXE := $(BUILD_DIR)/test
TEST_MAIN := $(TEST_DIR)/main/test.c

# Rules
.PHONY: all test clean install uninstall doc

all: $(LIB_A) $(LIB_SO)

test: CC = bear -- $(CC)
test: $(TEST_EXE)
	./$<

clean: rm -rf $(BUILD_DIR) $(DOC_DIR) compile_commands.json

install: $(LIB_A) $(LIB_SO) $(INC)
	cp $(LIB_A) $(LIB_INSTALL_DIR)/
	cp $(LIB_SO) $(LIB_INSTALL_DIR)/
	cp $(INC) $(INC_INSTALL_DIR)/

uninstall:
	rm $(addprefix $(LIB_INSTALL_DIR)/, $(notdir $(LIB_A)))
	rm $(addprefix $(LIB_INSTALL_DIR)/, $(notdir $(LIB_SO)))
	rm $(addprefix $(INC_INSTALL_DIR)/, $(notdir $(INC)))

doc:
	doxygen

$(LIB_A): $(OBJ) | $(BUILD_DIR)
	ar rcs $@ $^

$(LIB_SO): $(OBJ) |$(BUILD_DIR)
	$(CC) -shared $(CFLAGS) $(CPPFLAGS) $^ -o $@ $(LDFLAGS)

$(TEST_EXE): $(OBJ) $(TEST_OBJ) $(TEST_MAIN) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC) $(INC_PRIV) | $(OBJ_DIR)
	$(CC) -c -fPIC $(CFLAGS) $(CPPFLAGS) $< -o $@

$(TEST_OBJ_DIR)%.o: $(TEST_DIR)/%.c $(INC) $(INC_PRIV) $(TEST_INC_PRIV) | $(TEST_OBJ_DIR)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

$(OBJ_DIR):
	mkdir -p $@

$(TEST_OBJ_DIR):
	mkdir -p $@
