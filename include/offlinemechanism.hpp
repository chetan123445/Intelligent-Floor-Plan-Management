
#ifndef OFFLINEMECHANISM_HPP
#define OFFLINEMECHANISM_HPP

#include "auth.hpp"
#include "room.hpp"
#include "meetingroom.hpp"

class OfflineManager {
public:
    OfflineManager(Authentication& auth, RoomManager& rm, RoomBookingSystem& rbs);
    void goOffline();
    void goOnline();
    bool isOffline() const;

    void queueUploadRoom(const std::string& adminName, const std::string& roomName, int capacity, bool isAvailable);
    void queueModifyRoom(const std::string& adminName, const std::string& roomName, int capacity, bool isAvailable);
    void queueRegisterNewAdmin(const std::string& adminName, const std::string& newAdminUsername, const std::string& newAdminPassword);
    void queueBookRoom(const std::string& username, int participants, const std::string& roomName);
    void queueDeleteUser(const std::string& targetUsername);
    void queueEditUser(const std::string& targetUsername, const std::string& newPassword, Authentication::Role newRole);
    void queueDeleteRoom(const std::string& roomName, const std::string& adminName);
    void queueReleaseRoom(const std::string& username, const std::string& roomName); // New method
    std::vector<std::string> getQueueForDisplay();

private:
    bool offline;
    const std::string OFFLINE_CHANGES_FILE = "offline_changes.txt";
    const std::string OFFLINE_USERS_FILE = "offline_users.txt"; // For user/admin management


    Authentication& auth;
    RoomManager& rm;
    RoomBookingSystem& rbs;

    void synchronizeChanges();
    void applyUploadRoom(const std::string& adminName, const std::string& roomName, int capacity, bool isAvailable);
    void applyModifyRoom(const std::string& adminName, const std::string& roomName, int capacity, bool isAvailable);
    void applyRegisterNewAdmin(const std::string& adminName, const std::string& newAdminUsername, const std::string& newAdminPassword);
    void applyBookRoom(const std::string& username, int participants, const std::string& roomName);
    void applyDeleteUser(const std::string& targetUsername);
    void applyEditUser(const std::string& targetUsername, const std::string& newPassword, Authentication::Role newRole);
    void applyDeleteRoom(const std::string& roomName, const std::string& adminName);
    void applyReleaseRoom(const std::string& username, const std::string& roomName); // New method
};

#endif // OFFLINEMECHANISM_HPP
