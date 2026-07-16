#include "battery_monitor.h"
#include <iostream>
#include <iomanip>
#include <csignal>

namespace {
constexpr int kLowBatteryThreshold = 10;
constexpr int kHighBatteryThreshold = 90;
}

extern volatile sig_atomic_t g_shutdown_requested;

BatteryMonitor::BatteryMonitor()
    : battery_reader(std::make_unique<BatteryReader>()),
      netlink_listener(std::make_unique<NetlinkListener>()),
      notifier(std::make_unique<Notification>()),
      low_notified(false),
      high_notified(false),
      last_capacity(-1),
      last_status("") {}

BatteryMonitor::~BatteryMonitor() = default;

bool BatteryMonitor::init() {
    if (!battery_reader->is_available()) {
        std::cerr << "Battery device not found at /sys/class/power_supply/BAT0" << std::endl;
        return false;
    }
    
    notifier->init("Battery Monitor");
    
    // Read initial state
    BatteryInfo info = battery_reader->read();
    last_capacity = info.capacity;
    last_status = info.status;
    
    std::cout << "Battery Monitor initialized" << std::endl;
    std::cout << "Initial capacity: " << info.capacity << "%" << std::endl;
    std::cout << "Initial status: " << info.status << std::endl;
    
    return true;
}

void BatteryMonitor::run() {
    std::cout << "Starting battery monitoring..." << std::endl;
    
    netlink_listener->start([this]() {
        this->on_battery_event();
    });

    if (g_shutdown_requested != 0) {
        std::cout << "Shutdown requested, stopping monitor..." << std::endl;
        stop();
    }
}

void BatteryMonitor::stop() {
    std::cout << "Stopping battery monitor..." << std::endl;
    netlink_listener->stop();
}

void BatteryMonitor::on_battery_event() {
    BatteryInfo info = battery_reader->read();
    
    // Only process if something changed
    if (info.capacity != last_capacity || info.status != last_status) {
        std::cout << "Battery event: " << info.capacity << "% (" << info.status << ")" << std::endl;
        last_capacity = info.capacity;
        last_status = info.status;
        
        check_thresholds(info);
    }
}

void BatteryMonitor::check_thresholds(const BatteryInfo& info) {
    // Low battery threshold
    if (info.capacity <= kLowBatteryThreshold && info.status == "Discharging") {
        if (!low_notified) {
            std::cout << "Low battery notification triggered" << std::endl;
            notifier->notify("Battery Low", 
                           "Battery level is at " + std::to_string(info.capacity) + "%",
                           "critical");
            low_notified = true;
            high_notified = false;  // Reset high notification flag
        }
    } else if (info.capacity > kLowBatteryThreshold) {
        // Reset low notification flag when we go above the threshold
        low_notified = false;
    }
    
    // High battery threshold
    if (info.capacity >= kHighBatteryThreshold && info.status == "Charging") {
        if (!high_notified) {
            std::cout << "High battery notification triggered" << std::endl;
            notifier->notify("Battery Charged",
                           "Battery charged to " + std::to_string(info.capacity) + "%",
                           "normal");
            high_notified = true;
            low_notified = false;  // Reset low notification flag
        }
    } else if (info.capacity < kHighBatteryThreshold) {
        // Reset high notification flag when we go below the threshold
        high_notified = false;
    }
}
