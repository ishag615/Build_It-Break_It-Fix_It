#include <iostream>
#include <fstream>
#include <string>
#include <regex>

using namespace std;

/// ////////////////////////////////////////////CLASS VARIABLES AND FUNCTIONS/////////////////////////////////////////////////////////////
class appendLog{
private:
int lastStamp;
string token;
regex tokenPattern;
regex employeePattern;
bool employee, guest, enteredGallery, enteredLobby, enteredPaleontology, enteredEgyptian, enteredMesopotamian, enteredGreek, enteredSouthAsian, leaveGallery;
string currentRoom;
string employeeName, guestName, room;


public:
appendLog();
void TimeStamp(const string& tmp);
void Token(const string& tmp);
void EmployeeName(const string& tmp);
void GuestName(const string& tmp);
void Room(const string& tmp);
void EventArrival(const string& tmp, const string& tmpj);
void EventLeave(const string& tmp, const string& tmpj);
void leaveRoom(bool yourRoom);

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

void appendLog::TimeStamp(const string& tmp){// COMPLETE, maybe bounds checking for overflow
    string tmptime = tmp;
    int convertedTime = stoi(tmptime);
    if(  convertedTime<1 || convertedTime>1073741823) {// ensuring time is within these numbers
        cout<<"Time entered is not within range"<<endl;
        exit(255);
    }
    else if(convertedTime <= lastStamp){ //time should always increae
        cout<<"Time is lesser than the last time logged"<<endl;
        exit(255);
    }
    else{
        lastStamp = convertedTime; //storing most recent time
    }

}

void appendLog::Token(const string& tmp){// COMPLETE: need asserts
    if( !regex_match (tmp, tokenPattern)) {// whitelisting characters using regex
        printf("Invalid token format");
        exit(255);
   
    }
}

void appendLog::EmployeeName(const string& tmp){// NEED TO IMPLEMENT: tests, validation
    if( !regex_match (tmp, employeePattern) || tmp.length()>=20) {//adding a limit for name will change if needed, whitelisting regex
        printf("Invalid employee name");
        exit(255);
    } else{
        employee = true;
        if(employee && guest){// cannot add log if you are both a guest and employee
            cout<<"You cannot be a guest and an employee"<<endl;
            exit (255);
        }
    }

}
void appendLog::GuestName(const string& tmp){ // NEED TO IMPLEMENT: tests, validation
     if( !regex_match (tmp, employeePattern)|| tmp.length()>=20) {//adding a limit for name will change if needed, whitelisting regex
        printf("Invalid guest name");
        exit(255);
    }
    else {
        guest = true;
        if(employee && guest){// cannot add log if you are both a guest and employee
            cout<<"Cannot be employee and guest"<<endl;
            exit (255);
        }
    }

}

/*void appendLog::leaveRoom(bool yourRoom){
    Room(tmpj);
    if (!enteredGallery && (enteredEgyptian || enteredGreek || enteredMesopotamian || enteredPaleontology || enteredSouthAsian)){  //cannot enter room if you are not in gallery
        cout<<"Invalid room"<<endl;
        exit(255);
    }
}*/

void appendLog::Room(const string& tmp){
    currentRoom = tmp;    
    int convertedRoom = stoi(tmp); //converted string to integer to check room number
    if( convertedRoom>7|| convertedRoom<0) { //added** bounds checking
        cout<<"Invalid room entered"<<endl; //if number is >7 or <0 invalid
        exit(-1);// NEED TO IMPLEMENT: tests, validation
    }
    else{
        switch(convertedRoom){// adding rooms to check entry
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
            printf("Invalid room");
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
}

void appendLog::EventLeave(const string& tmp, const string& tmpj){
Room(tmpj);
     if (!enteredGallery && (enteredEgyptian || enteredGreek || enteredMesopotamian || enteredPaleontology || enteredSouthAsian)){  //cannot leave gallery without leaving room
        cout<<"Invalid, leave current room first"<<endl;
        exit(255); //NEED TO IMPLEMENT:additional cases for leaving a room, tests, bounds
    }
}



/////////////////////////////////////////////////////MAIN FILE///////////////////////////////////////////////////////////////////////////////////
int main (int argc, char* argv[]){

    if (argc> 40) {  //NEED TO IMPLEMENT: bounds checking for token, have to decide max elements for each part
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
    
     }
        for (int j =0; j<argc; j++){//NEED TO IMPLEMENT: Processing data to file filename following -B
             cout<<argv[j]<<" ";
        }
        cout<<"Done Logging";
    }

    
////////////////////////////////////////////////////////////END///////////////////////////////////////////////////////////////////////////






    



    
