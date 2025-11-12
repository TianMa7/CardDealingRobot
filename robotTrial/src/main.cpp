#include "vex.h"
#include "Odometry.h"
#include "UserBackup.h"
#include "Distribution.h"
#include "EndProgram.h"

using namespace vex;

void initializeRandomSeed(){
  wait(100,msec);
  double xAxis = BrainInertial.acceleration(xaxis) * 1000;
  double yAxis = BrainInertial.acceleration(yaxis) * 1000;
  double zAxis = BrainInertial.acceleration(zaxis) * 1000;
  // Combine these values into a single integer
  int seed = int(
    xAxis + yAxis + zAxis
  );
  // Set the seed
  srand(seed); 
}

// Converts a color to a string
const char* convertColorToString(color col) {
  if (col == colorType::red) return "red";
  else if (col == colorType::green) return "green";
  else if (col == colorType::blue) return "blue";
  else if (col == colorType::white) return "white";
  else if (col == colorType::yellow) return "yellow";
  else if (col == colorType::orange) return "orange";
  else if (col == colorType::purple) return "purple";
  else if (col == colorType::cyan) return "cyan";
  else if (col == colorType::black) return "black";
  else if (col == colorType::transparent) return "transparent";
  else if (col == colorType::red_violet) return "red_violet";
  else if (col == colorType::violet) return "violet";
  else if (col == colorType::blue_violet) return "blue_violet";
  else if (col == colorType::blue_green) return "blue_green";
  else if (col == colorType::yellow_green) return "yellow_green";
  else if (col == colorType::yellow_orange) return "yellow_orange";
  else if (col == colorType::red_orange) return "red_orange";
  else if (col == colorType::none) return "none";
  else return "unknown";
}

void vexcodeInit() {
  // Initializing random seed.
  initializeRandomSeed(); 
}
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
