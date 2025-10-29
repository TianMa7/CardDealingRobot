class UserInput : public User {
 public:
  int player[10][2]; // max 10 players, 2 columns
  int playerCount;
  int cardsPerPlayer;

  void initializePlayers() {
    for (int i = 0; i < playerCount; i++) {
      player[i][0] = i;            // player number
      player[i][1] = cardsPerPlayer; // cards per player
    }
  }

  void UpdatePlayerCount() {
    int count = getPlayerCount();

    if (Brain.buttonUp.pressing()) {
        count++;
        setPlayerCount(count);
        while(Brain.buttonUp.pressing()) {}
    }

    if (Brain.buttonDown.pressing()) {
        count--;
        setPlayerCount(count);
        while(Brain.buttonDown.pressing()) {}
    }
  }

  
  void updateCardsPerPlayer() {
    int cards = getCardsPerPlayer(); 

    if (Brain.buttonLeft.pressing()) {
        cards++;
        setCardsPerPlayer(cards);
        while(Brain.buttonLeft.pressing()){}
    }

    if (Brain.buttonRight.pressing()) {
        cards--;
        setCardsPerPlayer(cards);
        while(Brain.buttonRight.pressing()){}
    }
  }

};