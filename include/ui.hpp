
#ifndef UI_HPP
#define UI_HPP

#include <string>

class UI {
public:
    static void displayMainMenu();
    static void displayAdminMenu();
    static void displayUserMenu();
    static void displayInvalidChoice();
    static void clearScreen();
    static void displayMessage(const std::string& message);
};

#endif // UI_HPP
