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
    BatteryReader(const std::string& battery_path = "/sys/class/power_supply");
    
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
     * Resolve the battery directory to read from.
     * Supports either a direct battery directory or the parent power_supply directory.
     */
    std::string resolve_battery_path() const;
    
    /**
     * Read a single value from a sysfs file
     */
    std::string read_sysfs_file(const std::string& root_path, const std::string& filename) const;
};

#endif // BATTERY_READER_H
