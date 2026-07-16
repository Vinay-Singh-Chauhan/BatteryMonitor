#include "battery_reader.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

namespace fs = std::filesystem;

int main() {
    const fs::path temp_dir = fs::temp_directory_path() / "battery-reader-test";
    fs::remove_all(temp_dir);
    fs::create_directories(temp_dir / "BAT1");

    const std::string capacity_path = (temp_dir / "BAT1" / "capacity").string();
    const std::string status_path = (temp_dir / "BAT1" / "status").string();

    std::ofstream capacity_file(capacity_path);
    capacity_file << "77\n";
    capacity_file.close();

    std::ofstream status_file(status_path);
    status_file << "Discharging\n";
    status_file.close();

    BatteryReader reader(temp_dir.string());
    BatteryInfo info = reader.read();

    if (info.capacity != 77 || info.status != "Discharging") {
        std::cerr << "BatteryReader test failed" << std::endl;
        fs::remove_all(temp_dir);
        return 1;
    }

    fs::remove_all(temp_dir);
    return 0;
}
