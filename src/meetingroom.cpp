
#include "meetingroom.hpp"
#include "history.hpp"

#include "ui.hpp"

#include <iostream>

#include <fstream>

#include <algorithm>
#include <climits>



// RoomBookingSystem class implementation

RoomBookingSystem::RoomBookingSystem(RoomManager& rm) : rm(rm) {
    bookingHistoryManager = new BookingHistoryManager();
}

void RoomBookingSystem::suggestRoom(int participants) {

    std::vector<Room> suitableRooms;

    for (const auto& room : rm.getRooms()) {

        if (room.getCapacity() >= participants && room.isAvailable()) {

            suitableRooms.push_back(room);

        }

    }



    if (suitableRooms.empty()) {

        UI::displayMessage("No suitable rooms available for " + std::to_string(participants) + " participants.");

        return;

    }



    UI::displayMessage("Suggested Rooms:");

    for (const auto& room : suitableRooms) {

        UI::displayMessage(room.getName() + " (Capacity: " + std::to_string(room.getCapacity()) + ")");

    }

}



void RoomBookingSystem::bookRoom(const std::string& username) {



    int participants;



    std::cout << "Enter required capacity: ";



    std::cin >> participants;



    suggestRoom(participants);







    std::string roomName;



    std::cout << "Enter room name to book: ";



    std::cin >> roomName;







    Room* room = rm.findRoom(roomName);







    if (room) {



        if (room->isAvailable()) {



            if (room->getCapacity() >= participants) {



                room->setAvailable(false);



                room->setBookedBy(username);



                rm.saveRooms();



                UI::displayMessage(room->getName() + " has been successfully booked.");



            } else {



                UI::displayMessage("Room '" + roomName + "' does not have enough capacity.");



            }



        } else {



            UI::displayMessage(room->getName() + " is already booked.");



        }



    } else {



        UI::displayMessage("Room " + roomName + " not found.");



    }



}

Room* RoomBookingSystem::bookRoom(const std::string& username, int participants, const std::string& roomName) {
    Room* roomToBook = nullptr;

    if (!roomName.empty()) {
        roomToBook = rm.findRoom(roomName);
    } else {
        int minCapacityDiff = INT_MAX;
        Room* bestFitRoom = nullptr;
        for (auto& room : rm.getRooms()) {
            if (room.isAvailable() && room.getCapacity() >= participants) {
                int diff = room.getCapacity() - participants;
                if (diff < minCapacityDiff) {
                    minCapacityDiff = diff;
                    bestFitRoom = &room;
                }
            }
        }
        roomToBook = bestFitRoom;
    }

    if (roomToBook && roomToBook->isAvailable() && roomToBook->getCapacity() >= participants) {
        roomToBook->setAvailable(false);
        roomToBook->setBookedBy(username);
        bookingHistoryManager->logBooking(roomToBook->getName(), username);
        rm.saveRooms();
        return roomToBook;
    }
    return nullptr; // No suitable room found or room is not available
}

void RoomBookingSystem::showRoomStatuses(const std::string& /*username*/) {

    if (rm.getRooms().empty()) {

        UI::displayMessage("No rooms available.");

        return;

    }



    UI::displayMessage("\nRoom Statuses:");

    for (const auto& room : rm.getRooms()) {

        std::string status = "Room: " + room.getName() +

                           " | Capacity: " + std::to_string(room.getCapacity()) +

                           " | Available: " + (room.isAvailable() ? "Yes" : "No ");

        if (!room.isAvailable()) {

            status += "| Booked by: " + room.getBookedBy();

        }

        UI::displayMessage(status);

    }

}

ReleaseRoomStatus RoomBookingSystem::releaseRoom(const std::string& username, const std::string& roomName, bool isGui) {
    Room* room = rm.findRoom(roomName);

    if (room) {
        if (!room->isAvailable() && room->getBookedBy() == username) {
            room->setAvailable(true);
            room->setBookedBy(""); // Clear bookedBy
            bookingHistoryManager->logRelease(roomName, username);
            rm.saveRooms();
            if (!isGui) {
                UI::displayMessage("Room released");
            }
            return ReleaseRoomStatus::SUCCESS;
        } else if (room->isAvailable()) {
            if (!isGui) {
                UI::displayMessage("This room is not booked yet.");
            }
            return ReleaseRoomStatus::NOT_BOOKED;
        } else {
            if (!isGui) {
                UI::displayMessage("This room is not booked by you.");
            }
            return ReleaseRoomStatus::NOT_OWNER;
        }
    } else {
        if (!isGui) UI::displayMessage("No room found of this name.");
        return ReleaseRoomStatus::NOT_FOUND;
    }
}
