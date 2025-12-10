@echo off
if not exist output mkdir output
g++ -std=c++17 -Iinclude -Isrc -Llib gui_main.cpp src/auth.cpp src/room.cpp src/meetingroom.cpp src/offlinemechanism.cpp src/ui.cpp src/history.cpp -o output/ifm_gui.exe -lraylib -lopengl32 -lgdi32 -lwinmm -Wall -Wextra