#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <ctime>

using namespace std;

// Structure to represent a meeting room
struct Room {
    string name;
    int capacity;
    bool isAvailable;
    time_t lastBookedTime;
};

// Class to manage room bookings and suggestions
class RoomBookingSystem {
private:
    vector<Room> rooms;

public:
    // Initialize room list
    RoomBookingSystem() {
        // Sample rooms with different capacities
        rooms = {
            {"Room A", 10, true, 0},
            {"Room B", 15, true, 0},
            {"Room C", 20, true, 0},
            {"Room D", 5, true, 0}
        };
    }

    // Function to suggest a room based on number of participants
    void suggestRoom(int participants) {
        vector<Room> suitableRooms;

        // Filter rooms that can accommodate the participants and are available
        for (const Room& room : rooms) {
            if (room.capacity >= participants && room.isAvailable) {
                suitableRooms.push_back(room);
            }
        }

        if (suitableRooms.empty()) {
            cout << "No suitable rooms are available for " << participants << " participants." << endl;
            return;
        }

        // Suggest the first suitable room
        Room bestRoom = suitableRooms.front();
        cout << "Suggested Room: " << bestRoom.name << " (Capacity: " << bestRoom.capacity << ")" << endl;
    }

    // Function to book a room
    void bookRoom(const string& roomName) {
        for (Room& room : rooms) {
            if (room.name == roomName) {
                if (room.isAvailable) {
                    room.isAvailable = false;
                    room.lastBookedTime = time(0); // Record the booking time
                    cout << room.name << " has been successfully booked." << endl;
                } else {
                    cout << room.name << " is already booked." << endl;
                }
                return;
            }
        }
        cout << "Room " << roomName << " not found." << endl;
    }

    // Function to release a room after the meeting is over
    void releaseRoom(const string& roomName) {
        for (Room& room : rooms) {
            if (room.name == roomName) {
                if (!room.isAvailable) {
                    room.isAvailable = true;
                    cout << room.name << " is now available for booking." << endl;
                } else {
                    cout << room.name << " is already available." << endl;
                }
                return;
            }
        }
        cout << "Room " << roomName << " not found." << endl;
    }

    // Show all room statuses
    void showRoomStatuses() {
        cout << "\nRoom Statuses:" << endl;
        for (const Room& room : rooms) {
            cout << "Room: " << room.name
                 << " | Capacity: " << room.capacity
                 << " | Available: " << (room.isAvailable ? "Yes" : "No")
                 << " | Last Booked: " << (room.lastBookedTime == 0 ? "Never" : ctime(&room.lastBookedTime))
                 << endl;
        }
    }
};

// Function to authenticate user
bool authenticateUser(const string& username, const string& password) {
    ifstream userFile("plain_users.txt");
    string storedUsername, storedPassword;

    while (userFile >> storedUsername >> storedPassword) {
        if (storedUsername == username) {
            return storedPassword == password; // Return true only if the password matches
        }
    }
    return false; // Return false if username not found
}

// Function to register a new user
bool registerUser(const string& username, const string& password) {
    ofstream userFile("plain_users.txt", ios::app);
    userFile << username << " " << password << endl; // Store username and password
    return true;
}

// Function to display menu
void displayMenu() {
    cout << "\n1. Register\n2. Login\n3. Exit\n";
    cout << "Enter choice: ";
}

int main() {
    RoomBookingSystem bookingSystem;
    int choice;

    while (true) {
        displayMenu();
        cin >> choice;

        string username, password, tempUsername;
        bool userExists = false; // Initialize outside to avoid jump error
        int attempts = 0;        // Initialize attempts here
        ifstream userFile;       // Declare userFile here to avoid jump error

        switch (choice) {
            case 1: // Register a new user
                cout << "Enter username: ";
                cin >> username;
                cout << "Enter password: ";
                cin >> password;
                registerUser(username, password);
                cout << "Registration successful!" << endl;
                break;

            case 2: // Login
                cout << "Enter username: ";
                cin >> username;

                // Check if username exists
                userFile.open("plain_users.txt"); // Open the file here
                userExists = false; // Resetting for each login attempt

                while (userFile >> tempUsername) {
                    if (tempUsername == username) {
                        userExists = true;
                        break;
                    }
                }

                userFile.close(); // Close the file after checking

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

                        // Ask for capacity and suggest room
                        int participants;
                        cout << "Enter number of participants: ";
                        cin >> participants;
                        bookingSystem.suggestRoom(participants);

                        // Menu for booking or releasing rooms
                        int actionChoice;
                        while (true) {
                            cout << "\n1. Book a Room\n2. Release a Room\n3. Show Room Statuses\n4. Logout\n";
                            cout << "Enter choice: ";
                            cin >> actionChoice;

                            string roomName;
                            switch (actionChoice) {
                                case 1:
                                    cout << "Enter room name to book: ";
                                    cin >> roomName;
                                    bookingSystem.bookRoom(roomName);
                                    break;
                                case 2:
                                    cout << "Enter room name to release: ";
                                    cin >> roomName;
                                    bookingSystem.releaseRoom(roomName);
                                    break;
                                case 3:
                                    bookingSystem.showRoomStatuses();
                                    break;
                                case 4:
                                    cout << "Logging out..." << endl;
                                    break;
                                default:
                                    cout << "Invalid choice!" << endl;
                            }
                            if (actionChoice == 4) break; // Exit the inner loop
                        }
                        break; // Exit the password check loop
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
                return 0; // Exit the program
            default:
                cout << "Invalid choice!" << endl;
        }
    }

    return 0;
}
