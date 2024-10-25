#include <iostream> // Required for cout, cin, and endl
#include <fstream>  // Required for file operations
#include <string>   // Required for using std::string
#include <ctime>    // Required for time-related functions

using namespace std;

// Structure to store information about floor plans (both online and offline)
struct FloorPlan {
    string filename;          // Name of the floor plan file
    string lastModifiedBy;    // Last user who modified the floor plan
    time_t timestamp;         // Timestamp of the last modification
    int capacity;             // Capacity of the floor plan
    bool availability;        // Availability status of the floor plan
};

// Class to handle the floor plan management, including offline mode
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

    // Function to simulate going offline
    void goOffline() {
        isOffline = true;
        cout << "You are now offline." << endl;

        // Store the floor plan name in offline.txt
        ofstream offlineFile("offline.txt", ios::app);
        if (offlineFile.is_open()) {
            offlineFile << onlinePlan.filename << endl; // Store the floor plan name
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
    }

    // Function to upload or modify the floor plan
    void updateFloorPlan(const string& adminName) {
        cout << "Enter capacity for the floor plan: ";
        cin >> onlinePlan.capacity; // Prompt for capacity
        cout << "Enter availability status (1 for available, 0 for not available): ";
        cin >> onlinePlan.availability; // Prompt for availability

        if (isOffline) {
            // Store changes offline
            storeOfflineChanges(adminName);
            // Additionally, update the local floor plan file to include modifications
            modifyFloorPlanFileLocally(adminName);
        } else {
            // Directly modify online floor plan
            modifyOnlineFloorPlan(adminName);
        }
    }

    // Store changes locally (offline)
    void storeOfflineChanges(const string& adminName) {
        ofstream offlineFile("offline_changes.txt", ios::app);
        if (offlineFile.is_open()) {
            time_t currentTime = time(0);
            offlineFile << "Offline Update by: " << adminName << endl;
            offlineFile << "Capacity: " << onlinePlan.capacity << endl;
            offlineFile << "Availability: " << (onlinePlan.availability ? "Available" : "Not Available") << endl;
            offlineFile << "Timestamp: " << ctime(&currentTime);
            offlineFile.close();
            cout << "Changes saved offline." << endl;
        } else {
            cout << "Error saving offline changes." << endl;
        }
    }

    // Modify the online floor plan
    void modifyOnlineFloorPlan(const string& adminName) {
        // Read existing content and prepare to append new details
        ifstream existingFile(onlinePlan.filename);
        string existingContent;
        string line;

        // Store the existing content
        while (getline(existingFile, line)) {
            existingContent += line + "\n"; // Store all lines
        }
        existingFile.close();

        // Open file to write updated content
        ofstream file(onlinePlan.filename);
        if (file.is_open()) {
            onlinePlan.lastModifiedBy = adminName;
            onlinePlan.timestamp = time(0);

            // Write back existing content
            file << existingContent; // Write all previous contents

            // Append the new modification details
            file << "Modified by: " << adminName << endl;
            file << "Capacity: " << onlinePlan.capacity << endl; // Write capacity
            file << "Availability: " << (onlinePlan.availability ? "Available" : "Not Available") << endl; // Write availability
            file << "Timestamp: " << ctime(&onlinePlan.timestamp);
            file.close();
            cout << "Floor plan updated online." << endl;
        } else {
            cout << "Error updating floor plan." << endl;
        }
    }

    // Modify the local floor plan file during offline mode
    void modifyFloorPlanFileLocally(const string& adminName) {
        // Read existing content and prepare to append new details
        ifstream existingFile(onlinePlan.filename);
        string existingContent;
        string line;

        // Store the existing content
        while (getline(existingFile, line)) {
            existingContent += line + "\n"; // Store all lines
        }
        existingFile.close();

        // Open file to write updated content
        ofstream file(onlinePlan.filename);
        if (file.is_open()) {
            onlinePlan.lastModifiedBy = adminName;
            onlinePlan.timestamp = time(0);

            // Write back existing content
            file << existingContent; // Write all previous contents

            // Append the new modification details
            file << "Modified by: " << adminName << endl;
            file << "Capacity: " << onlinePlan.capacity << endl; // Write capacity
            file << "Availability: " << (onlinePlan.availability ? "Available" : "Not Available") << endl; // Write availability
            file << "Timestamp: " << ctime(&onlinePlan.timestamp);
            file.close();
            cout << "Offline changes updated in floor plan file." << endl;
        } else {
            cout << "Error updating local floor plan." << endl;
        }
    }

    // Function to check if the floor plan exists in floor_plans.txt
    bool doesFloorPlanExistInFile(const string& planName) {
        ifstream file("floor_plans.txt"); // Check if the floor plan exists in the file
        string existingPlan;

        while (file >> existingPlan) {
            if (existingPlan == planName) {
                return true; // Floor plan exists
            }
        }
        return false; // Floor plan does not exist
    }

    // Synchronize offline changes with the online version
    void synchronizeChanges() {
        // First, read from offline.txt and update floor_plans.txt
        ifstream offlineFile("offline.txt");
        ofstream floorPlansFile("floor_plans.txt", ios::app);
        string line;

        if (offlineFile.is_open()) {
            while (getline(offlineFile, line)) {
                // Remove the file extension for floor plan name
                string floorPlanName = line.substr(0, line.find_last_of('.'));

                // Check if the floor plan already exists before adding to floor_plans.txt
                if (!doesFloorPlanExistInFile(floorPlanName)) {
                    // Update floor_plans.txt with the floor plan name
                    floorPlansFile << floorPlanName << endl;

                    // Create an individual file for the floor plan
                    ofstream individualFile(line);
                    if (individualFile.is_open()) {
                        individualFile << "Floor Plan for: " << floorPlanName << endl; // Content has only the floor name
                        individualFile << "Uploaded by: " << onlinePlan.lastModifiedBy << endl;
                        individualFile << "Capacity: " << onlinePlan.capacity << endl; // Write capacity
                        individualFile << "Availability: " << (onlinePlan.availability ? "Available" : "Not Available") << endl; // Write availability
                        individualFile << "Timestamp: " << ctime(&onlinePlan.timestamp);
                        individualFile.close();
                    } else {
                        cout << "Error creating file for " << line << endl;
                    }
                } else {
                    cout << "Floor plan " << floorPlanName << " already exists. Skipping..." << endl;
                }
            }
            offlineFile.close();
            floorPlansFile.close();

            // Clear offline changes once synchronized
            // This will empty offline.txt for future use
            ofstream clearOfflineFile("offline.txt", ios::trunc);
            clearOfflineFile.close(); // Ensure offline.txt is emptied

            cout << "Offline changes synchronized successfully." << endl;
        } else {
            cout << "Error synchronizing changes." << endl;
        }
    }

    // Function to view floor plan details
void viewFloorPlan() {
    string planName;
    cout << "Enter the floor plan name you want to view: ";
    cin >> planName;

    // Check if the floor plan exists in floor_plans.txt
    ifstream floorPlansFile("floor_plans.txt");
    string existingPlan;
    bool planExists = false;

    while (floorPlansFile >> existingPlan) {
        if (existingPlan == planName) {
            planExists = true; // Floor plan found
            break;
        }
    }
    floorPlansFile.close();

    if (planExists) {
        // Attempt to read the floor plan file
        ifstream file(planName + ".txt"); // Expecting files to be named as 'floorname.txt'
        string line;

        if (file.is_open()) {
            while (getline(file, line)) {
                cout << line << endl; // Display each line of the file
            }
            file.close();
        } else {
            cout << "Error reading the floor plan file." << endl;
        }
    } else {
        cout << "No such floor exists." << endl;
    }
}

};

// Function to check admin credentials
bool checkAdminCredentials(const string& username, const string& password) {
    ifstream adminFile("plain_admins.txt");
    string fileUsername, filePassword;

    while (adminFile >> fileUsername >> filePassword) {
        if (fileUsername == username && filePassword == password) {
            return true; // Credentials match
        }
    }
    return false; // No match found
}

int main() {
    string adminUsername, adminPassword;
    cout << "Enter Admin Username: ";
    cin >> adminUsername;
    cout << "Enter Admin Password: ";
    cin >> adminPassword;

    // Check the entered admin credentials against the file
    if (!checkAdminCredentials(adminUsername, adminPassword)) {
        cout << "Invalid credentials. Access denied!" << endl;
        return 1; // Exit the program on invalid credentials
    }

    string floorPlanName;
    cout << "Enter the floor plan name: ";
    cin >> floorPlanName;

    FloorPlanManager manager(floorPlanName, adminUsername); // Create a manager for the floor plan

    // Main menu
    char choice;
    bool isOffline = false; // Track the current mode
    do {
        cout << "\nMenu:\n";
        cout << "1. Go Offline\n";
        cout << "2. Go Online\n";
        cout << "3. Exit\n"; // Exit option
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case '1':
                manager.goOffline();
                isOffline = true; // Set mode to offline
                break;

            case '2':
                manager.goOnline();
                isOffline = false; // Set mode to online
                break;

            case '3':
                cout << "Exiting..." << endl;
                break;

            default:
                cout << "Invalid choice. Please try again." << endl;
                continue; // Skip the rest of the loop and prompt again
        }

        // Prompt to view or add floor plans after switching modes
        char actionChoice;
        cout << (isOffline ? "Offline Mode" : "Online Mode") << ". What would you like to do?\n";
        cout << "1. View Floor Plan\n";
        cout << "2. Add Floor Plan\n";
        cout << "3. Back to Main Menu\n";
        cout << "Enter your choice: ";
        cin >> actionChoice;

        switch (actionChoice) {
            case '1':
                manager.viewFloorPlan(); // View floor plan based on the current mode
                break;

            case '2':
                manager.updateFloorPlan(adminUsername); // Add or update floor plan details
                break;

            case '3':
                // Simply break to return to the main menu
                break;

            default:
                cout << "Invalid choice. Returning to main menu." << endl;
        }
    } while (choice != '3');

    return 0; // Indicate successful execution
}

