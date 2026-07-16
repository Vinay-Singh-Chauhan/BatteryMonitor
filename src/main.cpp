#include "battery_monitor.h"
#include <iostream>
#include <csignal>
#include <memory>

std::unique_ptr<BatteryMonitor> g_monitor;

void signal_handler(int signal) {
    std::cout << "\nReceived signal " << signal << ", shutting down..." << std::endl;
    if (g_monitor) {
        g_monitor->stop();
    }
}

int main(int argc, char* argv[]) {
    std::cout << "Battery Monitor v1.0" << std::endl;
    
    g_monitor = std::make_unique<BatteryMonitor>();
    
    if (!g_monitor->init()) {
        std::cerr << "Failed to initialize Battery Monitor" << std::endl;
        return 1;
    }
    
    // Setup signal handlers for graceful shutdown
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Run the monitor (blocks)
    g_monitor->run();
    
    return 0;
}
