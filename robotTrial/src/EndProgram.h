#pragma once
#include "vex.h"
#include "UserBackup.h"
#include "Odometry.h"
#include "Distribution.h"

using namespace vex;

// robot hardware

touchled TouchLED(1);
colorsensor Colour(11);
brain Brain;
class EndProgram : public Distribution {
public:
    EndProgram(Odometry &o) : Distribution(o) {}

    // Waits for a TouchLED press (no break allowed)
    void waitForTouch() {
        while (TouchLED.pressing()){};
        while (!TouchLED.pressing()){};
    }

    // Left button = Yes, Right button = No
    int askYesNo(const char *message) {
        bool answered = false;
        int result = 0;

        Brain.Screen.clearScreen();
        Brain.Screen.print(message);
        Brain.Screen.newLine();
        Brain.Screen.print("Left = Yes");
        Brain.Screen.newLine();
        Brain.Screen.print("Right = No");

        while (!answered) {
            if (Brain.buttonLeft.pressing()) {
                while (Brain.buttonLeft.pressing());
                result = 1; // yes
                answered = true;
            }
            if (Brain.buttonRight.pressing()) {
                while (Brain.buttonRight.pressing());
                result = 0; // no
                answered = true;
            }
        }
        return result;
    }

    // Main shutdown handler
    bool handleEnd() {
        Brain.Screen.clearScreen();
        Brain.Screen.print("Touch LED to finish");

        waitForTouch();

        if (Colour.isNearObject()) {
            Brain.Screen.clearScreen();
            Brain.Screen.print("Remaining cards: ");
            Brain.Screen.print("%d", countRemainingCards());

            int repeat = askYesNo("Repeat process?");
            if (repeat == 1) {
                return true;   
            } else {
                int shutdown = askYesNo("Shut down robot?");
                if (shutdown == 1) {
                    Brain.Screen.clearScreen();
                    Brain.Screen.print("Shutting down...");
                    wait(1500, msec);
                    Brain.programStop();
                }
                return false; 
            }
        }

        Brain.Screen.clearScreen();
        Brain.Screen.print("No cards left!");
        Brain.Screen.newLine();
        Brain.Screen.print("Shutting down...");
        wait(1500, msec);
        Brain.programStop();
        return false;
    }
};
