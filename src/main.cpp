
#include "ui.hpp"
#include "auth.hpp"
#include "room.hpp"
#include "meetingroom.hpp"
#include "offlinemechanism.hpp"
#include <iostream>
#include <string>

void handleAdminMenu(const std::string& username, Authentication& auth, RoomManager& rm, OfflineManager& om);
void handleUserMenu(const std::string& username, RoomBookingSystem& rbs, OfflineManager& om);

int main() {
    Authentication auth;
    RoomManager rm;
    RoomBookingSystem rbs(rm);
    OfflineManager om(auth, rm, rbs);

    while (true) {
        UI::displayMainMenu();
        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1: {
                std::string username, password;
                std::cout << "Enter username: ";
                std::cin >> username;
                std::cout << "Enter password: ";
                std::cin >> password;
                if (auth.registerUser(username, password)) {
                    UI::displayMessage("Registration successful!");
                } else {
                    UI::displayMessage("Registration failed. Please try again.");
                }
                break;
            }
            case 2: {
                std::string username, password;
                std::cout << "Enter username: ";
                std::cin >> username;
                std::cout << "Enter password: ";
                std::cin >> password;
                Authentication::Role role;
                if (auth.loginUser(username, password, role)) {
                    UI::displayMessage("Login successful!");
                    if (role == Authentication::Role::ADMIN) {
                        UI::displayMessage("Admin login successful!");
                        handleAdminMenu(username, auth, rm, om);
                    } else {
                        handleUserMenu(username, rbs, om);
                    }
                } else {
                    UI::displayMessage("Invalid username or password.");
                }
                break;
            }
            case 3:
                return 0;
            default:
                UI::displayInvalidChoice();
                break;
        }
    }

    return 0;
}

void handleAdminMenu(const std::string& username, Authentication& auth, RoomManager& rm, OfflineManager& om) {
    while (true) {
        UI::displayAdminMenu();
        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1: {
                if (om.isOffline()) {
                    std::string roomName;
                    int capacity;
                    bool isAvailable;
                    std::cout << "Enter a unique name for the new room: ";
                    std::cin >> roomName;
                    std::cout << "Enter the capacity of the room: ";
                    std::cin >> capacity;
                    std::cout << "Is the room available for booking? (1 for Yes, 0 for No): ";
                    std::cin >> isAvailable;
                    om.queueUploadRoom(username, roomName, capacity, isAvailable);
                } else {
                    rm.uploadRoom(username);
                }
                break;
            }
            case 2: {
                if (om.isOffline()) {
                    std::string roomName;
                    int capacity;
                    bool isAvailable;
                    std::cout << "Enter the name of the room to modify: ";
                    std::cin >> roomName;
                    std::cout << "Enter the new capacity: ";
                    std::cin >> capacity;
                    std::cout << "Is the room available for booking? (1 for Yes, 0 for No): ";
                    std::cin >> isAvailable;
                    om.queueModifyRoom(username, roomName, capacity, isAvailable);
                } else {
                    rm.modifyRoom(username);
                }
                break;
            }
            case 3:
                rm.viewRoom();
                break;
            case 4: {
                if (om.isOffline()) {
                    std::string new_admin_username, new_admin_password;
                    std::cout << "Enter new admin username: ";
                    std::cin >> new_admin_username;
                    std::cout << "Enter new admin password: ";
                    std::cin >> new_admin_password;
                    om.queueRegisterNewAdmin(username, new_admin_username, new_admin_password);
                } else {
                    std::string new_admin_username, new_admin_password;
                    std::cout << "Enter new admin username: ";
                    std::cin >> new_admin_username;
                    std::cout << "Enter new admin password: ";
                    std::cin >> new_admin_password;
                    if (auth.registerAdmin(new_admin_username, new_admin_password)) {
                        UI::displayMessage("New admin registered successfully!");
                    } else {
                        UI::displayMessage("Admin registration failed. Please try again.");
                    }
                }
                break;
            }
            case 5: {
                std::cout << "1. Go Offline\n2. Go Online\n";
                int offline_choice;
                std::cin >> offline_choice;
                if (offline_choice == 1) {
                    om.goOffline();
                } else if (offline_choice == 2) {
                    om.goOnline();
                } else {
                    UI::displayInvalidChoice();
                }
                break;
            }
            case 6:
                return;
            default:
                UI::displayInvalidChoice();
                break;
        }
    }
}

void handleUserMenu(const std::string& username, RoomBookingSystem& rbs, OfflineManager& om) {
    while (true) {
        UI::displayUserMenu();
        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1:
                if (om.isOffline()) {
                    UI::displayMessage("You are offline. Please go online to book a room.");
                } else {
                    rbs.bookRoom(username);
                }
                break;
            case 2:
                if (om.isOffline()) {
                    std::string roomName;
                    std::cout << "Enter room name to release: ";
                    std::cin >> roomName;
                    om.queueReleaseRoom(username, roomName);
                } else {
                    rbs.releaseRoom(username);
                }
                break;
            case 3:
                rbs.showRoomStatuses(username);
                break;
            case 4:
                return;
            default:
                UI::displayInvalidChoice();
                break;
        }
    }
}
