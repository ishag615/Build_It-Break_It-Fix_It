#include <iostream>
#include <fstream>
#include <string>
#include <regex>

using namespace std;


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

appendLog::appendLog()
:
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



void appendLog::TimeStamp(const string& tmp){
    string tmptime = tmp;
    int convertedTime = stoi(tmptime);
    if(  convertedTime<1 || convertedTime>1073741823) {
        cout<<"Time entered is not within range"<<endl;
        exit(255);
    }
    else if(convertedTime <= lastStamp){
        cout<<"Time is lesser than the last time logged"<<endl;
        exit(255);
    }
    else{
        lastStamp = convertedTime;
    }

}

void appendLog::Token(const string& tmp){
    if( !regex_match (tmp, tokenPattern)) {
        printf("Invalid token format");
        exit(255);
   
    }
}

void appendLog::EmployeeName(const string& tmp){
    if( !regex_match (tmp, employeePattern) || tmp.length()>=20) {
        printf("Invalid employee name");
        exit(255);
    } else{
        employee = true;
        if(employee && guest){
            cout<<"You cannot be a guest and an employee"<<endl;
            exit (255);
        }
    }

}
void appendLog::GuestName(const string& tmp){
     if( !regex_match (tmp, employeePattern)|| tmp.length()>=20) {
        printf("Invalid guest name");
        exit(255);
    }
    else {
        guest = true;
        if(employee && guest){
            cout<<"Cannot be employee and gues"<<endl;
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
    if( convertedRoom>7|| convertedRoom<0) {
        cout<<"Invalid room entered"<<endl; //if number is >7 or <0 invalid
        exit(-1);
    }
    else{
        switch(convertedRoom){
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
        cout<<"Invalid room"<<endl;
        exit(255);
    }
}

void appendLog::EventLeave(const string& tmp, const string& tmpj){
Room(tmpj);
     if (!enteredGallery && (enteredEgyptian || enteredGreek || enteredMesopotamian || enteredPaleontology || enteredSouthAsian)){  //cannot leave gallery without leaving room
        cout<<"Invalid room"<<endl;
        exit(255);
    }
}



/////////////////////////////////////////////////////MAIN FILE///////////////////////////////////////////////////////////////////////////////////
int main (int argc, char* argv[]){

    if (argc> 40) {
        cout<<"Invalid Entry"<<endl;
        exit(255);
    }

    appendLog log;


    cout<<"Welcome to A&I museum system! Enter a log/sequence of actions to begin"<<endl;

    for (int i=1; i<argc; i++){
        if ((std::string (argv[i])) == "-T"){
            log.TimeStamp(argv[++i]);
        }
        else if ((std::string (argv[i])) == "-K"){
            log.Token(argv[++i]);
        }
        else if ((std::string (argv[i])) == "-E"){
            log.EmployeeName(argv[++i]);
        }
        else if ((std::string (argv[i])) == "-G"){
            log.GuestName(argv[++i]);
        }
        else if ((std::string (argv[i])) == "-A"){
            log.EventArrival(argv[i+1], argv[i+2]);
            i+=2;
        }
        else if ((std::string (argv[i])) == "-L"){
            log.EventLeave(argv[i+1], argv[i+2]);
            i+=2;
        }
        else if ((std::string (argv[i])) == "-R"){
            log.Room(argv[++i]);
        }
    
            }

        for (int j =0; j<argc; j++){
             cout<<argv[j]<<" ";
        }
        cout<<"Done Logging";
    }

    







    