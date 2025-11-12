#pragma region VEXcode Generated Robot Configuration
// Make sure all required headers are included.
#include "vex.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "Odometry.h"


using namespace vex;

brain Brain;
controller Controller;
motor MotorLeft(10,false);
motor MotorRight(6, true);


class UserBackup{
private:
    static const int MAX_USERS = 100;
    int users[MAX_USERS][2];
    float userLocation[MAX_USERS][2];
    int totalUsers;
    int cardsPerPlayer;

    Odometry &odo;

public:
    UserBackup(Odometry &o) : odo(o) {
        totalUsers = 0;
        cardsPerPlayer = 0;
        for (int i = 0; i < MAX_USERS; i++) {
            users[i][0] = i + 1;     
            users[i][1] = 0;        
            userLocation[i][0] = 0;  
            userLocation[i][1] = 0;  
        }
    }

    void controllerSetup() {
        int players = 1;
        int cards = 1;
        bool playerConfirmed = false;
        bool cardConfirmed = false;

        while (!playerConfirmed) {
            Brain.Screen.clearScreen();
            Brain.Screen.print("Players: %d", players);
            Brain.Screen.newLine();
            Brain.Screen.print("EUp:+  FDown:-");
            Brain.Screen.newLine();
            Brain.Screen.print("RUp = confirm");

            if (Controller.ButtonEUp.pressing()) {
                while (Controller.ButtonEUp.pressing()) wait(10, msec);
                while (!Controller.ButtonEUp.pressing()) wait(10, msec);
                players++;
                Brain.Screen.clearLine();
                Brain.Screen.print("Players: %d", players);
            }

            if (Controller.ButtonFDown.pressing()) {
                while (Controller.ButtonFDown.pressing()) wait(10, msec);
                while (!Controller.ButtonFDown.pressing()) wait(10, msec);
                players--;
                Brain.Screen.clearLine();
                Brain.Screen.print("Players: %d", players);
            }

            if (Controller.ButtonRUp.pressing()) {
                while (Controller.ButtonRUp.pressing()) wait(10, msec);
                Brain.Screen.clearScreen();
                Brain.Screen.print("Players confirmed: %d", players);
                wait(1000, msec);
                playerConfirmed = true;  
            }

            wait(20, msec);
        }


        while (!cardConfirmed) {
            Brain.Screen.clearScreen();
            Brain.Screen.print("Cards per Player: %d", cards);
            Brain.Screen.newLine();
            Brain.Screen.print("EUp:+  FDown:-");
            Brain.Screen.newLine();
            Brain.Screen.print("RUp = confirm");

            if (Controller.ButtonEUp.pressing()) {
                while (Controller.ButtonEUp.pressing()) wait(10, msec);
                while (!Controller.ButtonEUp.pressing()) wait(10, msec);
                cards++;
                Brain.Screen.clearLine();
                Brain.Screen.print("Cards per Player: %d", cards);
            }

            if (Controller.ButtonFDown.pressing()) {
                while (Controller.ButtonFDown.pressing()) wait(10, msec);
                while (!Controller.ButtonFDown.pressing()) wait(10, msec);
                cards--;
                Brain.Screen.clearLine();
                Brain.Screen.print("Cards per Player: %d", cards);
            }

            if (Controller.ButtonRUp.pressing()) {
                while (Controller.ButtonRUp.pressing()) wait(10, msec);
                Brain.Screen.clearScreen();
                Brain.Screen.print("Cards confirmed: %d", cards);
                wait(1000, msec);
                cardConfirmed = true;  
            }

            wait(20, msec);
        }

        initializeUsers(players, cards);
        Brain.Screen.clearScreen();
        Brain.Screen.print("Setup complete!");
    }

    



    void initializeUsers(int players, int cards) {
        totalUsers = players;
        cardsPerPlayer = cards;

        for (int i = 0; i < totalUsers; i++) {
            users[i][1] = cardsPerPlayer;   
        }
    }


    void recordAllPlayerLocations() {
        Brain.Screen.clearScreen();
        Brain.Screen.print("Drive to player");
        Brain.Screen.newLine();
        Brain.Screen.print("FUp = save pos");

        for (int i = 0; i < totalUsers; i++) {
            Brain.Screen.clearScreen();
            Brain.Screen.print("Player %d", i + 1);
            Brain.Screen.newLine();
            Brain.Screen.print("Drive & press FUp to save");

            bool saved = false; 

            while (!saved) {
                double moveInput = Controller.AxisA.position();
                double turnInput = Controller.AxisC.position();

                double leftSpeed  = moveInput + turnInput;
                double rightSpeed = moveInput - turnInput;

                if (leftSpeed > 100) leftSpeed = 100;
                if (leftSpeed < -100) leftSpeed = -100;
                if (rightSpeed > 100) rightSpeed = 100;
                if (rightSpeed < -100) rightSpeed = -100;

                MotorLeft.setVelocity(leftSpeed, percent);
                MotorRight.setVelocity(rightSpeed, percent);
                MotorLeft.spin(forward);
                MotorRight.spin(forward);

                if (Controller.ButtonFUp.pressing()) {
                    MotorLeft.stop();
                    MotorRight.stop();

                    saveCurrentLocation(i);

                    Brain.Screen.clearLine();
                    Brain.Screen.print("Saved P%d (%.1f, %.1f)",
                        i + 1,
                        userLocation[i][0],
                        userLocation[i][1]);
                        wait(700, msec);

                    saved = true; 
                }

                wait(20, msec);
            }
        }

        Brain.Screen.clearScreen();
        Brain.Screen.print("All positions saved");
        wait(800, msec);
    }

        


    void saveCurrentLocation(int idx) {
        float turns = odo.trackingCenter();
        float dist = turns * 20.0f;   
        float headingDeg = BrainInertial.heading(degrees);
        float headingRad = headingDeg * (M_PI / 180.0f);

        userLocation[idx][0] = dist * cos(headingRad);  
        userLocation[idx][1] = dist * sin(headingRad);  
    }

    void cardGivenTo(int userID) {
        // userID is 1-based
        int idx = userID - 1;
        if (idx >= 0 && idx < totalUsers) {
            if (users[idx][1] > 0) {
                users[idx][1]--;
            }
        }
    }

    int getTotalUsers() {
        return totalUsers;
    }

    int getCardsLeft(int idx) {
        if (idx >= 0 && idx < totalUsers) return users[idx][1];
        return 0;
    }

    float getUserX(int idx) {
        if (idx >= 0 && idx < totalUsers) return userLocation[idx][0];
        return 0;
    }

    float getUserY(int idx) {
        if (idx >= 0 && idx < totalUsers) return userLocation[idx][1];
        return 0;
    }

};
