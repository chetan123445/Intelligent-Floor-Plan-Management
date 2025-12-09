

#include "auth.hpp"
#include "ui.hpp"
#include <fstream>
#include <functional>

Authentication::Authentication() {
    load_users();
}

std::size_t Authentication::hash_password(const std::string& password) {
    return std::hash<std::string>{}(password);
}

void Authentication::load_users() {
    users.clear();
    std::ifstream users_file(HASHED_USERS_FILE);
    if (users_file.is_open()) {
        std::string username;
        std::size_t hashed_password;
        while (users_file >> username >> hashed_password) {
            users[username] = {hashed_password, Role::USER};
        }
        users_file.close();
    }

    std::ifstream admins_file(HASHED_ADMINS_FILE);
    if (admins_file.is_open()) {
        std::string username;
        std::size_t hashed_password;
        while (admins_file >> username >> hashed_password) {
            users[username] = {hashed_password, Role::ADMIN};
        }
        admins_file.close();
    }

    // Ensure super admin is always present
    if (users.find("Chetan") == users.end()) {
        users["Chetan"] = {hash_password("123"), Role::ADMIN};
        save_users();
    }
}

void Authentication::save_users() {
    std::ofstream users_file(HASHED_USERS_FILE);
    std::ofstream admins_file(HASHED_ADMINS_FILE);
    if (users_file.is_open() && admins_file.is_open()) {
        for (const auto& user : users) {
            if (user.second.second == Role::USER) {
                users_file << user.first << " " << user.second.first << std::endl;
            } else {
                admins_file << user.first << " " << user.second.first << std::endl;
            }
        }
        users_file.close();
        admins_file.close();
    }
}

bool Authentication::registerUser(const std::string& username, const std::string& password) {
    if (users.find(username) != users.end()) {
        UI::displayMessage("Username already exists.");
        return false;
    }

    std::size_t hashed_password = hash_password(password);
    users[username] = {hashed_password, Role::USER};
    save_users();
    return true;
}

bool Authentication::registerAdmin(const std::string& username, const std::string& password) {
    if (users.find(username) != users.end()) {
        UI::displayMessage("Username already exists.");
        return false;
    }

    std::size_t hashed_password = hash_password(password);
    users[username] = {hashed_password, Role::ADMIN};
    save_users();
    return true;
}

bool Authentication::login(const std::string& username, const std::string& password, bool isAdmin) {
    Role role;
    if (!loginUser(username, password, role)) return false;

    return (isAdmin && role == Role::ADMIN) || (!isAdmin && role == Role::USER);
}

bool Authentication::loginUser(const std::string& username, const std::string& password, Role& role) {
    auto it = users.find(username);
    if (it != users.end()) {
        std::size_t hashed_password = hash_password(password);
        if (it->second.first == hashed_password) {
            role = it->second.second;
            return true; // Login successful
        }
    }
    return false; // Invalid username or password
}

bool Authentication::deleteUser(const std::string& usernameToDelete) {
    if (usernameToDelete == "Chetan") {
        UI::displayMessage("Error: Cannot delete superadmin 'Chetan'.");
        return false;
    }

    auto it = users.find(usernameToDelete);
    if (it != users.end()) {
        users.erase(it);
        save_users();
        UI::displayMessage("User/Admin '" + usernameToDelete + "' deleted successfully.");
        return true;
    } else {
        UI::displayMessage("Error: User/Admin '" + usernameToDelete + "' not found.");
        return false;
    }
}

bool Authentication::editUser(const std::string& usernameToEdit, const std::string& newPassword, Role newRole) {
    if (usernameToEdit == "Chetan") {
        UI::displayMessage("Error: Cannot edit superadmin 'Chetan'.");
        return false;
    }

    auto it = users.find(usernameToEdit);
    if (it != users.end()) {
        it->second.first = hash_password(newPassword);
        it->second.second = newRole;
        save_users();
        UI::displayMessage("User/Admin '" + usernameToEdit + "' updated successfully.");
        return true;
    } else {
        UI::displayMessage("Error: User/Admin '" + usernameToEdit + "' not found.");
        return false;
    }
}

std::vector<std::pair<std::string, Authentication::Role>> Authentication::getUsersAndAdmins() const {
    std::vector<std::pair<std::string, Role>> list;
    for (const auto& pair : users) {
        list.push_back({pair.first, pair.second.second});
    }
    return list;
}
