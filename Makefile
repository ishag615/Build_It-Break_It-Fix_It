# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# Target executable 
TARGET = logappend
SRC = logappend.cpp
TEST = test_logappend.cpp

# Default rule
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Clean rule
clean:
	rm -f $(TARGET)
	rm -f test_logappend

# run tests
test: $(TEST) $(SRC)
	$(CXX) $(CXXFLAGS) -DTESTING -o test_logappend $(TEST) $(SRC)
	./test_logappend
