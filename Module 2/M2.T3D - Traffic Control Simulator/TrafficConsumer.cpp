#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

#define NUM_LIGHTS 12
#define MEASUREMENTS 13     // 12 per hour plus held avg value

// Function to print traffic details
void printDetails(int array[NUM_LIGHTS][MEASUREMENTS]){
    for(int i = 1; i <= NUM_LIGHTS ; i++){
        cout << to_string(i) + ":\t"; // Output the light number
        for (int j = 0 ; j < sizeof(array[i])/4 ; j++){
            cout << to_string(array[i-1][j]) + ";"; // Output each measurement for the light
        }
        cout<<endl;
    }
}

// Function to calculate average traffic volume
void calculateTrafficVolume(int array[NUM_LIGHTS][MEASUREMENTS]){
    cout<<"calculating...\t";
    for (int i = 0 ; i < NUM_LIGHTS ; i++){
        array[i][0] = 0; // Initialize the sum of traffic volumes for each light
        for (int j = 1 ; j < MEASUREMENTS ; j++){
            array[i][0]+=array[i][j]; // Calculate the total traffic volume for each light
        }
        array[i][0] = array[i][0] / (MEASUREMENTS-1); // Calculate the average traffic volume for each light (integer division for simplicity)
    }
    cout<<"Done"<<endl;
}

int main(){

    int Array[NUM_LIGHTS][MEASUREMENTS];
    int arraySize = sizeof(Array[1])/4;      // 4 bits

    long rawTime;
    int lightNum;
    int trafficVolume;

    fstream log("log.txt", ios::in); // Open log file for reading
    if(!log.is_open()){
        cout<<"Error: Log file not found"<<endl; // Output error message if log file cannot be opened
    } else {
        cout<<"Retrieving initial data from log.txt...\t";
        // Initialize array with zeros
        for(int i = 1 ; i <= NUM_LIGHTS ; i++){
            for (int j = 1 ; j < arraySize ; j++){
                Array[i-1][j]=0;
            }
        }
        
        // Read data from log file
        log >> rawTime >> lightNum >> trafficVolume;

        // Populate the array with data from the log file
        for(int k = 0 ; k < 12 ; k++){
            for(int i = 0 ; i < NUM_LIGHTS ; i++){
                log >> rawTime >> lightNum >> trafficVolume;

                // Shift the measurements in the array to the right
                for(int j = sizeof(Array[0]) / sizeof(Array[0][0]) - 1 ; j>0;j--){
                    Array[lightNum-1][j] = Array[lightNum-1][j-1];
                }
                Array[lightNum-1][1] = trafficVolume; // Store the new traffic volume measurement
            }
        }
        log.close(); // Close the log file
        cout<<"Complete"<<endl;

    }
    calculateTrafficVolume(Array); // Calculate average traffic volumes
    //printDetails(Array); // Uncomment this line to print traffic details
}
