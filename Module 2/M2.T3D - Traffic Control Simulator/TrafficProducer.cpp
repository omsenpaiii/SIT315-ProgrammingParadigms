/* TrafficProducer.cpp
 * Om Tomar
 * 04 March 2023
 * 
 * This file is designed to be run by a scheduler to log traffic information to the log.txt file 
 * 
 */

#include <iostream>
#include <random>
#include <ctime>
#include <fstream>
#include <time.h>

using namespace std;

#define NUM_LIGHTS 12       //number of traffic lights

int main(){

    string output;
    
    time_t rawTime;
    time(&rawTime);

    srand (time(NULL));

    for (int i = 1 ; i <= NUM_LIGHTS ; i++){
        
        output += to_string(rawTime) + " ";
        output+= to_string(i) + " ";
        int cars = rand() % ((10 - 1) + 1) + 1;
        output+= to_string(cars) + " \n";                   //log entry std::stamp ; light no. ; number of cars (random)
    }

    fstream log("log.txt", ios::app);                       //append to log file
    if(!log.is_open()){
        cout<<"Error: Log file not found"<<endl;
    } else {
        log<<output;
        cout<<output;
        cout<<"log.txt has been updated at "<<ctime(&rawTime)<<endl;
    }
    return 0;
}