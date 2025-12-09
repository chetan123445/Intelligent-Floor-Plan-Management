
#ifndef AUTH_HPP
#define AUTH_HPP

#include <string>
#include <unordered_map>

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

private:
    std::unordered_map<std::string, std::pair<std::size_t, Role>> users;
    const std::string HASHED_USERS_FILE = "hashed_users.txt";
    const std::string HASHED_ADMINS_FILE = "hashed_admins.txt";

    std::size_t hash_password(const std::string& password);
    void load_users();
    void save_users();
};

#endif // AUTH_HPP
