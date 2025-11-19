# Autonomous-Reliable-Car

Autonomous line following robot with obstacle avoidance for the 4tronix Initio robot car.

## Overview

This project implements a line follower with obstacle avoidance using a 5-state finite state machine. The car autonomously follows a black line while detecting and avoiding obstacles in real-time.

### Key Concepts (from lectures)

**Lecture 3 - Sensors & Actuators:**
- IR Line Sensors: Detect black/white surfaces (0=white, 1=black)
- IR Obstacle Sensors: Detect objects in path (0=clear, 1=obstacle)
- Motor Control: Speed ranges 0-100 for drive/spin commands
- Priority-based sensing for safety

**Lecture 4 - Control Systems:**
- Finite State Machine (FSM) with 5 states
- Sensor feedback drives state transitions
- Corrective control maintains line position

**Lecture 6 - Real-Time Systems:**
- Non-blocking input handling with ncurses
- Continuous sensor polling loop
- Graceful shutdown and resource cleanup

## Quick Start

### Installation

```bash
# Install dependencies
sudo apt-get update
sudo apt-get install -y build-essential git libncurses-dev libwiringpi-dev wiringpi pkgconfig

# Clone and install initio library
cd /tmp
git clone https://github.com/rkirner/initio_lib.git
cd initio_lib
make clean && make compile && make link
sudo make install
sudo ldconfig
```

### Compile & Run

```bash
cd /workspaces/Autonomous-Reliable-Car/line_follower
make clean && make
./line_follower
# Press 'q' to quit
```

## How It Works

### 5-State Line Following Logic

| Left Sensor | Right Sensor | Action |
|---|---|---|
| BLACK | BLACK | Drive straight |
| BLACK | WHITE | Turn left |
| WHITE | BLACK | Turn right |
| WHITE | WHITE | Stop (lost line) |
| OBSTACLE | - | Stop (priority) |

### Sensor API

```c
// Line sensors (0 = white, 1 = black)
initio_IrLineLeft();
initio_IrLineRight();

// Obstacle sensors (0 = clear, 1 = obstacle)
initio_IrLeft();
initio_IrRight();
```

### Motor Control

```c
initio_DriveForward(speed);    // 0-100
initio_SpinLeft(speed);        // Turn left
initio_SpinRight(speed);       // Turn right
initio_Init();                 // Initialize
initio_Cleanup();              // Cleanup
```

## Configuration

### Speed Settings

```c
#define FORWARD_SPEED 60  // Straight line speed
#define SPIN_SPEED 70     // Turn speed
```

**Tuning:**
- Too fast? Decrease `FORWARD_SPEED` (try 40-50)
- Drifting off line? Increase `SPIN_SPEED` (try 75-85)
- Turns too aggressive? Decrease `SPIN_SPEED` (try 60-65)

### Sensor Calibration

```c
#define WHITE(a) ((a)==0)  // Returns true when on white
#define BLACK(a) (!WHITE(a))
```

If sensors read inverted values, change to: `#define WHITE(a) ((a)!=0)`

## Program Display

Real-time output shows:
- **Line 3**: Current action (STOP, STRAIGHT, TURN LEFT, TURN RIGHT, LOST LINE)
- **Line 5**: Line sensor values and detection results
- **Line 6**: Obstacle sensor values and status

## Implementation Details

### Priority-Based Control

Obstacle detection takes priority over line following:

```c
if (obstacle_detected) {
    STOP;
} else if (both_on_line) {
    DRIVE_STRAIGHT;
} else if (left_on_right_off) {
    TURN_LEFT;
} else if (right_on_left_off) {
    TURN_RIGHT;
} else {
    STOP;  // Lost line
}
```

### Features

✅ Real-time sensor diagnostics  
✅ Graceful shutdown (press 'q')  
✅ Obstacle priority handling  
✅ Continuous line following  
✅ Terminal display with ncurses  

## Files

- `line_follower/line_follower.c` - Main program (all TODOs implemented)
- `line_follower/Makefile` - Build configuration
- `testIR/testIR.c` - IR sensor test program
- `testIR/Makefile` - Test build configuration

## Testing

1. Place car on black line with white borders
2. Run: `./line_follower`
3. Observe display for sensor values
4. Test obstacle avoidance by placing object in front
5. Verify car resumes when obstacle is removed

## Troubleshooting

| Problem | Solution |
|---|---|
| `cannot find -linitio` | Run `sudo ldconfig` and recompile |
| `initio.h not found` | Re-run library installation steps |
| Program won't start | Ensure you're in `line_follower` directory |
| Wrong sensor values | Check WHITE/BLACK macro calibration |
| Car too fast/slow | Adjust FORWARD_SPEED macro |
| Turns not sharp | Adjust SPIN_SPEED macro |

## Requirements (CW1)

✅ Autonomous line following  
✅ Obstacle detection and avoidance  
✅ Real-time sensor monitoring  
✅ Clean code with comments  
✅ Graceful error handling  

## Running from GitHub Codespaces to Remote Raspberry Pi

If your Raspberry Pi is on a different network, use SSH to execute commands remotely.

### Setup (One-Time)

#### 1. Configure SSH on Raspberry Pi
```bash
# On your Raspberry Pi, enable SSH:
sudo raspi-config
# Navigate to: Interface Options → SSH → Enable
# Then reboot: sudo reboot
```

#### 2. Find Your Raspberry Pi's IP Address
```bash
# On Raspberry Pi terminal:
hostname -I
# Note the IP address (e.g., 192.168.1.50 or 10.0.1.2)
```

#### 3. Add SSH Key to GitHub Codespaces (Optional but recommended)
```bash
# Generate SSH key in Codespaces (if you don't have one):
ssh-keygen -t rsa -b 4096 -f ~/.ssh/id_rsa -N ""

# Copy public key to Raspberry Pi:
ssh-copy-id -i ~/.ssh/id_rsa.pub e300user@<RASPBERRY_PI_IP>
# Example: ssh-copy-id -i ~/.ssh/id_rsa.pub e300user@192.168.1.50
```

### Installation on Remote Raspberry Pi

#### From GitHub Codespaces, run:

```bash
# Connect and install dependencies
ssh e300user@<RASPBERRY_PI_IP> << 'EOF'
sudo apt-get update
sudo apt-get install -y build-essential git libncurses-dev libwiringpi-dev wiringpi pkgconfig

# Clone and install initio library
cd /tmp
git clone https://github.com/rkirner/initio_lib.git
cd initio_lib
make clean && make compile && make link
sudo make install
sudo ldconfig

echo "✅ Raspberry Pi setup complete!"
EOF
```

**Replace `<RASPBERRY_PI_IP>` with your actual IP** (e.g., `192.168.1.50`)

### Compile Code on Remote Raspberry Pi

```bash
# Copy your code to Raspberry Pi
scp -r /workspaces/Autonomous-Reliable-Car/line_follower e300user@<RASPBERRY_PI_IP>:/home/e300user/

# Compile on Raspberry Pi
ssh e300user@<RASPBERRY_PI_IP> << 'EOF'
cd ~/line_follower
make clean && make
echo "✅ Compilation complete!"
EOF
```

### Run Program on Remote Raspberry Pi

#### Option A: Run and view output in real-time

```bash
ssh -t e300user@<RASPBERRY_PI_IP> << 'EOF'
cd ~/line_follower
sudo ./line_follower
EOF
# Press Ctrl+C to stop
```

#### Option B: Run in background and SSH back for control

```bash
# Start program in background
ssh e300user@<RASPBERRY_PI_IP> 'cd ~/line_follower && nohup sudo ./line_follower > output.log 2>&1 &'

# View output in real-time
ssh e300user@<RASPBERRY_PI_IP> 'tail -f ~/line_follower/output.log'

# Stop the program
ssh e300user@<RASPBERRY_PI_IP> 'sudo pkill -f line_follower'
```

#### Option C: SSH tunnel (if ncurses display needed)

```bash
ssh -X e300user@<RASPBERRY_PI_IP> << 'EOF'
cd ~/line_follower
sudo ./line_follower
EOF
```

### Quick Command Reference

```bash
# ALL-IN-ONE: Setup, compile, and run
RASPI_IP="192.168.1.50"  # Change this to your IP
USERNAME="e300user"      # Change if different username

# 1. Setup
ssh $USERNAME@$RASPI_IP << 'SETUP'
sudo apt-get update && sudo apt-get install -y build-essential git libncurses-dev libwiringpi-dev wiringpi pkgconfig
cd /tmp && git clone https://github.com/rkirner/initio_lib.git && cd initio_lib
make clean && make compile && make link && sudo make install && sudo ldconfig
SETUP

# 2. Copy code
scp -r /workspaces/Autonomous-Reliable-Car/line_follower $USERNAME@$RASPI_IP:~/

# 3. Compile
ssh $USERNAME@$RASPI_IP "cd ~/line_follower && make clean && make"

# 4. Run
ssh -t $USERNAME@$RASPI_IP "cd ~/line_follower && sudo ./line_follower"
```

### Troubleshooting Remote SSH

| Problem | Solution |
|---|---|
| `Connection refused` | SSH not enabled on Pi. Run `sudo raspi-config` |
| `Permission denied (publickey)` | Copy SSH key: `ssh-copy-id -i ~/.ssh/id_rsa.pub user@ip` |
| `Could not resolve hostname` | Check IP: `hostname -I` on Raspberry Pi |
| `sudo: no password entry` | Run with: `ssh -t user@ip 'sudo command'` (note the `-t`) |
| `Cannot access GPIO` | Run with `sudo`: prefix commands or add user to gpio group |

### Typical Workflow

```bash
# 1. Edit code in Codespaces
# 2. Push to Git (optional)
# 3. Deploy to Raspberry Pi:
scp -r line_follower e300user@192.168.1.50:~/
ssh -t e300user@192.168.1.50 'cd line_follower && make clean && make && sudo ./line_follower'
```

### Environment Variables

If you SSH frequently, set these in your `.bashrc` or `.zshrc` in Codespaces:

```bash
export RASPI_IP="192.168.1.50"
export RASPI_USER="e300user"

# Then use:
ssh -t $RASPI_USER@$RASPI_IP 'cd ~/line_follower && sudo ./line_follower'
```

## License

Part of 4COM2009 RTCS Coursework 1

## References

- [initio_lib GitHub](https://github.com/rkirner/initio_lib)
- 4tronix Initio Robot Car
- Raspberry Pi GPIO Control
- SSH Remote Execution