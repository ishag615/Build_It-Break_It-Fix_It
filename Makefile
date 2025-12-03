# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
LDFLAGS = -lssl -lcrypto

# Targets and source files
TARGET = logappend
LOGREAD = logread
SRC = logappend.cpp encryption.cpp
LOGREAD_SRC = logread.cpp encryption.cpp
TEST_LOGAPPEND = test_logappend.cpp
TEST_LOGREAD = test_logread.cpp

# Build targets
all: $(TARGET) $(LOGREAD)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

$(LOGREAD): $(LOGREAD_SRC)
	$(CXX) $(CXXFLAGS) -o $(LOGREAD) $(LOGREAD_SRC) $(LDFLAGS)

# Clean output files
clean:
	rm -f $(TARGET)
	rm -f $(LOGREAD)
	rm -f test_logappend
	rm -f test_logread
	rm -f test_state.log test_rooms.log test_invalid.log test_person.log test_conflict.log
	rm -f test_empty.log test_occupancy.log test_multi_rooms.log test_emp_guest.log
	rm -f output.txt

# Run logappend unit tests
test_logappend: $(TEST_LOGAPPEND) $(SRC)
	$(CXX) $(CXXFLAGS) -DTESTING -o test_logappend $(TEST_LOGAPPEND) $(SRC) $(LDFLAGS)
	./test_logappend

# Run logread integration tests
test_logread: $(TEST_LOGREAD) $(LOGREAD)
	$(CXX) $(CXXFLAGS) -o test_logread $(TEST_LOGREAD)
	./test_logread

# Run all tests
test: test_logappend test_logread
	@echo ""
	@echo "===================================="
	@echo "All tests completed!"
	@echo "===================================="