#include "battery_reader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <vector>

namespace fs = std::filesystem;

BatteryReader::BatteryReader(const std::string& battery_path)
    : battery_path(battery_path) {}

BatteryInfo BatteryReader::read() {
    BatteryInfo info;
    info.capacity = 0;
    info.status = "Unknown";

    const std::string resolved_path = resolve_battery_path();
    if (resolved_path.empty()) {
        return info;
    }
    
    try {
        std::string capacity_str = read_sysfs_file(resolved_path, "capacity");
        std::string status_str = read_sysfs_file(resolved_path, "status");
        
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
    return !resolve_battery_path().empty();
}

std::string BatteryReader::resolve_battery_path() const {
    if (battery_path.empty()) {
        return {};
    }

    const fs::path root_path(battery_path);
    if (!fs::exists(root_path) || !fs::is_directory(root_path)) {
        return {};
    }

    const fs::path capacity_file = root_path / "capacity";
    const fs::path status_file = root_path / "status";
    if (fs::exists(capacity_file) && fs::exists(status_file)) {
        return root_path.string();
    }

    std::vector<fs::path> candidates;
    for (const auto& entry : fs::directory_iterator(root_path)) {
        if (!entry.is_directory()) {
            continue;
        }

        const std::string name = entry.path().filename().string();
        if (name.rfind("BAT", 0) != 0) {
            continue;
        }

        const fs::path candidate_capacity = entry.path() / "capacity";
        const fs::path candidate_status = entry.path() / "status";
        if (fs::exists(candidate_capacity) && fs::exists(candidate_status)) {
            candidates.push_back(entry.path());
        }
    }

    std::sort(candidates.begin(), candidates.end(), [](const fs::path& lhs, const fs::path& rhs) {
        return lhs.filename().string() < rhs.filename().string();
    });

    if (!candidates.empty()) {
        return candidates.front().string();
    }

    return {};
}

std::string BatteryReader::read_sysfs_file(const std::string& root_path, const std::string& filename) const {
    const std::string file_path = root_path + "/" + filename;
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
