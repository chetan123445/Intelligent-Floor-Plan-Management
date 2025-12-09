#include "raylib.h"
#include "auth.hpp"
#include "room.hpp"
#include "meetingroom.hpp"
#include "offlinemechanism.hpp"
#include <random>   // For random number generation
#include <chrono>   // For seeding the random number generator
#include <vector>
#include <string>
#include <iostream>

// Force include dependencies for raygui implementation
#include <stdlib.h> // Required for: strtod
#include "raylib.h" // Required for: GetMousePosition, etc.

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

// GUI State Management
enum class AppState { LOGIN, USER_DASHBOARD, ADMIN_DASHBOARD };

//----------------------------------------------------------------------------------
// UI Helper Functions
//----------------------------------------------------------------------------------
void DrawCenteredText(const char* text, int y, int fontSize, Color color) {
    int textWidth = MeasureText(text, fontSize);
    DrawText(text, (GetScreenWidth() - textWidth) / 2, y, fontSize, color);
}

int main() {
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE); // Allow window to be resizable
    InitWindow(screenWidth, screenHeight, "Intelligent Floor Plan Management");
    SetTargetFPS(60);

    // Application State
    AppState currentState = AppState::LOGIN;
    Authentication auth;
    RoomManager roomManager;
    RoomBookingSystem bookingSystem(roomManager);
    OfflineManager offlineManager(auth, roomManager, bookingSystem);

    // Login Screen state
    char username[64] = "";
    char password[64] = "";
    bool passwordBoxEditMode = false;
    bool usernameBoxEditMode = false;
    std::string loginMessage = "";
    bool isAdminLogin = false;
    std::string statusMessage = ""; // To display online/offline status

    // Dashboard state
    std::string loggedInUser;
    Vector2 scroll = { 0, 0 };

    // Room creation state
    bool showAddRoomPopup = false;
    char roomName[64] = "";
    char roomCapacity[10] = "";
    bool roomNameBoxEditMode = false;
    bool roomCapacityBoxEditMode = false;

    // Book room state
    bool showBookRoomPopup = false;
    char bookCapacity[10] = "";
    char bookRoomName[64] = ""; // For specific room booking
    std::string bookingMessage = "";

    // View Offline Queue state
    bool showOfflineQueuePopup = false;

    // Random offline simulation state
    std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<float> onlineDurationDist(20.0f, 40.0f);   // Stay online for 20-40 seconds
    std::uniform_real_distribution<float> offlineDurationDist(8.0f, 20.0f);    // Stay offline for 8-20 seconds
    std::uniform_int_distribution<int> goOfflineChanceDist(1, 100);            // 40% chance to go offline

    float statusChangeTimer = onlineDurationDist(generator); // Time until next status event

    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        //----------------------------------------------------------------------------------
        // Handle state transitions and logic
        if (currentState != AppState::LOGIN) {
            statusChangeTimer -= GetFrameTime();

            if (statusChangeTimer <= 0.0f) {
                if (!offlineManager.isOffline()) {
                    // Currently online, check if we should go offline
                    if (goOfflineChanceDist(generator) <= 40) { // 40% chance to go offline
                        offlineManager.goOffline();
                        statusChangeTimer = offlineDurationDist(generator); // Set how long to stay offline
                    } else {
                        // Didn't go offline, reset timer for next check
                        statusChangeTimer = onlineDurationDist(generator);
                    }
                } else {
                    // Currently offline, time to go back online
                    offlineManager.goOnline();
                    statusChangeTimer = onlineDurationDist(generator); // Set how long to stay online
                }
            }
        }
        //----------------------------------------------------------------------------------


        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (currentState) {
            case AppState::LOGIN:
            {
                // Define larger, more modern dimensions for UI elements
                const float boxWidth = 400;
                const float boxHeight = 50;
                const float buttonWidth = 195;
                const float buttonHeight = 50;
                const float spacing = 15;

                // Calculate centered positions dynamically
                float currentScreenWidth = (float)GetScreenWidth();
                float currentScreenHeight = (float)GetScreenHeight();
                float startX = (currentScreenWidth - boxWidth) / 2;
                float startY = currentScreenHeight / 2 - (boxHeight * 2 + buttonHeight) / 2 - 40;

                DrawCenteredText("Intelligent Floor Plan Management", startY - 80, 50, DARKGRAY);

                GuiLabel(Rectangle{ startX, startY - 25, 100, 20 }, "Username");
                // Username Input
                if (GuiTextBox(Rectangle{ startX, startY, boxWidth, boxHeight }, username, 64, usernameBoxEditMode)) {
                    usernameBoxEditMode = !usernameBoxEditMode;
                }

                GuiLabel(Rectangle{ startX, startY + boxHeight + spacing - 25, 100, 20 }, "Password");
                // Password Input
                // Use a temporary buffer for secret view
                char passwordSecret[64];
                memset(passwordSecret, '*', strlen(password));
                passwordSecret[strlen(password)] = '\0';
                if (GuiTextBox(Rectangle{ startX, startY + boxHeight + spacing, boxWidth, boxHeight }, passwordBoxEditMode ? password : passwordSecret, 64, passwordBoxEditMode)) {
                    passwordBoxEditMode = !passwordBoxEditMode;
                }

                // Is Admin Checkbox
                GuiSetStyle(DEFAULT, TEXT_SIZE, 20); // Temporarily increase checkbox text size
                GuiCheckBox(Rectangle{ startX, startY + 2 * (boxHeight + spacing), 30, 30 }, "Login as Admin", &isAdminLogin);
                GuiSetStyle(DEFAULT, TEXT_SIZE, 10); // Reset to default

                // Login Button
                if (GuiButton(Rectangle{ startX, startY + 2 * (boxHeight + spacing) + 50, buttonWidth, buttonHeight }, "Login")) {
                    if (auth.login(username, password, isAdminLogin)) {
                        loginMessage = "Login successful!";
                        loggedInUser = username;
                        roomManager.loadRooms(); // Ensure rooms are loaded after login
                        currentState = isAdminLogin ? AppState::ADMIN_DASHBOARD : AppState::USER_DASHBOARD;
                    } else {
                        loginMessage = "Invalid credentials. Please try again.";
                    }
                }

                // Register Button
                GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(LIGHTGRAY)); // Give register a different color
                if (GuiButton(Rectangle{ startX + buttonWidth + spacing, startY + 2 * (boxHeight + spacing) + 50, buttonWidth, buttonHeight }, "Register")) {
                    if (auth.registerUser(username, password)) {
                        loginMessage = "Registration successful! Please login.";
                    } else {
                        loginMessage = "Username already exists or invalid data.";
                    }
                }

                GuiLoadStyleDefault(); // Reset style to avoid affecting other buttons
                // Display login message
                DrawCenteredText(loginMessage.c_str(), startY + 2 * (boxHeight + spacing) + 120, 20, loginMessage.find("successful") != std::string::npos ? LIME : MAROON);
            }
            break;

            case AppState::USER_DASHBOARD:
            case AppState::ADMIN_DASHBOARD:
            {
                // Display Online/Offline Status
                statusMessage = offlineManager.isOffline() ? "Status: OFFLINE" : "Status: ONLINE";
                DrawText(statusMessage.c_str(), GetScreenWidth() - 250, 55, 18, offlineManager.isOffline() ? RED : GREEN);


                // Common Dashboard UI
                std::string welcome_text = "Welcome, " + loggedInUser + "!";
                DrawText(welcome_text.c_str(), 20, 20, 20, DARKGRAY);

                if (GuiButton(Rectangle{ (float)screenWidth - 120, 20, 100, 30 }, "Logout")) {
                    loggedInUser = "";
                    // Clear credentials
                    memset(username, 0, 64);
                    memset(password, 0, 64);
                    loginMessage = "";
                    isAdminLogin = false;
                    currentState = AppState::LOGIN;
                    // Reset popups
                    showAddRoomPopup = false;
                    showBookRoomPopup = false;
                    showOfflineQueuePopup = false;
                    continue; // Skip rest of the frame
                }

                // Toggle Fullscreen Button
                if (GuiButton(Rectangle{ (float)screenWidth - 250, 20, 120, 30 }, "Toggle Fullscreen")) {
                    ToggleFullscreen();
                }

                // Toggle Online/Offline Button
                if (GuiButton(Rectangle{ (float)screenWidth - 400, 20, 140, 30 }, offlineManager.isOffline() ? "Go Online" : "Go Offline")) {
                    if (offlineManager.isOffline()) {
                        offlineManager.goOnline();
                    } else {
                        offlineManager.goOffline();
                    }
                }

                // --- Sidebar for actions ---
                int sidebarWidth = 250;
                DrawRectangle(0, 70, sidebarWidth, screenHeight - 70, LIGHTGRAY);
                DrawLine(sidebarWidth, 70, sidebarWidth, screenHeight, DARKGRAY);

                int buttonY = 80;
                if (currentState == AppState::ADMIN_DASHBOARD) {
                    DrawCenteredText("Admin Menu", buttonY, 20, DARKGRAY);
                    buttonY += 40;
                    if (GuiButton(Rectangle{ 20, (float)buttonY, (float)sidebarWidth - 40, 30 }, "Add Room")) {
                        showAddRoomPopup = true;
                        // Reset fields
                        memset(roomName, 0, 64);
                        memset(roomCapacity, 0, 10);
                    }
                    buttonY += 40;
                    // Add other admin buttons here: Modify Room, Add Admin etc.
                } else { // User Menu
                    DrawCenteredText("User Menu", buttonY, 20, DARKGRAY);
                    buttonY += 40;
                    if (GuiButton(Rectangle{ 20, (float)buttonY, (float)sidebarWidth - 40, 30 }, "Book a Room")) {
                        showBookRoomPopup = true;
                        bookingMessage = ""; // Clear previous messages
                        memset(bookCapacity, 0, 10); // Clear capacity input
                        memset(bookRoomName, 0, 64); // Clear room name input
                    }
                    buttonY += 40;
                    if (GuiButton(Rectangle{ 20, (float)buttonY, (float)sidebarWidth - 40, 30 }, "Release My Room")) {
                        // For simplicity, we'll assume releaseRoom in GUI will always ask for room name
                        // A more robust GUI would list booked rooms and allow selection.
                        // For now, we'll just queue a generic release or show a message.
                        // If offline, we can't directly ask for room name in a non-blocking way here.
                        // This would typically be handled by another popup or a list of booked rooms.
                        // For this example, we'll just queue a dummy release or show a message.
                        if (offlineManager.isOffline()) {
                            // Cannot release specific room without user input in this flow.
                            // A proper GUI would have a list of booked rooms to select from.
                            bookingMessage = "Cannot release specific room while offline. Go online to manage bookings.";
                        } else {
                            bookingSystem.releaseRoom(loggedInUser); // This will prompt for room name in console
                        }
                        bookingMessage = "Release request processed.";
                    }
                }

                buttonY += 60; // Gap
                if (GuiButton(Rectangle{ 20, (float)buttonY, (float)sidebarWidth - 40, 30 }, "View Offline Queue")) {
                    showOfflineQueuePopup = true;
                }

                // --- Main content area for rooms ---
                DrawText("Floor Plan", sidebarWidth + 20, 80, 30, GRAY);

                const std::vector<Room>& rooms = roomManager.getRooms();
                int roomBoxSize = 100;
                int padding = 20;
                int roomsPerRow = (screenWidth - sidebarWidth - padding) / (roomBoxSize + padding);
                
                for (size_t i = 0; i < rooms.size(); ++i) {
                    int row = i / roomsPerRow;
                    int col = i % roomsPerRow;

                    int x = sidebarWidth + padding + col * (roomBoxSize + padding);
                    int y = 140 + row * (roomBoxSize + padding);

                    Color roomColor = rooms[i].isAvailable() ? LIME : MAROON;
                    DrawRectangle(x, y, roomBoxSize, roomBoxSize, roomColor);
                    DrawRectangleLines(x, y, roomBoxSize, roomBoxSize, DARKBROWN);

                    DrawText(rooms[i].getName().c_str(), x + 10, y + 10, 20, WHITE);
                    std::string capacityStr = "Cap: " + std::to_string(rooms[i].getCapacity());
                    DrawText(capacityStr.c_str(), x + 10, y + 35, 15, WHITE);

                    if (!rooms[i].isAvailable()) {
                        std::string bookedByStr = "By: " + rooms[i].getBookedBy();
                        DrawText(bookedByStr.c_str(), x + 10, y + 60, 15, YELLOW);
                    } else {
                        DrawText("Available", x + 10, y + 60, 15, WHITE);
                    }
                }
                 if (rooms.empty()) {
                    DrawText("No rooms created yet. Admin needs to add rooms.", sidebarWidth + 20, 150, 20, GRAY);
                }

            }
            break;
        }

        // --- Popups ---
        if (showAddRoomPopup) {
            int popupWidth = 400;
            int popupHeight = 250;
            Rectangle popupRect = { (float)screenWidth/2 - popupWidth/2, (float)screenHeight/2 - popupHeight/2, (float)popupWidth, (float)popupHeight };
            
            showAddRoomPopup = !GuiWindowBox(popupRect, "Add New Room");

            GuiLabel(Rectangle{ popupRect.x + 20, popupRect.y + 50, 80, 20 }, "Room Name:");
            if (GuiTextBox(Rectangle{ popupRect.x + 110, popupRect.y + 40, 250, 40 }, roomName, 64, roomNameBoxEditMode)) {
                roomNameBoxEditMode = !roomNameBoxEditMode;
            }

            GuiLabel(Rectangle{ popupRect.x + 20, popupRect.y + 100, 80, 20 }, "Capacity:");
            if (GuiTextBox(Rectangle{ popupRect.x + 110, popupRect.y + 90, 250, 40 }, roomCapacity, 10, roomCapacityBoxEditMode)) {
                roomCapacityBoxEditMode = !roomCapacityBoxEditMode;
            }

            if (GuiButton(Rectangle{ popupRect.x + popupWidth/2 - 50, popupRect.y + 150, 100, 40 }, "Create")) {
                try {
                    int capacity = std::stoi(roomCapacity);
                    if (offlineManager.isOffline()) {
                        offlineManager.queueUploadRoom(loggedInUser, roomName, capacity, true);
                    } else {
                        roomManager.addRoom(loggedInUser, roomName, capacity, true); // Add room as available by default
                        roomManager.saveRooms(); // Persist change
                    }
                    showAddRoomPopup = false;
                } catch (const std::exception& e) {
                    // Handle invalid number format for capacity
                    std::cerr << "Invalid capacity input: " << e.what() << std::endl;
                }
            }
        }

        if (showBookRoomPopup) {
            int popupWidth = 400;
            int popupHeight = 220;
            Rectangle popupRect = { (float)screenWidth/2 - popupWidth/2, (float)screenHeight/2 - popupHeight/2, (float)popupWidth, (float)popupHeight };
            
            showBookRoomPopup = !GuiWindowBox(popupRect, "Book a Room");

            GuiLabel(Rectangle{ popupRect.x + 20, popupRect.y + 50, 120, 20 }, "Required Capacity:");
            // Need an edit mode for bookCapacity
            static bool bookCapacityEditMode = false;
            if (GuiTextBox(Rectangle{ popupRect.x + 150, popupRect.y + 40, 210, 40 }, bookCapacity, 10, bookCapacityEditMode)) {
                bookCapacityEditMode = !bookCapacityEditMode;
            }

            GuiLabel(Rectangle{ popupRect.x + 20, popupRect.y + 90, 120, 20 }, "Specific Room (Optional):");
            static bool bookRoomNameEditMode = false;
            if (GuiTextBox(Rectangle{ popupRect.x + 150, popupRect.y + 80, 210, 40 }, bookRoomName, 64, bookRoomNameEditMode)) {
                bookRoomNameEditMode = !bookRoomNameEditMode;
            }

            if (GuiButton(Rectangle{ popupRect.x + popupWidth/2 - 50, popupRect.y + 140, 100, 40 }, "Find & Book")) {
                try {
                    int capacity = std::stoi(bookCapacity);
                    std::string roomToBook = std::string(bookRoomName);
                    if (offlineManager.isOffline()) {
                        offlineManager.queueBookRoom(loggedInUser, capacity, roomToBook);
                        bookingMessage = "Booking queued for room: " + (roomToBook.empty() ? "any suitable" : roomToBook);
                    } else {
                        Room* bookedRoom = bookingSystem.bookRoom(loggedInUser, capacity, roomToBook);
                        if (bookedRoom) {
                        bookingMessage = "Successfully booked room: " + bookedRoom->getName();
                    } else {
                        bookingMessage = "No suitable room available.";
                    }
                    }
                    showBookRoomPopup = false;
                } catch (const std::exception& e) {
                    bookingMessage = "Invalid capacity entered.";
                    std::cerr << "Invalid capacity input: " << e.what() << std::endl;
                }
            }

            DrawText(bookingMessage.c_str(), popupRect.x + 20, popupRect.y + 190, 20, MAROON);
        }

        if (showOfflineQueuePopup) {
            int popupWidth = 600;
            int popupHeight = 400;
            Rectangle popupRect = { (float)screenWidth/2 - popupWidth/2, (float)screenHeight/2 - popupHeight/2, (float)popupWidth, (float)popupHeight };
            
            showOfflineQueuePopup = !GuiWindowBox(popupRect, "Offline Action Queue");

            std::vector<std::string> queue = offlineManager.getQueueForDisplay();

            if (queue.empty()) {
                DrawText("Offline queue is empty.", popupRect.x + 20, popupRect.y + 50, 20, GRAY);
            } else {
                Rectangle view = { popupRect.x + 10, popupRect.y + 40, popupRect.width - 20, popupRect.height - 50 };
                Rectangle content = { view.x, view.y, view.width - 20, (float)queue.size() * 25 };
                Rectangle viewScroll = { 0 };
                
                GuiScrollPanel(view, NULL, content, &scroll, &viewScroll);

                BeginScissorMode(view.x, view.y, view.width, view.height);
                for(size_t i = 0; i < queue.size(); ++i) {
                    DrawText(queue[i].c_str(), view.x + 10 + scroll.x, view.y + 10 + i * 25 + scroll.y, 15, DARKGRAY);
                }
                EndScissorMode();
            }
        }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
