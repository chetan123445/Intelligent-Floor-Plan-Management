#include "raylib.h"
#include "auth.hpp"
#include "room.hpp"
#include "meetingroom.hpp"
#include "offlinemechanism.hpp"
#include "history.hpp"
#include <random>   // For random number generation
#include <algorithm> // For std::transform
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
    RoomHistoryManager roomHistoryManager;
    BookingHistoryManager bookingHistoryManager;

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

    // Admin Management state
    bool showAddAdminPopup = false;
    char newAdminUsername[64] = "";
    char newAdminPassword[64] = "";
    bool newAdminUsernameEditMode = false;
    bool newAdminPasswordEditMode = false;
    std::string addAdminMessage = "";

    bool showDeleteUserAdminPopup = false;
    char deleteTargetUsername[64] = "";
    bool deleteTargetUsernameEditMode = false;
    std::string deleteUserAdminMessage = "";

    bool showEditUserAdminPopup = false;
    char editTargetUsername[64] = "";
    char editNewPassword[64] = "";
    int editNewRoleActive = 0; // 0 for USER, 1 for ADMIN (for GuiDropdownBox)
    bool editTargetUsernameEditMode = false;
    bool editNewPasswordEditMode = false;
    std::string editUserAdminMessage = "";

    bool showViewUsersAdminsPopup = false;
    std::vector<std::string> usersAdminsDisplayList;
    // Room modification state
    bool showModifyRoomPopup = false;
    char modifyRoomName[64] = "";
    char modifyRoomCapacity[10] = "";
    bool modifyRoomAvailability = false;
    bool modifyRoomNameEditMode = false;
    bool modifyRoomCapacityEditMode = false; // Declare missing variable
    std::string modifyRoomMessage = "";
    bool roomCapacityBoxEditMode = false;

    // Book room state
    bool showBookRoomPopup = false;
    char bookCapacity[10] = "";
    char bookRoomName[64] = ""; // For specific room booking
    std::string bookingMessage = "";

    // Release room state
    bool showReleaseRoomPopup = false;
    char releaseRoomName[64] = "";
    bool releaseRoomNameEditMode = false;
    std::string releaseRoomMessage = "";
    bool showReleaseStatusPopup = false;
    // View Offline Queue state
    bool showOfflineQueuePopup = false;

    // Room deletion state
    bool showRoomHistoryPopup = false;
    Vector2 roomHistoryScroll = { 0, 0 };
    std::vector<std::string> roomHistoryDisplayList;

    // Booking history state
    bool showBookingHistoryPopup = false;
    Vector2 bookingHistoryScroll = { 0, 0 };
    bool showDeleteRoomPopup = false;
    char deleteRoomName[64] = "";
    bool deleteRoomNameEditMode = false;
    Vector2 usersAdminsScroll = { 0, 0 };

    // Room Search and Filter state
    char searchRoomName[64] = "";
    bool searchRoomNameEditMode = false;
    bool filterAvailableRooms = false;
    bool filterBookedRooms = false;
    char filterCapacity[10] = "";
    bool filterCapacityEditMode = false;
    // Random offline simulation state
    Vector2 floorPlanScroll = { 0, 0 };

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

        float screenWidth = (float)GetScreenWidth();
        float screenHeight = (float)GetScreenHeight();

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
                float startX = (screenWidth - boxWidth) / 2;
                float startY = screenHeight / 2 - (boxHeight * 2 + buttonHeight) / 2 - 40;

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
                DrawText(statusMessage.c_str(), screenWidth - 250, 55, 18, offlineManager.isOffline() ? RED : GREEN);


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
                    showReleaseStatusPopup = false;
                    showRoomHistoryPopup = false;
                    continue; // Skip rest of the frame
                }

                // Toggle Fullscreen Button
                if (GuiButton(Rectangle{ screenWidth - 250, 20, 120, 30 }, "Toggle Fullscreen")) {
                    ToggleFullscreen();
                }

                // Toggle Online/Offline Button
                if (GuiButton(Rectangle{ screenWidth - 400, 20, 140, 30 }, offlineManager.isOffline() ? "Go Online" : "Go Offline")) {
                    if (offlineManager.isOffline()) {
                        offlineManager.goOnline();
                    } else {
                        offlineManager.goOffline();
                    }
                }

                // --- Sidebar for actions ---
                float sidebarWidth = screenWidth * 0.2f; // Sidebar is 20% of screen width
                DrawRectangle(0, 70, sidebarWidth, screenHeight - 70, LIGHTGRAY);
                DrawLine(sidebarWidth, 70, sidebarWidth, screenHeight, DARKGRAY);

                int buttonY = 80;
                if (currentState == AppState::ADMIN_DASHBOARD) {
                    DrawCenteredText("Admin Menu", buttonY, 20, DARKGRAY);
                    buttonY += 40;
                    if (GuiButton(Rectangle{ 20, (float)buttonY, sidebarWidth - 40, 30 }, "Add Room")) {
                        showAddRoomPopup = true;
                        // Reset fields
                        memset(roomName, 0, 64);
                        memset(roomCapacity, 0, 10);
                    }
                    buttonY += 40;
                    if (GuiButton(Rectangle{ 20, (float)buttonY, sidebarWidth - 40, 30 }, "Modify Room")) {
                        showModifyRoomPopup = true;
                        memset(modifyRoomName, 0, 64);
                        memset(modifyRoomCapacity, 0, 10);
                        modifyRoomAvailability = false;
                        modifyRoomMessage = "";
                    }
                    buttonY += 40;
                    if (GuiButton(Rectangle{ 20, (float)buttonY, sidebarWidth - 40, 30 }, "Delete Room")) {
                        showDeleteRoomPopup = true;
                        memset(deleteRoomName, 0, 64);
                    }
                    // Add other admin buttons here: Modify Room, Add Admin etc.
                    buttonY += 40;
                    if (GuiButton(Rectangle{ 20, (float)buttonY, sidebarWidth - 40, 30 }, "Add Admin")) {
                        showAddAdminPopup = true;
                        memset(newAdminUsername, 0, 64);
                        memset(newAdminPassword, 0, 64);
                        addAdminMessage = "";
                    }
                    buttonY += 40;
                    if (GuiButton(Rectangle{ 20, (float)buttonY, sidebarWidth - 40, 30 }, "Delete User/Admin")) {
                        showDeleteUserAdminPopup = true;
                        memset(deleteTargetUsername, 0, 64);
                        deleteUserAdminMessage = "";
                    }
                    buttonY += 40;
                    if (GuiButton(Rectangle{ 20, (float)buttonY, sidebarWidth - 40, 30 }, "Edit User/Admin")) {
                        showEditUserAdminPopup = true;
                        memset(editTargetUsername, 0, 64);
                        memset(editNewPassword, 0, 64);
                        editNewRoleActive = 0; // Default to User
                        editUserAdminMessage = "";
                    }
                    buttonY += 40;
                    if (GuiButton(Rectangle{ 20, (float)buttonY, sidebarWidth - 40, 30 }, "View Users/Admins")) {
                        showViewUsersAdminsPopup = true;
                        usersAdminsDisplayList.clear(); // Clear previous list
                        // Populate list when popup opens
                    }
                    buttonY += 40;
                    if (GuiButton(Rectangle{ 20, (float)buttonY, sidebarWidth - 40, 30 }, "View Room History")) {
                        showRoomHistoryPopup = true;
                        roomHistoryDisplayList.clear();
                    }

                } else { // User Menu
                    DrawCenteredText("User Menu", buttonY, 20, DARKGRAY);
                    buttonY += 40;
                    if (GuiButton(Rectangle{ 20, (float)buttonY, sidebarWidth - 40, 30 }, "Book a Room")) {
                        showBookRoomPopup = true;
                        bookingMessage = ""; // Clear previous messages
                        memset(bookCapacity, 0, 10); // Clear capacity input
                        memset(bookRoomName, 0, 64); // Clear room name input
                    }
                    buttonY += 40;
                    if (GuiButton(Rectangle{ 20, (float)buttonY, sidebarWidth - 40, 30 }, "Release a Room")) {
                        showReleaseRoomPopup = true;
                        releaseRoomMessage = "";
                        memset(releaseRoomName, 0, 64);
                    }
                    buttonY += 40;
                    if (GuiButton(Rectangle{ 20, (float)buttonY, sidebarWidth - 40, 30 }, "View Booking History")) {
                        showBookingHistoryPopup = true;
                    }

                }

                buttonY += 60; // Gap
                if (GuiButton(Rectangle{ 20, (float)buttonY, sidebarWidth - 40, 30 }, "View Offline Queue")) {
                    showOfflineQueuePopup = true;
                }

                // Common button for admins and users
                if (currentState == AppState::ADMIN_DASHBOARD) {
                    buttonY += 40;
                    if (GuiButton(Rectangle{ 20, (float)buttonY, sidebarWidth - 40, 30 }, "View Booking History")) {
                        showBookingHistoryPopup = true;
                    }
                }
                // --- Main content area for rooms and filters ---
                float contentAreaX = sidebarWidth + 20;
                float contentAreaY = 80;

                DrawText("Floor Plan", contentAreaX, contentAreaY, 30, GRAY);
                contentAreaY += 50; // Move down for filter controls

                // Search Bar
                GuiLabel(Rectangle{ (float)contentAreaX, (float)contentAreaY, 80, 20 }, "Search Room:");
                if (GuiTextBox(Rectangle{ (float)contentAreaX + 100, (float)contentAreaY - 5, 250, 30 }, searchRoomName, 64, searchRoomNameEditMode)) {
                    searchRoomNameEditMode = !searchRoomNameEditMode;
                }
                contentAreaY += 40;

                // Filters
                GuiCheckBox(Rectangle{ (float)contentAreaX, (float)contentAreaY, 20, 20 }, "Available Only", &filterAvailableRooms);                
                GuiCheckBox(Rectangle{ (float)contentAreaX, (float)contentAreaY + 30, 20, 20 }, "Booked Only", &filterBookedRooms);
                
                // Adjust position for capacity filter after new checkbox
                GuiLabel(Rectangle{ (float)contentAreaX + 200, (float)contentAreaY, 80, 20 }, "Min Capacity:"); // Align with first checkbox
                if (GuiTextBox(Rectangle{ (float)contentAreaX + 300, (float)contentAreaY - 5, 100, 30 }, filterCapacity, 10, filterCapacityEditMode)) { // Align with first checkbox
                    filterCapacityEditMode = !filterCapacityEditMode;
                }
                contentAreaY += 60; // Increase vertical space to ensure rooms are drawn below all filters

                // Filtered Rooms Logic
                std::vector<Room> displayedRooms;
                for (const auto& room : roomManager.getRooms()) {
                    bool matchesSearch = true;
                    if (strlen(searchRoomName) > 0) {
                        std::string roomNameLower = room.getName();
                        std::transform(roomNameLower.begin(), roomNameLower.end(), roomNameLower.begin(), ::tolower);
                        std::string searchLower = searchRoomName;
                        std::transform(searchLower.begin(), searchLower.end(), searchLower.begin(), ::tolower);
                        if (roomNameLower.find(searchLower) == std::string::npos) {
                            matchesSearch = false;
                        }
                    }

                    bool matchesAvailability = true; // Default to true if no availability filter is active
                    if (filterAvailableRooms && filterBookedRooms) {
                        // If both are checked, it's a conflicting state, effectively no availability filter applied.
                        // Or, you could make it show nothing, depending on desired UX.
                        // For now, if both are checked, we'll show all rooms (i.e., this filter is ignored).
                        matchesAvailability = true;
                    } else if (filterAvailableRooms) {
                        matchesAvailability = room.isAvailable();
                    } else if (filterBookedRooms) {
                        matchesAvailability = !room.isAvailable();
                    } else {
                        matchesAvailability = true; // Show all rooms by default if no specific availability filter is selected
                    }

                    bool matchesCapacity = true;
                    if (strlen(filterCapacity) > 0) {
                        try {
                            int minCapacity = std::stoi(filterCapacity);
                            if (room.getCapacity() < minCapacity) {
                                matchesCapacity = false;
                            }
                        } catch (const std::exception& e) {
                            // Handle invalid capacity input, perhaps display an error message
                        }
                    }

                    if (matchesSearch && matchesAvailability && matchesCapacity) {
                        displayedRooms.push_back(room);
                    }
                }

                // --- Scrollable Floor Plan ---
                Rectangle floorPlanView = { contentAreaX, contentAreaY, screenWidth - contentAreaX - 20, screenHeight - contentAreaY - 20 };
                DrawRectangleLinesEx(floorPlanView, 1, Fade(DARKGRAY, 0.5f));

                float roomBoxWidth = 180;
                float roomBoxHeight = 100;
                float padding = 20;
                int roomsPerRow = (floorPlanView.width > 0) ? (int)(floorPlanView.width / (roomBoxWidth + padding)) : 1;
                if (roomsPerRow == 0) roomsPerRow = 1;

                int numRows = (displayedRooms.size() + roomsPerRow - 1) / roomsPerRow;
                Rectangle floorPlanContent = { 0, 0, floorPlanView.width, (float)numRows * (roomBoxHeight + padding) };

                Rectangle viewScroll = { 0 };
                GuiScrollPanel(floorPlanView, NULL, floorPlanContent, &floorPlanScroll, &viewScroll);

                BeginScissorMode(viewScroll.x, viewScroll.y, viewScroll.width, viewScroll.height);
                {
                    for (size_t i = 0; i < displayedRooms.size(); ++i) {
                        int row = i / roomsPerRow;
                        int col = i % roomsPerRow;

                        float x = floorPlanView.x + col * (roomBoxWidth + padding) + floorPlanScroll.x;
                        float y = floorPlanView.y + row * (roomBoxHeight + padding) + floorPlanScroll.y;

                        Color roomColor = displayedRooms[i].isAvailable() ? Color{18, 160, 14, 255} : Color{190, 30, 45, 255};
                        DrawRectangle(x, y, roomBoxWidth, roomBoxHeight, roomColor);
                        DrawRectangleLines(x, y, roomBoxWidth, roomBoxHeight, DARKBROWN);

                        DrawText(displayedRooms[i].getName().c_str(), x + 10, y + 10, 20, WHITE);
                        std::string capacityStr = "Capacity: " + std::to_string(displayedRooms[i].getCapacity());
                        DrawText(capacityStr.c_str(), x + 10, y + 35, 15, WHITE);

                        if (!displayedRooms[i].isAvailable()) {
                            std::string bookedByStr = "Booked: " + displayedRooms[i].getBookedBy();
                            DrawText(bookedByStr.c_str(), x + 10, y + 60, 15, YELLOW);
                        } else {
                            DrawText("Available", x + 10, y + 60, 15, WHITE);
                        }
                    }
                }
                EndScissorMode();

                if (displayedRooms.empty() && !roomManager.getRooms().empty()) {
                    DrawText("No rooms match your search/filters.", contentAreaX + 20, contentAreaY + 20, 20, GRAY);
                } else if (roomManager.getRooms().empty()) {
                    DrawText("No rooms created yet. Admin needs to add rooms.", contentAreaX + 20, contentAreaY + 20, 20, GRAY);
                }

            }
            break;
        }

        // --- Popups ---
        if (showAddRoomPopup) {
            float popupWidth = screenWidth * 0.3f;
            float popupHeight = screenHeight * 0.4f;
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
            float popupWidth = screenWidth * 0.3f;
            float popupHeight = screenHeight * 0.35f;
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

        if (showReleaseRoomPopup) {
            float popupWidth = screenWidth * 0.3f;
            float popupHeight = screenHeight * 0.3f;
            Rectangle popupRect = { (float)screenWidth/2 - popupWidth/2, (float)screenHeight/2 - popupHeight/2, (float)popupWidth, (float)popupHeight };
            
            showReleaseRoomPopup = !GuiWindowBox(popupRect, "Release a Room");

            GuiLabel(Rectangle{ popupRect.x + 20, popupRect.y + 50, 100, 20 }, "Room Name:");
            if (GuiTextBox(Rectangle{ popupRect.x + 130, popupRect.y + 40, 250, 40 }, releaseRoomName, 64, releaseRoomNameEditMode)) {
                releaseRoomNameEditMode = !releaseRoomNameEditMode;
            }

            if (GuiButton(Rectangle{ popupRect.x + popupWidth/2 - 50, popupRect.y + 140, 100, 40 }, "Release")) {
                if (offlineManager.isOffline()) {
                    offlineManager.queueReleaseRoom(loggedInUser, releaseRoomName);
                    releaseRoomMessage = "Room release has been queued due to being offline.";
                } else {
                    ReleaseRoomStatus status = bookingSystem.releaseRoom(loggedInUser, releaseRoomName, true);
                    switch (status) {
                        case ReleaseRoomStatus::SUCCESS:
                            releaseRoomMessage = "Room '" + std::string(releaseRoomName) + "' was released successfully.";
                            break;
                        case ReleaseRoomStatus::NOT_FOUND:
                            releaseRoomMessage = "Room '" + std::string(releaseRoomName) + "' not found.";
                            break;
                        case ReleaseRoomStatus::NOT_BOOKED:
                            releaseRoomMessage = "Room '" + std::string(releaseRoomName) + "' is already available.";
                            break;
                        case ReleaseRoomStatus::NOT_OWNER:
                            releaseRoomMessage = "You can only release rooms booked by you.";
                            break;
                    }
                }
                showReleaseRoomPopup = false; // Close the action popup
                showReleaseStatusPopup = true; // Show the status popup
            }
        }
        if (showReleaseStatusPopup) {
            float popupWidth = screenWidth * 0.35f;
            float popupHeight = screenHeight * 0.25f;
            Rectangle popupRect = { (float)screenWidth/2 - popupWidth/2, (float)screenHeight/2 - popupHeight/2, (float)popupWidth, (float)popupHeight };
            if (GuiMessageBox(popupRect, "Release Status", releaseRoomMessage.c_str(), "OK") == 1) {
                showReleaseStatusPopup = false;
                releaseRoomMessage = "";
            }
        }

        if (showModifyRoomPopup) {
            float popupWidth = screenWidth * 0.35f;
            float popupHeight = screenHeight * 0.5f;
            Rectangle popupRect = { (float)screenWidth/2 - popupWidth/2, (float)screenHeight/2 - popupHeight/2, (float)popupWidth, (float)popupHeight };
            
            showModifyRoomPopup = !GuiWindowBox(popupRect, "Modify Room Details");

            GuiLabel(Rectangle{ popupRect.x + 20, popupRect.y + 50, 100, 20 }, "Room Name:");
            if (GuiTextBox(Rectangle{ popupRect.x + 130, popupRect.y + 40, 250, 40 }, modifyRoomName, 64, modifyRoomNameEditMode)) {
                modifyRoomNameEditMode = !modifyRoomNameEditMode;
            }

            if (GuiButton(Rectangle{ popupRect.x + 130, popupRect.y + 90, 100, 30 }, "Load Details")) {
                Room* room = roomManager.findRoom(modifyRoomName);
                if (room) {
                    snprintf(modifyRoomCapacity, 10, "%d", room->getCapacity());
                    modifyRoomAvailability = room->isAvailable();
                    modifyRoomMessage = "Details loaded.";
                } else {
                    modifyRoomMessage = "Room not found.";
                    memset(modifyRoomCapacity, 0, 10);
                    modifyRoomAvailability = false;
                }
            }

            GuiLabel(Rectangle{ popupRect.x + 20, popupRect.y + 140, 100, 20 }, "New Capacity:");
            if (GuiTextBox(Rectangle{ popupRect.x + 130, popupRect.y + 130, 250, 40 }, modifyRoomCapacity, 10, modifyRoomCapacityEditMode)) {
                modifyRoomCapacityEditMode = !modifyRoomCapacityEditMode;
            }

            GuiSetStyle(DEFAULT, TEXT_SIZE, 20); // Temporarily increase checkbox text size
            GuiCheckBox(Rectangle{ popupRect.x + 20, popupRect.y + 190, 30, 30 }, "Available", &modifyRoomAvailability);
            GuiSetStyle(DEFAULT, TEXT_SIZE, 10); // Reset to default

            if (GuiButton(Rectangle{ popupRect.x + popupWidth/2 - 70, popupRect.y + 240, 140, 40 }, "Save Changes")) {
                try {
                    int capacity = std::stoi(modifyRoomCapacity);
                    if (offlineManager.isOffline()) {
                        offlineManager.queueModifyRoom(loggedInUser, modifyRoomName, capacity, modifyRoomAvailability);
                        modifyRoomMessage = "Modification queued.";
                    } else {
                        roomManager.modifyRoom(loggedInUser, modifyRoomName, capacity, modifyRoomAvailability);
                        roomManager.saveRooms();
                        modifyRoomMessage = "Room modified successfully.";
                    }
                    showModifyRoomPopup = false;
                } catch (const std::exception& e) {
                    modifyRoomMessage = "Invalid capacity.";
                    std::cerr << "Invalid capacity input: " << e.what() << std::endl;
                }
            }
            DrawText(modifyRoomMessage.c_str(), popupRect.x + 20, popupRect.y + 290, 20, MAROON);
        }

        if (showDeleteRoomPopup) {
            float popupWidth = screenWidth * 0.3f;
            float popupHeight = screenHeight * 0.4f;
            Rectangle popupRect = { (float)screenWidth/2 - popupWidth/2, (float)screenHeight/2 - popupHeight/2, (float)popupWidth, (float)popupHeight };
            
            showDeleteRoomPopup = !GuiWindowBox(popupRect, "Delete Room");

            GuiLabel(Rectangle{ popupRect.x + 20, popupRect.y + 50, 100, 20 }, "Room Name:");
            if (GuiTextBox(Rectangle{ popupRect.x + 130, popupRect.y + 40, 250, 40 }, deleteRoomName, 64, deleteRoomNameEditMode)) {
                deleteRoomNameEditMode = !deleteRoomNameEditMode;
            }

            if (GuiButton(Rectangle{ popupRect.x + popupWidth/2 - 50, popupRect.y + 150, 100, 40 }, "Delete")) {
                Room* room = roomManager.findRoom(deleteRoomName);
                if (room) {
                    // Check if room is booked before deleting
                    if (!room->isAvailable()) {
                        // Optionally, you could add a confirmation step here
                        // For now, we'll just prevent deletion if booked
                        DrawText("Cannot delete booked room!", popupRect.x + 20, popupRect.y + 200, 20, RED);
                    } else {
                        if (offlineManager.isOffline()) {                            
                            offlineManager.queueDeleteRoom(deleteRoomName, loggedInUser);
                            DrawText("Deletion queued.", popupRect.x + 20, popupRect.y + 200, 20, LIME);
                        } else {
                            roomManager.deleteRoom(deleteRoomName, loggedInUser);
                            DrawText("Room deleted successfully.", popupRect.x + 20, popupRect.y + 200, 20, LIME);
                        }
                        showDeleteRoomPopup = false;
                    }
                } else {
                    DrawText("Room not found!", popupRect.x + 20, popupRect.y + 200, 20, RED);
                }
            }
            // If the popup is still open after trying to delete a booked room, the message will persist.
            // A more robust solution would use a dedicated message variable for this popup.
            if (showDeleteRoomPopup) {
                // This is a placeholder for a message if deletion fails due to booking
                // The actual message is drawn inside the button logic.
            }
        }

        if (showAddAdminPopup) {
            float popupWidth = screenWidth * 0.3f;
            float popupHeight = screenHeight * 0.45f;
            Rectangle popupRect = { (float)screenWidth/2 - popupWidth/2, (float)screenHeight/2 - popupHeight/2, (float)popupWidth, (float)popupHeight };
            
            showAddAdminPopup = !GuiWindowBox(popupRect, "Add New Admin");

            GuiLabel(Rectangle{ popupRect.x + 20, popupRect.y + 50, 100, 20 }, "Username:");
            if (GuiTextBox(Rectangle{ popupRect.x + 130, popupRect.y + 40, 250, 40 }, newAdminUsername, 64, newAdminUsernameEditMode)) {
                newAdminUsernameEditMode = !newAdminUsernameEditMode;
            }

            GuiLabel(Rectangle{ popupRect.x + 20, popupRect.y + 100, 100, 20 }, "Password:");
            // Obscure password input
            char newAdminPasswordSecret[64];
            memset(newAdminPasswordSecret, '*', strlen(newAdminPassword));
            newAdminPasswordSecret[strlen(newAdminPassword)] = '\0';
            if (GuiTextBox(Rectangle{ popupRect.x + 130, popupRect.y + 90, 250, 40 }, newAdminPasswordEditMode ? newAdminPassword : newAdminPasswordSecret, 64, newAdminPasswordEditMode)) {
                newAdminPasswordEditMode = !newAdminPasswordEditMode;
            }

            if (GuiButton(Rectangle{ popupRect.x + popupWidth/2 - 50, popupRect.y + 150, 100, 40 }, "Register")) {
                if (offlineManager.isOffline()) {
                    offlineManager.queueRegisterNewAdmin(loggedInUser, newAdminUsername, newAdminPassword);
                    addAdminMessage = "Admin registration queued.";
                } else {
                    if (auth.registerAdmin(newAdminUsername, newAdminPassword)) {
                        addAdminMessage = "Admin registered successfully!";
                    } else {
                        addAdminMessage = "Admin registration failed (username exists).";
                    }
                }
            }
            DrawText(addAdminMessage.c_str(), popupRect.x + 20, popupRect.y + 210, 20, MAROON);
        }

        if (showDeleteUserAdminPopup) {
            float popupWidth = screenWidth * 0.3f;
            float popupHeight = screenHeight * 0.4f;
            Rectangle popupRect = { (float)screenWidth/2 - popupWidth/2, (float)screenHeight/2 - popupHeight/2, (float)popupWidth, (float)popupHeight };
            
            showDeleteUserAdminPopup = !GuiWindowBox(popupRect, "Delete User/Admin");

            GuiLabel(Rectangle{ popupRect.x + 20, popupRect.y + 50, 100, 20 }, "Username:");
            if (GuiTextBox(Rectangle{ popupRect.x + 130, popupRect.y + 40, 250, 40 }, deleteTargetUsername, 64, deleteTargetUsernameEditMode)) {
                deleteTargetUsernameEditMode = !deleteTargetUsernameEditMode;
            }

            if (GuiButton(Rectangle{ popupRect.x + popupWidth/2 - 50, popupRect.y + 150, 100, 40 }, "Delete")) {
                if (std::string(deleteTargetUsername) == "Chetan") {
                    deleteUserAdminMessage = "Cannot delete superadmin 'Chetan'.";
                } else {
                    if (offlineManager.isOffline()) {
                        offlineManager.queueDeleteUser(deleteTargetUsername);
                        deleteUserAdminMessage = "Deletion queued.";
                    } else {
                        if (auth.deleteUser(deleteTargetUsername)) {
                            deleteUserAdminMessage = "User/Admin deleted successfully.";
                        } else {
                            deleteUserAdminMessage = "User/Admin not found.";
                        }
                    }
                }
            }
            DrawText(deleteUserAdminMessage.c_str(), popupRect.x + 20, popupRect.y + 200, 20, MAROON);
        }

        if (showEditUserAdminPopup) {
            float popupWidth = screenWidth * 0.35f;
            float popupHeight = screenHeight * 0.5f;
            Rectangle popupRect = { (float)screenWidth/2 - popupWidth/2, (float)screenHeight/2 - popupHeight/2, (float)popupWidth, (float)popupHeight };
            
            showEditUserAdminPopup = !GuiWindowBox(popupRect, "Edit User/Admin Details");

            GuiLabel(Rectangle{ popupRect.x + 20, popupRect.y + 50, 100, 20 }, "Username:");
            if (GuiTextBox(Rectangle{ popupRect.x + 130, popupRect.y + 40, 250, 40 }, editTargetUsername, 64, editTargetUsernameEditMode)) {
                editTargetUsernameEditMode = !editTargetUsernameEditMode;
            }

            if (GuiButton(Rectangle{ popupRect.x + 130, popupRect.y + 90, 100, 30 }, "Load Details")) {
                // Find user/admin to pre-fill details
                std::vector<std::pair<std::string, Authentication::Role>> users = auth.getUsersAndAdmins();
                bool found = false;
                for (const auto& userPair : users) {
                    if (userPair.first == std::string(editTargetUsername)) {
                        // For simplicity, we don't load password, just role
                        editNewRoleActive = (userPair.second == Authentication::Role::ADMIN) ? 1 : 0;
                        editUserAdminMessage = "Details loaded.";
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    editUserAdminMessage = "User/Admin not found.";
                    editNewRoleActive = 0;
                }
            }

            GuiLabel(Rectangle{ popupRect.x + 20, popupRect.y + 140, 100, 20 }, "New Password:");
            char editNewPasswordSecret[64];
            memset(editNewPasswordSecret, '*', strlen(editNewPassword));
            editNewPasswordSecret[strlen(editNewPassword)] = '\0';
            if (GuiTextBox(Rectangle{ popupRect.x + 130, popupRect.y + 130, 250, 40 }, editNewPasswordEditMode ? editNewPassword : editNewPasswordSecret, 64, editNewPasswordEditMode)) {
                editNewPasswordEditMode = !editNewPasswordEditMode;
            }

            GuiLabel(Rectangle{ popupRect.x + 20, popupRect.y + 190, 100, 20 }, "New Role:");
            GuiDropdownBox(Rectangle{ popupRect.x + 130, popupRect.y + 180, 150, 40 }, "USER;ADMIN", &editNewRoleActive, true);

            if (GuiButton(Rectangle{ popupRect.x + popupWidth/2 - 70, popupRect.y + 240, 140, 40 }, "Save Changes")) {
                if (std::string(editTargetUsername) == "Chetan") {
                    editUserAdminMessage = "Cannot edit superadmin 'Chetan'.";
                } else {
                    Authentication::Role newRole = (editNewRoleActive == 1) ? Authentication::Role::ADMIN : Authentication::Role::USER;
                    if (offlineManager.isOffline()) {
                        offlineManager.queueEditUser(editTargetUsername, editNewPassword, newRole);
                        editUserAdminMessage = "Modification queued.";
                    } else {
                        if (auth.editUser(editTargetUsername, editNewPassword, newRole)) {
                            editUserAdminMessage = "User/Admin modified successfully.";
                        } else {
                            editUserAdminMessage = "User/Admin not found.";
                        }
                    }
                }
            }
            DrawText(editUserAdminMessage.c_str(), popupRect.x + 20, popupRect.y + 290, 20, MAROON);
        }

        if (showViewUsersAdminsPopup) {
            float popupWidth = screenWidth * 0.4f;
            float popupHeight = screenHeight * 0.6f;
            Rectangle popupRect = { (float)screenWidth/2 - popupWidth/2, (float)screenHeight/2 - popupHeight/2, (float)popupWidth, (float)popupHeight };
            
            showViewUsersAdminsPopup = !GuiWindowBox(popupRect, "Users and Admins");

            float maxTextWidth = 0;
            if (usersAdminsDisplayList.empty()) { // Populate list only once when opened
                std::vector<std::pair<std::string, Authentication::Role>> users = auth.getUsersAndAdmins();
                for (const auto& userPair : users) {
                    std::string line = userPair.first + " (" + (userPair.second == Authentication::Role::ADMIN ? "Admin" : "User") + ")";
                    usersAdminsDisplayList.push_back(line);
                    float currentWidth = MeasureText(line.c_str(), 15);
                    if (currentWidth > maxTextWidth) {
                        maxTextWidth = currentWidth;
                    }
                }
            }

            Rectangle view = { popupRect.x + 10, popupRect.y + 40, popupRect.width - 20, popupRect.height - 60 };
            float contentWidth = (maxTextWidth > view.width) ? maxTextWidth + 20 : view.width;
            Rectangle content = { 0, 0, contentWidth, (float)usersAdminsDisplayList.size() * 25 };

            Rectangle viewScroll = { 0 };
            GuiScrollPanel(view, NULL, content, &usersAdminsScroll, &viewScroll);

            BeginScissorMode(viewScroll.x, viewScroll.y, viewScroll.width, viewScroll.height);
            {
                for(size_t i = 0; i < usersAdminsDisplayList.size(); ++i) {
                    DrawText(usersAdminsDisplayList[i].c_str(), view.x + 10 + usersAdminsScroll.x, view.y + 10 + i * 25 + usersAdminsScroll.y, 15, DARKGRAY);
                }
            }
            EndScissorMode();
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
                float maxTextWidth = 0;
                for(const auto& line : queue) {
                    float currentWidth = MeasureText(line.c_str(), 15);
                    if (currentWidth > maxTextWidth) maxTextWidth = currentWidth;
                }

                Rectangle view = { popupRect.x + 10, popupRect.y + 40, popupRect.width - 20, popupRect.height - 60 };
                float contentWidth = (maxTextWidth > view.width) ? maxTextWidth + 20 : view.width;
                Rectangle content = { 0, 0, contentWidth, (float)queue.size() * 25 };
                Rectangle viewScroll = { 0 };
                
                GuiScrollPanel(view, NULL, content, &scroll, &viewScroll);

                BeginScissorMode(viewScroll.x, viewScroll.y, viewScroll.width, viewScroll.height);
                {
                    for(size_t i = 0; i < queue.size(); ++i) {
                        DrawText(queue[i].c_str(), view.x + 10 + scroll.x, view.y + 10 + i * 25 + scroll.y, 15, DARKGRAY);
                    }
                }
                EndScissorMode();
            }
        }

        if (showRoomHistoryPopup) {
            float popupWidth = screenWidth * 0.5f;
            float popupHeight = screenHeight * 0.7f;
            Rectangle popupRect = { (float)GetScreenWidth()/2 - popupWidth/2, (float)GetScreenHeight()/2 - popupHeight/2, (float)popupWidth, (float)popupHeight };
            
            showRoomHistoryPopup = !GuiWindowBox(popupRect, "Room Modification History");

            float maxTextWidth = 0;
            // Recalculate history list and max width if it's empty
            if (roomHistoryDisplayList.empty()) {
                auto history = roomHistoryManager.getAllHistory();
                for (const auto& entry : history) {
                    char buffer[200];
                    struct tm * timeinfo;
                    timeinfo = localtime(&entry.timestamp);
                    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
                    std::string line = std::string(buffer) + " | Room: " + entry.roomName + " | Action: " + entry.action + " | By: " + entry.adminName;
                    if (entry.action != "DELETE") {
                        line += " | Capacity: " + std::to_string(entry.capacity) + " | Available: " + (entry.isAvailable ? "Yes" : "No");
                    }
                    roomHistoryDisplayList.push_back(line);
                }
            }

            // Always calculate max width from the existing list to ensure scrolling works
            for (const auto& line : roomHistoryDisplayList) {
                maxTextWidth = fmaxf(maxTextWidth, MeasureText(line.c_str(), 15));
            }

            Rectangle view = { popupRect.x + 10, popupRect.y + 40, popupRect.width - 20, popupRect.height - 60 };
            float contentWidth = (maxTextWidth > view.width) ? maxTextWidth + 20 : view.width;
            Rectangle content = { 0, 0, contentWidth, (float)roomHistoryDisplayList.size() * 25 };
            Rectangle viewScroll = { 0 };
            
            GuiScrollPanel(view, NULL, content, &roomHistoryScroll, &viewScroll);

            BeginScissorMode(viewScroll.x, viewScroll.y, viewScroll.width, viewScroll.height);
            {
                for(size_t i = 0; i < roomHistoryDisplayList.size(); ++i) {
                    DrawText(roomHistoryDisplayList[i].c_str(), view.x + 10 + roomHistoryScroll.x, view.y + 10 + i * 25 + roomHistoryScroll.y, 15, DARKGRAY);
                }
            }
            EndScissorMode();
        }

        if (showBookingHistoryPopup) {
            float popupWidth = screenWidth * 0.45f;
            float popupHeight = screenHeight * 0.6f;
            Rectangle popupRect = { (float)GetScreenWidth()/2 - popupWidth/2, (float)GetScreenHeight()/2 - popupHeight/2, (float)popupWidth, (float)popupHeight };
            
            showBookingHistoryPopup = !GuiWindowBox(popupRect, "Booking History");

            std::vector<std::string> bookingHistoryDisplayList;
            float maxTextWidth = 0;
            auto history = bookingHistoryManager.getAllHistory();
            for (const auto& entry : history) {
                if (currentState == AppState::USER_DASHBOARD && entry.username != loggedInUser) {
                    continue; // Users only see their own history
                }
                char buffer[200];
                struct tm * timeinfo;
                timeinfo = localtime(&entry.timestamp);
                strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
                std::string line = std::string(buffer) + " | Room: " + entry.roomName + " | User: " + entry.username + " | Action: " + entry.action;
                float currentWidth = MeasureText(line.c_str(), 15);
                if (currentWidth > maxTextWidth) maxTextWidth = currentWidth;
                bookingHistoryDisplayList.push_back(line);
            }

            Rectangle view = { popupRect.x + 10, popupRect.y + 40, popupRect.width - 20, popupRect.height - 60 };
            float contentWidth = (maxTextWidth > view.width) ? maxTextWidth + 20 : view.width;
            Rectangle content = { 0, 0, contentWidth, (float)bookingHistoryDisplayList.size() * 25 };
            Rectangle viewScroll = { 0 };
            GuiScrollPanel(view, NULL, content, &bookingHistoryScroll, &viewScroll);
            BeginScissorMode(viewScroll.x, viewScroll.y, viewScroll.width, viewScroll.height);
            {
                for(size_t i = 0; i < bookingHistoryDisplayList.size(); ++i) {
                    DrawText(bookingHistoryDisplayList[i].c_str(), view.x + 10 + bookingHistoryScroll.x, view.y + 10 + i * 25 + bookingHistoryScroll.y, 15, DARKGRAY);
                }
            }
            EndScissorMode();
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
