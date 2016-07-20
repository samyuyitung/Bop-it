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

*****************************************************************************/

//Allow for timer0 interrupts
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

// include the LCD Library:
#include <LiquidCrystal.h>


//START STRUCT / VARIABLE DECLARATION
struct Player {
  int pName; //Name of the player ( 1 or 2 )
  int score; //rounds won
  int inputPin; //analog input pin
  int val; //value of analog button press
  int trigger; // 1 when interrupt is triggered
  double responseTime; //time from start to end
  String summary; //LCD output
  double startPress; //start of button press
  double pressDuration; //duration of button press
};

const int POINTS_TO_WIN = 2;
// My timer variables
volatile unsigned long myTimer0_millis = 0;
static unsigned char myTimer0_fract = 0;

//Players of the game:
Player p1;
Player p2;

//Middle leds to show which button to press.
int roundLed;

//Button to start pin location
int startButton = 8;

/**
    0 - init state no game played

    1 - start round
    2 - listening
    3 - post round

    4 - post game / game over
*/
int gameState;
int roundWinner;
long roundStartTime;
long gameStartTime;
long gameLengthTime;
//LCD
LiquidCrystal lcd(1, 0, 4, 5, 6, 7);

void setup() {
  p1 = initPlayer(1);
  p2 = initPlayer(2);
  setupDigitalPins();
  setupAnalogPins();
  gameState = 0;
  srand(analogRead(2));
  lcd.begin(16, 2);
  initLCD();
}
/*
   NEED 1 MORE PIN FOR LCD

   Pin - Usage
   0 - LCD enable
   1 - LCD reg store
   2 - player 1 interrupt
   3 - player 2 interrupt
   4 - LCD reg 1
   5 - LCD reg 2
   6 - LCD reg 3
   7 - LCD reg 4
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

  for (int i = 9; i < 14; i++)
    pinMode(i, OUTPUT);

  //Attach interrupts
  attachInterrupt(0, PLAYER_1_ISR, CHANGE); //Pin 2
  attachInterrupt(1, PLAYER_2_ISR, CHANGE); //Pin 3
}

/*
  A0 - Player 1
  A5 - Player 2
*/
void setupAnalogPins() {
  pinMode(p1.inputPin, INPUT);
  pinMode(p2.inputPin, INPUT);
}

/*
  Init player (Nothing special)
*/
Player initPlayer(int playerNumber) {
  Player tempPlayer;
  tempPlayer.pName = playerNumber;
  tempPlayer.score = 0;
  tempPlayer.inputPin = (playerNumber - 1) * 5;
  tempPlayer.val = -1;
  tempPlayer.summary = "";
  return tempPlayer;
}

void initLCD() {
  lcdWrite("  Press black", "button to start");
}
// END INITIATION STUFF
// START GAME STUFF
void loop() {
  switch (gameState) {
    case 0:
      preGame();
      break;
    case 1:
      startRound();
      break;
    case 2:
      listenForResponse();
      break;
    case 3:
      endRound();
      break;
    case 4:
      gameOver();
      break;
    //Only used for testing
    case 5:
      debug();
      break;
  }
}
void debug() {
  if (p1.trigger) {
    p1.trigger = false;
  }

  if (p2.trigger) {
    p2.trigger = false;
  }
}
/*
  Init state of the game, only exist until the start button is pressed
*/
void resetGame() {
  p1.score = 0;
  p2.score = 0;
  initLCD();
  gameState = 0;
}

void preGame() {
  //Start game
  if (digitalRead(startButton) == LOW) {
    preGameLightsShow();
  }
}

/*
  Small transitions to signify the start of the game!
*/

void preGameLightsShow() {
  lcd.clear();
  lcd.setCursor(0, 0);
  //         1234567890123456
  lcd.print("Starting game...");
  //lights will all turn on one at a time, blink on and off twice

  for (int i = 9; i < 14; i++) {
    digitalWrite(i, HIGH); // turn all LEDs on
    delay1(500);
  }
  for (int i = 9; i < 14; i++) {
    digitalWrite(i, LOW); // turn all LEDs off
  }
  //Blink all leds twice
  for (int j = 0; j < 2; j++) {
    delay1 (500);
    for (int i = 9; i < 14; i++) {
      digitalWrite(i, HIGH); // turn all LEDs on
    }
    delay1(500);
    for (int i = 9; i < 14; i++) {
      digitalWrite(i, LOW); // turn all LEDs off
    }
  }
  resetLCD();
  //Wait a second before starting
  delay1(1000);
  gameState = 1;
  gameStartTime = millis1();
}
void resetLCD() {
  lcdWrite("P1:", "P2:");
}
/*
  light up the pin for the round
*/
void startRound() {
  resetLCD();
  //Randomly select pin for round
  roundLed = 9 + (int) rand() % 5;
  //Turn it on!
  digitalWrite(roundLed, HIGH);
  //reset some stuff
  p1.trigger = p2.trigger =  roundWinner = 0;
  p1.pressDuration = p2.pressDuration = -1;
  p1.responseTime = p2.responseTime = -1;
  p1.summary = p2.summary = "";
  gameState = 2;

  //pull start time before switching
  roundStartTime = millis1();
}

/**
   Listen for button interrupts.
   Leave (change gameState) if:
   1. More than 5 seconds
   2. Both people have triggered it.
*/
void lcdWrite(String line1, String line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}
void listenForResponse() {
  //Go to next round if longer than 5 seconds
  if (millis1() - roundStartTime >= 5000) {
    String l2 = "no winner";
    if (roundWinner == 1)
      l2 = "p1 won";
    if (roundWinner == 2)
      l2 = "p2 won";
    lcdWrite("Timed out", l2 );
    delay1(1000);
    gameState = 3;
    return;
  }

  //TRIGGERED IF ISR'd
  if (p1.trigger == 1) {
    p1.trigger = 2;
    if (checkRight(decodeVal(p1.val))) {
      if (roundWinner == 0) {
        roundWinner = 1;
        p1.score ++;
        p1.summary = "Winner";
      } else
        p1.summary = "Too slow";
    }
    else
      p1.summary = "Wrong";
    lcdWrite("P1: " + p1.summary, "P2: " + p2.summary);
  }



  if (p2.trigger == 1) {
    p2.trigger = 2;
    if (checkRight(decodeVal(p2.val))) {
      if (roundWinner == 0) {
        roundWinner = 2;
        p2.score ++;
        p2.summary = "Winner";
      } else
        p2.summary = "Too slow";
    }
    else
      p2.summary = "Wrong";

    lcdWrite("P1: " + p1.summary, "P2: " + p2.summary);
  }
  if (p1.trigger == 2 && p2.trigger == 2 ) {
    delay1(1000);
    gameState = 3;
  }
}

/*
   Decode the trigger based off analog in value
*/
int decodeVal(int val) {
  if (val < 50)
    return 4;
  else if (val < 80)
    return 3;
  else if (val < 120)
    return 2;
  else if (val < 180)
    return 1;
  else if (val < 220)
    return 0;

  //Nothing or something not known
  return -1;

}

boolean checkRight(int val) {
  if (val + 9 == roundLed)
    return true;
  return false;
}

void endRound() {
  if (p1.score == POINTS_TO_WIN || p2.score == POINTS_TO_WIN) {
    gameLengthTime = deltaTimeToSeconds(gameStartTime, millis1());
    gameState = 4;
    return;
  }

  digitalWrite(roundLed, LOW);
  writeCurrentScore();
  writeResponseTime();
  writePressTime();
  gameState = 1;
}

void writeCurrentScore() {
  lcdWrite("P1 Score: " + String(p1.score), "P2 Score: " + String(p2.score));
  delay1(2000);
}

void writeResponseTime() {
  if (p1.responseTime + p2.responseTime == -2)
    return;

  String p1String = "N/A";
  String p2String = "N/A";
  if (p1.responseTime > 0)
    p1String = String(p1.responseTime) + "s";

  if (p2.responseTime > 0)
    p2String = String(p2.responseTime) + "s";

  lcdWrite("P1 res tm: " + p1String, "P2 res tm: " + p2String);
  delay1(2000);
}

void writePressTime() {
  if (p1.pressDuration + p2.pressDuration == -2)
    return;

  String p1String = "N/A";
  String p2String = "N/A";
  if (p1.pressDuration > 0)
    p1String = String(p1.pressDuration) + "s";

  if (p2.pressDuration > 0)
    p2String = String(p2.pressDuration) + "s";
  lcdWrite("P1 pre tm: " + p1String, "P2 pre tm: " + p2String);
  delay1(2000);
}
/*
  State after game ends
  Shows the score
  option to restart game with start button push
*/
void gameOver() {
  writeScores();
  endingLights();
  delay1(4500);
  showGameTime();
  delay1(4500);
  resetGame();
}

/*
   Print the players scores to the LCD
*/
void writeScores() {
  if (p1.score == POINTS_TO_WIN )
    lcdWrite("Player 1 wins!", "final score " + String(p1.score) + "-" + String(p2.score));
  else
    lcdWrite("Player 2 wins!", "final score " + String(p2.score) + "-" + String(p1.score));
}
void showGameTime(){
  lcdWrite("Game length was", "      " + String(gameLengthTime) + "s");
}

void endingLights() {
  //lights will all turn on one at a time, blink on and off twice
  for (int j = 0; j < 2; j++) { //all lights blink on and off twice
    delay1 (500);
    for (int i = 9; i < 14; i++) {
      digitalWrite(i, HIGH); // turn all LEDs on
    }
    delay1(500);
    for (int i = 9; i < 14; i++) {
      digitalWrite(i, LOW); // turn all LEDs off
    }
  }
  for (int i = 13; i >= 9; i--) {
    digitalWrite(i, HIGH); // turn all LEDs on
    delay1(500);
  }
  for (int i = 9; i < 14; i++) {
    digitalWrite(i, LOW); // turn all LEDs off
  }
  // all lights on with slight delay1, all off

  delay1(1000);

}
//END GAME STUFF

//START INTERRUPTS
void PLAYER_1_ISR() {
  long iTime = millis1();
  if (p1.trigger == 0) {
    p1.val = analogRead(p1.inputPin);
    p2.responseTime = deltaTimeToSeconds(roundStartTime, iTime);
    //only allow one read per round
    p1.trigger = 1;
    p1.startPress = iTime;
  } else if ( p1.pressDuration < 0) {
    p1.pressDuration = deltaTimeToSeconds(p1.startPress, iTime);
  }

}

void PLAYER_2_ISR() {
  long iTime = millis1();
  p2.val = analogRead(p2.inputPin);
  if (p2.trigger == 0) {
    p2.responseTime = deltaTimeToSeconds(roundStartTime, iTime);
    p2.trigger = 1;
    p2.startPress = iTime;
  } else if ( p2.pressDuration < 0) {
    p2.pressDuration = deltaTimeToSeconds(p2.startPress, iTime);
  }
}
//END INTERRUPTS
//START TIMER
/**
   HOTW: (How our timer work);
   Each clock cycle is 250,000 hz or 0.000004s --> 0.004 ms
   everytime it overflows, 1.024 ms has elapsed
   So what we do is:

   Math stuff:
   1 / 0.024 = 41.6667 <-- every 41.667 clock overflows we gotta add an extra millis;
   since this is a fraction we triple the addition so that we add An extra millis every 125 to deal with integers
   every overflow add 1 to millis and add 3 to fraction
*/


ISR(TIMER0_OVF_vect) {
  myTimer0_millis += 1;
  myTimer0_fract += 3;
  //add the compounded 0.024 and reset decimal counter.
  if (myTimer0_fract >= 125) {
    myTimer0_fract -= 125;
    myTimer0_millis += 1;
  }
}

unsigned long millis1() {
  unsigned long m;
  cli();
  m = myTimer0_millis;
  sei();
  return m;
}

void delay1(long waitMillis) {
  long startTime = millis1();
  while (millis1() - startTime < waitMillis)
    continue;
}
int deltaTimeToSeconds(long startTime, long endTime){
  return (endTime - startTime) / 1000.0
}
//END TIMER
