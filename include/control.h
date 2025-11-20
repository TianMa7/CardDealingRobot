#ifndef CONTROL_H
#define CONTROL_H

// includes:
#include "vex.h"
#include "movement.h"
#include "userInfo.h"

using namespace vex;

// start of class
class Control
{

public:
    // constructor
    Control(UserInfo &u, Movement &m, brain &br, controller &control, touchled &touch) : user(u), move(m), Brain(br), controller(control), TouchLED(touch)
    {
    }

    // public functions here
    bool controllerSetup(UserInfo &userManager);

    void recordAllPlayerLocations(Movement &move, UserInfo &userManager);

    void resetScreen()
    {
        Brain.Screen.clearScreen();
        Brain.Screen.setCursor(1, 1);
    }

    void resetLine(int i)
    {
        Brain.Screen.setCursor(i, 1);
        Brain.Screen.clearLine();
    }


    bool endProgram(int &restartMode);
private:
    UserInfo &user;
    Movement &move;
    brain &Brain;
    controller &controller;
    touchled &TouchLED;

    // private functions here
    int askYesNo();

    float deadband(float num) const
    {
        if (abs(num) < 0.1)
        {
            return 0;
        }
        else
        {
            return num;
        }
    }
};

#endif
