#ifndef BATTERY_MONITOR_H
#define BATTERY_MONITOR_H

#include "battery_reader.h"
#include "netlink_listener.h"
#include "notification.h"
#include <memory>

/**
 * Main daemon that coordinates battery monitoring
 * - Listens for Netlink events
 * - Reads battery status
 * - Maintains threshold state
 * - Sends notifications
 */
class BatteryMonitor {
public:
    BatteryMonitor();
    ~BatteryMonitor();
    
    /**
     * Initialize the monitor
     * @return true if initialization successful
     */
    bool init();
    
    /**
     * Start monitoring (blocks until stopped)
     */
    void run();
    
    /**
     * Stop the monitor (can be called from signal handler)
     */
    void stop();

private:
    std::unique_ptr<BatteryReader> battery_reader;
    std::unique_ptr<NetlinkListener> netlink_listener;
    std::unique_ptr<Notification> notifier;
    
    // Threshold state flags
    bool low_notified;
    bool high_notified;
    
    int last_capacity;
    std::string last_status;
    
    /**
     * Called when a power supply event is received
     */
    void on_battery_event();
    
    /**
     * Check thresholds and send notifications
     */
    void check_thresholds(const BatteryInfo& info);
};

#endif // BATTERY_MONITOR_H
