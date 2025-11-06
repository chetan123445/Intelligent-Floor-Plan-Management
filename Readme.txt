
# Intelligent Room Management System

## Description

This project is a command-line application for managing and booking rooms in a building. It is designed with an object-oriented approach to ensure modularity, maintainability, and scalability.

### Features:

*   **User and Admin Authentication:** Secure login and registration system for users and admins.
*   **Role-Based Access Control:**
    *   **Super Admin:** A default super admin ("Chetan") has the highest level of access and can add other admins.
    *   **Admins:** Can manage rooms, including uploading, modifying, and viewing them. They can also register new admins.
    *   **Users:** Can book and release rooms, and view the status of all rooms.
*   **Room Management:** Admins can create, modify, and view rooms, including details like capacity and availability.
*   **Room Booking System:** Users can find and book suitable rooms based on their capacity requirements.
*   **Offline Mechanism:** The system provides a mechanism to store changes offline and synchronize them when the user is back online.

## Object-Oriented Design

The project is structured using the following classes, each with a specific responsibility:

*   **`Authentication`:** Manages all aspects of user and admin authentication, including registration, login, password hashing, and the super admin functionality.
*   **`Room`:** Represents a single room as an object, encapsulating its data (name, capacity, availability, bookedBy, etc.) and related operations.
*   **`RoomManager`:** Manages the entire collection of `Room` objects, providing an interface for loading, saving, uploading, modifying, and viewing rooms.
*   **`RoomBookingSystem`:** Manages all room booking operations, including booking, releasing, and displaying the status of all rooms.
*   **`OfflineManager`:** Encapsulates the logic for handling offline data storage and synchronization.
*   **`UI`:** Provides a set of static methods for displaying the user interface, ensuring a clean separation between the application's logic and its presentation.

## How to Compile and Run

### Prerequisites

*   A C++ compiler that supports C++17 (e.g., g++).

### Compilation

To compile the project, open a terminal in the project's root directory and run the following command:

```powershell
g++ -std=c++17 -Iinclude -o intelligent_floor_plan.exe src\auth.cpp src\room.cpp src\main.cpp src\meetingroom.cpp src\offlinemechanism.cpp src\ui.cpp
```

This will compile all the source files and create an executable named `intelligent_floor_plan.exe`.

### Running the Application

Once the project is compiled, you can run the application by executing the following command in the terminal from the project's root directory:

```powershell
.\intelligent_floor_plan.exe
```

This will start the Intelligent Room Management System, and you can interact with it through the command-line interface.
