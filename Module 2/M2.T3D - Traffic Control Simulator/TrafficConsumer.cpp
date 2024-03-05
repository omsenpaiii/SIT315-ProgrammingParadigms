#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

#define NUM_LIGHTS 12
#define MEASUREMENTS 13     //12 per hour plus held avg value

void printDetails(int array[NUM_LIGHTS][MEASUREMENTS]){
    for(int i = 1; i <= NUM_LIGHTS ; i++){
        cout << to_string(i) + ":\t";
        for (int j = 0 ; j < sizeof(array[i])/4 ; j++){
            cout << to_string(array[i-1][j]) + ";";
        }
        cout<<endl;
    }
}

void caluclateTrafficVolume(int array[NUM_LIGHTS][MEASUREMENTS]){
    cout<<"calculating...\t";
    for (int i = 0 ; i < NUM_LIGHTS ; i++){
        array[i][0] = 0;
        for (int j = 1 ; j < MEASUREMENTS ; j++){
            array[i][0]+=array[i][j];
        }
        //cout<< to_string(i)+":\t" + to_string(array[i][0]) + " = ";
        array[i][0] = array[i][0] / (MEASUREMENTS-1);                   //integer division for simplicity
        //cout<< to_string(array[i][0]) <<endl;
    }
    cout<<"Done"<<endl;
}

int main(){

    int Array[NUM_LIGHTS][MEASUREMENTS];
    int arraySize = sizeof(Array[1])/4;      //4bit
    long rawTime;

    int lightNum;
    int trafficVolume;

    fstream log("log.txt", ios::in);
    if(!log.is_open()){
        cout<<"Error: Log file not found"<<endl;
    } else {
        cout<<"Retriving initial data from log.txt...\t";
        for(int i = 1 ; i <= NUM_LIGHTS ; i++){
            for (int j = 1 ; j < arraySize ; j++){
                Array[i-1][j]=0;
            }
        }
        
        log >> rawTime >> lightNum >> trafficVolume;
        //cout<< "\n" << ctime(&rawTime) << " - " << to_string(lightNum) << " - " << to_string(trafficVolume) << endl;

        for(int k = 0 ; k < 12 ; k++){
            for(int i = 0 ; i < NUM_LIGHTS ; i++){
                log >> rawTime >> lightNum >> trafficVolume;
                //cout<<to_string(rawTime)<<to_string(lightNum)<<to_string(trafficVolume)<<endl;

                for(int j = sizeof(Array[0]) / sizeof(Array[0][0]) - 1 ; j>0;j--){
                    Array[lightNum-1][j] = Array[lightNum-1][j-1];
                }
                Array[lightNum-1][1] = trafficVolume;
            }
        }
        log.close();
        cout<<"Complete"<<endl;

    }
    caluclateTrafficVolume(Array);
    //printDetails(Array);
}