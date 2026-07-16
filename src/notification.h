#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <string>

/**
 * Send desktop notifications via D-Bus using libnotify
 */
class Notification {
public:
    Notification();
    ~Notification();
    
    /**
     * Initialize the notification system
     * Must be called before sending notifications
     */
    void init(const std::string& app_name = "Battery Monitor");
    
    /**
     * Send a notification
     * @param summary Title of the notification
     * @param body Body text (can be empty)
     * @param urgency Level: "low", "normal", "critical"
     */
    void notify(const std::string& summary, const std::string& body = "", const std::string& urgency = "normal");

private:
    bool initialized;
};

#endif // NOTIFICATION_H
