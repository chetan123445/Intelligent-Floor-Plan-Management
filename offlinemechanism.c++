#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <set>

using namespace std;

// Structure to store information about floor plans
struct FloorPlan {
    string filename;          // Name of the floor plan file
    string lastModifiedBy;    // Last user who modified the floor plan
    time_t timestamp;         // Timestamp of the last modification
    int capacity;             // Capacity of the floor plan
    string availability;        // Availability status of the floor plan
};

// Class to handle the floor plan management
class FloorPlanManager {
private:
    FloorPlan onlinePlan; // Object to store information about the online floor plan
    bool isOffline = false; // Simulate offline mode

public:
    FloorPlanManager(const string& planName, const string& adminName) {
        onlinePlan.filename = planName + ".txt"; // Store filename as 'floorname.txt'
        onlinePlan.lastModifiedBy = adminName;
        onlinePlan.timestamp = time(0);
        onlinePlan.capacity = 0; // Initialize capacity
        onlinePlan.availability = true; // Initialize availability
    }

    // Function to check if a floor plan already exists
    static bool floorExists(const string& floorName) {
        ifstream floorPlansFile("floor_plans.txt");
        string existingFloor;
        while (getline(floorPlansFile, existingFloor)) {
            if (existingFloor == floorName) {
                return true;
            }
        }
            return false;
    }

    // Function to simulate going offline
    void goOffline() {
        isOffline = true;
        cout << "You are now offline." << endl;

        // Store the floor plan name in offline.txt
        ofstream offlineFile("offline.txt", ios::app);
        if (offlineFile.is_open()) {
            offlineFile << onlinePlan.filename << endl;
            offlineFile.close();
            cout << "Floor plan name stored offline." << endl;
        } else {
            cout << "Error saving offline floor plan name." << endl;
        }
    }

    // Function to simulate coming back online
    void goOnline() {
        isOffline = false;
        cout << "You are now back online. Synchronizing changes..." << endl;
        synchronizeChanges();

        // Clear offline.txt after synchronization
        ofstream clearOfflineFile("offline.txt", ofstream::out | ofstream::trunc);
        clearOfflineFile.close();
        cout << "Offline names file cleared successfully." << endl;
    }

    // Function to add or modify a floor plan
    void updateFloorPlan(const string& adminName, int capacity,const string& availability) {
        onlinePlan.capacity = capacity;
        onlinePlan.availability = availability;
        if (isOffline) {
            // Store changes offline
            storeOfflineChanges(adminName);
        } else {
            // Check if this is the first time we are adding this floor plan
            if (!floorExists(onlinePlan.filename.substr(0, onlinePlan.filename.size() - 4))) { // Check without ".txt"
                ofstream floorPlansFile("floor_plans.txt", ios::app);
                if (floorPlansFile.is_open()) {
                    floorPlansFile << onlinePlan.filename.substr(0, onlinePlan.filename.size() - 4) << endl; // Write the floor name without ".txt"
                    floorPlansFile.close();
                }
            }
            // Directly modify online floor plan
            modifyOnlineFloorPlan(adminName);
        }
    }

    // Function to store changes locally (offline)
    void storeOfflineChanges(const string& adminName) {
        ofstream offlineFile("offline_changes.txt", ios::app);
        if (offlineFile.is_open()) {
            time_t currentTime = time(0);
            offlineFile << "Floor: " << onlinePlan.filename << endl;
            offlineFile << "Modified by: " << adminName << endl;
            offlineFile << "Capacity: " << onlinePlan.capacity << endl;
            offlineFile << "Availability: " << onlinePlan.availability << endl;
            offlineFile << "Timestamp: " << ctime(&currentTime);
            offlineFile.close();
            cout << "Changes saved offline." << endl;
        } else {
            cout << "Error saving offline changes." << endl;
        }
    }

    // Function to modify the online floor plan
    void modifyOnlineFloorPlan(const string& adminName) {
        ofstream file(onlinePlan.filename, ios::app);
        if (file.is_open()) {
            onlinePlan.lastModifiedBy = adminName;
            onlinePlan.timestamp = time(0);

            file << "Modified by: " << adminName << endl;
            file << "Capacity: " << onlinePlan.capacity << endl;
            file << "Availability: " << onlinePlan.availability << endl;
            file << "Timestamp: " << ctime(&onlinePlan.timestamp);
            file.close();
            cout << "Floor plan updated online." << endl;
        } else {
            cout << "Error updating floor plan." << endl;
        }
    }

    // Function to synchronize offline changes with the online version
    void synchronizeChanges() {
        ifstream offlineFile("offline_changes.txt");
        if (!offlineFile.is_open()) {
            cout << "No offline changes to synchronize." << endl;
            return;
        }

        // Open floor_plans.txt in read mode to check existing floors
        ifstream floorPlansFile("floor_plans.txt");
        set<string> existingFloors; // Set to store existing floor names
        string existingFloor;

        // Read existing floor names into the set
        while (getline(floorPlansFile, existingFloor)) {
            existingFloors.insert(existingFloor);
        }
        floorPlansFile.close();

        string line, currentFloor;
        while (getline(offlineFile, line)) {
            if (line.rfind("Floor: ", 0) == 0) {
                currentFloor = line.substr(7);

                // Remove the .txt extension if it exists
                if (currentFloor.size() > 4 && currentFloor.substr(currentFloor.size() - 4) == ".txt") {
                    currentFloor = currentFloor.substr(0, currentFloor.size() - 4);
                }

                // Write only the floor name (without .txt) to floor_plans.txt if it’s not already present
                if (existingFloors.find(currentFloor) == existingFloors.end()) {
                    ofstream appendFloorPlansFile("floor_plans.txt", ios::app);
                    if (appendFloorPlansFile.is_open()) {
                        appendFloorPlansFile << currentFloor << endl;
                        appendFloorPlansFile.close();
                        existingFloors.insert(currentFloor);
                        cout << "Added floor: " << currentFloor << " to floor_plans.txt" << endl;
                    } else {
                        cout << "Error opening floor_plans.txt for appending." << endl;
                    }
                }
                // Append line to the specific floor file
                ofstream floorFile(currentFloor + ".txt", ios::app);
                if (floorFile.is_open()) {
                    floorFile << line << endl;
                    floorFile.close();
                }
            }
        }

        offlineFile.close();

        // Clear offline changes after synchronization
        ofstream clearOffline("offline_changes.txt", ofstream::out | ofstream::trunc);
        clearOffline.close();
        cout << "Offline changes synchronized and file cleared successfully." << endl;
    }

    // Function to view floor plan details
    void viewFloorPlan() {
        string line;
        ifstream file(onlinePlan.filename);
        if (file.is_open()) {
            while (getline(file, line)) {
                cout << line << endl;
            }
            file.close();
        } else {
            cout << "Error reading the floor plan file." << endl;
        }
    }
};

// Function to check admin credentials
bool checkAdminCredentials(const string& username, const string& password) {
    ifstream adminFile("plain_admins.txt");
    string fileUsername, filePassword;
    while (adminFile >> fileUsername >> filePassword) {
        if (fileUsername == username && filePassword == password) {
            return true;
        }
    }
    return false;
}

int main() {
    string adminUsername, adminPassword;
    cout << "Enter Admin Username: ";
    cin >> adminUsername;
    cout << "Enter Admin Password: ";
    cin >> adminPassword;

    // Check admin credentials
    if (!checkAdminCredentials(adminUsername, adminPassword)) {
        cout << "Invalid credentials. Access denied!" << endl;
        return 1;
    }

    string floorPlanName;
    cout << "Enter the floor plan name: ";
    cin >> floorPlanName;

    // Check if the floor already exists
    if (FloorPlanManager::floorExists(floorPlanName)) {
        cout << "Error: A floor plan with this name already exists. Please choose a different name." << endl;
        return 1;
    }

    FloorPlanManager manager(floorPlanName, adminUsername);

    // Main menu
    char choice;
    do {
        cout << "\nMenu:\n";
        cout << "1. Go Offline\n";
        cout << "2. Go Online\n";
        cout << "3. View Floor Plan\n";
        cout << "4. Add or Modify Floor Plan\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case '1':
                manager.goOffline();
                break;

            case '2':
                manager.goOnline();
                break;

            case '3':
                manager.viewFloorPlan();
                break;

            case '4': {
                int capacity;
                string availability;
                cout << "Enter capacity: ";
                cin >> capacity;
                cout << "Enter availability (Yes/No): ";
                cin >> availability;
                manager.updateFloorPlan(adminUsername, capacity, availability);
                break;
            }

            case '5':
                cout << "Exiting..." << endl;
                break;

            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    } while (choice != '5');

    return 0;
}
