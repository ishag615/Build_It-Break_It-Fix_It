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

class LogRead {
private:
    string token;
    string logFileName;
    string queryType; // "S" for state, "R" for room history
    string queryPerson;
    bool isQueryEmployee;
    
    map<string, PersonState> employeeStates;
    map<string, PersonState> guestStates;
    
    bool validateToken(const string& tok);
    bool checkFileName(const string& fileName);
    bool readExistingLog();
    void printStateQuery();
    void printRoomHistory();
    
public:
    LogRead();
    bool processArguments(int argc, char* argv[]);
};

LogRead::LogRead() : queryType(""), isQueryEmployee(false) {}

bool LogRead::validateToken(const string& tok) {
    if (tok.empty()) return false;
    for (size_t i = 0; i < tok.length(); i++) {
        char c = tok[i];
        if (!regex_match(string(1, c), tokenPattern)) {
            return false;
        }
    }
    return true;
}

bool LogRead::checkFileName(const string& fileName) {
    if (fileName.find("..") != string::npos || fileName.find("//") != string::npos || fileName.find('\\') != string::npos) {
        return false;
    }
    return true;
}

bool LogRead::readExistingLog() {
    ifstream inFile(logFileName.c_str());
    if (!inFile.is_open()) {
        cout << "invalid" << endl;
        return false;
    }

    if (!checkFileName(logFileName)) {
        cout << "Resource injection attempt detected" << endl;
        inFile.close();
        return false;
    }
    
    string fileToken;
    if (!getline(inFile, fileToken)) {
        cout << "invalid" << endl;
        inFile.close();
        return false;
    }
    
    if (fileToken != token) {
        cout << "integrity violation" << endl;
        inFile.close();
        return false;
    }
    
    // Read all entries to reconstruct state
    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;
        
        vector<string> parts;
        string temp = line;
        size_t pos;
        
        while ((pos = temp.find(',')) != string::npos) {
            parts.push_back(temp.substr(0, pos));
            temp.erase(0, pos + 1);
        }
        parts.push_back(temp);
        
        if (parts.size() < 5) continue;
        
        string name = parts[1];
        bool isEmp = (parts[2] == "E");
        bool isArr = (parts[3] == "A");
        int room = atoi(parts[4].c_str());
        
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

void LogRead::printStateQuery() {
    // Print employees
    cout << "Employees: ";
    for (auto& emp : employeeStates) {
        cout << emp.first;
        if (emp.second.currentRoom >= 0) {
            cout << " (Room " << emp.second.currentRoom << ")";
        } else if (emp.second.inGallery) {
            cout << " (Gallery)";
        }
        cout << " ";
    }
    cout << endl;
    
    // Print guests
    cout << "Guests: ";
    for (auto& guest : guestStates) {
        cout << guest.first;
        if (guest.second.currentRoom >= 0) {
            cout << " (Room " << guest.second.currentRoom << ")";
        } else if (guest.second.inGallery) {
            cout << " (Gallery)";
        }
        cout << " ";
    }
    cout << endl;
    
    // Print room occupancy
    map<int, vector<string>> roomOccupancy;
    for (auto& emp : employeeStates) {
        if (emp.second.currentRoom >= 0) {
            roomOccupancy[emp.second.currentRoom].push_back(emp.first);
        }
    }
    for (auto& guest : guestStates) {
        if (guest.second.currentRoom >= 0) {
            roomOccupancy[guest.second.currentRoom].push_back(guest.first);
        }
    }
    
    for (auto& room : roomOccupancy) {
        cout << "Room " << room.first << ": ";
        for (size_t i = 0; i < room.second.size(); i++) {
            cout << room.second[i];
            if (i < room.second.size() - 1) cout << ", ";
        }
        cout << endl;
    }
}

void LogRead::printRoomHistory() {
    map<string, PersonState>& stateMap = isQueryEmployee ? employeeStates : guestStates;
    
    if (stateMap.find(queryPerson) == stateMap.end()) {
        // Person not found - print nothing
        return;
    }
    
    PersonState& state = stateMap[queryPerson];
    
    // Print room history
    for (size_t i = 0; i < state.roomsVisited.size(); i++) {
        cout << state.roomsVisited[i];
        if (i < state.roomsVisited.size() - 1) {
            cout << ",";
        }
    }
    if (!state.roomsVisited.empty()) {
        cout << endl;
    }
}

bool LogRead::processArguments(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "invalid" << endl;
        return false;
    }
    
    bool hasToken = false;
    bool hasStateQuery = false;
    bool hasRoomHistory = false;
    bool hasEmployee = false;
    bool hasGuest = false;
    
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        
        if (arg == "-K") {
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
        else if (arg == "-S") {
            hasStateQuery = true;
        }
        else if (arg == "-R") {
            hasRoomHistory = true;
        }
        else if (arg == "-E") {
            if (i + 1 >= argc) {
                cout << "invalid" << endl;
                return false;
            }
            queryPerson = argv[++i];
            isQueryEmployee = true;
            hasEmployee = true;
        }
        else if (arg == "-G") {
            if (i + 1 >= argc) {
                cout << "invalid" << endl;
                return false;
            }
            queryPerson = argv[++i];
            isQueryEmployee = false;
            hasGuest = true;
        }
        else if (arg[0] != '-') {
            logFileName = arg;
        }
    }
    
    // Validate arguments
    if (!hasToken || logFileName.empty()) {
        cout << "invalid" << endl;
        return false;
    }
    
    if (!hasStateQuery && !hasRoomHistory) {
        cout << "invalid" << endl;
        return false;
    }
    
    if (hasStateQuery && hasRoomHistory) {
        cout << "invalid" << endl;
        return false;
    }
    
    if (hasEmployee && hasGuest) {
        cout << "invalid" << endl;
        return false;
    }
    
    if (hasRoomHistory && !hasEmployee && !hasGuest) {
        cout << "invalid" << endl;
        return false;
    }
    
    // Read log file
    if (!readExistingLog()) {
        return false;
    }
    
    // Execute query
    if (hasStateQuery) {
        printStateQuery();
    } else if (hasRoomHistory) {
        printRoomHistory();
    }
    
    return true;
}

int main(int argc, char* argv[]) {
    LogRead log;
    
    if (!log.processArguments(argc, argv)) {
        return 255;
    }
    
    return 0;
}