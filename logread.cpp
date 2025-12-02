#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <cstdlib>
#include <regex>

using namespace std;

regex tokenPattern("^[a-zA-Z0-9]+$");
regex namePattern("^[a-zA-Z]{1,19}$");

struct PersonState {
    bool inGallery;
    int currentRoom;
    vector<int> roomsVisited;
    
    PersonState() : inGallery(false), currentRoom(-1) {}
};

class LogAppend {
private:
    string token;
    string logFileName;
    long long timestamp;
    string personName;
    bool isEmployee;
    bool isArrival;
    int roomId;
    bool hasRoom;
    
    long long lastTimestamp;
    map<string, PersonState> employeeStates;
    map<string, PersonState> guestStates;
    
    bool validateToken(const string& tok);
    bool validateName(const string& name);
    bool readExistingLog();
    bool validateStateTransition();
    bool writeToLog();
    
public:
    LogAppend();
    bool processArguments(int argc, char* argv[]);
};

LogAppend::LogAppend() : 
    timestamp(-1),
    isEmployee(false),
    isArrival(false),
    roomId(-1),
    hasRoom(false),
    lastTimestamp(0) {}

bool LogAppend::validateToken(const string& tok) {
    if (tok.empty()) return false;
    for (size_t i = 0; i < tok.length(); i++) {
        char c = tok[i];
        if (!regex_match(string(1, c), tokenPattern)) {
            return false;
        }
    }
    return true;
}

bool LogAppend::validateName(const string& name) {
    if (name.empty()) return false;
    for (size_t i = 0; i < name.length(); i++) {
        char c = name[i];
        if (!regex_match(string(1, c), namePattern)) {
            return false;
        }
    }
    return true;
}

bool LogAppend::readExistingLog() {
    ifstream inFile(logFileName.c_str());
    if (!inFile.is_open()) {
        return true; // File doesn't exist yet
    }

    if(!logFileName.find("..") != string::npos || !logFileName.find("//") != string::npos) {
        cout << "Resource injection attempt detected" << endl;
        inFile.close();
        return false;
    }
    
    string fileToken;
    if (!getline(inFile, fileToken)) {
        inFile.close();
        return false;
    }
    
    if (fileToken != token) {
        cout << "invalid" << endl;
        inFile.close();
        return false;
    }
    
    // Read all entries to reconstruct state
    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;
        
        vector<string> tokens;
        string temp = line;
        size_t pos;
        
        while ((pos = temp.find(',')) != string::npos) {
            tokens.push_back(temp.substr(0, pos));
            temp.erase(0, pos + 1);
        }
        tokens.push_back(temp);
        
        if (tokens.size() < 5) continue;
        
        long long ts = atoll(tokens[0].c_str());
        string name = tokens[1];
        bool isEmp = (tokens[2] == "E");
        bool isArr = (tokens[3] == "A");
        int room = atoi(tokens[4].c_str());
        
        if (ts > lastTimestamp) {
            lastTimestamp = ts;
        }
        
        // Update state
        map<string, PersonState>& stateMap = isEmp ? employeeStates : guestStates;
        PersonState& state = stateMap[name];
        
        if (isArr) {
            if (room == -1) {
                state.inGallery = true;
                state.currentRoom = -1;
            } else {
                state.currentRoom = room;
                state.roomsVisited.push_back(room);
            }
        } else {
            if (room == -1) {
                state.inGallery = false;
                state.currentRoom = -1;
            } else {
                state.currentRoom = -1;
            }
        }
    }
    
    inFile.close();
    return true;
}

bool LogAppend::validateStateTransition() {
    map<string, PersonState>& stateMap = isEmployee ? employeeStates : guestStates;
    PersonState& state = stateMap[personName];
    
    if (isArrival) {
        if (!hasRoom) {
            // Arriving at gallery
            if (state.inGallery) {
                cout << "invalid" << endl;
                return false; // Already in gallery
            }
        } else {
            // Arriving at specific room
            if (!state.inGallery) {
                cout << "invalid" << endl;
                return false; // Must enter gallery first
            }
            if (state.currentRoom != -1) {
                cout << "invalid" << endl;
                return false; // Must leave previous room first
            }
        }
    } else {
        // Departure
        if (!hasRoom) {
            // Leaving gallery
            if (!state.inGallery) {
                cout << "invalid" << endl;
                return false; // Not in gallery
            }
            if (state.currentRoom != -1) {
                cout << "invalid" << endl;
                return false; // Must leave room first
            }
        } else {
            // Leaving specific room
            if (state.currentRoom != roomId) {
                cout << "invalid" << endl;
                return false; // Not in that room
            }
        }
    }
    
    return true;
}

bool LogAppend::writeToLog() {
    ifstream checkFile(logFileName.c_str());
    bool fileExists = checkFile.good();
    checkFile.close();
    
    ofstream outFile;
    
    if (!fileExists) {
        outFile.open(logFileName.c_str());
        if (!outFile.is_open()) {
            cout << "invalid" << endl;
            return false;
        }
        outFile << token << endl;
    } else {
        outFile.open(logFileName.c_str(), ios::app);
        if (!outFile.is_open()) {
            cout << "invalid" << endl;
            return false;
        }
    }
    
    outFile << timestamp << ","
            << personName << ","
            << (isEmployee ? "E" : "G") << ","
            << (isArrival ? "A" : "L") << ","
            << roomId << endl;
    
    outFile.close();
    return true;
}

bool LogAppend::processArguments(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "invalid" << endl;
        return false;
    }
    
    bool hasTimestamp = false;
    bool hasToken = false;
    bool hasPerson = false;
    bool hasAction = false;
    bool hasEmployee = false;
    bool hasGuest = false;
    bool hasArrival = false;
    bool hasDeparture = false;
    
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        
        if (arg == "-T") {
            if (i + 1 >= argc) {
                cout << "invalid" << endl;
                return false;
            }
            timestamp = stoll(argv[++i]);
            hasTimestamp = true;
        }
        else if (arg == "-K") {
            if (i + 1 >= argc) {
                cout << "invalid" << endl;
                return false;
            }
            token = argv[++i];
            if (!validateToken(token)) {
                cout << "invalid" << endl;
                return false;
            }
            hasToken = true;
        }
        else if (arg == "-E") {
            if (i + 1 >= argc) {
                cout << "invalid" << endl;
                return false;
            }
            personName = argv[++i];
            if (!validateName(personName)) {
                cout << "invalid" << endl;
                return false;
            }
            isEmployee = true;
            hasEmployee = true;
            hasPerson = true;
        }
        else if (arg == "-G") {
            if (i + 1 >= argc) {
                cout << "invalid" << endl;
                return false;
            }
            personName = argv[++i];
            if (!validateName(personName)) {
                cout << "invalid" << endl;
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
        else if (arg == "-R") {
            if (i + 1 >= argc) {
                cout << "invalid" << endl;
                return false;
            }
            roomId = atoi(argv[++i]);
            if (roomId < 0 || roomId > 1073741823) {
                cout << "invalid" << endl;
                return false;
            }
            hasRoom = true;
        }
        else if (arg == "-B") {
            cout << "invalid" << endl;
            return false; // Batch not implemented yet
        }
        else if (arg[0] != '-') {
            logFileName = arg;
        }
    }
    
    // Validate required arguments
    if (!hasTimestamp || !hasToken || !hasPerson || !hasAction) {
        cout << "invalid" << endl;
        return false;
    }
    
    if (hasEmployee && hasGuest) {
        cout << "invalid" << endl;
        return false;
    }
    
    if (hasArrival && hasDeparture) {
        cout << "invalid" << endl;
        return false;
    }
    
    if (logFileName.empty()) {
        cout << "invalid" << endl;
        return false;
    }
    
    if (timestamp < 1 || timestamp > 1073741823) {
        cout << "invalid" << endl;
        return false;
    }
    
    if (!hasRoom) {
        roomId = -1;
    }
    
    // Read existing log
    if (!readExistingLog()) {
        return false;
    }
    
    // Validate timestamp
    if (timestamp <= lastTimestamp) {
        cout << "invalid" << endl;
        return false;
    }
    
    // Validate state transition
    if (!validateStateTransition()) {
        return false;
    }
    
    // Write to log
    if (!writeToLog()) {
        return false;
    }
    
    return true;
}


int main(int argc, char* argv[]) {
    LogAppend log;
    
    if (!log.processArguments(argc, argv)) {
        return 255;
    }
    
    return 0;
}
