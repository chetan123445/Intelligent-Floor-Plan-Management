
#include "ui.hpp"
#include <iostream>

void UI::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void UI::displayMessage(const std::string& message) {
    std::cout << message << std::endl;
}

void UI::displayMainMenu() {
    // clearScreen();
    std::cout << "=========================================" << std::endl;
    std::cout << "  Intelligent Room Management System" << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << "1. Register (As a new user)" << std::endl;
    std::cout << "2. Login (To access your account)" << std::endl;
    std::cout << "3. Exit" << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << "Admins can be registered by other admins after logging in." << std::endl;
    std::cout << "Enter your choice: ";
}

void UI::displayAdminMenu() {
    // clearScreen();
    std::cout << "=========================================" << std::endl;
    std::cout << "              Admin Menu" << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << "1. Upload Room" << std::endl;
    std::cout << "2. Modify Room" << std::endl;
    std::cout << "3. View Room" << std::endl;
    std::cout << "4. Register New Admin" << std::endl;
    std::cout << "5. Manage Offline Data" << std::endl;
    std::cout << "6. Logout" << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << "Enter your choice: ";
}

void UI::displayUserMenu() {
    // clearScreen();
    std::cout << "=========================================" << std::endl;
    std::cout << "              User Menu" << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << "1. Book a Room" << std::endl; // Existing
    std::cout << "2. Release a Room" << std::endl; // New
    std::cout << "3. Show Rooms Status" << std::endl; // Existing
    std::cout << "4. Logout" << std::endl; // Existing
    std::cout << "=========================================" << std::endl;
    std::cout << "Enter your choice: ";
}

void UI::displayInvalidChoice() {
    std::cout << "Invalid choice. Please try again." << std::endl;
}
