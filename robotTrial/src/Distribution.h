#include "vex.h"
#include "UserBackup.h"
#include "Odometry.h"

using namespace vex;


motor MotorOutput(11);
touchled TouchLED(1);

class Distribution : public UserBackup, public Odometry {
private:
    float targetLocation[2] = {0, 0};
    float home[2] = {0, 0};
    bool allDone = false;

public:
    Distribution(Odometry &o) : UserBackup(o) {}


    bool playerHasCards(int playerIndex) {
        return getCardsLeft(playerIndex) > 0;
    }

    int randomPlayer() {
        int total = getTotalUsers();
        int pick = -1;
        bool found = false;

        while (!found && !allDone) {
            pick = rand() % total;
            if (playerHasCards(pick)) {
                found = true;
            } else {
                bool someoneHasCard = false;
                for (int i = 0; i < total; i++) {
                    if (playerHasCards(i)) {
                        someoneHasCard = true;
                    }
                }
                if (!someoneHasCard) {
                    allDone = true;
                    pick = -1;
                }
            }
        }
        return pick;
    }


    float calculateDistance(float targetX, float targetY) {
        float currentX = getX();
        float currentY = getY();

        float deltaX = targetX - currentX;
        float deltaY = targetY - currentY;
        float distance = sqrt((deltaX * deltaX) + (deltaY * deltaY));
        return distance;
    }


    void turnToAngle(float deltaAngle) {
        float initial = BrainInertial.heading(degrees);
        bool turning = true;

        if (deltaAngle > 0) {
            MotorLeft.setVelocity(30, percent);
            MotorRight.setVelocity(-30, percent);
            MotorLeft.spin(forward);
            MotorRight.spin(forward);
            while (turning) {
                if (BrainInertial.heading(degrees) >= initial + deltaAngle) {
                    turning = false;
                }
                wait(10, msec);
            }
        } else {
            MotorLeft.setVelocity(-30, percent);
            MotorRight.setVelocity(30, percent);
            MotorLeft.spin(forward);
            MotorRight.spin(forward);
            while (turning) {
                if (BrainInertial.heading(degrees) <= initial + deltaAngle) {
                    turning = false;
                }
                wait(10, msec);
            }
        }

        MotorLeft.stop();
        MotorRight.stop();
    }


    void driveStraight(float cm) {
        const float wheelCircumference = 20.0; 
        float turnsNeeded = cm / wheelCircumference;
        MotorLeft.setPosition(0, turns);
        MotorRight.setPosition(0, turns);

        MotorLeft.setVelocity(50, percent);
        MotorRight.setVelocity(50, percent);
        MotorLeft.spin(forward);
        MotorRight.spin(forward);

        bool moving = true;
        while (moving) {
            float avgTurns = (MotorLeft.position(turns) + MotorRight.position(turns)) / 2.0;
            if (avgTurns >= turnsNeeded) {
                moving = false;
            }
            wait(10, msec);
        }

        MotorLeft.stop();
        MotorRight.stop();
    }


    void driveTo(float x, float y, float finalHeading) {
        float currentHeading = BrainInertial.heading(degrees);
        float deltaX = x - getX();
        float deltaY = y - getY();
        float targetAngle = atan2(deltaY, deltaX) * 180.0 / M_PI;
        float angleDiff = targetAngle - currentHeading;


        turnToAngle(angleDiff);


        float distance = sqrt(deltaX * deltaX + deltaY * deltaY);
        driveStraight(distance);


        float correction = finalHeading - BrainInertial.heading(degrees);
        turnToAngle(correction);
    }


    void ejectCard() {
        MotorOutput.setPosition(0, turns);
        MotorOutput.setVelocity(50, percent);

        MotorOutput.spin(reverse);
        while (fabs(MotorOutput.position(turns) * 200) < 95) {
            wait(10, msec);
        }

        MotorOutput.spin(forward);
        wait(500, msec);
        MotorOutput.stop();
        MotorOutput.setPosition(0, degrees);
    }

    void startDistribution() {
        allDone = false;

        Brain.Screen.clearScreen();
        Brain.Screen.setCursor(1, 1);
        Brain.Screen.print("Waiting for touch LED...");

        while (TouchLED.pressing()) wait(20, msec);
        while (!TouchLED.pressing()) wait(20, msec);

        Brain.Screen.clearScreen();
        Brain.Screen.print("Distribution started");

        bool running = true;
        while (running) {
            int playerIndex = randomPlayer();

            if (allDone || playerIndex == -1) {
                running = false;
            } else {
                float px = getUserX(playerIndex);
                float py = getUserY(playerIndex);

                Brain.Screen.clearScreen();
                Brain.Screen.setCursor(1, 1);
                Brain.Screen.print("Driving to P%d", playerIndex + 1);

                driveTo(px, py, BrainInertial.heading(degrees) + 90);


                if (calculateDistance(px, py) <= 1.0) {
                    Brain.Screen.newLine();
                    Brain.Screen.print("Ejecting...");
                    ejectCard();
                    cardGivenTo(playerIndex + 1);
                }

                Brain.Screen.newLine();
                Brain.Screen.print("Remaining cards: %d", countRemainingCards());
            }
        }
        
        Brain.Screen.newLine();
        Brain.Screen.print("Returning home...");
        driveTo(home[0], home[1], 0);
        wait(500, msec);
        Brain.Screen.clearScreen();
        Brain.Screen.print("Cards delivered");
    }
    int countRemainingCards() {
        int total = getTotalUsers();
        int remaining = 0;

        for (int i = 0; i < total; i++) {
            remaining += getCardsLeft(i);
        }

        return remaining;
    }


    float getX(){ 
        return 0; 
    }
    float getY(){ 
        return 0; 
    }
};
