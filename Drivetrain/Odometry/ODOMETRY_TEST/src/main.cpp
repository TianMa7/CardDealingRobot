#pragma region VEXcode Generated Robot Configuration
// Make sure all required headers are included.
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>


#include "vex.h"

using namespace vex;

// Brain should be defined by default
brain Brain;


// START IQ MACROS
#define waitUntil(condition)                                                   \
  do {                                                                         \
    wait(5, msec);                                                             \
  } while (!(condition))

#define repeat(iterations)                                                     \
  for (int iterator = 0; iterator < iterations; iterator++)
// END IQ MACROS


// Robot configuration code.
inertial BrainInertial = inertial();
motor MotorLeft = motor(PORT11, false);
motor MotorRight = motor(PORT8, true);

// generating and setting random seed
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


// Convert colorType to string
const char* convertColorToString(colorType col) {
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

#pragma endregion VEXcode Generated Robot Configuration

//----------------------------------------------------------------------------
//                                                                            
//    Module:       main.cpp                                                  
//    Author:       {Tian Ma}                                                  
//    Created:      {28 October 2025}                                                    
//    Description:  Odometry test code for card dealer                                              
//                                                                            
//----------------------------------------------------------------------------

// Include the IQ Library
#include "iq_cpp.h"

// Allows for easier use of the VEX Library
using namespace vex;

void configureAllSensors();
float trackingCenter();
void locationUpdate();
void driveArc(float radius, float angle, float speed);
void driveStraight(float distance, float speed);
float location[2] = {0,0}; // x,y

int main() {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();
  // Begin project code
  configureAllSensors();
  wait(1000,msec);

  thread locationThread = thread(locationUpdate);

  //drive in a circle with radius 30 cm, 360 degrees at speed 30%
  driveArc(30,-90,20);
  wait(1000,msec);

  printf("Final Location: X: %d cm, Y: %d cm\n", (int)location[0], (int)location[1]);

}

void configureAllSensors(){
  BrainInertial.calibrate();
  wait(2,seconds);
  BrainInertial.setHeading(0,degrees);
  BrainInertial.setRotation(0,degrees);
  MotorLeft.setPosition(0,turns);
  MotorRight.setPosition(0,turns);
  Brain.Screen.clearScreen();
  Brain.Screen.setFont(mono15);

}

float trackingCenter()
{
  float leftDistance = MotorLeft.position(turns);
  float rightDistance = MotorRight.position(turns);
  float centerPosition = (leftDistance + rightDistance) / 2.0;
  return centerPosition;
}

void locationUpdate()
{
  const float wheelCircumference = 20.0; // cm
  float waitTime = 10; //ms
  float lastLeft = MotorLeft.position(turns);
  float lastRight = MotorRight.position(turns);
  // float lastHeading = BrainInertial.heading(degrees);
  float lastCenter = (lastLeft + lastRight) / 2.0;
  
  wait(waitTime, msec); // Initial wait before starting updates
  while(true)
  {
    float currentLeft = MotorLeft.position(turns);
    float currentRight = MotorRight.position(turns);
    float currentHeading = BrainInertial.heading(degrees);
    float currentCenter = (currentLeft + currentRight) / 2.0;
    float deltaCenter = (currentCenter - lastCenter) * wheelCircumference;

    // Update last values for next iteration
    lastLeft = currentLeft;
    lastRight = currentRight;
    lastCenter = currentCenter;

    //tests and flags
    if(deltaCenter > 100 || deltaCenter < -100)
    {
      printf("Warning: Large deltaCenter detected");
    }

    location[0] += deltaCenter * cos(currentHeading * (M_PI / 180.0)); // x
    location[1] += deltaCenter * sin(currentHeading * (M_PI / 180.0)); // y

    wait(waitTime, msec); // Adjust the update rate as needed
  }
}

void driveArc(float radius, float angle, float speed)
{
  float wheelBase = 15.8; // cm, distance between left and right wheels
  float initialRotation = BrainInertial.rotation(degrees);
  float diffSpeeds = (radius - (wheelBase / 2.0))/(radius + (wheelBase / 2.0));
  float inSpeed = speed * diffSpeeds;
  float outSpeed = speed;

  if(angle == 0)
  {
    return;
  }
  else if(angle < 0)
  {
    //turn left
    MotorLeft.setVelocity(inSpeed, percent);
    MotorRight.setVelocity(outSpeed, percent);
    MotorLeft.spin(forward);
    MotorRight.spin(forward);
    while(BrainInertial.rotation(degrees) > initialRotation + angle)
    {
      wait(10,msec);
    }
  }
  else
  {
    //turn right
    MotorLeft.setVelocity(outSpeed, percent);
    MotorRight.setVelocity(inSpeed, percent);
    MotorLeft.spin(forward);
    MotorRight.spin(forward);
    while(BrainInertial.rotation(degrees) < initialRotation + angle)
    {
      wait(10,msec);
    }
  }
  MotorLeft.stop();
  MotorRight.stop();
}