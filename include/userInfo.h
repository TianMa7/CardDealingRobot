#ifndef USER_INFO_H
#define USER_INFO_H

// Make sure all required headers are included.
#include "vex.h"
#include "movement.h"

using namespace vex;

class UserInfo
{
private:
    static const int MAX_USERS = 8;
    int users[MAX_USERS];
    float userLocation[3][MAX_USERS];
    int totalUsers = 0;
    int cardsPerPlayer = 0;
    // int riggedIndex;

    Movement *move;

public:
    UserInfo(Movement *m) : move(m)
    { // constructor
        totalUsers = 0;
        cardsPerPlayer = 0;
        // riggedIndex = 0;
        for (int i = 0; i < MAX_USERS; i++)
        {
            users[i] = 0;
            userLocation[0][i] = 0; // x
            userLocation[1][i] = 0; // y
        }
    }

    // PUBLIC FUNCTIONS INLINE
    float getUserX(int idx)
    {
        /*if (idx >= 0 && idx < totalUsers) return userLocation[idx][0];
        return -1;
        */
        return userLocation[0][idx]; //(*odo).location[0];//userLocation[idx][0];
    }

    float getUserY(int idx)
    {
        // if (idx >= 0 && idx < totalUsers) return userLocation[idx][1];
        return userLocation[1][idx]; //(*odo).location[1];//userLocation[idx][1];
    }

    float getUserHeading(int idx)
    {
        // if (idx >= 0 && idx < totalUsers) return userLocation[idx][2];
        return userLocation[2][idx]; //(*odo).location[1];//userLocation[idx][1];
    }

    void initializeUsers(int players, int cards)
    {
        totalUsers = players;
        cardsPerPlayer = cards;

        for (int i = 0; i < totalUsers; i++)
        {
            users[i] = cardsPerPlayer;
        }
    }

    void saveCurrentLocation(int idx)
    { // saves x,y coordinates to user info
        userLocation[0][idx] = (*move).getX();
        userLocation[1][idx] = (*move).getY();
        userLocation[2][idx] = (*move).getHeading();
    }

    void cardGivenTo(int userID)
    {
        users[userID]--;
    }

    int getCardsLeft(int idx) const
    {
        return users[idx];
    }

    int getTotalUsers() const
    {
        return totalUsers;
    }

    int getCardPerPlayer() const
    {
        return cardsPerPlayer;
    }

    float findMean(float arr[], int n) const;
    void resetCards();
    void adaptCenter(int radius); // readjusts data for location based dealing
};
#endif
