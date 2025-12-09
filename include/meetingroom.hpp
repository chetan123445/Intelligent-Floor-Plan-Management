
#ifndef MEETINGROOM_HPP
#define MEETINGROOM_HPP

#include "room.hpp"
#include <string>
#include <vector>

enum class ReleaseRoomStatus {
    SUCCESS,
    NOT_FOUND,
    NOT_BOOKED,
    NOT_OWNER
};

class RoomBookingSystem {
public:
    RoomBookingSystem(RoomManager& rm);
    void bookRoom(const std::string& username);
    Room* bookRoom(const std::string& username, int participants, const std::string& roomName);
    ReleaseRoomStatus releaseRoom(const std::string& username, const std::string& roomName, bool isGui = false);
    void showRoomStatuses(const std::string& username);

private:
    RoomManager& rm;
    void suggestRoom(int participants);
};

#endif // MEETINGROOM_HPP
