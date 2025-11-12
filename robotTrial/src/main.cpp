#include "vex.h"
#include "Odometry.h"
#include "UserBackup.h"
#include "Distribution.h"
#include "EndProgram.h"

using namespace vex;

brain Brain;

int main() {
    vexcodeInit();
    Odometry odo;


    
    Distribution dist(odo);  
    EndProgram end(odo);

    dist.controllerSetup();
    dist.recordAllPlayerLocations();
    Brain.Screen.clearScreen();
    Brain.Screen.print("Starting card distribution...");
    wait(1, seconds);

    bool running = true;
    while (running) {
        odo.locationUpdate();
        dist.startDistribution();
        running = false;
    }
    end.ShutDown();
    while (true) {
        wait(100, msec);
    }

    return 0;
}
