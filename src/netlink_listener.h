#ifndef NETLINK_LISTENER_H
#define NETLINK_LISTENER_H

#include <functional>

/**
 * Listens for kernel Netlink events (KOBJ_CHANGE) from power_supply subsystem
 */
class NetlinkListener {
public:
    using EventCallback = std::function<void()>;
    
    NetlinkListener();
    ~NetlinkListener();
    
    /**
     * Start listening for Netlink events
     * Blocks until socket error or shutdown
     * @param callback Function to call when power_supply event is received
     */
    void start(EventCallback callback);
    
    /**
     * Stop the listener (can be called from signal handler)
     */
    void stop();

private:
    int socket_fd;
    volatile bool running;
    
    /**
     * Create and bind Netlink socket for KOBJ_CHANGE events
     */
    int create_netlink_socket();
    
    /**
     * Parse Netlink message and check if it's a power_supply event
     */
    bool is_power_supply_event(const char* buffer, int length);
};

#endif // NETLINK_LISTENER_H
