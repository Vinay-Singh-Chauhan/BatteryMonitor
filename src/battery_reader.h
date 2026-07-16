#ifndef BATTERY_READER_H
#define BATTERY_READER_H

#include <string>
#include <cstdint>

struct BatteryInfo {
    int capacity;
    std::string status;
};

class BatteryReader {
public:
    BatteryReader(const std::string& battery_path = "/sys/class/power_supply/BAT0");
    
    /**
     * Read current battery capacity and status
     * @return BatteryInfo struct with capacity (0-100) and status string
     */
    BatteryInfo read();
    
    /**
     * Check if battery path is accessible
     */
    bool is_available() const;

private:
    std::string battery_path;
    
    /**
     * Read a single value from a sysfs file
     */
    std::string read_sysfs_file(const std::string& filename) const;
};

#endif // BATTERY_READER_H
