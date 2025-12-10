
#ifndef AUTH_HPP
#define AUTH_HPP

#include <string>
#include <unordered_map>
#include <vector>

class Authentication {
public:
    enum class Role {
        USER,
        ADMIN
    };

    Authentication();
    bool registerUser(const std::string& username, const std::string& password);
    bool registerAdmin(const std::string& username, const std::string& password);
    bool loginUser(const std::string& username, const std::string& password, Role& role);
    bool login(const std::string& username, const std::string& password, bool isAdmin);
    bool deleteUser(const std::string& usernameToDelete);
    bool editUser(const std::string& usernameToEdit, const std::string& newPassword, Role newRole);
    std::vector<std::pair<std::string, Role>> getUsersAndAdmins() const;

private:
    std::unordered_map<std::string, std::pair<std::size_t, Role>> users;
    const std::string HASHED_USERS_FILE = "output/hashed_users.txt";
    const std::string HASHED_ADMINS_FILE = "output/hashed_admins.txt";

    std::size_t hash_password(const std::string& password);
    void load_users();
    void save_users();
};

#endif // AUTH_HPP
