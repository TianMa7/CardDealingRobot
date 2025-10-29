

class Function : public UserInput {      
  public:
  Function(int numPlayers = 1, int cardsPerPlayer = 0, double spacing = 0.0)
    : UserInput(numPlayers, cardsPerPlayer, spacing) {}


  void trackPlayerCard()
  {
  for (int i = 0; i < getCardsPerPlayer()*getPlayerCount(); i++){
    for (int j = 0; j < getCardsPerPlayer; j++){
      int temp = randomPlayer();
      if(player[temp][1] > 0){
        suspendCard();
        player[temp][1]--;
        } else{
          j--;
        }
      }
    }
  }

  void suspendCard(){
    output_Motor.setVelocity(100, percent);
    output_Motor.resetPosition();
    output_Motor.spin(forward);
    while((output_Motor.position(degree) * WHEELE_CIRCUMFRENER / 10) < (20 - TAPE_DISTANCE)){
    }
    output_Motor.stop();
  }

  int randomPlayer() {
    return rand() % getPlayerCount();
  }
}
