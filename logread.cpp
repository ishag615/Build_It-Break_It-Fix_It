#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <regex>

using namespace std;

// Structure to represent a log entry
struct LogEntry {
    int timestamp;
    string name;
    bool isEmployee;  // true for employee, false for guest
    bool isArrival;   // true for arrival, false for departure
    int roomId;       // -1 for gallery, >= 0 for specific room
};

// Structure to track person's state
struct PersonState {
    bool inGallery;
    int currentRoom;  // -1 if in gallery but no specific room
    vector<int> roomsVisited;
    int entryTime;    // Time entered gallery
    int totalTime;    // Total time spent
};

class LogReader {
private:
    string token;
    string logFile;
    vector<LogEntry> entries;
    map<string, PersonState> employeeStates;
    map<string, PersonState> guestStates;
    regex tokenPattern;
    regex namePattern;
    
    bool validateToken(const string& tok);
    bool validateName(const string& name);
    bool readLog();
    void processEntries();
    void printState();
    void printRooms(const string& name, bool isEmployee);
    void printTime(const string& name, bool isEmployee);
    void printIntersection(const vector<pair<string, bool>>& persons);
    
public:
    LogReader();
    int execute(int argc, char* argv[]);
};

LogReader::LogReader() : 
    tokenPattern("^[A-Za-z0-9]+$"),
    namePattern("^[A-Za-z]+$") {}

bool LogReader::validateToken(const string& tok) {
    return regex_match(tok, tokenPattern);
}

bool LogReader::validateName(const string& name) {
    return regex_match(name, namePattern) && !name.empty();
}

bool LogReader::readLog() {
    ifstream inFile(logFile);
    if (!inFile.is_open()) {
        return false;
    }
    
    // Read and verify token
    string fileToken;
    if (!getline(inFile, fileToken) || fileToken != token) {
        cout << "integrity violation" << endl;
        return false;
    }
    
    // Read log entries
    string line;
    while (getline(inFile, line)) {
        // Parse log entry format: timestamp,name,type,action,room
        LogEntry entry;
        
        // This is a placeholder 
        size_t pos = 0;
        string delimiter = ",";
        vector<string> tokens;
        string temp = line;
        
        while ((pos = temp.find(delimiter)) != string::npos) {
            tokens.push_back(temp.substr(0, pos));
            temp.erase(0, pos + delimiter.length());
        }
        tokens.push_back(temp);
        
        if (tokens.size() < 5) continue;
        
        entry.timestamp = stoi(tokens[0]);
        entry.name = tokens[1];
        entry.isEmployee = (tokens[2] == "E");
        entry.isArrival = (tokens[3] == "A");
        entry.roomId = stoi(tokens[4]);
        
        entries.push_back(entry);
    }
    
    inFile.close();
    return true;
}

void LogReader::processEntries() {
    for (const auto& entry : entries) {
        map<string, PersonState>& stateMap = entry.isEmployee ? employeeStates : guestStates;
        PersonState& state = stateMap[entry.name];
        
        if (entry.isArrival) {
            if (entry.roomId == -1) {
                // Entering gallery
                state.inGallery = true;
                state.currentRoom = -1;
                state.entryTime = entry.timestamp;
            } else {
                // Entering a room
                state.currentRoom = entry.roomId;
                if (find(state.roomsVisited.begin(), state.roomsVisited.end(), entry.roomId) == state.roomsVisited.end()) {
                    state.roomsVisited.push_back(entry.roomId);
                }
            }
        } else {
            // Departure
            if (entry.roomId == -1) {
                // Leaving gallery
                state.inGallery = false;
                state.totalTime += (entry.timestamp - state.entryTime);
                state.currentRoom = -1;
            } else {
                // Leaving a room
                state.currentRoom = -1;
            }
        }
    }
}

void LogReader::printState() {
    // Print employees in gallery
    vector<string> employeesInGallery;
    for (const auto& pair : employeeStates) {
        if (pair.second.inGallery) {
            employeesInGallery.push_back(pair.first);
        }
    }
    sort(employeesInGallery.begin(), employeesInGallery.end());
    
    for (size_t i = 0; i < employeesInGallery.size(); i++) {
        cout << employeesInGallery[i];
        if (i < employeesInGallery.size() - 1) cout << ",";
    }
    cout << endl;
    
    // Print guests in gallery
    vector<string> guestsInGallery;
    for (const auto& pair : guestStates) {
        if (pair.second.inGallery) {
            guestsInGallery.push_back(pair.first);
        }
    }
    sort(guestsInGallery.begin(), guestsInGallery.end());
    
    for (size_t i = 0; i < guestsInGallery.size(); i++) {
        cout << guestsInGallery[i];
        if (i < guestsInGallery.size() - 1) cout << ",";
    }
    cout << endl;
    
    // Print room occupancy
    map<int, vector<string>> roomOccupancy;
    
    for (const auto& pair : employeeStates) {
        if (pair.second.inGallery && pair.second.currentRoom >= 0) {
            roomOccupancy[pair.second.currentRoom].push_back(pair.first);
        }
    }
    
    for (const auto& pair : guestStates) {
        if (pair.second.inGallery && pair.second.currentRoom >= 0) {
            roomOccupancy[pair.second.currentRoom].push_back(pair.first);
        }
    }
    
    for (auto& pair : roomOccupancy) {
        sort(pair.second.begin(), pair.second.end());
        cout << pair.first << ": ";
        for (size_t i = 0; i < pair.second.size(); i++) {
            cout << pair.second[i];
            if (i < pair.second.size() - 1) cout << ",";
        }
        cout << endl;
    }
}

void LogReader::printRooms(const string& name, bool isEmployee) {
    const map<string, PersonState>& stateMap = isEmployee ? employeeStates : guestStates;
    
    auto it = stateMap.find(name);
    if (it == stateMap.end() || it->second.roomsVisited.empty()) {
        return;
    }
    
    const vector<int>& rooms = it->second.roomsVisited;
    for (size_t i = 0; i < rooms.size(); i++) {
        cout << rooms[i];
        if (i < rooms.size() - 1) cout << ",";
    }
    cout << endl;
}

void LogReader::printTime(const string& name, bool isEmployee) {
    const map<string, PersonState>& stateMap = isEmployee ? employeeStates : guestStates;
    
    auto it = stateMap.find(name);
    if (it == stateMap.end()) {
        return;
    }
    
    int totalTime = it->second.totalTime;
    if (it->second.inGallery) {
        // Still in gallery, add current time
        totalTime += (entries.back().timestamp - it->second.entryTime);
    }
    
    cout << totalTime << endl;
}

void LogReader::printIntersection(const vector<pair<string, bool>>& persons) {
    cout << "unimplemented" << endl;
}

int LogReader::execute(int argc, char* argv[]) {
    if (argc < 4) {
        cout << "invalid" << endl;
        return 255;
    }
    
    bool hasS = false, hasR = false, hasT = false, hasI = false;
    string queryName;
    bool queryIsEmployee = false;
    vector<pair<string, bool>> intersectionPersons;
    
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        
        if (arg == "-K") {
            if (i + 1 >= argc) {
                cout << "invalid" << endl;
                return 255;
            }
            token = argv[++i];
            if (!validateToken(token)) {
                cout << "invalid" << endl;
                return 255;
            }
        }
        else if (arg == "-S") {
            hasS = true;
        }
        else if (arg == "-R") {
            hasR = true;
        }
        else if (arg == "-T") {
            hasT = true;
        }
        else if (arg == "-I") {
            hasI = true;
        }
        else if (arg == "-E") {
            if (i + 1 >= argc) {
                cout << "invalid" << endl;
                return 255;
            }
            string name = argv[++i];
            if (!validateName(name)) {
                cout << "invalid" << endl;
                return 255;
            }
            if (hasI) {
                intersectionPersons.push_back({name, true});
            } else {
                queryName = name;
                queryIsEmployee = true;
            }
        }
        else if (arg == "-G") {
            if (i + 1 >= argc) {
                cout << "invalid" << endl;
                return 255;
            }
            string name = argv[++i];
            if (!validateName(name)) {
                cout << "invalid" << endl;
                return 255;
            }
            if (hasI) {
                intersectionPersons.push_back({name, false});
            } else {
                queryName = name;
                queryIsEmployee = false;
            }
        }
        else if (arg[0] != '-') {
            logFile = arg;
        }
    }
    
    // Validate arguments
    int queryCount = (hasS ? 1 : 0) + (hasR ? 1 : 0) + (hasT ? 1 : 0) + (hasI ? 1 : 0);
    if (queryCount != 1) {
        cout << "invalid" << endl;
        return 255;
    }
    
    if (token.empty() || logFile.empty()) {
        cout << "invalid" << endl;
        return 255;
    }
    
    // Read and process log
    if (!readLog()) {
        return 255;
    }
    
    processEntries();
    
    // Execute query
    if (hasS) {
        printState();
    } else if (hasR) {
        if (queryName.empty()) {
            cout << "invalid" << endl;
            return 255;
        }
        printRooms(queryName, queryIsEmployee);
    } else if (hasT) {
        if (queryName.empty()) {
            cout << "invalid" << endl;
            return 255;
        }
        printTime(queryName, queryIsEmployee);
    } else if (hasI) {
        if (intersectionPersons.empty()) {
            cout << "invalid" << endl;
            return 255;
        }
        printIntersection(intersectionPersons);
    }
    
    return 0;
}

int main(int argc, char* argv[]) {
    LogReader reader;
    return reader.execute(argc, argv);
}
