
#include "meetingroom.hpp"

#include "ui.hpp"

#include <iostream>

#include <fstream>

#include <algorithm>



// RoomBookingSystem class implementation

RoomBookingSystem::RoomBookingSystem(RoomManager& rm)

    : rm(rm) {}



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
        for (auto& room : rm.getRooms()) {
            if (room.isAvailable() && room.getCapacity() >= participants) {
                roomToBook = &room;
                break; // Find the first suitable room
            }
        }
    }

    if (roomToBook && roomToBook->isAvailable() && roomToBook->getCapacity() >= participants) {
        roomToBook->setAvailable(false);
        roomToBook->setBookedBy(username);
        rm.saveRooms();
        return roomToBook;
    }
    return nullptr; // No suitable room found or room is not available
}


void RoomBookingSystem::releaseRoom(const std::string& username) {

    std::vector<Room*> bookedRooms;

    for (auto& room : rm.getRooms()) {

        if (room.getBookedBy() == username) {

            bookedRooms.push_back(&room);

        }

    }



    if (bookedRooms.empty()) {

        UI::displayMessage("You have not booked any rooms.");

        return;

    }



    UI::displayMessage("Rooms booked by you:");

    for (const auto* room : bookedRooms) {

        UI::displayMessage(room->getName());

    }



    std::string roomName;

    std::cout << "Enter room name to release: ";

    

    std::cin >> roomName;



    Room* room = rm.findRoom(roomName);



    if (room) {

        if (!room->isAvailable()) {

            if (room->getBookedBy() == username) {

                room->setAvailable(true);

                room->setBookedBy("");

                rm.saveRooms();

                UI::displayMessage(room->getName() + " has been successfully released.");

            } else {

                UI::displayMessage("You cannot release a room that you have not booked.");

            }

        } else {

            UI::displayMessage(room->getName() + " is already available.");

        }

    } else {

        UI::displayMessage("Room " + roomName + " not found.");

    }

}



void RoomBookingSystem::releaseRoom(const std::string& username, const std::string& roomName) {

    Room* room = rm.findRoom(roomName);



    if (room) {

        if (!room->isAvailable()) {

            if (room->getBookedBy() == username) {

                room->setAvailable(true);

                room->setBookedBy("");

                rm.saveRooms();

                UI::displayMessage(room->getName() + " has been successfully released.");

            } else {

                UI::displayMessage("You cannot release a room that you have not booked.");

            }

        } else {

            UI::displayMessage(room->getName() + " is already available.");

        }

    } else {

        UI::displayMessage("Room " + roomName + " not found.");

    }

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
