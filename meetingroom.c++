#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <cctype> // For isalnum and isalpha

using namespace std;

struct Room {
    string name;
    int capacity;
    bool isAvailable;
    time_t lastBookedTime;
};

class RoomBookingSystem {
private:
    vector<Room> rooms;

public:
    // Initialize room list by reading from files
    RoomBookingSystem(const string& floorPlanFile) {
        ifstream floorPlan(floorPlanFile);
        string floorName;

        // Read each floor name from the floor plan file
        while (getline(floorPlan, floorName)) {
            // Construct the filename for each floor
            string roomFilePath = floorName + ".txt"; // 

            ifstream roomFile(roomFilePath);
            string line;
            int capacity = -1; // Default capacity (invalid)
            bool isAvailable = false; // Default availability (invalid)
            
            // Read and parse room information
            while (getline(roomFile, line)) {
                if (line.find("Capacity:") != string::npos) {
                    // Extract initial capacity
                    capacity = stoi(line.substr(line.find(":") + 1));
                } else if (line.find("Is Available:") != string::npos) {
                    // Extract initial availability
                    string availableStr = line.substr(line.find(":") + 1);
                    isAvailable = (availableStr.find("Yes") != string::npos);
                } else if (line.find("Updated Capacity:") != string::npos) {
                    // Update capacity if found in modified data
                    capacity = stoi(line.substr(line.find(":") + 1));
                } else if (line.find("Updated Availability:") != string::npos) {
                    // Update availability if found in modified data
                    string updatedAvailableStr = line.substr(line.find(":") + 1);
                    isAvailable = (updatedAvailableStr.find("Yes") != string::npos);
                }
            }
            // Ensure valid capacity and availability are found
            if (capacity != -1) {
                rooms.push_back({floorName, capacity, isAvailable, 0});
            }
        }
    }

    // Suggest rooms based on participants count and availability
    void suggestRoom(int participants) {
        vector<Room> suitableRooms;

        for (const Room& room : rooms) {
            if (room.capacity > participants && room.isAvailable) {
                suitableRooms.push_back(room);
            }
        }

        if (suitableRooms.empty()) {
            cout << "No suitable rooms available for " << participants << " participants." << endl;
            return;
        }

        cout << "Suggested Rooms:" << endl;
        for (const Room& room : suitableRooms) {
            cout << room.name << " (Capacity: " << room.capacity << ")" << endl;
        }
    }

    void bookRoom(const string& roomName, const string& username) {
        for (Room& room : rooms) {
            if (room.name == roomName) {
                if (room.isAvailable) {
                    room.isAvailable = false;
                    room.lastBookedTime = time(0);
                    cout << room.name << " has been successfully booked." << endl;

                    // Update the corresponding floor file
                    string floorFileName = roomName.substr(0, roomName.find('_')) + ".txt"; // Extract floor name
                    ofstream floorFile(floorFileName, ios::app);
                    
                    if (floorFile.is_open()) {
                        // Append booking details
                        floorFile << "Booked by: " << username << endl;
                        floorFile << "Updated Availability: No" << endl; // or 0 based on your requirement
                        floorFile.close();
                        cout << "Booking information added to " << floorFileName << endl;
                    } else {
                        cout << "Failed to open " << floorFileName << " for updating." << endl;
                    }

                } else {
                    cout << room.name << " is already booked." << endl;
                }
                return;
            }
        }
        cout << "Room " << roomName << " not found." << endl;
    }

    void releaseRoom(const string& roomName, const string& username) {
        for (Room& room : rooms) {
            if (room.name == roomName) {
                if (!room.isAvailable) {
                    room.isAvailable = true; // Mark the room as available
                    room.lastBookedTime = 0; // Reset the last booked time
                    cout << room.name << " has been successfully released." << endl;

                    // Update the corresponding floor file
                    string floorFileName = roomName.substr(0, roomName.find('_')) + ".txt"; // Extract floor name
                    ofstream floorFile(floorFileName, ios::app);
                    
                    if (floorFile.is_open()) {
                        // Append release details
                        floorFile << "Released by: " << username << endl;
                        floorFile << "Updated Availability: Yes" << endl; // or 1 based on your requirement
                        floorFile.close();
                        cout << "Release information added to " << floorFileName << endl;
                    } else {
                        cout << "Failed to open " << floorFileName << " for updating." << endl;
                    }
                } else {
                    cout << room.name << " is already available." << endl;
                }
                return;
            }
        }
        cout << "Room " << roomName << " not found." << endl;
    }


    void showRoomStatuses() {
        cout << "\nRoom Statuses:" << endl;
        for (const Room& room : rooms) {
            cout << "Room: " << room.name
                 << " | Capacity: " << room.capacity
                 << " | Available: " << (room.isAvailable ? "Yes" : "No ")
                 << " | Last Booked: " << (room.lastBookedTime == 0 ? "Never" : ctime(&room.lastBookedTime))
                 << endl;
        }
    }
};

// Simple hash function for demonstration (not secure for production)
string hashPassword(const string& password) {
    // Example hashing: convert each character to its ASCII value and concatenate
    string hashed;
    for (char c : password) {
        hashed += to_string(static_cast<int>(c));
    }
    return hashed;
}

// Authenticate user credentials
bool authenticateUser(const string& username, const string& password) {
    ifstream userFile("hashed_users.txt");
    string storedUsername, storedPassword;

    while (userFile >> storedUsername >> storedPassword) {
        if (storedUsername == username) {
            return storedPassword == hashPassword(password);
        }
    }
    return false;
}

// Validate password
bool isValidPassword(const string& password) {
    if (password.length() < 1 || password.length() > 8) {
        return false; // Length should be between 1 and 8
    }

    for (char c : password) {
        if (!isalnum(c) && c != '_') {
            return false; // Must be alphanumeric or underscore
        }
    }
    return true;
}

// Register a new user
bool registerUser(const string& username, const string& password) {
    ofstream plainUserFile("plain_users.txt", ios::app);
    ofstream hashedUserFile("hashed_users.txt", ios::app);

    // Save plain username and password
    plainUserFile << username << " " << password << endl;

    // Save hashed username and hashed password
    hashedUserFile << username << " " << hashPassword(password) << endl;

    return true;
}

void displayMenu() {
    cout << "\n1. Register\n2. Login\n3. Exit\n";
    cout << "Enter choice: ";
}

int main() {
    RoomBookingSystem bookingSystem("floor_plans.txt"); // Load room data from floor plan
    int choice;

    while (true) {
        displayMenu();
        cin >> choice;

        string username, password, tempUsername;
        bool userExists = false;
        int attempts = 0;
        ifstream userFile;

        switch (choice) {
            case 1: // Register a new user
                cout << "Enter username: ";
                cin >> username;
                cout << "Enter password: ";
                cin >> password;

                if (!isValidPassword(password)) {
                    cout << "Invalid password! Password must be alphanumeric, can contain underscores, and must be between 1 and 8 characters long." << endl;
                    break; // Exit case 1
                }

                registerUser(username, password);
                cout << "Registration successful!" << endl;
                break;

            case 2: // Login
                cout << "Enter username: ";
                cin >> username;

                // Check if username exists
                userFile.open("hashed_users.txt");
                userExists = false;

                while (userFile >> tempUsername) {
                    if (tempUsername == username) {
                        userExists = true;
                        break;
                    }
                }
                userFile.close();

                if (!userExists) {
                    cout << "Username not found. Exiting..." << endl;
                    return 1;
                }

                // Password check with 3 attempts
                while (attempts < 3) {
                    cout << "Enter password: ";
                    cin >> password;
                    if (authenticateUser(username, password)) {
                        cout << "Login successful!" << endl;
                        int actionChoice;
                        while (true) {
                            cout << "\n1. Book a Room\n2. Release a Room\n3. Show Room Statuses\n4. Logout\n";
                            cout << "Enter choice: ";
                            cin >> actionChoice;

                            string roomName;
                            switch (actionChoice) {
                                case 1: {
                                    int participants;
                                    cout << "Enter required capacity: ";
                                    cin >> participants;
                                    bookingSystem.suggestRoom(participants);
                                    cout << "Enter room name to book: ";
                                    cin >> roomName;
                                    bookingSystem.bookRoom(roomName,username);
                                    break;
                                }
                                case 2: { // Release a Room
                                    cout << "Enter room name to release: ";
                                    cin >> roomName;
                                    bookingSystem.releaseRoom(roomName, username);
                                    break;
                                }
                                case 3:
                                    bookingSystem.showRoomStatuses();
                                    break;
                                case 4:
                                    cout << "Logging out..." << endl;
                                    break;
                                default:
                                    cout << "Invalid choice!" << endl;
                            }
                            if (actionChoice == 4) break;
                        }
                        break;
                    } else {
                        cout << "Incorrect password. Try again." << endl;
                        attempts++;
                    }
                }

                if (attempts == 3) {
                    cout << "Too many incorrect attempts. Exiting..." << endl;
                    return 1;
                }
                break;

            case 3:
                cout << "Exiting the program." << endl;
                return 0;
            default:
                cout << "Invalid choice!" << endl;
        }
    }

    return 0;
}
