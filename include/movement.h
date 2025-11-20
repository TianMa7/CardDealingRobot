#ifndef MOVEMENT_H
#define MOVEMENT_H

// includes:
#include "vex.h"

using namespace vex;

class Movement
{

public:
  Movement(
      motor &Left,
      motor &Right,
      inertial &IMU)
      : MotorLeft(Left),
        MotorRight(Right),
        BrainInertial(IMU)
  {
    location[0] = 0;
    location[1] = 1;
  }
  // Add public methods for robot movement and odometry here
  float getX() const
  {
    return location[0];
  }

  float getY() const
  {
    return location[1];
  }

  float getHeading() const
  {
    return 360 - BrainInertial.heading();
  }

  void moveTo(float x, float y, float finalAngle, float speed);

  void driveStraight(double rightMotorSpeed, double leftMotorSpeed);

  void spinToDegree(double motorSpeed, double angle);

  void locationUpdate();

  void stopMotors()
  {
    MotorLeft.stop();
    MotorRight.stop();
  }

private:
  motor &MotorLeft;
  motor &MotorRight;
  inertial &BrainInertial;

  double location[2] = {0, 0};            // x, y
  const double wheelCircumference = 20.0; // cm
  const double wheelBase = 14.5;          // cm

  float arcRatio(int radius) const
  {
    return (1 - (wheelBase / (2 * radius))) / (1 + (wheelBase / (2 * radius)));
  }

  float findTangent(float centerX, float centerY, float radius) const
  {
    float angleCenter = atan2f(centerY - location[1], centerX - location[0]) * 180 / M_PI;
    float angleOffset = atan2f(radius, hypot(centerY - location[1], centerX - location[0])) * 180 / M_PI;

    if (angleCenter < 0)
    {
      angleCenter += 360;
    }

    return fmod(angleCenter + angleOffset, 360);
  }

  // MATH EXPLAINED BY CHATGPT I COULDNT THINK CLEAR ENOUGH
  float normalizeAngle(float targetAngle, float cwLimit) const
  {
    float robotAngle = 360 - BrainInertial.heading();
    float angleDiff = targetAngle - robotAngle;

    // if the robot angle is larger than target, value is negative
    // Negative values mean that we must rotate clockwise to reach target
    // Positive values means that we must rotate ccw to reach target
    // the magnitude of angleDiff is the amount of degrees needed to rotate
    if (angleDiff < cwLimit * (-1))
    {
      return angleDiff + 360;
    }
    if (angleDiff > 360 - cwLimit)
    {
      return angleDiff - 360;
    }
    return angleDiff;
  }

  bool finalAngleCheck(float finalAngle, float error) const
  {
    float robotAngle = 360 - BrainInertial.heading();

    float angle = fabs(finalAngle - robotAngle);

    if (angle < error || 360 - angle < error)
    {
      return false;
    }
    else
    {
      return true;
    }
  }

  float smooth(float difference)
  {
      // Simple proportional controller
    float a = 60.0;
    float b = 5.0;
    float percentReturn = (100*difference + (a*b))/(100*(difference + a));

    return percentReturn;

    //(100x + ab)/(100(x + a))
  }

  float accelerate(float currentSpeed, float targetSpeed)
  {
    int interval = 5;
    if(currentSpeed >= targetSpeed)
    {
      return targetSpeed;
    }
    else
    {
      return currentSpeed += 1;
    }
  }
};

#endif