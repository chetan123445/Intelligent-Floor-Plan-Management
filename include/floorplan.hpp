#ifndef FLOORPLAN_HPP
#define FLOORPLAN_HPP

#include <string>
#include <vector>
#include <ctime>

class FloorPlan {
public:
    FloorPlan(const std::string& name, const std::string& lastModifiedBy, int capacity, bool isAvailable);

    std::string getName() const;
    std::string getLastModifiedBy() const;
    time_t getTimestamp() const;
    int getCapacity() const;
    bool isAvailable() const;

    void setCapacity(int capacity);
    void setAvailable(bool isAvailable);
    void setLastModifiedBy(const std::string& adminName);
    void setTimestamp(time_t timestamp);

private:
    std::string name;
    std::string lastModifiedBy;
    time_t timestamp;
    int capacity;
    bool m_isAvailable;
};

class FloorPlanManager {
public:
    FloorPlanManager();
    void uploadFloorPlan(const std::string& adminName);
    void uploadFloorPlan(const std::string& adminName, const std::string& planName, int capacity, bool isAvailable);
    void modifyFloorPlan(const std::string& adminName);
    void modifyFloorPlan(const std::string& adminName, const std::string& planName, int capacity, bool isAvailable);
    void viewFloorPlan();

private:
    std::vector<FloorPlan> floorPlans;
    const std::string FLOOR_PLANS_FILE = "output/floor_plans.txt";

    void loadFloorPlans();
    void saveFloorPlans();
};

#endif // FLOORPLAN_HPP