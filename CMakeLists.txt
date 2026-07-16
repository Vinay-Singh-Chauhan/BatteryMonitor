cmake_minimum_required(VERSION 3.10)
project(BatteryMonitor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBNOTIFY REQUIRED libnotify)

# Include directories
include_directories(${LIBNOTIFY_INCLUDE_DIRS})

# Source files
set(SOURCES
    src/main.cpp
    src/battery_monitor.cpp
    src/battery_reader.cpp
    src/netlink_listener.cpp
    src/notification.cpp
)

# Create executable
add_executable(battery-monitor ${SOURCES})

# Link libraries
target_link_libraries(battery-monitor 
    PRIVATE ${LIBNOTIFY_LIBRARIES}
)

# Compiler flags
target_compile_options(battery-monitor PRIVATE -Wall -Wextra -Wpedantic)

# Installation
install(TARGETS battery-monitor DESTINATION bin)
install(FILES systemd/battery-monitor.service DESTINATION ~/.config/systemd/user/)
