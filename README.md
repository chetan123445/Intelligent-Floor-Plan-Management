# Intelligent Floor Plan Management

A C++ application for managing and booking meeting rooms within a floor plan, built with a graphical user interface using the [Raylib](https://www.raylib.com/) library. This tool provides functionalities for user authentication, room booking, viewing booking history, and offline data synchronization.

https://drive.google.com/drive/u/0/folders/1AojUALUEAeI08RRcIJnD3uTrKkH5M2ro?ths=true

---

## Default Admin: 
        You can login as a default admin with username: Admin and password : 123

## ✨ Features

- **Interactive GUI**: A clean graphical user interface built with Raylib to visualize the floor plan and interact with meeting rooms.
- **User Authentication**: A secure login system for users to access the application.
- **Meeting Room Management**:
    - View the status of all meeting rooms on the floor plan.
    - Book available meeting rooms for specific time slots.
    - View details and existing bookings for any room.
- **Booking History**: Users can view a history of their past and current room bookings.
- **Offline Capability**: The application is designed to handle temporary disconnections. All actions performed offline are saved and synchronized once a connection is re-established.

---

## 🚀 Getting Started

Follow these instructions to compile and run the application on your local machine.

### Prerequisites

- A C++ compiler that supports C++17 (like MinGW g++ on Windows).
- The **Raylib** library files.

### Directory Structure
```
c:\Users\CHETAN DUDI\OneDrive\Desktop\IFM\Intelligent-Floor-Plan-Management\
├───.gitignore
├───build_gui.bat
├───build.bat
├───commands.txt
├───gui_main.cpp
├───README.md
├───.git\...
├───.vscode\
│   ├───c_cpp_properties.json
│   ├───launch.json
│   └───settings.json
├───include\
│   ├───auth.hpp
│   ├───floorplan.hpp
│   ├───meetingroom.hpp
│   ├───offlinemechanism.hpp
│   ├───raygui.h
│   ├───raylib.h
│   ├───raymath.h
│   ├───rlgl.h
│   ├───room.hpp
│   └───ui.hpp
├───lib\
│   ├───libraylib.a
│   ├───libraylibdll.a
│   └───raylib.dll
├───output\
│   └───ifm_gui.exe
└───src\
    ├───auth.cpp
    ├───floorplan.cpp
    ├───history.cpp
    ├───history.hpp
    ├───main.cpp
    ├───meetingroom.cpp
    ├───offlinemechanism.cpp
    ├───room.cpp
    └───ui.cpp
```

### Compilation and Execution

1.  **Navigate to the project directory**:
    ```bash
    cd "C:\Users\CHETAN DUDI\OneDrive\Desktop\IFM\Intelligent-Floor-Plan-Management"
    ```
2.  **Build the application**:
    ```bash
    ./build_gui.bat
    ```
3.  **Run the application**:
    ```bash
    ./output/ifm_gui.exe
    ```

---

## 📖 How to Use the Application

1.  **Login**: Launch the application and enter your credentials to log in.
2.  **View Floor Plan**: After logging in, you will see the main floor plan view, showing all the meeting rooms and their current status (e.g., available, booked).
3.  **Book a Room**:
    - Click on an available room to view its details.
    - Select a desired time slot and confirm your booking.
4.  **Check History**: Navigate to the history section to see a list of all your previous and upcoming bookings.
5.  **Work Offline**: If your connection is lost, you can continue to interact with the application. Your bookings and other actions will be saved locally and synced with the server when you are back online.

---

## 🧠 Core Logic & Architecture

The application is built around a modular architecture, with different components handling specific functionalities.

1.  **Authentication (`auth.cpp`)**: Manages user login and session validation, ensuring that only authorized users can access the system.

2.  **Room & Meeting Room (`room.cpp`, `meetingroom.cpp`)**: These classes define the core objects of the floor plan. They manage the properties and state of each room, such as its capacity, status, and booking schedule.

3.  **User Interface (`gui_main.cpp`, `ui.cpp`)**: Powered by the Raylib library, this component is responsible for drawing the floor plan, rooms, buttons, and all other visual elements. It captures user input (mouse clicks, keyboard entries) and translates them into actions within the application.

4.  **Booking & History (`history.cpp`)**: This module handles the logic for creating new bookings and retrieving historical booking data for the logged-in user.

5.  **Offline Mechanism (`offlinemechanism.cpp`)**: This is a critical feature that ensures application reliability. When the application is offline, all user actions (like booking a room) are logged locally. Upon reconnection, this module synchronizes the locally stored actions with the central server to ensure data consistency across the system.

---
