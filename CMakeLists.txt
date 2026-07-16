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

# Tests
enable_testing()
add_executable(battery_reader_test tests/battery_reader_test.cpp src/battery_reader.cpp)
target_include_directories(battery_reader_test PRIVATE src)
target_compile_options(battery_reader_test PRIVATE -Wall -Wextra -Wpedantic)
add_test(NAME battery_reader_test COMMAND battery_reader_test)

# Installation
include(GNUInstallDirs)
install(TARGETS battery-monitor DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES systemd/battery-monitor.service DESTINATION ${CMAKE_INSTALL_DATADIR}/systemd/user/)
