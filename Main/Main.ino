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
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif



//START STRUCT / VARIABLE DECLARATION
struct Player {
  int pName; //Name of the player ( 1 or 2 )
  int score;
  double pTime;
  int roundTime;
  int inputPin;
  int val;
};

// My timer variables
volatile unsigned long myTimer2_overflow_count = 0;
volatile unsigned long myTimer2_millis = 0;
static unsigned char myTimer2_fract = 0;

long roundStartTime;
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

    1 - start round
    2 - listening
    3 - post round

    2 - post game / game over
*/
int gameState;
int roundWinner;


void setup() {
  p1 = initPlayer(1);
  p2 = initPlayer(2);
  setupDigitalPins();
  setupAnalogPins();
  gameState = 0;
  seenScores = false;
}
/*
   NEED 1 MORE PIN FOR LCD

   Pin - Usage
   1 - LCD
   2 - player 1 interrupt
   3 - player 2 interrupt
   4 - LCD
   5 - LCD
   6 - LCD
   7 - LCD
   8 - Start button
   9 - LED Choice 1
   10 - LED Choice 2 (green)
   11 - LED Choice 3 (red)
   12 - LED Choice 4 (blue)
   13 - LED Choice 5
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
  tempPlayer.val = -1;

  return tempPlayer;
}
/*
  A0 - Player 1
  A5 - Player 2
*/
void setupAnalogPins() {
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
      gameOver();
      break;
  }
}
/*
  Init state of the game, only exist until the start button is pressed
*/
void preGame() {
  gaG
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

  for (int i = 9; 9 < 14; i++) {
    digitalWrite(i, HIGH); // turn all LEDs on
  }

  delay(1000);
  for (int i = 9; 9 < 14; i++) {
    digitalWrite(i, LOW); // turn all LEDs off
  }
}


void inGame() {
  switch (gameState) {
    case 1:
      startRound();
      break;
    case 2:
      listenForResponse();
      break;
    case 3:
      endRound();
      break;
  }
}
/*
  light up the pin for the round
*/
void startRound() {
  roundLed = (int) rand() % 5;
  roundLed += 9;
  digitalWrite(roundLed, HIGH);
  p1.triggered = false;
  p2.triggered = false;
  gameState += 1;
  roundStartTime = millis1();
}

/**
   Listen for button interrupts.
   Leave (change gameState) if:
   1. More than 5 seconds
   2. Both people have triggered it.
*/
void listenForResponse() {
  if (millis1() - roundStartTime >= 5000)
    gameState++;

  if (p1.trigger && roundWinner != 1) {
    if (checkRight(decodeVal(p1.val))) {
      p1.score ++;
      if (roundWinner == 0)
        roundWinner = 1;
    }
  }

  else if (p2.trigger && roundWinner != 2) {
    if (checkRight(decodeVal(p2.val))) {
      p2.score ++;
      if (roundWinner == 0)
        roundWinner = 2;
    }
  }

  if (p1.trigger && p2.trigger)
    gameState++;
}

/*
   Decode the trigger based off analog in value
*/
int decodeVal(int val) {
  if (val < 50)
    return 0;
  else if (val < 80)
    return 1;
  else if (val < 120)
    return 2;
  else if (val < 180)
    return 3;
  else if (val < 220)
    return 4;

  //Nothing
  return -1;

}
boolean checkRight(int val) {
  if (val + 9 == roundLed)
    return true;
  return false;
}

void endRound() {
  digitalWrite(roundLed, LOW);

  gameState = 1;

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
  gameState = 0;
}

/*
   Print the players scores to the LCD
*/
void writeScores() {
  //  lcd.locate(

}
//END GAME STUFF
//START INTERRUPTS
void PLAYER_1_ISR() {
  p1.val = analogRead(p1.inputPin);
  p1.trigger = true;
  p1.responseTime = millis1() - roundStartTime;
}

void PLAYER_2_ISR() {
  val = analogRead(p2.inputPin);
  p2.trigger = true;
  p2.responseTime = millis1() - roundStartTime;
}
//END INTERRUPTS
//START TIMER
ISR(TIMER2_OVF_vect) {
  myTimer2_millis += 1;
  myTimer2_fract += 3;
  if (myTimer2_fract >= 125) {
    myTimer2_fract -= 125;
    myTimer2_millis += 1;
  }
  myTimer2_overflow_count++;
}
unsigned long millis1()
{
  unsigned long m;
  uint8_t oldSREG = SREG;

  // disable interrupts while we read timer0_millis or we might get an
  // inconsistent value (e.g. in the middle of a write to timer0_millis)
  cli();
  m = myTimer2_millis;
  SREG = oldSREG;
  cli();
  return m;
}
//END TIMER
