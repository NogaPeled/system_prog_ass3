CXX = g++
CXXFLAGS = -std=c++20 -Wall -fPIC -Iinclude `pkg-config --cflags Qt5Widgets`
LDFLAGS = `pkg-config --libs Qt5Widgets`

SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = include
TEST_DIR = tests

TARGET = CoupGame

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

MOC_CPP = $(OBJ_DIR)/moc_CoupGUI.cpp
MOC_OBJ = $(OBJ_DIR)/moc_CoupGUI.o

MAIN_SRC = $(SRC_DIR)/main.cpp
MAIN_OBJ = $(OBJ_DIR)/main.o

TEST_SRC = $(TEST_DIR)/tests.cpp
TEST_OBJ = $(OBJ_DIR)/tests.o

# Only logic files (no GUI or main)
LOGIC_SRCS = Game.cpp Player.cpp Governor.cpp General.cpp Judge.cpp Merchant.cpp Spy.cpp Baron.cpp
LOGIC_OBJS = $(addprefix $(OBJ_DIR)/, $(LOGIC_SRCS:.cpp=.o))

all: $(TARGET)

# Compile object files from src/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile object files from tests/
$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# MOC step
$(MOC_CPP): $(INCLUDE_DIR)/CoupGUI.hpp
	mkdir -p $(OBJ_DIR)
	moc $(INCLUDE_DIR)/CoupGUI.hpp -o $(MOC_CPP)

$(MOC_OBJ): $(MOC_CPP)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Main GUI build
$(TARGET): $(OBJECTS) $(MOC_OBJ)
	$(CXX) $(OBJECTS) $(MOC_OBJ) -o $(TARGET) $(LDFLAGS)

# Run GUI demo
Main: $(MAIN_OBJ) $(OBJECTS) $(MOC_OBJ)
	$(CXX) $(CXXFLAGS) -o Main $^ $(LDFLAGS)
	./Main

# Run tests (only logic code + test file)
test: $(TEST_OBJ) $(LOGIC_OBJS)
	$(CXX) $(CXXFLAGS) -o test_runner $^
	./test_runner

# Run tests with valgrind
valgrind: test
	valgrind ./test_runner

# Clean up
clean:
	rm -rf $(OBJ_DIR) Main test_runner $(TARGET)
