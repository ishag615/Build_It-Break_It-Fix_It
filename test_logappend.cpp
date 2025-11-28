#include <iostream>
#include <cassert>
#include "logappend.hpp"

using namespace std;

LogAppend logTest;

void testRoomEntry() {
    assert(logTest.validateRoomId(3) == true);
    assert(logTest.validateRoomId(-2) == false);
    assert(logTest.validateRoomId(8) == false);
    cout<<"Done testing room entry"<<endl;
}

void testName() {
    assert(logTest.validateName("alisson") == true);
    assert(logTest.validateName("allison123") == false);
    assert(logTest.validateName("aaaaaaaaaaaaaaaahhhhhhhhhhhhhhhhhhhhh") == false); //checking for overflow
    cout<<"Done testing name"<<endl;
}

void testToken() {
    assert(logTest.validateToken("thist0k3n1sval1d") == true);
    assert(logTest.validateToken("@!!!!!#$%%Y^&&##") == false);
    cout<<"Done testing token"<<endl;
}

void testTimestamp() {
    assert(logTest.validateTimestamp(23) == true);
    assert(logTest.validateTimestamp(-2738723482878274) == false); //checking for overflow
    cout<<"Done testing timestamp"<<endl;
}

// to correct error of two mains
#ifndef TESTING
int main() {
    cout<<"Starting tests"<<endl;
    testRoomEntry();
    testName();
    testToken();
    testTimestamp();
    cout<<"Done testing"<<endl;
    return 0;
}
#endif