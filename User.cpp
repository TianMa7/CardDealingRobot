class User {      
    
    private:
    int playerCount;
    int cardsPerPlayer;
    double playerSpacing;
     
    public:
    User(int numPlayers = 1, int cardsPerPlayer = 0, double spacing = 0.0)
        : playerCount(numPlayers), cardsPerPlayer(cardsPerPlayer), playerSpacing(spacing) {

        }

    //gatters
    int  getPlayerCount(){ 
        return playerCount; 
    }
    int  getCardsPerPlayer(){ 
        return cardsPerPlayer; 
    }
    double getPlayerSpacing(){ 
        return playerSpacing; 
    }


    //setters
    void setPlayerCount(int u){ 
        this->playerCount = u; 
    }
    void setCardsPerPlayer(int c){ 
        this->cardsPerPlayer = c; 
    }
    void setPlayerSpacing(double d){ 
        this->playerSpacing = d; 
    }


    void calculatePlayerSpacing() {
    if (getPlayerCount() <= 0) return;
    setPlayerSpacing(360.0 / getPlayerCount());
    }

};