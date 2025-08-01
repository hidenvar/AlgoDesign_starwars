# Compiler and flags
CC = g++
CFLAGS = -Wall -Wextra -std=c++17 -O2 -Iinclude/core -Iinclude/scenarios -Ilib
MAKEFLAGS += -j$(shell nproc)

# Directories
SRC_CORE = src/core
SRC_SCENARIOS = src/scenarios
INC_CORE = include/core
INC_SCENARIOS = include/scenarios
BUILD_DIR = build
BIN_DIR = bin
TEST_DIR = test
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
	@if [ "$(filter-out $@,$(MAKECMDGOALS))" = "" ]; then \
		echo "▶ Running interactively..."; \
		./$(BIN_DIR)/StarWars; \
	else \
		SCENARIO=$(filter-out $@,$(MAKECMDGOALS)); \
		if [ ! -f $(TEST_DIR)/scenario$$SCENARIO/testcase.txt ]; then \
			echo "❌ Error: test/scenario$$SCENARIO/testcase.txt not found!"; \
			exit 1; \
		fi; \
		echo "▶ Running scenario $$SCENARIO..."; \
		./$(BIN_DIR)/StarWars $$SCENARIO < $(TEST_DIR)/scenario$$SCENARIO/testcase.txt; \
	fi

# Prevent Make from treating numbers as targets
%:
	@:

#PHONY
.PHONY: all clean run