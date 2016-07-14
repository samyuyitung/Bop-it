/****************************************************************************
        .______     ______   .______             __  .___________.
        |   _  \   /  __  \  |   _  \           |  | |           |
        |  |_)  | |  |  |  | |  |_)  |  ______  |  | `---|  |----`
        |   _  <  |  |  |  | |   ___/  |______| |  |     |  |
        |  |_)  | |  `--'  | |  |               |  |     |  |
        |______/   \______/  | _|               |__|     |__|


      SYDE 192L project
      July 2016

      By:
         Ashley Hu
         Michael Shiozaki
         Sam Yuyitung

      Read in depth description in project_desc.txt
*****************************************************************************/

#include <LiquidCrystal.h>

//START STRUCT / VARIABLE DECLARATION
struct Player {
  int pName; //Name of the player ( 1 or 2 )
  int score;
  double pTime;
  int inputPin;
};


//Players of the game:
Player p1;
Player p2;

//Middle leds to show which button to press.
int choiceLed [3];
int roundLed;

//Button to start / pause the game
int startButton = 7;
int pauseButton = 8;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

/**
    0 - init state no game played
    1 - in game (two player) Timed
    2 - post game / game over
*/
int gameState;
boolean seenScores;
void setup() {
  p1 = initPlayer(1);
  p2 = initPlayer(2);
  setupDigitalPins();
  setupAnalogPins();
  gameState = 0;
  seenScores = false;
}
/*
   Pin - Usage

   2 - player 1 interrupt
   3 - player 2 interrupt

   7 - Start button
   8 - Pause button?

   10 - LED Choice 1 (green)
   11 - LED Choice 2 (red)
   12 - LED Choice 3 (blue)

*/
void setupDigitalPins() {
  //Setup start button
  pinMode(startButton, INPUT);
  pinMode(pauseButton, INPUT);

  //Set led pin 10,11,12 as OUTPUTS
  for (int i = 0; i < 3; i++) {
    choiceLed[i] = 10 + i;
    pinMode(choiceLed[i], OUTPUT);
  }

  //Attach interrupts
  attachInterrupt(0, PLAYER_1_ISR, CHANGE);
  attachInterrupt(1, PLAYER_2_ISR, CHANGE);
}

/*
  Init player (Nothing special)
*/
Player initPlayer(int playerNumber) {
  Player tempPlayer;
  tempPlayer.pName = playerNumber;
  tempPlayer.score = tempPlayer.pTime = 0;
  tempPlayer.inputPin = (playerNumber - 1) * 5;
  return tempPlayer;
}
/*
  A0 - Player 1
  A5 - Player 2
*/
void setupAnalogPins(){
  pinMode(p1.inputPin, INPUT);
  pinMode(p2.inputPin, INPUT);
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
/*
  Init state of the game, only exist until the start button is pressed
*/
void preGame() {
  //Start game
  if (digitalRead(startButton) == HIGH) {
    startGame();
  }
}

/*
  Small transitions to signify the start of the game!
*/

void startGame() {
  // Flash the lights
  // Play a sound
  // LCD Stuff maybe?
  gameState = 1;

}


void inGame() {
  correctInput = startRound();
  listenForResponse();
  endRond();
}

/*
  light up the pin for the round
*/
void startRound(){
  roundLed = (int) rand() % 3
  ledChoice += 7;
  digitalWrite(ledChoice, HIGH);
  p1.triggered = false;
  p2.triggered = false;
}

void listenForResponse(){

  while(true){
    if(p1.triggered)

  }

}



/*
  State after game ends
  Shows the score
  option to restart game with start button push
*/
void gameOver() {
  writeScores();
  //allow them to see it before restarting
  delay(5000);
  //go to ready to start mode
  gameMode = 0;
}

/*
   Print the players scores to the LCD
*/
void writeScores() {

}
//END GAME STUFF

//START INTERRUPTS
void PLAYER_1_ISR() {

}

void PLAYER_2_ISR() {

}
//END INTERRUPTS
