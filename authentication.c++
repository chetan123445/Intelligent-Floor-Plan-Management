#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <functional>
#include <cctype> // For character checking

using namespace std;

class UserAuth {
private:
    unordered_map<string, size_t> users; // Store username and hashed password
    const string hashedPasswordFile = "hashed_users.txt"; // File for hashed passwords
    const string plainPasswordFile = "plain_users.txt"; // File for plain passwords

public:
    UserAuth() {
        loadUsers(); // Load users from file during initialization
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

    // Register new user (admin)
    void registerUser(const string& username) {
        // Check if the username already exists
        if (users.find(username) != users.end()) {
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
            for (const auto& user : users) {
                if (user.second == hashedPassword) {
                    cout << "Password already exists." << endl;
                    return; // Exit if password exists
                }
            }

            users[username] = hashedPassword;
            saveUsers(); // Save to files
            savePlainPasswords(username, password); // Save plain passwords to a separate file
            cout << "User " << username << " registered successfully." << endl;
            break; // Exit the password input loop upon successful registration
        }
    }

    // Login user
    bool loginUser(const string& username, const string& password) {
        size_t hashedPassword = hash<string>{}(password);
        if (users.find(username) != users.end() && users[username] == hashedPassword) {
            cout << "Login successful! Welcome, " << username << "." << endl;
            return true;
        } else {
            cout << "Invalid username or password." << endl;
            return false;
        }
    }

    // Load users from hashed passwords file
    void loadUsers() {
        ifstream file(hashedPasswordFile);
        if (file.is_open()) {
            string username;
            size_t hashedPassword;
            while (file >> username >> hashedPassword) {
                users[username] = hashedPassword;
            }
            file.close();
        }
    }

    // Save hashed passwords to file
    void saveUsers() {
        ofstream file(hashedPasswordFile);
        if (file.is_open()) {
            for (const auto& user : users) {
                file << user.first << " " << user.second << endl;
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
    UserAuth authSystem;

    int choice;
    string username;

    while (true) {
        cout << "\n1. Register\n2. Login\n3. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1) {
            cout << "Enter username: ";
            cin >> username;
            authSystem.registerUser(username);
        } else if (choice == 2) {
            cout << "Enter username: ";
            cin >> username;
            string password;
            cout << "Enter password: ";
            cin >> password;
            authSystem.loginUser(username, password);
        } else if (choice == 3) {
            break;
        } else {
            cout << "Invalid choice, try again." << endl;
        }
    }

    return 0;
}
