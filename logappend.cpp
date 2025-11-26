#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
#include <map>

using namespace std;

/// ////////////////////////////////////////////CLASS VARIABLES AND FUNCTIONS/////////////////////////////////////////////////////////////

struct Person {
    string currentToken;
    string name;
    bool isGuest = false;
    vector<int> roomsVisited; //*need to implement room visit tracking*
    bool inGallery = false;
    int currentRoom = -1;
};

class appendLog {
    private:
    int lastStamp; //stores the last timestamp logged
    string token; //stores token
    regex tokenPattern; //stores regex pattern for token 
    regex employeePattern; //stores regex pattern for employee
    bool employee, guest, enteredGallery, enteredLobby, enteredPaleontology, enteredEgyptian, enteredMesopotamian, enteredGreek, enteredSouthAsian, leaveGallery;
    string currentRoom; //stores current room
    string employeeName, guestName, room; 
    map<string, Person> group; //stores all persons logged, and their details
    string fileName;

public:
    appendLog();
    void TimeStamp(const string& tmp); //logs time
    void Token(const string& tmp); //logs token
    void EmployeeName(const string& tmp); //logs employee employee status
    void GuestName(const string& tmp); //logs guest status
    void Room(const string& tmp); //logs room entered
    void EventArrival(const string& tmp, const string& tmpj); //logs when -A is called
    void EventLeave(const string& tmp, const string& tmpj); //logs when -L is called
    void bindDetails(const string& token, const string& name, bool isGuest, const string& room);//using this to bind name, token, guest/employee status, room to ensure consistency
    void storeFileName(const string& tmp, int argc, char* argv[]); //stores the filename 
    bool validateLogEntered(const string& token, const string& name, bool isGuest, const string& room); //validates the log file

};

appendLog::appendLog():
    lastStamp(0),
    guest(false),
    employee(false),
    tokenPattern("^[A-Za-z0-9]+$"),
    employeePattern("^[A-Za-z]+$"),
    enteredGallery(false),
    currentRoom(""),
    leaveGallery(false),
    enteredLobby(false), 
    enteredPaleontology(false), 
    enteredEgyptian(false), 
    enteredMesopotamian(false), 
    enteredGreek(false), 
    enteredSouthAsian(false)
{}

//////////////////////////////////////////FUNCTION DEFINITIONS///////////////////////////////////////////////////

void appendLog::bindDetails(const string& token, const string& name, bool isGuest, const string& room){ //binding details to map with token as key

    Person& logPerson = group[token]; //reference to person in map
    logPerson.currentToken = token;
    if (!logPerson.name.empty()){// checking if empty to avoid storing tokens in memory if not needed
    if (logPerson.name != name){
        cout<<"Invalid Token entry, utilizing token for another person!"<<endl;
        exit(255);
    }
    }

    if(!logPerson.name.empty() && logPerson.isGuest == isGuest){
        if(logPerson.isGuest == false){
            cout<<"You cannot be guest and employee!"<<endl;
            exit(255);
        }
    }
    else {
        logPerson.isGuest = isGuest;
    }

    if(!logPerson.name.empty() && logPerson.isGuest == !isGuest){
        if(logPerson.isGuest == true){
            cout<<"You cannot be guest and employee!"<<endl;
            exit(255);
        }
    }

}

void appendLog::TimeStamp(const string& tmp){// COMPLETE, maybe bounds checking for overflow
    string tmptime = tmp;
    int convertedTime = stoi(tmptime);
     convertedTime = stoi(tmptime);
        if(  convertedTime<1 || convertedTime>1073741823) {// ensuring time is within these numbers
        cout<<"Time entered is not within range"<<endl;
        exit(255);
    }
    else if(convertedTime <= lastStamp){ //time should always increae
        cout<<"Time is lesser than the last time logged"<<endl;
        exit(255);
    }
    else {
        lastStamp = convertedTime; //storing most recent time
    }
}

void appendLog::Token(const string& tmp){// COMPLETE: need asserts
    if( !regex_match (tmp, tokenPattern)) {// whitelisting characters using regex
        cout<<"Invalid token format"<<endl;
        exit(255);
    }
    else{
        token = tmp; //storing token
    }
}

void appendLog::EmployeeName(const string& tmp){// NEED TO IMPLEMENT: tests
    if( !regex_match (tmp, employeePattern) || tmp.length()>=20) {//adding a limit for name will change if needed, whitelisting regex
        cout<<"Invalid employee name"<<endl;
        exit(255);
    } 
    bindDetails(token, tmp, false, "");
}

void appendLog::GuestName(const string& tmp){ // NEED TO IMPLEMENT: tests
     if( !regex_match (tmp, employeePattern)|| tmp.length()>=20) {//adding a limit for name will change if needed, whitelisting regex
        cout<<"Invalid guest name"<<endl;
        exit(255);
    }
    
    bindDetails(token, tmp, true, "");
}

void appendLog::Room(const string& tmp){
    currentRoom = tmp;    
    int convertedRoom = stoi(tmp); //converted string to integer to check room number
    if( convertedRoom>7|| convertedRoom<-1) { //added** bounds checking
        cout<<"Invalid room entered"<<endl; //if number is >7 or <0 invalid
        exit(-1);// NEED TO IMPLEMENT: tests
    }
    else{
        switch(convertedRoom){// adding rooms to check entry
            case -1: enteredGallery = false;
            break;
            case 0: enteredGallery = true;
            break;
            case 1: enteredLobby = true;
            break;
            case 2: enteredPaleontology = true;
            break;
            case 3: enteredMesopotamian = true;
            break;
            case 4: enteredGreek = true;
            break;
            case 5: enteredEgyptian= true;
            break;
            case 6: enteredSouthAsian = true;
            break;
            case 7: leaveGallery = true; //enter 7 to go outside
            break;
            default:
            cout<<"Invalid room"<<endl;
            exit(255);
        }
    }

    
}

void appendLog::EventArrival(const string& tmp, const string& tmpj){
    Room(tmpj);
    if (!enteredGallery && (enteredEgyptian || enteredGreek || enteredMesopotamian || enteredPaleontology || enteredSouthAsian)){  //cannot enter room if you are not in gallery
        cout<<"Invalid room, enter gallery first"<<endl;
        exit(255); //NEED TO IMPLEMENT:additional cases for arriving at a room, tests, bounds
    }
    bindDetails(token, "", false, tmpj);
}

void appendLog::EventLeave(const string& tmp, const string& tmpj){
    Room(tmpj);
     if (!enteredGallery && (enteredEgyptian || enteredGreek || enteredMesopotamian || enteredPaleontology || enteredSouthAsian)){  //cannot leave gallery without leaving room
        cout<<"Invalid, leave current room first"<<endl;
        exit(255); //NEED TO IMPLEMENT:additional cases for leaving a room, tests, bounds
    }
    bindDetails(token, "", false, tmpj);
}

    
void appendLog::storeFileName(const string& tmp, int argc, char* argv[]){
    fileName = tmp;
    ofstream logFile(fileName, ios::app); //appending to file
    for(int i=1; i< argc; i++){ // Save all arguments as entered
        logFile << argv[i];
        if (i < argc - 1) logFile << " "; // add spaces between args
    }
    logFile << endl; // end line after entire log
    logFile.close();
}

void validateLogFile(const string& filename) {
    
}


/////////////////////////////////////////////////////MAIN FILE///////////////////////////////////////////////////////////////////////////////////
int main (int argc, char* argv[]){

    if (argc<2 || argc>40) {  //NEED TO IMPLEMENT: bounds checking for token, have to decide max elements for each part
        cout<<"Invalid Entry"<<endl;
        exit(255);
    }

    appendLog log;


    cout<<"Welcome to A&I museum system! Enter a log/sequence of actions to begin"<<endl;

        for (int i=1; i<argc; i++){ //Tried implementing switch case cannot use with argv
        if ((std::string (argv[i])) == "-T"){
            log.TimeStamp(argv[++i]);
        }
        else if ((std::string (argv[i])) == "-K"){// NEED TO IMPLEMENT: -B, bound checking
            log.Token(argv[++i]);
        }
        else if ((std::string (argv[i])) == "-E"){
            log.EmployeeName(argv[++i]);
        }
        else if ((std::string (argv[i])) == "-G"){
            log.GuestName(argv[++i]);
        }
        else if ((std::string (argv[i])) == "-A"){
            log.EventArrival(argv[i+1], argv[i+2]);// checking room number which is two tokens ahead
            i+=2;// skipping tokens checked
        }
        else if ((std::string (argv[i])) == "-L"){
            log.EventLeave(argv[i+1], argv[i+2]);// checking room number which is two tokens ahead
            i+=2;// skipping tokens checked
        }
        else if ((std::string (argv[i])) == "-R"){
            log.Room(argv[++i]);
        }
        else if ((std::string (argv[i])) == "-B"){
            log.storeFileName(argv[++i], argc, argv);
        }
        }
        cout<<"Done Logging";
    }

    
////////////////////////////////////////////////////////////END///////////////////////////////////////////////////////////////////////////






    
