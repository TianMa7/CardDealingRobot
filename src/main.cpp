/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       rgao5                                                     */
/*    Created:      10/30/2025, 8:29:49 PM                                    */
/*    Description:  IQ2 project                                               */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#include "vex.h"
#include "UserInput.cpp"
#include <vector>

using namespace vex;

// A global instance of vex::brain used for printing to the IQ2 brain screen
vex::brain       Brain;
inertial BrainInertial = inertial();
motor MotorLeft = motor(PORT11, false);
motor MotorRight = motor(PORT8, true);
controller XBoxController = controller();

// define your global instances of motors and other devices here
class Player
{
    private:
    float node[3];
    int numOfCards;
    bool cheater;
    public:
        Player()
        {
            numOfCards = 0;
            cheater = false;
        }
        void setNode(float x, float y, float angle)
        {
            this->node[0] = x;
            this->node[1] = y;
            this->node[2] = angle;
        }

        float getX()
        {
            return this->node[0];
        }

        float getY()
        {
            return this->node[1];
        }

        float getAngle()
        {
            return this->node[2];
        }
        void addCard()
        {
            numOfCards++;
        }
        void toggleCheater()
        {
            if(this->cheater)
            {
                this->cheater = false;
            }
            else
            {
                this->cheater = true;
            }
        }


};

class SetupPlayerMode{
    private:
        int numOfPlayers;
        bool complexSetup;

    public:
        SetupPlayerMode() {
            numOfPlayers = 2;
            complexSetup = false;
        }
        void getPlayers(){
            while (!XBoxController.ButtonEUp.pressing()){
                UserInput.updatePlayerCount();
                if (XBoxController.ButtonFUp.pressing()){
                    this->complexSetup = true;
                }
            }

        }

};

class SetUpLocMode
{
    private:
        int numPlayers;
        vector<Player> players;

    public:
        SetUpLocMode(int numPlayers)
        {
            this->numPlayers = numPlayers;
        }
        void setNewNode()
        {
            this->players.push_back(getCurrentLocation());
        }
        void deleteNode()
        {
            this->players.pop_back();
        }

};


int main() {
	
    Brain.Screen.printAt( 2, 30, "Hello IQ2" );
   
    while(1) {
        
        // Allow other tasks to run
        this_thread::sleep_for(10);
    }
}