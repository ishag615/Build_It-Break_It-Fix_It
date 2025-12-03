#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <cstdlib>
#include <regex>
#include "logappend.hpp"
#include <unistd.h> // for close() function
#include <fcntl.h> // for O_CREAT, O_WRONLY, O_APPEND
#include <sys/file.h> //for flock()
#include "encryption.hpp"

using namespace std;

//regex patterns for validation
regex tokenPattern("^[a-zA-Z0-9]+$");
regex employeeNamePattern("^[a-zA-Z]{1,19}$"); 
regex guestNamePattern("^[a-zA-Z]{1,19}$");

//Add constructor to initialize variables
LogAppend::LogAppend() :
    timestamp(-1),
    isEmployee(false),
    isArrival(false),
    roomId(-1),
    hasRoom(false),
    lastTimestamp(0) {}

//Token validation with character checking
bool LogAppend::validateToken(const string& tok) {
    if (tok.empty()) return false;
    
    //checking each character against regex
    for (size_t i = 0; i < tok.length(); i++) {
        char c = tok[i];
        if (!(regex_match(string(1, c), tokenPattern))) { 
            return false;
        }
    }
    return true;
}

//boundschecking and regex checking for validation
bool LogAppend::validateName(const string& name) {
    if (name.empty() || name.length() >= 20) {  // boundschecking length
        return false;
    }
    
    // Alphabetic only: a-z, A-Z
    for (size_t i = 0; i < name.length(); i++) {
        char c = name[i];
        if (!(regex_match(string(1, c), employeeNamePattern) || regex_match(string(1, c), guestNamePattern))) { //checking regex for both employee and guest names
            return false;
        }
    }
    return true;
}

//check filename for .. , // and '\'

bool LogAppend::checkFileName(const string& fileName) {
    if (fileName.find("..") != string::npos || fileName.find("//") != string::npos || fileName.find('\\') != string::npos) {  
        return false;
    }
    return true;
}   

//Timestamp validation
bool LogAppend::validateTimestamp(long long ts) {
    if (ts < 1 || ts > 1073741823) { //making sure timestamp is in valid range
        cout << "Timestamp is invalid" << endl;
        return false;
    }
    if (ts <= lastTimestamp) {
        cout << "Time is lesser than previous timestamp" << endl;
        return false;
    }
    return true;
}

// Room validation 
bool LogAppend::validateRoomId(long long room) {
    if (room < -1 || room > 7) { //making sure room is between -1 and 7
        cout << "Room number out of range" << endl;
        return false;
    }
    return true;
}

bool LogAppend::readExistingLog() {
    ifstream inFile(logFileName.c_str());
    if (!inFile.is_open()) {
        return true; 
    }

    // Creating decryption object
    AESEncryption decrypt(token);

    if (!(checkFileName(logFileName))) {
        cout << "Resource injection attempt detected" << endl;
        inFile.close();
        return false;
    }

    //write only, creating file if doesnt exist. 0602- owner may read/write- mode is append, others can write only
    int filePath = open(logFileName.c_str(), O_RDONLY, 0602); //read only

    flock(filePath, 2); // 2-locks file for reading only

    // Read token from first line
    string fileToken;
    if (!getline(inFile, fileToken)) {
        inFile.close();
        return true; //to handle empty file
    }
    
    // Verify token matches
    if (fileToken != token) {
        cout << "Token mismatch" << endl;
        inFile.close();
        return false;
    }
    
    // Read all entries and reconstruct state
    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;
        
        // Decrypting ciphertext
        string plainTextLog = decrypt.decrypt(line);
        
        // Parse CSV: timestamp,name,E/G,A/L,roomId
        vector<string> parts;
        string temp = plainTextLog; //assigning decreypted text to temp variable
        size_t pos;
        
        // Split by comma
        while ((pos = temp.find(',')) != string::npos) {
            parts.push_back(temp.substr(0, pos));
            temp.erase(0, pos + 1);
        }
        parts.push_back(temp);
        
        if (parts.size() < 5) continue;
        
         //extracting fields from log file
        long long ts = stoll(parts[0].c_str()); //using stoll to handle long long
        string name = parts[1];
        bool isEmp = (parts[2] == "E");
        bool isArr = (parts[3] == "A");
        long long room = stoll(parts[4].c_str());
        
        if (ts > lastTimestamp) {
            lastTimestamp = ts; //updating last timestamp
        }
        
        //getting correct state
        map<string, PersonState>& stateMap = isEmp ? employeeStates : guestStates;
        PersonState& state = stateMap[name];
        
        if (state.name.empty()) {
            state.name = name;
        }
        
        // Replay this event
        if (isArr) {
            if (room == -1) {
                state.inGallery = true;
                state.currentRoom = -1;
                state.galleryEntryTime = ts;
            } else {
                state.currentRoom = room;
                state.roomsVisited.push_back(room);
            }
        } else {
            if (room == -1) {
                state.inGallery = false;
                state.totalTimeInGallery += (ts - state.galleryEntryTime);
                state.currentRoom = -1;
            } else {
                state.currentRoom = -1;
            }
        }
    }
    
    flock(filePath, 8); // 8-unlocks
    close(filePath); //closing file after unlocking
    inFile.close();
    return true;
}

bool LogAppend::validateStateTransition() {
    //Check if person exists in opposite category
    if (isEmployee) {
        if (guestStates.find(personName) != guestStates.end()) {
            cout << "Cannot be employee and guest" << endl;
            return false;
        }
    } else {
        if (employeeStates.find(personName) != employeeStates.end()) {
            cout << "Cannot be employee and guest" << endl;
            return false;
        }
    }
    
    //Get person's current state
    map<string, PersonState>& stateMap = isEmployee ? employeeStates : guestStates;
    PersonState& state = stateMap[personName];
    
    if (state.name.empty()) {
        state.name = personName;
    }
    
    //Validate the transition
    if (isArrival) {
        if (!hasRoom) {
            // Arriving at gallery
            if (state.inGallery) { 
                cout << "Already in gallery" << endl;
                return false;
            }
        } else {
            // Arriving at specific room
            if (!state.inGallery) {
                cout << "Please enter gallery before entering a room" << endl;
                return false; // Must enter gallery first
            }
            if (state.currentRoom != -1) {
                cout << "Please leave previous room before entering a new one" << endl;
                return false; // Must leave previous room first
            }
        }
    } else {
        // Departure
        if (!hasRoom) {
            // Leaving gallery
            if (!state.inGallery) {
                cout << "Not in gallery" << endl;
                return false;
            }
            if (state.currentRoom != -1) {
                cout << "Please leave room before leaving gallery" << endl;
                return false; // Must leave room before leaving gallery
            }
        } else {
            // Leaving specific room
            if (state.currentRoom != roomId) {
                cout << "Please leave the correct room" << endl;
                return false;
            }
        }
    }
    
    return true;
}

// Append new entry to log file
bool LogAppend::writeToLog() {
    ifstream checkFile(logFileName.c_str());
    bool fileExists = checkFile.good();
    checkFile.close();

    if(!checkFileName(logFileName)) { //checking for resource injection
        cout << "Resource injection attempt detected" << endl;
        return false;
    }

    //write only, creating file if doesnt exist. 0602- owner may read/write- mode is append, others can write only
    int filePath = open(logFileName.c_str(), O_WRONLY | O_APPEND |O_CREAT, 0602); 

    flock(filePath, 2); // 2-locks

    // making logentry and converting timestamp and roomId to string
    string logEntry = to_string(timestamp) + "," + personName + "," + (isEmployee ? "E" : "G") + "," + (isArrival ? "A" : "L") + "," + to_string(roomId);
    
    // Creating encryption object
    AESEncryption encryptObj(token);

    // encrypting log entry
    string encryptedLog = encryptObj.encrypt(logEntry);
    
    ofstream outFile;
    
    if (!fileExists) {
        outFile.open(logFileName.c_str());
        if (!outFile.is_open()) {
            cout << "File does not open" << endl;
            return false;
        }
        outFile << token << endl;
    } else {
        outFile.open(logFileName.c_str(), ios::app);
        if (!outFile.is_open()) {
            cout << "File does not open" << endl;
            return false;
        }
    }
    
    //writing encrypted log to file
    outFile << encryptedLog << endl;

    flock(filePath, 8); // 8-unlocks
    close(filePath); //closing file after unlocking

    outFile.close();
    return true;
}

bool LogAppend::processArguments(int argc, char* argv[]) {
    // Argument count validation
    if (argc < 2 || argc > 40) { //bounds checking arguments
        cout << "Too many arguments" << endl;
        return false;
    }
    
    bool hasTimestamp = false, hasToken = false, hasPerson = false, hasAction = false;
    bool hasEmployee = false, hasGuest = false, hasArrival = false, hasDeparture = false;
    
    // Parse arguments
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        
        if (arg == "-T" && i + 1 < argc) {
            timestamp = stoll(argv[++i]);
            hasTimestamp = true;
        }
        else if (arg == "-K" && i + 1 < argc) {
            token = argv[++i];
            if (!validateToken(token)) {
                cout << "Invalid Token entered" << endl;
                return false;
            }
            hasToken = true;
        }
        else if (arg == "-E" && i + 1 < argc) {
            personName = argv[++i];
            if (!validateName(personName)) {
                cout << "Invalid Name entered" << endl;
                return false;
            }
            isEmployee = true;
            hasEmployee = true;
            hasPerson = true;
        }
        else if (arg == "-G" && i + 1 < argc) {
            personName = argv[++i];
            if (!validateName(personName)) {
                cout << "Invalid Name entered" << endl;
                return false;
            }
            isEmployee = false;
            hasGuest = true;
            hasPerson = true;
        }
        else if (arg == "-A") {
            isArrival = true;
            hasArrival = true;
            hasAction = true;
        }
        else if (arg == "-L") {
            isArrival = false;
            hasDeparture = true;
            hasAction = true;
        }
        else if (arg == "-R" && i + 1 < argc) {
            roomId = stoll(argv[++i]);
            if (!validateRoomId(roomId)) {
                cout << "Invalid Room ID entered" << endl;
                return false;
            }
            hasRoom = true;
        }
        else if (arg == "-B") {
            return false;
        }
        else if (arg[0] != '-') {
            logFileName = arg;
        }
    }
    
    // check for missing argumers
    if (!hasTimestamp || !hasToken || !hasPerson || !hasAction || logFileName.empty()) {
        cout << "Missing information in log format" << endl;
        return false;
    }
    
    // Check conflicts
    if ((hasEmployee && hasGuest) || (hasArrival && hasDeparture)) {
        cout << "Cannot be employee and guest" << endl;
        return false;
    }
    
    if (!hasRoom) {
        roomId = -1;
    }
    
    if (!readExistingLog()) return false;

    if (!validateTimestamp(timestamp)) { //validate timestamp after reading log;
        return false;
    }
    
    if (!validateStateTransition()) return false;
    if (!writeToLog()) return false;
    
    return true;
}


#ifndef TESTING //to avoid using this main when testing
int main(int argc, char* argv[]) {
    cout << "Welcome to A&I museum system! Enter a log/sequence of actions to begin" << endl;
    cout << "Please use the following format: -T <timestamp> -K <token> -E <employee_name> -A -R <room_id> <log_file>" << endl;
    
    LogAppend log;
    
    if (!log.processArguments(argc, argv)) {
        return 255;
    }
    cout << "Done Logging" << endl;
    
    return 0;
}
#endif
