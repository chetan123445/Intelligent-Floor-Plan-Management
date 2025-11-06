
#include "floorplan.hpp"
#include "ui.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>

// FloorPlan class implementation
FloorPlan::FloorPlan(const std::string& name, const std::string& lastModifiedBy, int capacity, bool isAvailable)
    : name(name), lastModifiedBy(lastModifiedBy), capacity(capacity), m_isAvailable(isAvailable) {
    timestamp = time(0);
}

std::string FloorPlan::getName() const { return name; }
std::string FloorPlan::getLastModifiedBy() const { return lastModifiedBy; }
time_t FloorPlan::getTimestamp() const { return timestamp; }
int FloorPlan::getCapacity() const { return capacity; }
bool FloorPlan::isAvailable() const { return m_isAvailable; }

void FloorPlan::setCapacity(int capacity) { this->capacity = capacity; }
void FloorPlan::setAvailable(bool isAvailable) { this->m_isAvailable = isAvailable; }
void FloorPlan::setLastModifiedBy(const std::string& adminName) { this->lastModifiedBy = adminName; }
void FloorPlan::setTimestamp(time_t timestamp) { this->timestamp = timestamp; }

// FloorPlanManager class implementation
FloorPlanManager::FloorPlanManager() {
    loadFloorPlans();
}

void FloorPlanManager::loadFloorPlans() {
    floorPlans.clear();
    std::ifstream file(FLOOR_PLANS_FILE);
    if (file.is_open()) {
        std::string name, lastModifiedBy, capacity_str, isAvailable_str;
        while (file >> name >> lastModifiedBy >> capacity_str >> isAvailable_str) {
            int capacity = std::stoi(capacity_str);
            bool isAvailable = (isAvailable_str == "Yes");
            floorPlans.emplace_back(name, lastModifiedBy, capacity, isAvailable);
        }
        file.close();
    }
}

void FloorPlanManager::saveFloorPlans() {
    std::ofstream file(FLOOR_PLANS_FILE);
    if (file.is_open()) {
        for (const auto& plan : floorPlans) {
            file << plan.getName() << " " << plan.getLastModifiedBy() << " " << plan.getCapacity() << " " << (plan.isAvailable() ? "Yes" : "No") << std::endl;
        }
        file.close();
    }
}

void FloorPlanManager::uploadFloorPlan(const std::string& adminName) {
    std::string planName;
    int capacity;
    bool isAvailable;

    std::cout << "\n--- Upload New Floor Plan ---" << std::endl;
    std::cout << "Enter a unique name for the new floor plan: ";
    std::cin >> planName;

    auto it = std::find_if(floorPlans.begin(), floorPlans.end(), [&](const FloorPlan& p) {
        return p.getName() == planName;
    });

    if (it != floorPlans.end()) {
        UI::displayMessage("Error: Floor plan '" + planName + "' already exists. Please choose a different name.");
        return;
    }

    std::cout << "Enter the capacity of the floor plan: ";
    std::cin >> capacity;
    std::cout << "Is the floor plan available for booking? (1 for Yes, 0 for No): ";
    std::cin >> isAvailable;

    floorPlans.emplace_back(planName, adminName, capacity, isAvailable);
    saveFloorPlans();
    UI::displayMessage("Floor plan '" + planName + "' has been uploaded successfully.");
}

void FloorPlanManager::uploadFloorPlan(const std::string& adminName, const std::string& planName, int capacity, bool isAvailable) {
    auto it = std::find_if(floorPlans.begin(), floorPlans.end(), [&](const FloorPlan& p) {
        return p.getName() == planName;
    });

    if (it != floorPlans.end()) {
        UI::displayMessage("Error: Floor plan '" + planName + "' already exists. Please choose a different name.");
        return;
    }

    floorPlans.emplace_back(planName, adminName, capacity, isAvailable);
    saveFloorPlans();
    UI::displayMessage("Floor plan '" + planName + "' has been uploaded successfully.");
}

void FloorPlanManager::modifyFloorPlan(const std::string& adminName) {
    std::string planName;
    int capacity;
    bool isAvailable;

    std::cout << "\n--- Modify Existing Floor Plan ---" << std::endl;
    std::cout << "Enter the name of the floor plan to modify: ";
    std::cin >> planName;

    auto it = std::find_if(floorPlans.begin(), floorPlans.end(), [&](const FloorPlan& p) {
        return p.getName() == planName;
    });

    if (it != floorPlans.end()) {
        std::cout << "Enter the new capacity for '" << planName << "': ";
        std::cin >> capacity;
        std::cout << "Is '" << planName << "' available for booking? (1 for Yes, 0 for No): ";
        std::cin >> isAvailable;

        it->setCapacity(capacity);
        it->setAvailable(isAvailable);
        it->setLastModifiedBy(adminName);
        it->setTimestamp(time(0));

        saveFloorPlans();
        UI::displayMessage("Floor plan '" + planName + "' has been modified successfully.");
    } else {
        UI::displayMessage("Error: Floor plan '" + planName + "' does not exist.");
    }
}

void FloorPlanManager::modifyFloorPlan(const std::string& adminName, const std::string& planName, int capacity, bool isAvailable) {
    auto it = std::find_if(floorPlans.begin(), floorPlans.end(), [&](const FloorPlan& p) {
        return p.getName() == planName;
    });

    if (it != floorPlans.end()) {
        it->setCapacity(capacity);
        it->setAvailable(isAvailable);
        it->setLastModifiedBy(adminName);
        it->setTimestamp(time(0));

        saveFloorPlans();
        UI::displayMessage("Floor plan '" + planName + "' has been modified successfully.");
    } else {
        UI::displayMessage("Error: Floor plan '" + planName + "' does not exist.");
    }
}

void FloorPlanManager::viewFloorPlan() {
    std::string planName;
    std::cout << "\n--- View Floor Plan Details ---" << std::endl;
    std::cout << "Enter the name of the floor plan to view: ";
    std::cin >> planName;

    auto it = std::find_if(floorPlans.begin(), floorPlans.end(), [&](const FloorPlan& p) {
        return p.getName() == planName;
    });

    if (it != floorPlans.end()) {
        std::cout << "\n--- Details for Floor Plan: " << it->getName() << " ---" << std::endl;
        std::cout << "Last Modified By: " << it->getLastModifiedBy() << std::endl;
        const time_t timestamp = it->getTimestamp();
        std::cout << "Last Modified On: " << ctime(&timestamp);
        std::cout << "Capacity: " << it->getCapacity() << std::endl;
        std::cout << "Available: " << (it->isAvailable() ? "Yes" : "No") << std::endl;
        std::cout << "------------------------------------" << std::endl;
    } else {
        UI::displayMessage("Error: Floor plan '" + planName + "' does not exist.");
    }
}
