#include "vex.h"
#include "UserBackup.h"
#include "Odometry.h"
#include "Distribution.h"
#pragma region VEXcode Generated Robot Configuration

using namespace vex;

// robot hardware

touchled TouchLED(1);
colorsensor Colour(11);
brain Brain;

class EndProgram : public Distribution
{
public:
    // Constructor that passes Odometry reference to the parent (Distribution)
    EndProgram(Odometry &o) : Distribution(o) {}

    void ShutDown()
    {
        while (TouchLED.pressing())
        {
        };
        while (!TouchLED.pressing())
        {
        };

        if (!Colour.isNearObject())
        {
            Brain.Screen.clearScreen();
            Brain.Screen.print("No card left in the deck\nNow shutting down\n");
            wait(2, seconds);
            Brain.programStop();
        }
        else
        {
            Brain.Screen.clearScreen();
            Brain.Screen.print("%d cards are left in the deck\n", countRemainingCards());
            wait(2, seconds);
        }
    }
};
