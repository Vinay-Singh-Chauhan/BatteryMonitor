#include "notification.h"
#include <libnotify/notify.h>
#include <iostream>

Notification::Notification()
    : initialized(false) {}

Notification::~Notification() {
    if (initialized) {
        notify_uninit();
    }
}

void Notification::init(const std::string& app_name) {
    if (initialized) return;
    
    if (!notify_is_initted()) {
        if (!notify_init(app_name.c_str())) {
            std::cerr << "Failed to initialize libnotify" << std::endl;
            return;
        }
    }
    initialized = true;
}

void Notification::notify(const std::string& summary, const std::string& body, const std::string& urgency) {
    if (!initialized) {
        std::cerr << "Notification system not initialized" << std::endl;
        return;
    }
    
    NotifyNotification* notification = notify_notification_new(
        summary.c_str(),
        body.empty() ? nullptr : body.c_str(),
        nullptr  // No icon
    );
    
    // Set urgency hint
    if (urgency == "critical") {
        notify_notification_set_urgency(notification, NOTIFY_URGENCY_CRITICAL);
    } else if (urgency == "low") {
        notify_notification_set_urgency(notification, NOTIFY_URGENCY_LOW);
    } else {
        notify_notification_set_urgency(notification, NOTIFY_URGENCY_NORMAL);
    }
    
    // Set timeout (5 seconds)
    notify_notification_set_timeout(notification, 5000);
    
    GError* error = nullptr;
    if (!notify_notification_show(notification, &error)) {
        std::cerr << "Failed to send notification: ";
        if (error) {
            std::cerr << error->message << std::endl;
            g_error_free(error);
        } else {
            std::cerr << "Unknown error" << std::endl;
        }
    }
    
    g_object_unref(notification);
}
