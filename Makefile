# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# Target executable
TARGET = logappend.exe
SRC = logappend.cpp
TEST = test_logappend.cpp

# Default rule
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Clean rule
clean:
	rm -f $(TARGET)
	rm -f test_logappend.exe

#run tests
test: $(TEST) 
	$(CXX) $(CXXFLAGS) -DTESTING -o test_logappend.exe $(TEST) logappend.cpp
	./test_logappend.exe


	