
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
    void queueReleaseRoom(const std::string& username, const std::string& roomName);
    void queueBookRoom(const std::string& username, int participants, const std::string& roomName);
    std::vector<std::string> getQueueForDisplay();

private:
    bool offline;
    const std::string OFFLINE_CHANGES_FILE = "offline_changes.txt";

    Authentication& auth;
    RoomManager& rm;
    RoomBookingSystem& rbs;

    void synchronizeChanges();
    void applyUploadRoom(const std::string& adminName, const std::string& roomName, int capacity, bool isAvailable);
    void applyModifyRoom(const std::string& adminName, const std::string& roomName, int capacity, bool isAvailable);
    void applyRegisterNewAdmin(const std::string& adminName, const std::string& newAdminUsername, const std::string& newAdminPassword);
    void applyBookRoom(const std::string& username, int participants, const std::string& roomName);
    void applyReleaseRoom(const std::string& username, const std::string& roomName);
};

#endif // OFFLINEMECHANISM_HPP
