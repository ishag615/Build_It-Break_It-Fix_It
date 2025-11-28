#ifndef LOGAPPEND_HPP
#define LOGAPPEND_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <cstdlib>
#include <regex>

using namespace std;

struct PersonState {
    string name;
    bool inGallery;
    int currentRoom;              // -1 = gallery, 0-7 = specific rooms
    vector<int> roomsVisited;
    int galleryEntryTime;
    int totalTimeInGallery;
    
    PersonState() : 
        name(""),
        inGallery(false), 
        currentRoom(-1),
        galleryEntryTime(0),
        totalTimeInGallery(0) {}
};

class LogAppend {
private:
    // Command line inputs
    string token;
    string logFileName;
    long long timestamp;
    string personName;
    bool isEmployee;
    bool isArrival;
    int roomId;
    bool hasRoom;
    
    // State tracking
    long long lastTimestamp;
    map<string, PersonState> employeeStates;
    map<string, PersonState> guestStates;
    
    // Core methods
    bool readExistingLog();
    bool validateStateTransition();
    bool writeToLog();
   
    // Test methods
    void testRoomValidation();
    void testNameValidation();
    void testTokenValidation();
    void testTimestampValidation();
    void executeTests();
    
public:
    LogAppend();
    //security checks
    bool validateToken(const string& tok);
    bool validateName(const string& name);
    bool validateTimestamp(long long ts);
    bool validateRoomId(int room);
    bool checkFileName(const string& filename);
    bool processArguments(int argc, char* argv[]);
};

#endif 