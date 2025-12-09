
#ifndef MEETINGROOM_HPP
#define MEETINGROOM_HPP

#include "room.hpp"
#include <string>
#include <vector>

class RoomBookingSystem {
public:
    RoomBookingSystem(RoomManager& rm);
    void bookRoom(const std::string& username);
    Room* bookRoom(const std::string& username, int participants, const std::string& roomName);
    void releaseRoom(const std::string& username);
    void releaseRoom(const std::string& username, const std::string& roomName);
    void showRoomStatuses(const std::string& username);

private:
    RoomManager& rm;
    void suggestRoom(int participants);
};

#endif // MEETINGROOM_HPP
