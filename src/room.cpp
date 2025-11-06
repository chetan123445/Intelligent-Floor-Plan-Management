#include "room.hpp"
#include "ui.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>

// Room class implementation
Room::Room(const std::string& name, const std::string& lastModifiedBy, int capacity, bool isAvailable)
    : name(name), lastModifiedBy(lastModifiedBy), capacity(capacity), m_isAvailable(isAvailable) {
    timestamp = time(0);
}

std::string Room::getName() const { return name; }
std::string Room::getLastModifiedBy() const { return lastModifiedBy; }
time_t Room::getTimestamp() const { return timestamp; }
int Room::getCapacity() const { return capacity; }
bool Room::isAvailable() const { return m_isAvailable; }
std::string Room::getBookedBy() const { return bookedBy; }

void Room::setCapacity(int capacity) { this->capacity = capacity; }
void Room::setAvailable(bool isAvailable) { this->m_isAvailable = isAvailable; }
void Room::setLastModifiedBy(const std::string& adminName) { this->lastModifiedBy = adminName; }
void Room::setTimestamp(time_t timestamp) { this->timestamp = timestamp; }
void Room::setBookedBy(const std::string& username) { this->bookedBy = username; }

// RoomManager class implementation
RoomManager::RoomManager() {
    loadRooms();
}

void RoomManager::loadRooms() {
    rooms.clear();
    std::ifstream file(ROOMS_FILE);
    if (file.is_open()) {
        std::string name, lastModifiedBy, capacity_str, isAvailable_str, bookedBy;
        while (file >> name >> lastModifiedBy >> capacity_str >> isAvailable_str >> bookedBy) {
            int capacity = std::stoi(capacity_str);
            bool isAvailable = (isAvailable_str == "Yes");
            Room room(name, lastModifiedBy, capacity, isAvailable);
            if (bookedBy == "none") {
                room.setBookedBy("");
            } else {
                room.setBookedBy(bookedBy);
            }
            rooms.push_back(room);
        }
        file.close();
    }
}

std::vector<Room>& RoomManager::getRooms() {
    return rooms;
}

void RoomManager::saveRooms() {
    std::ofstream file(ROOMS_FILE);
    if (file.is_open()) {
        for (const auto& room : rooms) {
            file << room.getName() << " " << room.getLastModifiedBy() << " " << room.getCapacity() << " " << (room.isAvailable() ? "Yes" : "No") << " " << (room.getBookedBy().empty() ? "none" : room.getBookedBy()) << std::endl;
        }
        file.close();
    }
}

Room* RoomManager::findRoom(const std::string& roomName) {
    auto it = std::find_if(rooms.begin(), rooms.end(), [&](const Room& r) {
        return r.getName() == roomName;
    });

    if (it != rooms.end()) {
        return &(*it);
    }

    return nullptr;
}

void RoomManager::uploadRoom(const std::string& adminName) {
    std::string roomName;
    int capacity;
    bool isAvailable;

    std::cout << "\n--- Upload New Room ---" << std::endl;
    std::cout << "Enter a unique name for the new room: ";
    std::cin >> roomName;

    auto it = std::find_if(rooms.begin(), rooms.end(), [&](const Room& r) {
        return r.getName() == roomName;
    });

    if (it != rooms.end()) {
        UI::displayMessage("Error: Room '" + roomName + "' already exists. Please choose a different name.");
        return;
    }

    std::cout << "Enter the capacity of the room: ";
    std::cin >> capacity;
    std::cout << "Is the room available for booking? (1 for Yes, 0 for No): ";
    std::cin >> isAvailable;

    rooms.emplace_back(roomName, adminName, capacity, isAvailable);
    saveRooms();
    UI::displayMessage("Room '" + roomName + "' has been uploaded successfully.");
}

void RoomManager::uploadRoom(const std::string& adminName, const std::string& roomName, int capacity, bool isAvailable) {
    auto it = std::find_if(rooms.begin(), rooms.end(), [&](const Room& r) {
        return r.getName() == roomName;
    });

    if (it != rooms.end()) {
        UI::displayMessage("Error: Room '" + roomName + "' already exists. Please choose a different name.");
        return;
    }

    rooms.emplace_back(roomName, adminName, capacity, isAvailable);
    saveRooms();
    UI::displayMessage("Room '" + roomName + "' has been uploaded successfully.");
}

void RoomManager::modifyRoom(const std::string& adminName) {
    std::string roomName;
    int capacity;
    bool isAvailable;

    std::cout << "\n--- Modify Existing Room ---" << std::endl;
    std::cout << "Enter the name of the room to modify: ";
    std::cin >> roomName;

    auto it = std::find_if(rooms.begin(), rooms.end(), [&](const Room& r) {
        return r.getName() == roomName;
    });

    if (it != rooms.end()) {
        std::cout << "Enter the new capacity for '" << roomName << "': ";
        std::cin >> capacity;
        std::cout << "Is '" << roomName << "' available for booking? (1 for Yes, 0 for No): ";
        std::cin >> isAvailable;

        it->setCapacity(capacity);
        it->setAvailable(isAvailable);
        it->setLastModifiedBy(adminName);
        it->setTimestamp(time(0));

        saveRooms();
        UI::displayMessage("Room '" + roomName + "' has been modified successfully.");
    } else {
        UI::displayMessage("Error: Room '" + roomName + "' does not exist.");
    }
}

void RoomManager::modifyRoom(const std::string& adminName, const std::string& roomName, int capacity, bool isAvailable) {
    auto it = std::find_if(rooms.begin(), rooms.end(), [&](const Room& r) {
        return r.getName() == roomName;
    });

    if (it != rooms.end()) {
        it->setCapacity(capacity);
        it->setAvailable(isAvailable);
        it->setLastModifiedBy(adminName);
        it->setTimestamp(time(0));

        saveRooms();
        UI::displayMessage("Room '" + roomName + "' has been modified successfully.");
    } else {
        UI::displayMessage("Error: Room '" + roomName + "' does not exist.");
    }
}

void RoomManager::viewRoom() {
    std::string roomName;
    std::cout << "\n--- View Room Details ---" << std::endl;
    std::cout << "Enter the name of the room to view: ";
    std::cin >> roomName;

    auto it = std::find_if(rooms.begin(), rooms.end(), [&](const Room& r) {
        return r.getName() == roomName;
    });

    if (it != rooms.end()) {
        std::cout << "\n--- Details for Room: " << it->getName() << " ---" << std::endl;
        std::cout << "Last Modified By: " << it->getLastModifiedBy() << std::endl;
        const time_t timestamp = it->getTimestamp();
        std::cout << "Last Modified On: " << ctime(&timestamp);
        std::cout << "Capacity: " << it->getCapacity() << std::endl;
        std::cout << "Available: " << (it->isAvailable() ? "Yes" : "No") << std::endl;
        std::cout << "------------------------------------" << std::endl;
    } else {
        UI::displayMessage("Error: Room '" + roomName + "' does not exist.");
    }
}
