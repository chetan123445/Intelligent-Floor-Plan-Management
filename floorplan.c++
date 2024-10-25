#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <ctime>
#include <sstream>

using namespace std;

// Structure to store information about each floor plan
struct FloorPlan {
    string filename;
    string lastModifiedBy;
    time_t timestamp; // To keep track of when the plan was last modified
    int capacity;      // Capacity of the floor
    bool isAvailable;  // Availability of the floor
};

// Class to handle the floor plan management
class FloorPlanManager {
private:
    map<string, FloorPlan> floorPlans; // Key: Floor plan name, Value: FloorPlan struct

    // Load existing floor plans from files
    void loadFloorPlans() {
        ifstream file("floor_plans.txt"); // Use a file to store floor plan names
        if (file.is_open()) {
            string planName;
            while (file >> planName) {
                FloorPlan plan;
                plan.filename = planName + ".txt"; // Assuming files are named as planName.txt
                // Initialize other attributes
                plan.lastModifiedBy = "Unknown"; // Default value
                plan.timestamp = 0; // Default value
                plan.capacity = 0;  // Default capacity
                plan.isAvailable = true; // Default availability
                floorPlans[planName] = plan;
            }
            file.close();
        }
    }

public:
    FloorPlanManager() {
        loadFloorPlans(); // Load existing floor plans during initialization
    }

    // Upload a new floor plan
    void uploadFloorPlan(const string& planName, const string& adminName, int capacity, bool isAvailable) {
        FloorPlan newPlan;
        newPlan.filename = planName + ".txt";
        newPlan.lastModifiedBy = adminName;
        newPlan.timestamp = time(0); // Current time
        newPlan.capacity = capacity;  // Set the capacity
        newPlan.isAvailable = isAvailable; // Set the availability

        // Create a new file for the floor plan
        ofstream file(newPlan.filename);
        if (file.is_open()) {
            file << "Floor Plan for: " << planName << endl;
            file << "Uploaded by: " << adminName << endl;
            file << "Timestamp: " << ctime(&newPlan.timestamp);
            file << "Capacity: " << capacity << endl;  // Save capacity
            file << "Is Available: " << (isAvailable ? "Yes" : "No") << endl;  // Save availability
            file.close();
            floorPlans[planName] = newPlan;

            // Save only the floor plan name to floor_plans.txt
            ofstream plansFile("floor_plans.txt", ios::app);
            if (plansFile.is_open()) {
                plansFile << planName << endl; // Save only the plan name
                plansFile.close();
            }

            cout << "Floor plan " << planName << " uploaded successfully." << endl;
        } else {
            cout << "Error creating floor plan file." << endl;
        }
    }

    // Modify an existing floor plan
    void modifyFloorPlan(const string& planName, const string& adminName, int capacity, bool isAvailable) {
        if (floorPlans.find(planName) != floorPlans.end()) {
            FloorPlan& plan = floorPlans[planName];
            plan.lastModifiedBy = adminName;
            plan.timestamp = time(0); // Update timestamp
            plan.capacity = capacity; // Update capacity
            plan.isAvailable = isAvailable; // Update availability

            // Append modification details to the floor plan file
            ofstream file(plan.filename, ios::app);
            if (file.is_open()) {
                file << "\nModified by: " << adminName << endl;
                file << "Timestamp: " << ctime(&plan.timestamp);
                file << "Updated Capacity: " << capacity << endl;  // Save updated capacity
                file << "Updated Availability: " << (isAvailable ? "Yes" : "No") << endl;  // Save updated availability
                file.close();
                cout << "Floor plan " << planName << " modified successfully." << endl;
            } else {
                cout << "Error modifying floor plan file." << endl;
            }
        } else {
            cout << "Floor plan " << planName << " does not exist." << endl;
        }
    }

    // View floor plan details (full content of the file)
    void viewFloorPlan(const string& planName) {
        if (floorPlans.find(planName) != floorPlans.end()) {
            FloorPlan& plan = floorPlans[planName];

            // Open the floor plan file to display its content
            ifstream file(plan.filename);
            if (file.is_open()) {
                cout << "Contents of " << plan.filename << ":" << endl;
                string line;
                while (getline(file, line)) {
                    cout << line << endl; // Print each line of the floor plan file
                }
                file.close();
            } else {
                cout << "Error opening floor plan file: " << plan.filename << endl;
            }
        } else {
            cout << "Floor plan " << planName << " does not exist." << endl;
        }
    }
};

// Function to authenticate admin
bool authenticateAdmin(const string& username, const string& password) {
    ifstream file("plain_admins.txt"); // Assuming plain_admins.txt contains entries in format: username password
    string storedUsername, storedPassword;

    while (file >> storedUsername >> storedPassword) {
        if (storedUsername == username && storedPassword == password) {
            return true;
        }
    }

    return false;
}

int main() {
    FloorPlanManager manager;
    string adminName, planName, password;
    int choice;
    int capacity;
    bool isAvailable;

    // Admin login
    cout << "Enter your admin username: ";
    cin >> adminName;
    cout << "Enter your password: ";
    cin >> password;

    if (!authenticateAdmin(adminName, password)) {
        cout << "You are not an admin; you can't modify or upload floor plans." << endl;
        return 0; // Exit if authentication fails
    }

    // Main menu for authenticated admins
    while (true) {
        cout << "\n1. Upload Floor Plan\n2. Modify Floor Plan\n3. View Floor Plan\n4. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1: // Upload Floor Plan
            cout << "Enter floor plan name: ";
            cin >> planName;
            cout << "Enter capacity: ";
            cin >> capacity;
            cout << "Is the floor available? (1 for Yes, 0 for No): ";
            cin >> isAvailable;
            manager.uploadFloorPlan(planName, adminName, capacity, isAvailable);
            break;
        case 2: // Modify Floor Plan
            cout << "Enter floor plan name: ";
            cin >> planName;
            cout << "Enter new capacity: ";
            cin >> capacity;
            cout << "Is the floor available? (1 for Yes, 0 for No): ";
            cin >> isAvailable;
            manager.modifyFloorPlan(planName, adminName, capacity, isAvailable);
            break;
        case 3: // View Floor Plan
            cout << "Enter floor plan name: ";
            cin >> planName;
            manager.viewFloorPlan(planName);
            break;
        case 4: // Exit
            return 0;
        default:
            cout << "Invalid choice!" << endl;
        }
    }

    return 0;
}
