/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       tianm                                                     */
/*    Created:      11/18/2025, 6:58:11 PM                                    */
/*    Description:  IQ2 project                                               */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#include "vex.h"
#include "movement.h"
#include "userInfo.h"
#include "control.h"
#include "distribution.h"

using namespace vex;

// Define inputs and outputs
motor MotorLeft = motor(PORT11, false);
motor MotorRight = motor(PORT8, true);
motor MotorOutput = motor(PORT10, false);
touchled TouchLED = touchled(PORT9);
optical Optical = optical(PORT7);
brain Brain = brain();
inertial BrainInertial = inertial();
controller Controller = controller();
bumper Bumper = bumper(PORT12);

// define your global instances of motors and other devices here
void initializeRandomSeed()
{
  wait(100, msec);
  double xAxis = BrainInertial.acceleration(xaxis) * 1000;
  double yAxis = BrainInertial.acceleration(yaxis) * 1000;
  double zAxis = BrainInertial.acceleration(zaxis) * 1000;
  // Combine these values into a single integer
  int seed = int(
      xAxis + yAxis + zAxis);
  // Set the seed
  srand(seed);
}

void vexcodeInit()
{
  // Initializing random seed.
  initializeRandomSeed();
}

void configureAllSensors()
{
  BrainInertial.calibrate();
  wait(2, seconds);
  BrainInertial.setHeading(0, degrees);
  BrainInertial.setRotation(0, degrees);
  MotorLeft.setPosition(0, turns);
  MotorRight.setPosition(0, turns);
  Optical.setLight(ledState::on);
  // might want to add a wait here to ensure calibration is done
}

int main()
{
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();
  // Begin project code

  // Initialize robot and objects
  configureAllSensors();

  Movement move = Movement(MotorLeft, MotorRight, BrainInertial);
  UserInfo user = UserInfo(&move);
  Control control = Control(user, move, Brain, Controller, TouchLED);
  Distribution distribution = Distribution(move, user, control, MotorOutput, TouchLED, Optical, Brain);

  bool runProgram = false;

  while (runProgram)
  {
    bool isRigged = false;
    bool locBased = control.controllerSetup(user);
    if (locBased)
    {
      control.recordAllPlayerLocations(move, user);
    }
    control.resetScreen();
    Brain.Screen.print("Starting...");
    while (runProgram)
    {
      if (locBased)
      {
        wait(1, seconds);
        bool running = true;
        distribution.locDistribution();
        running = false;
      }
      else
      {
        wait(1, seconds);
        distribution.spinDistribution();
      }

      runProgram = !control.endProgram();
      wait(500, msec);
    }
  }

  for(int i = 0; i < 54; i++)
  {
    distribution.ejectCard();
  }

  Brain.programStop();
}

// UserInfo Class Public

float UserInfo::findMean(float arr[], int n) const
{
  int totalCount = 0;
  for (int i = 0; i < n; i++)
  {
    totalCount += arr[i];
  }
  totalCount /= n;
  return totalCount;
}

void UserInfo::adaptCenter(int radius)
{
  // find center
  float centerX = findMean(userLocation[0], totalUsers);
  float centerY = findMean(userLocation[1], totalUsers);

  // for each x,y pair, find new coordinates, update x y
  for (int i = 0; i < totalUsers; i++)
  {

    // find the distance for x and y to the center
    float dx = centerX - userLocation[0][i];
    float dy = centerY - userLocation[1][i];
    float d = hypot(dx, dy);

    // exit if too close
    if (fabs(d) < radius)
    {
      return;
    }

    // normalize the changes. This represents how much of the radius is actually being applied to each vector
    float nx = dx / d;
    float ny = dy / d;

    // modifies original array values
    userLocation[0][i] += radius * nx;
    userLocation[1][i] += radius * ny;
  }
}

// Movement Class Public

void Movement::moveTo(float centerX, float centerY, float finalAngle, float speed = 100)
{
  const int lastRadius = 10; // cm radius of arc to turn
  const int distError = 3;   // acceptable cm error
  const int angleError = 5;
  float deltaAngle = normalizeAngle(findTangent(centerX, centerY, lastRadius), 270);
  static float lastSpeed = 0; // int used to smooth speed
  float speedRatio = arcRatio(lastRadius);

  if (hypot((location[1] - centerY), (location[0] - centerX)) < (distError + lastRadius)) // if we are too close, end function
  {
    MotorLeft.stop();
    MotorRight.stop();
    return;
  }

  if (deltaAngle > 0) // needs to turn cww
  {
    MotorLeft.setVelocity(speedRatio * lastSpeed, percent);
    MotorRight.setVelocity(lastSpeed, percent);
  }
  else // needs to turn cw
  {
    MotorLeft.setVelocity(lastSpeed, percent);
    MotorRight.setVelocity(lastSpeed * speedRatio, percent);
  }

  MotorLeft.spin(forward);
  MotorRight.spin(forward);

  // Wait until we are tanget to circle
  while (fabs(deltaAngle) > angleError)
  {
    lastSpeed = accelerate(lastSpeed, speed);
    if (deltaAngle > 0) // needs to turn cww
    {
      MotorLeft.setVelocity(speedRatio * lastSpeed, percent);
      MotorRight.setVelocity(lastSpeed, percent);
    }
    else // needs to turn cw
    {
      MotorLeft.setVelocity(lastSpeed, percent);
      MotorRight.setVelocity(lastSpeed * speedRatio, percent);
    }
    // updates odometry and find a new tangent based on updated location
    locationUpdate();
    deltaAngle = normalizeAngle(findTangent(centerX, centerY, lastRadius), 270);
  }

  MotorLeft.setVelocity(lastSpeed, percent);
  MotorRight.setVelocity(lastSpeed, percent);

  while (hypot((location[1] - centerY), (location[0] - centerX)) > (distError + lastRadius) && abs(deltaAngle) < 90)
  {
    if (fabs(deltaAngle) < angleError) // on course
    {
      MotorLeft.setVelocity(lastSpeed, percent);
      MotorRight.setVelocity(lastSpeed, percent);
    }
    else if (deltaAngle > 0) // needs to turn ccw
    {
      MotorLeft.setVelocity(lastSpeed * 0.9, percent);
      MotorRight.setVelocity(lastSpeed, percent);
    }
    else // needs to turn cw
    {
      MotorLeft.setVelocity(lastSpeed, percent);
      MotorRight.setVelocity(lastSpeed * 0.9, percent);
    }

    // update odom
    locationUpdate();
    deltaAngle = normalizeAngle(findTangent(centerX, centerY, lastRadius), 270);
  }

  // Brain.Screen.print("phase4\n");
  // Enters final arc to drive in
  while (finalAngleCheck(finalAngle, angleError))
  {
    lastSpeed *= smooth(fabs(normalizeAngle(finalAngle, 360)), 40, 5);
    MotorLeft.setVelocity(lastSpeed, percent);
    MotorRight.setVelocity(lastSpeed * speedRatio, percent);
    locationUpdate();
  }
  // stops motors to ensure that odometry remains accurate)
  // DELETE THESE IF WE ARE NOT STOPPING AFTER MOVE TO

  MotorRight.stop();
  MotorLeft.stop();
}

void Movement::locationUpdate()
{
  double waitTime = 10; // ms (ADJUST AS NEEDED)

  // Initial Values
  double lastLeft = MotorLeft.position(turns);
  double lastRight = MotorRight.position(turns);
  double lastCenter = (lastLeft + lastRight) / 2.0;

  wait(waitTime, msec); // wait before starting updates

  // Final Values
  double currentLeft = MotorLeft.position(turns);
  double currentRight = MotorRight.position(turns);
  double currentCenter = (currentLeft + currentRight) / 2.0;
  double currentHeading = (360 - BrainInertial.heading(degrees)) * (M_PI / 180.0);

  double deltaCenter = (currentCenter - lastCenter) * wheelCircumference;

  // Updates values of x and y
  location[0] += deltaCenter * cos(currentHeading); // x
  location[1] += deltaCenter * sin(currentHeading); // y
}

void Movement::driveStraight(double rightMotorSpeed, double leftMotorSpeed)
{
  if (!Bumper.pressing())
  {
    MotorLeft.setVelocity(leftMotorSpeed, percent);
    MotorRight.setVelocity(rightMotorSpeed, percent);
    MotorLeft.spin(forward);
    MotorRight.spin(forward);
    // AGAIN NOT SURE IF THIS WORKS HOW I WANT IT TO
    // odom.locationUpdate();
    // wait(20, msec);
  }
  else
  {
    MotorLeft.stop();
    MotorRight.stop();
  }
}

void Movement::spinToDegree(double motorSpeed, double angle)
{
  // ODOMETRY DOES NOT MATTER FOR THIS
  // motorSpeed -=5; //subtract five because of velocity adjustment formula
  const float angleError = 3;
  float deltaAngle = normalizeAngle(angle, 180); // finds the angle needed to turn to the spot
  if (fabs(deltaAngle) < angleError)
  {
    return; // already at angle
  }

  float smoothFactor = smooth(fabs(deltaAngle), 40, 5);
  MotorLeft.setVelocity(motorSpeed * smoothFactor, percent);
  MotorRight.setVelocity(motorSpeed * smoothFactor, percent);

  // check which direction we need to rotate
  if (deltaAngle < 0) // turn cw
  {
    MotorLeft.spin(forward);
    MotorRight.spin(reverse);
  }
  else
  {
    MotorLeft.spin(reverse);
    MotorRight.spin(forward);
  }

  while (fabs(deltaAngle) > angleError)
  {
    deltaAngle = normalizeAngle(angle, 180);
    smoothFactor = smooth(fabs(deltaAngle), 40, 5);
    if (deltaAngle < 0) // turning cw
    {
      MotorLeft.setVelocity(motorSpeed * smoothFactor, percent);
      MotorRight.setVelocity(motorSpeed * smoothFactor * -1, percent);
    }
    else // turning ccw
    {
      MotorLeft.setVelocity(motorSpeed * smoothFactor * -1, percent);
      MotorRight.setVelocity(motorSpeed * smoothFactor, percent);
    }
    // MotorLeft.setVelocity(motorSpeed * smoothFactor, percent);
    // MotorRight.setVelocity(motorSpeed * smoothFactor, percent);
    // PLSPLSPLSPLSPLSPLSPLSPSLSPLS WORK
  }

  MotorLeft.stop();
  MotorRight.stop();
}

// Control class public
bool Control::controllerSetup(UserInfo &userManager)
{
  int players = 2;
  int cards = 1;
  bool playerConfirmed = false;
  bool cardConfirmed = false;
  resetScreen();
  Brain.Screen.print("Players: %d", players);
  Brain.Screen.newLine();
  Brain.Screen.print("EUp/>:+  EDown/<:-");
  Brain.Screen.newLine();
  Brain.Screen.print("FUp = location");
  Brain.Screen.newLine();
  Brain.Screen.print("Checkmark = spin");

  bool locBased = false;
  while (!playerConfirmed)
  {
    if ((Controller.ButtonEUp.pressing() || Brain.buttonRight.pressing()) && players < 8)
    {
      while (Controller.ButtonEUp.pressing() || Brain.buttonRight.pressing())
      {
      }
      /*wait(10, msec);
    while (!Controller.ButtonEUp.pressing())
      wait(10, msec);*/
      players++;
      resetLine(1);
      Brain.Screen.print("Players: %d", players);
    }
    else if ((Controller.ButtonEDown.pressing() || Brain.buttonLeft.pressing()) && players > 2)
    {
      while (Controller.ButtonEDown.pressing() || Brain.buttonLeft.pressing())
      {
      }
      /*wait(10, msec);
    while (!Controller.ButtonEDown.pressing())
      wait(10, msec);*/
      players--;
      resetLine(1);
      Brain.Screen.print("Players: %d", players);
    }

    if (Controller.ButtonFUp.pressing())
    {
      while (Controller.ButtonFUp.pressing())
      {
      }
      /*wait(10, msec);*/
      resetScreen();
      Brain.Screen.print("Players: %d", players);
      Brain.Screen.newLine();
      Brain.Screen.print("Location based");
      wait(1000, msec);
      // userManager.setTotalUsers(players);
      playerConfirmed = true;
      locBased = true; // dealing based on location
    }
    if (Brain.buttonCheck.pressing())
    {
      while (Brain.buttonCheck.pressing())
      {
      }
      /*wait(10, msec);*/
      resetScreen();
      Brain.Screen.print("Players: %d", players);
      Brain.Screen.newLine();
      Brain.Screen.print("Spinning based");
      wait(1000, msec);
      // userManager.setTotalUsers(players);
      playerConfirmed = true;
      locBased = false; // dealing done spinning in place
    }
    // wait(20, msec);
  }
  resetScreen();
  cards = 54 / players;
  Brain.Screen.print("Num of Cards: %d", cards);
  Brain.Screen.newLine();
  Brain.Screen.print("EUp/>:+  EDown/<:-");
  Brain.Screen.newLine();
  Brain.Screen.print("FUp = confirm");
  Brain.Screen.newLine();
  Brain.Screen.print("Check = confirm");

  while (!cardConfirmed)
  {

    if ((Controller.ButtonEUp.pressing() || Brain.buttonRight.pressing()))
    {
      while (Controller.ButtonEUp.pressing() || Brain.buttonRight.pressing())
      {
      }
      cards++;
      resetLine(1);
      Brain.Screen.print("Num of Cards: %d", cards);
    }

    if (Controller.ButtonEDown.pressing() || Brain.buttonLeft.pressing() && cards > 1)
    {
      while (Controller.ButtonEDown.pressing() || Brain.buttonLeft.pressing())
      {
      }
      cards--;
      resetLine(1);
      Brain.Screen.print("Num of Cards: %d", cards);
    }

    if (Controller.ButtonFUp.pressing() || Brain.buttonCheck.pressing())
    {
      while (Controller.ButtonFUp.pressing() || Brain.buttonCheck.pressing())
      {
      }
      resetScreen();
      Brain.Screen.print("Num of Cards: %d", cards);
      wait(1000, msec);
      cardConfirmed = true;
    }

    wait(20, msec);
  }

  userManager.initializeUsers(players, cards);
  resetScreen();
  Brain.Screen.print("Setup complete!");
  return locBased;
}

void Control::recordAllPlayerLocations(Movement &move, UserInfo &userManager)
{
  resetScreen();
  Brain.Screen.print("Drive to player");
  Brain.Screen.newLine();
  Brain.Screen.print("FUp = save pos");
  Brain.Screen.newLine();
  Brain.Screen.print("Drive & press FUp to save");
  int totalUsers = userManager.getTotalUsers();
  for (int i = 0; i < totalUsers; i++)
  {
    resetLine(1);
    Brain.Screen.print("Player %d", i + 1);

    bool saved = false;

    while (!saved)
    {
      double moveInput = deadband(Controller.AxisA.position());
      double turnInput = deadband(Controller.AxisC.position());

      double leftSpeed = moveInput + turnInput;
      double rightSpeed = moveInput - turnInput;

      if (leftSpeed > 100)
        leftSpeed = 100;
      if (leftSpeed < -100)
        leftSpeed = -100;
      if (rightSpeed > 100)
        rightSpeed = 100;
      if (rightSpeed < -100)
        rightSpeed = -100;

      // MotorLeft.setVelocity(leftSpeed, percent);
      // MotorRight.setVelocity(rightSpeed, percent);
      // MotorLeft.spin(forward);
      // MotorRight.spin(forward);
      move.driveStraight(rightSpeed, leftSpeed);

      move.locationUpdate();

      if (Controller.ButtonFUp.pressing())
      {
        move.stopMotors();

        userManager.saveCurrentLocation(i);

        resetLine(1);
        Brain.Screen.print("Saved(%.1f, %.1f)", userManager.getUserX(i), userManager.getUserY(i));
        wait(1000, msec);

        saved = true;
      }

      // wait(20, msec);
    }
  }

  resetScreen();
  Brain.Screen.print("All positions saved");
  wait(800, msec);
}

bool Control::endProgram()
{
  // can only rig the first person;
  resetScreen();
  Brain.Screen.print("Dealing Ended");
  Brain.Screen.newLine();
  Brain.Screen.print("Press touch sensor");
  while (TouchLED.pressing())
  {
  };
  while (!TouchLED.pressing())
  {
  };
  bool endProgram = true;

  if (!Optical.isNearObject())
  {
    resetScreen();
    Brain.Screen.print("No cards left");
    Brain.Screen.newLine();
    Brain.Screen.print("Now shutting down");
    wait(2, seconds);
  }
  else
  {
    resetScreen();
    Brain.Screen.print("Repeat process?");
    int shutdown = askYesNo();
    if (shutdown == 1)
    {
      resetScreen();
      Brain.Screen.print("Shutting down...");
      wait(1500, msec);
      Brain.programStop();
    }
    endProgram = false;
  }
  return endProgram;
}

// Control private Functions
int Control::askYesNo()
{
  bool answered = false;
  int result = 0;
  Brain.Screen.newLine();
  Brain.Screen.print("Right = Yes");
  Brain.Screen.newLine();
  Brain.Screen.print("Left = No");

  while (!answered)
  {
    if (Brain.buttonLeft.pressing())
    {
      while (Brain.buttonLeft.pressing())
        ;
      result = 1; // yes
      answered = true;
    }
    if (Brain.buttonRight.pressing())
    {
      while (Brain.buttonRight.pressing())
        ;
      result = 0; // no
      answered = true;
    }
  }
  return result;
}

// Distribution class Private functions
void Distribution::ejectCard()
{
  MotorOutput.setPosition(0, turns);
  MotorOutput.setVelocity(100, percent);

  MotorOutput.spin(reverse);
  while (fabs(MotorOutput.position(turns) * 200) < 104)
  {
  }

  MotorOutput.spin(forward);
  wait(225, msec);
  MotorOutput.stop();
  MotorOutput.setPosition(0, degrees);
}

// int Distribution::randomPlayer() // potentionally try and make this better
// {
//   int total = user.getTotalUsers();
//   int pick = -1;
//   bool found = false;

//   while (!found && !allDone)
//   {
//     pick = rand() % total;
//     if (playerHasCards(pick))
//     {
//       found = true;
//     }
//     else
//     {
//       bool someoneHasCard = false;
//       for (int i = 0; i < total; i++)
//       {
//         if (playerHasCards(i))
//         {
//           someoneHasCard = true;
//         }
//       }
//       if (!someoneHasCard)
//       {
//         allDone = true;
//         pick = -1;
//       }
//     }
//   }
//   return pick;
// }

// shuffle formula from https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle
void Distribution::randomPlayer(int players[], int size)
{
  for (int i = size - 1; i > 0; i--)
  {
    int j = rand() % (i + 1); // random index from 0..i
    int temp = players[i];
    players[i] = players[j];
    players[j] = temp;
  }
}

int Distribution::countRemainingCards()
{
  int total = user.getTotalUsers();
  int remaining = 0;

  for (int i = 0; i < total; i++)
  {
    remaining += user.getCardsLeft(i);
  }

  return remaining;
}

// DIstribution class Public functions

void Distribution::locDistribution()
{
  int speed = 100;
  allDone = false;

  control.resetScreen();
  Brain.Screen.print("Waiting for touch LED...");

  while (TouchLED.pressing())
    wait(20, msec);
  while (!TouchLED.pressing())
    wait(20, msec);

  Brain.Screen.clearScreen();
  Brain.Screen.print("Distribution started");

  bool running = true;

  int playerArray[user.getTotalUsers()];
  for (int i = 0; i < user.getTotalUsers(); i++)
  {
    playerArray[i] = i;
  }

  randomPlayer(playerArray, user.getTotalUsers());

  while (running)
  {
    for (int i = 0; i < user.getTotalUsers(); i++)
    {
      if (!running)
      {
      }
      else
      {
        control.resetScreen();
        Brain.Screen.print("Driving to P%d", playerArray[i]);

        move.moveTo(user.getUserX(playerArray[i]), user.getUserY(playerArray[i]), user.getUserHeading(playerArray[i]), speed);
        // driveTo(px, py, BrainInertial.heading(degrees) + 90);

        // if (calculateDistance(px, py) <= 1.0) {
        control.resetScreen();
        Brain.Screen.print("Ejecting...");
        ejectCard();
        user.cardGivenTo(playerArray[i]);

        Brain.Screen.newLine();
        Brain.Screen.print("Remaining cards: %d", countRemainingCards());

        if (countRemainingCards() == 0)
        {
          running = false;
        }
      }
    }
    randomPlayer(playerArray, user.getTotalUsers());
  }
  Brain.Screen.newLine();
  Brain.Screen.print("Returning home...");
  move.moveTo(0, -10, 0, speed);

  // driveTo(home[0], home[1], 0);
  wait(500, msec);
  Brain.Screen.clearScreen();
  Brain.Screen.print("Cards delivered");
}

void Distribution::spinDistribution()
{
  control.resetScreen();
  Brain.Screen.print("Touch LED ");
  Brain.Screen.newLine();
  Brain.Screen.print("to start...");

  while (TouchLED.pressing())
    wait(20, msec);
  while (!TouchLED.pressing())
    wait(20, msec);

  int totalPlayers = user.getTotalUsers();
  float angleIncrement = 360 / totalPlayers;
  bool running = true;

  int playerArray[totalPlayers];
  for (int i = 0; i < totalPlayers; i++)
  {
    playerArray[i] = i;
  }
  randomPlayer(playerArray, totalPlayers);

  while (running)
  {
    for (int i = 0; i < totalPlayers; i++)
    {
      if (!running)
      {
      }
      else
      {
        control.resetScreen();
        Brain.Screen.print("Spin to P%d", playerArray[i]);

        move.spinToDegree(100, fmod(angleIncrement * playerArray[i], 360));
        Brain.Screen.newLine();
        Brain.Screen.print("Ejecting...");
        ejectCard();
        user.cardGivenTo(playerArray[i]);

        Brain.Screen.newLine();
        Brain.Screen.print("Remaining: %d", countRemainingCards());

        if (countRemainingCards() == 0)
        {
          running = false;
        }
      }
    }
    randomPlayer(playerArray, totalPlayers);
  }

  control.resetScreen();
  Brain.Screen.print("Cards done");
}
