#include "battery_reader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

BatteryReader::BatteryReader(const std::string& battery_path)
    : battery_path(battery_path) {}

BatteryInfo BatteryReader::read() {
    BatteryInfo info;
    info.capacity = 0;
    info.status = "Unknown";
    
    try {
        std::string capacity_str = read_sysfs_file("capacity");
        std::string status_str = read_sysfs_file("status");
        
        if (!capacity_str.empty()) {
            info.capacity = std::stoi(capacity_str);
            // Clamp to 0-100 range
            if (info.capacity < 0) info.capacity = 0;
            if (info.capacity > 100) info.capacity = 100;
        }
        
        if (!status_str.empty()) {
            info.status = status_str;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error reading battery info: " << e.what() << std::endl;
    }
    
    return info;
}

bool BatteryReader::is_available() const {
    return fs::exists(battery_path) && fs::is_directory(battery_path);
}

std::string BatteryReader::read_sysfs_file(const std::string& filename) const {
    std::string file_path = battery_path + "/" + filename;
    std::ifstream file(file_path);
    
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + file_path);
    }
    
    std::string line;
    if (std::getline(file, line)) {
        // Remove trailing whitespace/newlines
        line.erase(line.find_last_not_of(" \n\r\t") + 1);
        return line;
    }
    
    return "";
}
