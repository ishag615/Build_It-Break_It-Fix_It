# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
LDFLAGS = -lssl -lcrypto

# Target executable 
TARGET = logappend
SRC = logappend.cpp encryption.cpp
TEST = test_logappend.cpp

# Default rule
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

# Clean rule
clean:
	rm -f $(TARGET)
	rm -f test_logappend

# run tests
test: $(TEST) $(SRC)
	$(CXX) $(CXXFLAGS) -DTESTING -o test_logappend $(TEST) $(SRC)
	./test_logappend
