# Compiler
CC = gcc

# Directories
SRC = ./src
INCLUDE = ./include
BIN = ./bin
BUILD = ./build
RESULTS = ./results

# Compiler flags
CFLAGS = -pthread -Wall -g3 -std=c99 -I$(INCLUDE)

# Source files
SRCS := $(wildcard $(SRC)/*.c)
# Object files
OBJS := $(patsubst $(SRC)/%.c,$(BUILD)/%.o,$(SRCS))
COMMON_OBJ = $(BUILD)/common.o

# Executable names
1_1 = $(BIN)/1_1
1_2 = $(BIN)/1_2
1_3 = $(BIN)/1_3
1_4 = $(BIN)/1_4

# Create directories if they don't exist
.PHONY: directories
directories:
	@mkdir -p $(BIN) $(BUILD) $(RESULTS)

# Rule to build object files from source files
$(BUILD)/%.o: $(SRC)/%.c | directories
	$(CC) $(CFLAGS) -c -o $@ $<

# Rule to build the common object file
$(COMMON_OBJ): $(SRC)/common.c | directories
	$(CC) $(CFLAGS) -c -o $@ $<

# Rule to build the executable
$(1_1): $(BUILD)/1_1.o $(COMMON_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Rule to build the executable
$(1_2): $(BUILD)/1_2.o $(COMMON_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Rule to build the executable
$(1_3): $(BUILD)/1_3.o $(COMMON_OBJ)
	$(CC) $(CFLAGS) -o $@ $^


# Rule to build the 1_4 object files
$(BUILD)/pth_ll_rwl.o: $(SRC)/1_4/pth_ll_rwl.c | directories
	$(CC) $(CFLAGS) -c -o $@ $<
$(BUILD)/my_rand.o: $(SRC)/1_4/my_rand.c | directories
	$(CC) $(CFLAGS) -c -o $@ $<
$(BUILD)/custom_rw_lock.o: $(SRC)/1_4/custom_rw_lock.c | directories
	$(CC) $(CFLAGS) -c -o $@ $<

# Rule to build the 1_4 executable
$(1_4): $(BUILD)/pth_ll_rwl.o $(BUILD)/my_rand.o $(BUILD)/custom_rw_lock.o $(COMMON_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Clean rule
.PHONY: clean
clean:
	rm -f $(BUILD)/*.o $(1_1) $(1_2) $(1_3) $(1_4)

# All rule
.PHONY: all
all: $(1_1) $(1_2) $(1_3) $(1_4)