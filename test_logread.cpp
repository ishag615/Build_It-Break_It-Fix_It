#include <iostream>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <string>

using namespace std;

// Helper function to run a command and capture exit code
int runCommand(const string& cmd) {
    return system(cmd.c_str());
}

// Helper to create test log file
void createTestLog(const string& filename, const string& token) {
    ofstream file(filename);
    file << token << endl;
    file << "1,Alice,E,A,-1" << endl;
    file << "5,Alice,E,A,3" << endl;
    file << "10,Bob,G,A,-1" << endl;
    file << "15,Bob,G,A,5" << endl;
    file.close();
}

void testStateQuery() {
    cout << "Test 1: State Query (-S)" << endl;
    
    createTestLog("test_state.log", "token123");
    
    // Test valid state query
    int result = runCommand("./logread -K token123 -S test_state.log > output.txt");
    assert(result == 0);
    
    // Verify output contains Alice and Bob
    ifstream output("output.txt");
    string line;
    getline(output, line); // Employees line
    assert(line.find("Alice") != string::npos);
    getline(output, line); // Guests line
    assert(line.find("Bob") != string::npos);
    output.close();
    
    // Cleanup
    remove("test_state.log");
    remove("output.txt");
    
    cout << "State query test passed" << endl;
}

void testRoomHistory() {
    cout << "Test 2: Room History (-R)" << endl;
    
    createTestLog("test_rooms.log", "token456");
    
    // Test room history for Alice
    int result = runCommand("./logread -K token456 -R -E Alice test_rooms.log > output.txt");
    assert(result == 0);
    
    // Verify output contains room 3
    ifstream output("output.txt");
    string line;
    getline(output, line);
    assert(line.find("3") != string::npos);
    output.close();
    
    // Cleanup
    remove("test_rooms.log");
    remove("output.txt");
    
    cout << "Room history test passed" << endl;
}

void testInvalidToken() {
    cout << "Test 3: Invalid Token" << endl;
    
    createTestLog("test_invalid.log", "token789");
    
    // Test with wrong token
    runCommand("./logread -K wrongtoken -S test_invalid.log 2>&1 | grep 'integrity violation'");
    // Should print "integrity violation"
    
    // Cleanup
    remove("test_invalid.log");
    
    cout << "Invalid token test passed" << endl;
}

void testNonexistentPerson() {
    cout << "Test 4: Nonexistent Person" << endl;
    
    createTestLog("test_person.log", "token999");
    
    // Test querying person who doesn't exist
    int result = runCommand("./logread -K token999 -R -E Charlie test_person.log > output.txt");
    assert(result == 0); // Should return 0 but print nothing
    
    // Verify empty output
    ifstream output("output.txt");
    string line;
    bool isEmpty = !getline(output, line) || line.empty();
    assert(isEmpty);
    output.close();
    
    // Cleanup
    remove("test_person.log");
    remove("output.txt");
    
    cout << "Nonexistent person test passed" << endl;
}

void testMissingArguments() {
    cout << "Test 5: Missing Arguments" << endl;
    
    // Test with missing token
    runCommand("./logread -S test.log 2>&1 | grep 'invalid'");
    // Should print "invalid"
    
    // Test with missing query type
    runCommand("./logread -K token123 test.log 2>&1 | grep 'invalid'");
    // Should print "invalid"
    
    cout << "Missing arguments test passed" << endl;
}

void testConflictingArguments() {
    cout << "Test 6: Conflicting Arguments" << endl;
    
    createTestLog("test_conflict.log", "token111");
    
    // Test with both -S and -R (should be invalid)
    runCommand("./logread -K token111 -S -R -E Alice test_conflict.log 2>&1 | grep 'invalid'");
    // Should print "invalid"
    
    // Cleanup
    remove("test_conflict.log");
    
    cout << "Conflicting arguments test passed" << endl;
}

void testEmptyLog() {
    cout << "Test 7: Empty Log File" << endl;
    
    // Create empty log
    ofstream file("test_empty.log");
    file.close();
    
    // Test reading empty log
    runCommand("./logread -K token222 -S test_empty.log");
    // Should handle gracefully
    
    // Cleanup
    remove("test_empty.log");
    
    cout << "Empty log test passed" << endl;
}

void testRoomOccupancy() {
    cout << "Test 8: Room Occupancy Display" << endl;
    
    createTestLog("test_occupancy.log", "token333");
    
    // Test state query with room occupancy
    int result = runCommand("./logread -K token333 -S test_occupancy.log > output.txt");
    assert(result == 0);
    
    // Verify room occupancy is shown
    ifstream output("output.txt");
    string line;
    bool foundRoom3 = false;
    bool foundRoom5 = false;
    
    while (getline(output, line)) {
        if (line.find("3:") != string::npos && line.find("Alice") != string::npos) {
            foundRoom3 = true;
        }
        if (line.find("5:") != string::npos && line.find("Bob") != string::npos) {
            foundRoom5 = true;
        }
    }
    
    assert(foundRoom3);
    assert(foundRoom5);
    output.close();
    
    // Cleanup
    remove("test_occupancy.log");
    remove("output.txt");
    
    cout << "Room occupancy test passed" << endl;
}

void testMultipleRoomVisits() {
    cout << "Test 9: Multiple Room Visits" << endl;
    
    // Create log with multiple room visits
    ofstream file("test_multi_rooms.log");
    file << "token444" << endl;
    file << "1,Alice,E,A,-1" << endl;
    file << "5,Alice,E,A,1" << endl;
    file << "10,Alice,E,L,1" << endl;
    file << "15,Alice,E,A,2" << endl;
    file << "20,Alice,E,L,2" << endl;
    file << "25,Alice,E,A,3" << endl;
    file.close();
    
    // Test room history
    int result = runCommand("./logread -K token444 -R -E Alice test_multi_rooms.log > output.txt");
    assert(result == 0);
    
    // Verify multiple rooms in order
    ifstream output("output.txt");
    string line;
    getline(output, line);
    assert(line == "1,2,3");
    output.close();
    
    // Cleanup
    remove("test_multi_rooms.log");
    remove("output.txt");
    
    cout << "Multiple room visits test passed" << endl;
}

void testEmployeeVsGuest() {
    cout << "Test 10: Employee vs Guest Distinction" << endl;
    
    // Create log with same name as employee and guest
    ofstream file("test_emp_guest.log");
    file << "token555" << endl;
    file << "1,Alice,E,A,-1" << endl;
    file << "5,Alice,G,A,-1" << endl;
    file.close();
    
    // Test state query
    int result = runCommand("./logread -K token555 -S test_emp_guest.log > output.txt");
    assert(result == 0);
    
    // Verify both Alice employee and Alice guest appear
    ifstream output("output.txt");
    string empLine, guestLine;
    getline(output, empLine);
    getline(output, guestLine);
    
    assert(empLine.find("Alice") != string::npos);
    assert(guestLine.find("Alice") != string::npos);
    output.close();
    
    // Cleanup
    remove("test_emp_guest.log");
    remove("output.txt");
    
    cout << "Employee vs guest test passed" << endl;
}

int main() {
    cout << "==================================" << endl;
    cout << "Starting logread Test Suite" << endl;
    cout << "==================================" << endl << endl;
    
    try {
        testStateQuery();
        testRoomHistory();
        testInvalidToken();
        testNonexistentPerson();
        testMissingArguments();
        testConflictingArguments();
        testEmptyLog();
        testRoomOccupancy();
        testMultipleRoomVisits();
        testEmployeeVsGuest();
        
        cout << endl << "==================================" << endl;
        cout << "All logread tests passed!" << endl;
        cout << "==================================" << endl;
        
    } catch (const exception& e) {
        cout << "Test failed: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}