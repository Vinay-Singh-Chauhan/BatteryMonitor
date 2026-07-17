# Quick Start Guide

## Build

```bash
cd ~/Skills/Projects/BatteryMonitor
cmake -B build
cmake --build build
```

The executable will be at: `build/battery-monitor` 

## Test Run

```bash
./build/battery-monitor
```

Expected output:
```
Battery Monitor v1.0
Battery Monitor initialized
Initial capacity: 75%
Initial status: Charging
Starting battery monitoring...
```

Press `Ctrl+C` to exit.

## Install as User Service

### Step 1: Install to user directory
```bash
mkdir -p ~/.local/bin
cp build/battery-monitor ~/.local/bin/
```

### Step 2: Install systemd service
```bash
mkdir -p ~/.config/systemd/user
cp systemd/battery-monitor.service ~/.config/systemd/user/
```

### Step 3: Enable and start
```bash
systemctl --user daemon-reload
systemctl --user enable battery-monitor
systemctl --user start battery-monitor
```

### Step 4: Verify it's running
```bash
systemctl --user status battery-monitor
```

View logs:
```bash
journalctl --user -u battery-monitor -f
```

## How It Works

1. **Netlink Listener** subscribes to kernel power supply events
2. **Battery Reader** reads from `/sys/class/power_supply/BAT0/` or the BAT# supply that occurs first in lexicographical order
3. **Threshold Logic** sends notifications when:
   - Battery ≤ 10% while discharging → "Battery Low" (critical)
   - Battery ≥ 90% while charging → "Battery Charged" (normal)
4. **State flags** prevent duplicate notifications for the same threshold


## Troubleshooting

**"Battery device not found"**
```bash
# Check what battery devices exist
ls /sys/class/power_supply/
```
Common names: BAT0, BAT1, BAT2, or Custom0

**"Cannot create Netlink socket"**
- Try with sudo for debugging:
```bash
sudo ./build/battery-monitor
```

**No notifications appearing**
- Ensure D-Bus notification daemon is running
- Check D-Bus session:
```bash
echo $DBUS_SESSION_BUS_ADDRESS
```

**View daemon logs**
```bash
journalctl --user -u battery-monitor -n 50
```

## Customization

Edit thresholds in `src/battery_monitor.cpp`, function `check_thresholds()`:

```cpp
constexpr int kLowBatteryThreshold = 10;
constexpr int kHighBatteryThreshold = 90;
```

Then rebuild:
```bash
cmake --build build
```

## Performance

- **Memory**: ~2-5 MB
- **CPU**: 0% when idle (event-driven)
- **Startup time**: < 100ms

## Stopping the Service

```bash
systemctl --user stop battery-monitor
systemctl --user disable battery-monitor
```

Then remove files:
```bash
rm ~/.local/bin/battery-monitor
rm ~/.config/systemd/user/battery-monitor.service
systemctl --user daemon-reload
```
