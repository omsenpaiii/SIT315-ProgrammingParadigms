#include <iostream>                // Input/output stream library
#include <fstream>                 // File stream library
#include <sstream>                 // String stream library
#include <vector>                  // Vector container library
#include <queue>                   // Queue container library
#include <mutex>                   // Mutex library for synchronization
#include <condition_variable>      // Condition variable library for synchronization
#include <thread>                  // Thread library
#include <chrono>                  // Chrono library for time-related functionality
#include <algorithm>               // Algorithm library for sorting
#include <pthread.h>               // Pthreads library for POSIX threads

using namespace std;              // Using standard namespace

// Structure to represent traffic data
struct TrafficData {
    time_t timestamp;             // Timestamp of the traffic data
    int lightId;                  // Traffic light ID
    int carsPassed;               // Number of cars passed by the traffic light
};

// Class to manage traffic data
class TrafficManager {
private:
    queue<TrafficData> buffer;             // Buffer to store traffic data
    priority_queue<pair<int, int>> topTrafficLights; // Priority queue for top N most congested traffic lights
    pthread_mutex_t mtx;                   // Mutex for synchronization
    pthread_cond_t cv;                     // Condition variable for synchronization
    const int bufferSize;                  // Size of the buffer
    const int topN;                        // Number of top congested traffic lights

public:
    // Constructor to initialize TrafficManager
    TrafficManager(int bufferSize, int topN) : bufferSize(bufferSize), topN(topN) {
        pthread_mutex_init(&mtx, nullptr);            // Initialize mutex
        pthread_cond_init(&cv, nullptr);              // Initialize condition variable
    }

    // Destructor to clean up resources
    ~TrafficManager() {
        pthread_mutex_destroy(&mtx);                   // Destroy mutex
        pthread_cond_destroy(&cv);                     // Destroy condition variable
    }

    // Function to produce traffic data
    void produceTraffic(const string& filename) {
        ifstream file(filename);                           // Open file for reading
        if (!file.is_open()) {
            cerr << "Error: Unable to open file: " << filename << endl;   // Print error if unable to open file
            return;
        }

        string line;
        while (getline(file, line)) {
            istringstream iss(line);                       // Create string stream from line
            TrafficData data;
            iss >> data.timestamp >> data.lightId >> data.carsPassed;  // Parse traffic data from line

            pthread_mutex_lock(&mtx);                      // Lock mutex
            while (buffer.size() >= bufferSize) {          // Wait if buffer is full
                pthread_cond_wait(&cv, &mtx);
            }

            buffer.push(data);                             // Push traffic data to buffer
            pthread_mutex_unlock(&mtx);                    // Unlock mutex
            pthread_cond_signal(&cv);                      // Signal consumer thread

            this_thread::sleep_for(chrono::seconds(5));    // Simulate 12 measurements per hour
        }
        file.close();                                      // Close file
    }

    // Function to consume traffic data
    void consumeTraffic() {
        while (true) {
            pthread_mutex_lock(&mtx);                      // Lock mutex
            while (buffer.empty()) {                       // Wait if buffer is empty
                pthread_cond_wait(&cv, &mtx);
            }

            vector<TrafficData> temp;
            while (!buffer.empty()) {                      // Copy buffer to temporary vector
                temp.push_back(buffer.front());
                buffer.pop();
            }
            pthread_mutex_unlock(&mtx);                    // Unlock mutex
            pthread_cond_signal(&cv);                      // Signal producer thread

            for (const auto& data : temp) {
                topTrafficLights.push({data.carsPassed, data.lightId});   // Push traffic data to priority queue
                if (topTrafficLights.size() > topN) {       // Maintain top N most congested traffic lights
                    topTrafficLights.pop();
                }
            }

            printTopTrafficLights();                       // Print top N most congested traffic lights
        }
    }

    // Function to print top N most congested traffic lights
    void printTopTrafficLights() {
        vector<pair<int, int>> topNList;
        while (!topTrafficLights.empty()) {                // Copy priority queue to temporary vector
            topNList.push_back(topTrafficLights.top());
            topTrafficLights.pop();
        }
        reverse(topNList.begin(), topNList.end());         // Reverse temporary vector

        cout << "Top " << topN << " most congested traffic lights:" << endl;
        for (const auto& p : topNList) {
            cout << "Light ID: " << p.second << ", Cars Passed: " << p.first << endl;   // Print traffic light details
            topTrafficLights.push(p);                      // Push traffic light details back to priority queue
        }
    }
};

// Main function
int main() {
    const int bufferSize = 10; // Buffer size
    const int topN = 5;        // Top N most congested traffic lights

    TrafficManager manager(bufferSize, topN);   // Create TrafficManager object

    pthread_t producerThread, consumerThread;   // Declare producer and consumer threads
    pthread_create(&producerThread, nullptr, [](void* arg) -> void* {   // Create producer thread
        TrafficManager* mgr = static_cast<TrafficManager*>(arg);
        mgr->produceTraffic("log.txt");
        return nullptr;
    }, &manager);

    pthread_create(&consumerThread, nullptr, [](void* arg) -> void* {   // Create consumer thread
        TrafficManager* mgr = static_cast<TrafficManager*>(arg);
        mgr->consumeTraffic();
        return nullptr;
    }, &manager);

    pthread_join(producerThread, nullptr);   // Join producer thread
    pthread_join(consumerThread, nullptr);   // Join consumer thread

    return 0;   // Return success
}