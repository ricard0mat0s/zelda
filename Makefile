CXX ?= g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2
INCLUDES = -Isrc -Ivendor/raylib/include -Ivendor

# Static Raylib linkage for hermetic execution on Linux
RAYLIB_LIB = vendor/raylib/lib/libraylib.a
LDFLAGS = -lGL -lm -lpthread -ldl -lrt -lX11

BIN_DIR = bin
BUILD_DIR = build
SRC_DIR = src
TEST_DIR = tests

TARGET = $(BIN_DIR)/zelda_game
TEST_TARGET = $(BIN_DIR)/test_runner

# All cpp files in src
APP_SRCS = $(shell find $(SRC_DIR) -name "*.cpp")
APP_OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(APP_SRCS))

# Test sources
TEST_SRCS = $(shell find $(TEST_DIR) -name "*.cpp")

.PHONY: all test clean run

all: $(TARGET)

$(TARGET): $(APP_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ $(RAYLIB_LIB) $(LDFLAGS) -o $@
	@echo "Built: $@"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

test: $(TEST_TARGET)
	@echo "Executing tests..."
	@./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_SRCS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -Isrc -Ivendor $^ -o $@

run: $(TARGET)
	@./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
