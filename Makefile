CXX = g++
CXXFLAGS = -std=c++20 -Wall -fPIC -Iinclude `pkg-config --cflags Qt5Widgets`
LDFLAGS = `pkg-config --libs Qt5Widgets`
SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = include
TARGET = CoupGame

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))
MOC_CPP = $(OBJ_DIR)/moc_CoupGUI.cpp
MOC_OBJ = $(OBJ_DIR)/moc_CoupGUI.o

all: $(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(MOC_CPP): $(INCLUDE_DIR)/CoupGUI.hpp
	mkdir -p $(OBJ_DIR)
	moc $(INCLUDE_DIR)/CoupGUI.hpp -o $(MOC_CPP)

$(MOC_OBJ): $(MOC_CPP)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS) $(MOC_OBJ)
	$(CXX) $(OBJECTS) $(MOC_OBJ) -o $(TARGET) $(LDFLAGS)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
