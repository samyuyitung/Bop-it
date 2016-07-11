
//START STRUCT / VARIABLE DECLARATION
struct Player {
  int pName; //Name of the player ( 1 or 2 )
  int score;
  double pTime;
  int inputPin [3];

};


//Players of the game:
Player p1;
Player p2;

//Middle leds to show which button to press.
int choiceLed [3];

//Button to start the game
int startButton = 7;

/**
    0 - init state no game played
    1 - in game (two player) Timed
    2 - post game / game over
*/
int gameState = 0;

void setup() {
  p1 = initPlayer(1);
  p2 = initPlayer(2);
  setupPins();

}
/**
   Pin - Usage

   0 - P1 input 1 (button)
   1 - P1 input 2 (tilt)
   2 - P1 input 3 (photo-resistor)

   3 - P2 input 1 (photo-resistor)
   4 - P2 input 2 (tilt)
   5 - P2 input 3 (button)
   6
   7 - Start button
   8
   9
   10 - LED Choice 1 (green)
   11 - LED Choice 2 (red)
   12 - LED Choice 3 (blue)

*/

void setupPins() {
  //Setup start button
  pinMode(startButton, INPUT);

  //Set led pin 10,11,12 as OUTPUTS
  for (int i = 0; i < 3; i++) {
    choiceLed[i] = 10 + i;
    pinMode(choiceLed[i], OUTPUT);
  }
  //set up pins (0,1,2) as inputs for player 1
  //   and pins (3,4,5) as inputs for player 2
  for (int i = 0; i < 3; i++) {
    p1.inputPin[i] = i;
    p2.inputPin[i] = i + 3;
    pinMode(p1.inputPin[i], INPUT);
    pinMode(p2.inputPin[i], INPUT);
  }

  //Attach interrupts
  attachInterrupt(0, PLAYER_1_ISR, CHANGE);
  attachInterrupt(1, PLAYER_2_ISR, CHANGE);
}

/**
   Init player (Nothing special)
*/
Player initPlayer(int playerNumber) {
  Player tempPlayer;
  tempPlayer.pName = playerNumber;
  tempPlayer.score = tempPlayer.pTime = 0;

  return tempPlayer;
}

// END INITIATION STUFF
// START GAME STUFF
void loop() {
  switch (gameState) {
    case 0:
      preGame();
      break;
    case 1:
      inGame();
      break;
    case 2:
      postGame();
      break;
  }


}

void preGame() {

}

void inGame() {

}


void postGame() {

}

//END GAME STUFF
//START INTERRUPTS

void PLAYER_1_ISR() {

}

void PLAYER_2_ISR() {

}

//END INTERRUPTS
