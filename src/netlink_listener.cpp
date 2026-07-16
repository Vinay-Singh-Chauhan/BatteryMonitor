#include "netlink_listener.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <cerrno>

NetlinkListener::NetlinkListener()
    : socket_fd(-1), running(false) {}

NetlinkListener::~NetlinkListener() {
    stop();
    if (socket_fd >= 0) {
        close(socket_fd);
    }
}

void NetlinkListener::start(EventCallback callback) {
    socket_fd = create_netlink_socket();
    if (socket_fd < 0) {
        std::cerr << "Failed to create Netlink socket" << std::endl;
        return;
    }
    
    running = true;
    char buffer[4096];
    
    while (running) {
        int len = recvfrom(socket_fd, buffer, sizeof(buffer) - 1, 0, nullptr, nullptr);
        
        if (len < 0) {
            if (running) {
                std::cerr << "Error receiving from Netlink socket: " << strerror(errno) << std::endl;
            }
            break;
        }
        
        if (len > 0 && is_power_supply_event(buffer, len)) {
            callback();
        }
    }
}

void NetlinkListener::stop() {
    running = false;
    if (socket_fd >= 0) {
        close(socket_fd);
        socket_fd = -1;
    }
}

int NetlinkListener::create_netlink_socket() {
    int sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_KOBJECT_UEVENT);
    if (sock < 0) {
        std::cerr << "Cannot create Netlink socket: " << strerror(errno) << std::endl;
        return -1;
    }
    
    struct sockaddr_nl addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_pid = getpid();
    addr.nl_groups = 1;  // NETLINK_KOBJECT_UEVENT group
    
    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Cannot bind Netlink socket: " << strerror(errno) << std::endl;
        close(sock);
        return -1;
    }
    
    return sock;
}

bool NetlinkListener::is_power_supply_event(const char* buffer, int length) {
    // Netlink uevent format: "action@devpath\0KEY=VALUE\0KEY=VALUE\0..."
    // We're looking for action==KOBJ_CHANGE and subsystem==power_supply
    
    // Skip the action@devpath part
    const char* pos = buffer;
    while (pos < buffer + length && *pos != '\0') {
        pos++;
    }
    pos++; // Skip the null terminator
    
    bool found_power_supply = false;
    
    // Parse key-value pairs
    while (pos < buffer + length && *pos != '\0') {
        if (strncmp(pos, "SUBSYSTEM=power_supply", 22) == 0) {
            found_power_supply = true;
        }
        
        // Move to next pair
        while (pos < buffer + length && *pos != '\0') {
            pos++;
        }
        pos++;
    }
    
    return found_power_supply;
}
