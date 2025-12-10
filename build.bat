if not exist output mkdir output
g++ -std=c++17 -Iinclude -Isrc -o output\intelligent_floor_plan.exe src\auth.cpp src\floorplan.cpp src\main.cpp src\meetingroom.cpp src\offlinemechanism.cpp src\ui.cpp