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

#define NUM_LIGHTS 12       // Number of traffic lights

int main(){

    string output;
    
    time_t rawTime;
    time(&rawTime);

    srand (time(NULL)); // Seed the random number generator with the current time

    // Generate traffic information for each traffic light
    for (int i = 1 ; i <= NUM_LIGHTS ; i++){
        
        output += to_string(rawTime) + " "; // Add timestamp to output string
        output+= to_string(i) + " ";         // Add traffic light number to output string

        // Generate a random number of cars between 1 and 10
        int cars = rand() % ((10 - 1) + 1) + 1;
        
        output+= to_string(cars) + " \n";    // Add number of cars to output string with newline (log entry format: timestamp ; light number ; number of cars)
    }

    fstream log("log.txt", ios::app);                       // Open log file in append mode
    if(!log.is_open()){
        cout<<"Error: Log file not found"<<endl;            // Output error message if log file cannot be opened
    } else {
        log<<output;                                        // Write output string to log file
        cout<<output;                                       // Output the generated traffic information
        cout<<"log.txt has been updated at "<<ctime(&rawTime)<<endl; // Output message indicating log file update time
    }
    return 0;
}
