# Compiler and flags
CC = g++
CFLAGS = -Wall -Wextra -std=c++17 -O2 -Iinclude/core -Iinclude/scenarios -Ilib

# Directories
SRC_CORE = src/core
SRC_SCENARIOS = src/scenarios
INC_CORE = include/core
INC_SCENARIOS = include/scenarios
BUILD_DIR = build
BIN_DIR = bin
TEST_DIR = test/scenarios
LIB_DIR = lib

# Core object files
CORE_SRCS = $(wildcard $(SRC_CORE)/*.cpp)
CORE_OBJS = $(patsubst $(SRC_CORE)/%.cpp, $(BUILD_DIR)/%.o, $(CORE_SRCS))

# Scenarios object files
SCENARIO_SRCS = $(wildcard $(SRC_SCENARIOS)/*.cpp)
SCENARIO_OBJS = $(patsubst $(SRC_SCENARIOS)/%.cpp, $(BUILD_DIR)/%.o, $(SCENARIO_SRCS))

# Merge all object files
OBJS =  $(CORE_OBJS) $(SCENARIO_OBJS)

# Compile rules
$(BUILD_DIR)/%.o: $(SRC_CORE)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_SCENARIOS)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Link final executable
$(BIN_DIR)/StarWars: $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

# Default target
all: $(BIN_DIR)/StarWars

# Clean
clean:
	rm -rf $(BUILD_DIR)/*.o $(BIN_DIR)/StarWars

# Run
run: all
	./$(BIN_DIR)/StarWars
	
#PHONY
.PHONY: all clean run