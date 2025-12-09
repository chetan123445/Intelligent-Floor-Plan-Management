#include "offlinemechanism.hpp"
#include "ui.hpp"
#include "room.hpp"
#include "auth.hpp"
#include "meetingroom.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

OfflineManager::OfflineManager(Authentication& auth, RoomManager& rm, RoomBookingSystem& rbs)
    : auth(auth), rm(rm), rbs(rbs), offline(false) {}

void OfflineManager::goOffline() {
    offline = true;
    UI::displayMessage("\nYou are now in offline mode. Changes will be saved locally and synchronized when you go online.");
}

void OfflineManager::goOnline() {
    offline = false;
    UI::displayMessage("\nYou are now back online.");
    synchronizeChanges();
}

bool OfflineManager::isOffline() const {
    return offline;
}

void OfflineManager::queueUploadRoom(const std::string& adminName, const std::string& roomName, int capacity, bool isAvailable) {
    std::ofstream offlineFile(OFFLINE_CHANGES_FILE, std::ios::app);
    if (offlineFile.is_open()) {
        offlineFile << "UPLOAD_ROOM " << adminName << " " << roomName << " " << capacity << " " << (isAvailable ? "Yes" : "No") << std::endl;
        offlineFile.close();
        UI::displayMessage("Offline action: Upload room '" + roomName + "' queued.");
    } else {
        UI::displayMessage("Error: Unable to save offline changes.");
    }
}

void OfflineManager::queueModifyRoom(const std::string& adminName, const std::string& roomName, int capacity, bool isAvailable) {
    std::ofstream offlineFile(OFFLINE_CHANGES_FILE, std::ios::app);
    if (offlineFile.is_open()) {
        offlineFile << "MODIFY_ROOM " << adminName << " " << roomName << " " << capacity << " " << (isAvailable ? "Yes" : "No") << std::endl;
        offlineFile.close();
        UI::displayMessage("Offline action: Modify room '" + roomName + "' queued.");
    } else {
        UI::displayMessage("Error: Unable to save offline changes.");
    }
}

void OfflineManager::queueRegisterNewAdmin(const std::string& adminName, const std::string& newAdminUsername, const std::string& newAdminPassword) {
    std::ofstream offlineFile(OFFLINE_CHANGES_FILE, std::ios::app);
    if (offlineFile.is_open()) {
        offlineFile << "REGISTER_NEW_ADMIN " << adminName << " " << newAdminUsername << " " << newAdminPassword << std::endl;
        offlineFile.close();
        UI::displayMessage("Offline action: Register new admin '" + newAdminUsername + "' queued.");
    } else {
        UI::displayMessage("Error: Unable to save offline changes.");
    }
}

void OfflineManager::queueReleaseRoom(const std::string& username, const std::string& roomName) {
    std::ofstream offlineFile(OFFLINE_CHANGES_FILE, std::ios::app);
    if (offlineFile.is_open()) {
        offlineFile << "RELEASE_ROOM " << username << " " << roomName << std::endl;
        offlineFile.close();
        UI::displayMessage("Offline action: Release room '" + roomName + "' queued.");
    } else {
        UI::displayMessage("Error: Unable to save offline changes.");
    }
}

void OfflineManager::synchronizeChanges() {
    std::ifstream offlineFile(OFFLINE_CHANGES_FILE);
    if (!offlineFile.is_open()) {
        UI::displayMessage("No offline changes to synchronize.");
        return;
    }

    UI::displayMessage("Synchronizing offline changes...");
    std::string line;
    while (std::getline(offlineFile, line)) {
        std::stringstream ss(line);
        std::string action;
        ss >> action;

        if (action == "UPLOAD_ROOM") {
            std::string adminName, roomName, isAvailable_str;
            int capacity;
            ss >> adminName >> roomName >> capacity >> isAvailable_str;
            applyUploadRoom(adminName, roomName, capacity, isAvailable_str == "Yes");
        } else if (action == "MODIFY_ROOM") {
            std::string adminName, roomName, isAvailable_str;
            int capacity;
            ss >> adminName >> roomName >> capacity >> isAvailable_str;
            applyModifyRoom(adminName, roomName, capacity, isAvailable_str == "Yes");
        } else if (action == "REGISTER_NEW_ADMIN") {
            std::string adminName, newAdminUsername, newAdminPassword;
            ss >> adminName >> newAdminUsername >> newAdminPassword;
            applyRegisterNewAdmin(adminName, newAdminUsername, newAdminPassword);
        } else if (action == "RELEASE_ROOM") {
            std::string username, roomName;
            ss >> username >> roomName;
            applyReleaseRoom(username, roomName);
        }
    }

    offlineFile.close();
    std::ofstream clearOffline(OFFLINE_CHANGES_FILE, std::ofstream::out | std::ofstream::trunc);
    clearOffline.close();

    UI::displayMessage("Offline changes have been synchronized successfully.");
}

void OfflineManager::applyUploadRoom(const std::string& adminName, const std::string& roomName, int capacity, bool isAvailable) {
    rm.addRoom(adminName, roomName, capacity, isAvailable);
}

void OfflineManager::applyModifyRoom(const std::string& adminName, const std::string& roomName, int capacity, bool isAvailable) {
    rm.modifyRoom(adminName, roomName, capacity, isAvailable);
}

void OfflineManager::applyRegisterNewAdmin(const std::string& /*adminName*/, const std::string& newAdminUsername, const std::string& newAdminPassword) {
    auth.registerAdmin(newAdminUsername, newAdminPassword);
}

void OfflineManager::applyReleaseRoom(const std::string& username, const std::string& roomName) {
    rbs.releaseRoom(username, roomName);
}

std::vector<std::string> OfflineManager::getQueueForDisplay() {
    std::vector<std::string> queue;
    std::ifstream offlineFile(OFFLINE_CHANGES_FILE);
    if (offlineFile.is_open()) {
        std::string line;
        while (std::getline(offlineFile, line)) {
            queue.push_back(line);
        }
        offlineFile.close();
    }
    return queue;
}