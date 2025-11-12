#pragma region VEXcode Generated Robot Configuration
// Make sure all required headers are included.
#include "vex.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>


using namespace vex;

// Brain should be defined by default
brain Brain;
inertial BrainInertial = inertial();
motor MotorLeft = motor(PORT11, false);
motor MotorRight = motor(PORT8, true);

class Odometry{

    private:
    float location[2] = {0,0}; // x,y

    public:
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
};
