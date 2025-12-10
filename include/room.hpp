#ifndef ROOM_HPP
#define ROOM_HPP

#include <string>
#include <vector>
#include <ctime>

class RoomHistoryManager;

class Room {
public:
    Room(const std::string& name, const std::string& lastModifiedBy, int capacity, bool isAvailable);

    std::string getName() const;
    std::string getLastModifiedBy() const;
    time_t getTimestamp() const;
    int getCapacity() const;
    bool isAvailable() const;
    std::string getBookedBy() const;

    void setCapacity(int capacity);
    void setAvailable(bool isAvailable);
    void setLastModifiedBy(const std::string& adminName);
    void setTimestamp(time_t timestamp);
    void setBookedBy(const std::string& username);

private:
    std::string name;
    std::string lastModifiedBy;
    time_t timestamp;
    int capacity;
    bool m_isAvailable;
    std::string bookedBy;
};

class RoomManager {
public:
    RoomManager();
    void loadRooms(); // Made public for GUI to explicitly load
    void uploadRoom(const std::string& adminName); // Console-based upload
    void modifyRoom(const std::string& adminName); // Console-based modify
    void modifyRoom(const std::string& adminName, const std::string& roomName, int capacity, bool isAvailable);
    void viewRoom();
    std::vector<Room>& getRooms();
    void saveRooms();
    Room* findRoom(const std::string& roomName);
    void addRoom(const std::string& adminName, const std::string& roomName, int capacity, bool isAvailable = true);
    void deleteRoom(const std::string& roomName, const std::string& adminName);

private:
    std::vector<Room> rooms;
    const std::string ROOMS_FILE = "rooms.txt";
    RoomHistoryManager* historyManager;
};

#endif // ROOM_HPP