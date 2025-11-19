#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H

// includes:
#include "vex.h"
#include "movement.h"
#include "userInfo.h"
#include "control.h"

using namespace vex;

class Distribution
{
private:
    float targetLocation[2] = {0, 0};
    bool allDone = false;
    Movement &move;
    UserInfo &user;
    Control &control;
    motor &MotorOutput;
    touchled &TouchLED;
    brain &Brain;
    optical &Optical;

    //private functions
    void randomPlayer(int players[], int size);

    void ejectCard();

    bool playerHasCards(int playerIndex)
    {
        return user.getCardsLeft(playerIndex) > 0;
    }

    int countRemainingCards();
public:
    Distribution(Movement &m, UserInfo &userBackup, Control &c, motor &roller, touchled &tLed, optical &colour, brain &b) : move(m), user(userBackup), control(c), MotorOutput(roller), TouchLED(tLed), Optical(colour), Brain(b)
    {
    }


    // bool isBlueCard()
    // {

    //     if (Optical.hue() < 280 && Optical.hue() > 250)
    //     {
    //         Brain.Screen.print("blue");
    //         wait(500, msec);
    //         return true;
    //     }
    //     return false;
    // }
    void locDistribution();

    void spinDistribution();


    // void sortCards()
    // {
    //     bool sorting = true;
    //     float angle1 = 100;
    //     float angle2 = 200;
    //     while (sorting)
    //     {
    //         if (isBlueCard())
    //         {
    //             Drive.spinToDegree(50, angle1);
    //         }
    //         else
    //         {
    //             Drive.spinToDegree(50, angle2);
    //         }
    //         ejectCard();
    //         if (!Optical.isNearObject())
    //         {
    //             sorting = false;
    //         }
    //     }
    // }
};

#endif