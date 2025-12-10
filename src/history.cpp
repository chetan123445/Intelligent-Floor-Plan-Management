#include "history.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

// RoomHistoryManager implementation
void RoomHistoryManager::logCreate(const std::string& roomName, const std::string& adminName, int capacity, bool isAvailable) {
    std::ofstream historyFile(ROOM_HISTORY_FILE, std::ios::app);
    if (historyFile.is_open()) {
        historyFile << time(0) << " CREATE " << roomName << " " << adminName << " " << capacity << " " << (isAvailable ? "Yes" : "No") << std::endl;
    }
}

void RoomHistoryManager::logModify(const std::string& roomName, const std::string& adminName, int capacity, bool isAvailable) {
    std::ofstream historyFile(ROOM_HISTORY_FILE, std::ios::app);
    if (historyFile.is_open()) {
        historyFile << time(0) << " MODIFY " << roomName << " " << adminName << " " << capacity << " " << (isAvailable ? "Yes" : "No") << std::endl;
    }
}

void RoomHistoryManager::logDelete(const std::string& roomName, const std::string& adminName) {
    std::ofstream historyFile(ROOM_HISTORY_FILE, std::ios::app);
    if (historyFile.is_open()) {
        historyFile << time(0) << " DELETE " << roomName << " " << adminName << " -1 No" << std::endl;
    }
}

std::vector<RoomHistoryEntry> RoomHistoryManager::getAllHistory() {
    std::vector<RoomHistoryEntry> history;
    std::ifstream historyFile(ROOM_HISTORY_FILE);
    std::string line;
    while (std::getline(historyFile, line)) {
        std::stringstream ss(line);
        RoomHistoryEntry entry;
        std::string capacity_str, isAvailable_str;

        ss >> entry.timestamp >> entry.action >> entry.roomName >> entry.adminName >> capacity_str >> isAvailable_str;
        
        try {
            entry.capacity = std::stoi(capacity_str);
        } catch (...) {
            entry.capacity = -1; // For DELETE actions
        }
        entry.isAvailable = (isAvailable_str == "Yes");
        history.push_back(entry);
    }
    return history;
}

std::vector<RoomHistoryEntry> RoomHistoryManager::getHistoryForRoom(const std::string& roomName) {
    std::vector<RoomHistoryEntry> allHistory = getAllHistory();
    std::vector<RoomHistoryEntry> roomHistory;
    for (const auto& entry : allHistory) {
        if (entry.roomName == roomName) {
            roomHistory.push_back(entry);
        }
    }
    return roomHistory;
}

// BookingHistoryManager implementation
void BookingHistoryManager::logBooking(const std::string& roomName, const std::string& username) {
    std::ofstream historyFile(BOOKING_HISTORY_FILE, std::ios::app);
    if (historyFile.is_open()) {
        historyFile << time(0) << " BOOK " << roomName << " " << username << std::endl;
    }
}

void BookingHistoryManager::logRelease(const std::string& roomName, const std::string& username) {
    std::ofstream historyFile(BOOKING_HISTORY_FILE, std::ios::app);
    if (historyFile.is_open()) {
        historyFile << time(0) << " RELEASE " << roomName << " " << username << std::endl;
    }
}

std::vector<BookingHistoryEntry> BookingHistoryManager::getAllHistory() {
    std::vector<BookingHistoryEntry> history;
    std::ifstream historyFile(BOOKING_HISTORY_FILE);
    std::string line;
    while (std::getline(historyFile, line)) {
        std::stringstream ss(line);
        BookingHistoryEntry entry;
        ss >> entry.timestamp >> entry.action >> entry.roomName >> entry.username;
        history.push_back(entry);
    }
    return history;
}