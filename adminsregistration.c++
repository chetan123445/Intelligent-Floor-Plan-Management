#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <functional>
#include <cctype> // For character checking

using namespace std;

class AdminAuth {
private:
    unordered_map<string, size_t> admins; // Store admin username and hashed password
    const string hashedPasswordFile = "hashed_admins.txt"; // File for hashed passwords
    const string plainPasswordFile = "plain_admins.txt"; // File for plain passwords

public:
    AdminAuth() {
        loadAdmins(); // Load admins from file during initialization
    }

    // Validate password
    bool isValidPassword(const string& password) {
        // Check length
        if (password.length() < 1 || password.length() > 8) {
            cout << "Password must be between 1 and 8 characters." << endl;
            return false;
        }

        // Check for valid characters (alphanumeric and underscore)
        for (char c : password) {
            if (!isalnum(c) && c != '_') {
                cout << "Password can only contain alphanumeric characters and underscores." << endl;
                return false;
            }
        }

        return true; // Password is valid
    }

    // Register new admin
    void registerAdmin(const string& username) {
        if (admins.find(username) != admins.end()) {
            cout << "Username already exists." << endl;
            return; // Exit if username exists
        }

        string password;
        while (true) {
            cout << "Enter password: ";
            cin >> password;

            // Validate password before registration
            if (!isValidPassword(password)) {
                continue; // Prompt for password again if validation fails
            }

            size_t hashedPassword = hash<string>{}(password);

            // Check if the password already exists
            for (const auto& admin : admins) {
                if (admin.second == hashedPassword) {
                    cout << "Password already exists." << endl;
                    return; // Exit if password exists
                }
            }

            admins[username] = hashedPassword;
            saveAdmins(); // Save to files
            savePlainPasswords(username, password); // Save plain passwords to a separate file
            cout << "Admin " << username << " registered successfully." << endl;
            break; // Exit the password input loop upon successful registration
        }
    }

    // Load admins from hashed passwords file
    void loadAdmins() {
        ifstream file(hashedPasswordFile);
        if (file.is_open()) {
            string username;
            size_t hashedPassword;
            while (file >> username >> hashedPassword) {
                admins[username] = hashedPassword;
            }
            file.close();
        }
    }

    // Save hashed passwords to file
    void saveAdmins() {
        ofstream file(hashedPasswordFile);
        if (file.is_open()) {
            for (const auto& admin : admins) {
                file << admin.first << " " << admin.second << endl;
            }
            file.close();
        }
    }

    // Save plain passwords to a separate file
    void savePlainPasswords(const string& username, const string& password) {
        ofstream file(plainPasswordFile, ios::app); // Append mode
        if (file.is_open()) {
            file << username << " " << password << endl; // Save the username and plain password
            file.close();
        }
    }
};

int main() {
    AdminAuth authSystem;

    string username;

    while (true) {
        cout << "\n1. Register Admin\n2. Exit\n";
        cout << "Enter choice: ";
        int choice;
        cin >> choice;

        if (choice == 1) {
            cout << "Enter username: ";
            cin >> username;
            authSystem.registerAdmin(username);
        } else if (choice == 2) {
            break;
        } else {
            cout << "Invalid choice, try again." << endl;
        }
    }

    return 0;
}
