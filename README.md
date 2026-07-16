# Battery Monitor

A lightweight daemon that monitors battery status on Linux systems and sends desktop notifications when battery reaches critical thresholds.

## Architecture Overview

```
             Linux Kernel
                  │
     power_supply uevents
                  │
       Netlink (KOBJ_CHANGE)
                  │
      Battery Monitor Daemon
                  │
        Read /sys/class/power_supply/BAT0/
                  │
     Threshold Detection (10%, 90%)
                  │
      D-Bus Desktop Notification
```

## Features

- **Event-driven**: Listens for kernel power supply events via Netlink
- **Efficient**: Only processes when battery state actually changes
- **Smart thresholds**: Maintains state flags to send notifications only once per threshold crossing
  - Low battery (≤10% while discharging): "Battery Low" notification
  - Charged (≥90% while charging): "Battery Charged" notification
- **D-Bus notifications**: Uses Freedesktop notification service for desktop integration
- **Systemd integration**: Runs as a user service with automatic restart

## Components

### 1. **Netlink Listener** (`netlink_listener.cpp`)
- Subscribes to `NETLINK_KOBJECT_UEVENT` for power supply subsystem
- Filters for `KOBJ_CHANGE` events
- Triggers battery check on each event

### 2. **Battery Reader** (`battery_reader.cpp`)
- Reads from `/sys/class/power_supply/BAT0/capacity` (0-100%)
- Reads from `/sys/class/power_supply/BAT0/status` (Charging/Discharging/Full/etc)
- Handles sysfs errors gracefully

### 3. **Threshold Logic** (`battery_monitor.cpp`)
```
Maintains state flags:
  low_notified: true after low battery notification until capacity > 10%
  high_notified: true after charged notification until capacity < 90%

Logic:
  if (capacity <= 10 && status == "Discharging" && !low_notified)
    → Show "Battery Low" notification, set low_notified = true
  
  if (capacity >= 90 && status == "Charging" && !high_notified)
    → Show "Battery Charged" notification, set high_notified = true
```

### 4. **Notifications** (`notification.cpp`)
- Uses libnotify library (D-Bus wrapper)
- Sets urgency level appropriately (critical for low, normal for charged)
- 5-second timeout for notifications

## Dependencies

```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake libnotify-dev

# Fedora
sudo dnf install cmake libnotify-devel gcc-c++

# Arch
sudo pacman -S cmake libnotify gcc
```

## Building

### 1. Configure
```bash
cd BatteryMonitor
mkdir -p build
cd build
cmake ..
```

### 2. Build
```bash
cmake --build .
```

### 3. Install (optional)
```bash
sudo cmake --install .
```

Or install to user directory:
```bash
cmake --install . --prefix ~/.local
```

## Running

### Manual execution
```bash
./build/battery-monitor
```

Press `Ctrl+C` to stop.

### Install as systemd user service

1. Copy the binary:
```bash
mkdir -p ~/.local/bin
cp build/battery-monitor ~/.local/bin/
```

2. Install the service file:
```bash
mkdir -p ~/.config/systemd/user
cp systemd/battery-monitor.service ~/.config/systemd/user/
```

3. Enable and start the service:
```bash
systemctl --user daemon-reload
systemctl --user enable battery-monitor
systemctl --user start battery-monitor
```

4. Verify it's running:
```bash
systemctl --user status battery-monitor
journalctl --user -u battery-monitor -f
```

### Disable the service
```bash
systemctl --user stop battery-monitor
systemctl --user disable battery-monitor
```

## Configuration

Currently, the thresholds are hardcoded:
- Low battery: 10% while discharging
- Charged: 90% while charging

To customize, edit the `check_thresholds()` method in `src/battery_monitor.cpp`.

## Troubleshooting

### Battery device not found
```
Error: Battery device not found at /sys/class/power_supply/BAT0
```
Your battery might have a different name. Check available devices:
```bash
ls /sys/class/power_supply/
```
Then update `BatteryReader::BatteryReader()` with the correct path.

### Notifications not showing
- Ensure your desktop environment supports D-Bus notifications (GNOME, KDE, etc.)
- Check if notification daemon is running:
```bash
ps aux | grep -i notification
```

### Netlink socket permission error
The daemon needs to read kernel events. If running as a regular user (systemd --user):
- This should work fine for power supply events
- If not, try running with `sudo`

### Check daemon logs
```bash
journalctl --user -u battery-monitor -n 50
```

## Performance

- **Memory**: ~2-5 MB idle
- **CPU**: <0.1% idle, wakes only on battery events
- **No polling**: Event-driven architecture means zero CPU usage when idle

## Files

```
BatteryMonitor/
├── src/
│   ├── main.cpp                 # Entry point
│   ├── battery_monitor.h/cpp    # Main daemon logic
│   ├── battery_reader.h/cpp     # sysfs reader
│   ├── netlink_listener.h/cpp   # Kernel event listener
│   └── notification.h/cpp       # D-Bus notifications
├── systemd/
│   └── battery-monitor.service  # systemd user service
├── CMakeLists.txt               # Build configuration
└── README.md                    # This file
```

## License

MIT License - feel free to use and modify

## Contributing

Issues and pull requests welcome!
