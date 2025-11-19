# Autonomous-Reliable-Car

Autonomous line following robot with obstacle avoidance for the 4tronix Initio robot car.

## Overview

This project implements a line follower with obstacle avoidance using a 5-state finite state machine. The car autonomously follows a black line while detecting and avoiding obstacles in real-time.

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

## License

Part of 4COM2009 RTCS Coursework 1

## References

- [initio_lib GitHub](https://github.com/rkirner/initio_lib)
- 4tronix Initio Robot Car
- Raspberry Pi GPIO Control