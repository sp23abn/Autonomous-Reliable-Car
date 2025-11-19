//======================================================================
//
// Line Follower program with obstacle avoidance for the
// 4tronix initio robot car. Follows a black line while avoiding obstacles.
//
// author: Raimund Kirner, University of Hertfordshire
//         initial version: Oct.2016
//         completed version for CW1 assignment
//
// license: GNU LESSER GENERAL PUBLIC LICENSE
//          Version 2.1, February 1999
//          (for details see LICENSE file)
//
// Compilation: 
// gcc -o line_follower -Wall -Werror -lcurses -lwiringPi -lpthread -linitio line_follower.c
//
// Description:
// The car will drive forward while following a black line. It will:
// 1. Stop when an obstacle is detected (IR sensors)
// 2. Resume when the path is clear
// 3. Turn left/right to stay on the black line
// 4. Move straight when both line sensors detect the line
//
//======================================================================

#include <stdlib.h>
#include <initio.h>
#include <curses.h>

// Calibration macro: Adjust these based on your specific car's line sensors
// Line sensors return boolean: TRUE (1) when line detected, FALSE (0) when on white
// The standard calibration is: 0 = white/background, 1 = black/line
#define WHITE(a) ((a)==0)  // Returns TRUE if reading is white (line sensor not triggered)
#define BLACK(a) (!WHITE(a))  // Returns TRUE if reading is black (line sensor triggered)

// Motor speed settings (0-100)
#define FORWARD_SPEED 60      // Speed for straight line following
#define SPIN_SPEED 70         // Speed for turning (higher to overcome friction)
#define TURN_SPEED 50         // Speed for gentle turns

//======================================================================
// line_follower():
// Main line-following logic with obstacle avoidance:
// - Detects obstacles using IR sensors and stops if needed
// - Follows a black line using left/right line sensors
// - Performs corrective turns to maintain line position
// - Resumes after obstacle clears
//======================================================================
void line_follower(int argc, char *argv[]) 
{
  int ch = 0;
  int irL, irR;           // IR obstacle sensors (left, right)
  int lfL, lfR;           // Line sensors (left, right)
  int display_count = 0;  // Counter for display updates

  // Start driving forward
  initio_DriveForward(FORWARD_SPEED);

  while (ch != 'q') {
    mvprintw(1, 1, "%s: Press 'q' to end program", argv[0]);

    // Read all sensors
    irL = initio_IrLeft();
    irR = initio_IrRight();
    lfL = initio_IrLineLeft();   // Read left line sensor
    lfR = initio_IrLineRight();  // Read right line sensor

    // PRIORITY 1: Handle obstacle detection - Stop if any IR sensor triggered
    if (irL != 0 || irR != 0) {
      mvprintw(3, 1, "ACTION 1: STOP - Obstacle detected! (IR: L=%d, R=%d)      ", irL, irR);
      initio_DriveForward(0); // Stop completely
    }
    // PRIORITY 2: No obstacle - Focus on line following
    else if (BLACK(lfL) && BLACK(lfR)) {
      // Both sensors on line - drive straight forward
      mvprintw(3, 1, "ACTION 2: STRAIGHT - Both sensors on line (L=%d, R=%d)    ", lfL, lfR);
      initio_DriveForward(FORWARD_SPEED);
    }
    else if (BLACK(lfL) && WHITE(lfR)) {
      // Left sensor on line, right sensor on white - car too far right, turn left
      mvprintw(3, 1, "ACTION 3: TURN LEFT - Line on left only (L=%d, R=%d)      ", lfL, lfR);
      initio_SpinLeft(SPIN_SPEED);
    }
    else if (WHITE(lfL) && BLACK(lfR)) {
      // Right sensor on line, left sensor on white - car too far left, turn right
      mvprintw(3, 1, "ACTION 4: TURN RIGHT - Line on right only (L=%d, R=%d)     ", lfL, lfR);
      initio_SpinRight(SPIN_SPEED);
    }
    else {
      // Both sensors white - lost the line completely
      mvprintw(3, 1, "LOST LINE - Both sensors on white (L=%d, R=%d)           ", lfL, lfR);
      initio_DriveForward(0); // Stop and wait
    }

    // Handle keyboard input
    ch = getch();
    if (ch != ERR) {
      mvprintw(2, 1, "Key pressed: '%c' (ASCII %d) - Press 'q' to quit         ", ch, ch);
    }

    // Display sensor calibration info periodically (for debugging/calibration)
    if (++display_count % 5 == 0) {
      mvprintw(5, 1, "LINE SENSOR VALUES: Left=%d (BLACK=%d)  Right=%d (BLACK=%d)", 
               lfL, BLACK(lfL), lfR, BLACK(lfR));
      mvprintw(6, 1, "IR OBSTACLE SENSORS: Left=%d  Right=%d (0=clear, 1=obstacle) ", irL, irR);
    }

    refresh();  // Update curses display
  } // while

  // Stop the car when exiting
  initio_DriveForward(0);
  return;
}


//======================================================================
// main(): initialisation of libraries, etc
//======================================================================
int main (int argc, char *argv[])
{
  WINDOW *mainwin = initscr();  // curses: init screen
  noecho ();                    // curses: prevent the key being echoed
  cbreak();                     // curses: disable line buffering 
  nodelay(mainwin, TRUE);       // curses: set getch() as non-blocking 
  keypad (mainwin, TRUE);       // curses: enable detection of cursor and other keys

  initio_Init (); // initio: init the library

  line_follower(argc, argv);

  initio_Cleanup ();  // initio: cleanup the library (reset robot car)
  endwin();           // curses: cleanup the library
  return EXIT_SUCCESS;
}

