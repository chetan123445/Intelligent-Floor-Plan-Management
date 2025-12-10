#ifndef HISTORY_HPP
#define HISTORY_HPP

#include <string>
#include <vector>
#include <ctime>

const std::string ROOM_HISTORY_FILE = "room_history.log";
const std::string BOOKING_HISTORY_FILE = "booking_history.log";

struct RoomHistoryEntry {
    time_t timestamp;
    std::string roomName;
    std::string action; // CREATE, MODIFY, DELETE
    std::string adminName;
    int capacity;
    bool isAvailable;
};

class RoomHistoryManager {
public:
    void logCreate(const std::string& roomName, const std::string& adminName, int capacity, bool isAvailable);
    void logModify(const std::string& roomName, const std::string& adminName, int capacity, bool isAvailable);
    void logDelete(const std::string& roomName, const std::string& adminName);
    std::vector<RoomHistoryEntry> getHistoryForRoom(const std::string& roomName);
    std::vector<RoomHistoryEntry> getAllHistory();
};

struct BookingHistoryEntry {
    time_t timestamp;
    std::string roomName;
    std::string username;
    std::string action; // BOOK, RELEASE
};

class BookingHistoryManager {
public:
    void logBooking(const std::string& roomName, const std::string& username);
    void logRelease(const std::string& roomName, const std::string& username);
    std::vector<BookingHistoryEntry> getAllHistory();
};

#endif // HISTORY_HPP