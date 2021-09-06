// Name: Al Hisham Anik
// ID #: 1585385
// CMPUT 275, Winter 2019
// Assignment: CMPUT 275 Final Project  
// Reference: Using the code snippets from assignemnt 1 and 2 from the course for 
// touch user interface and Joystick controls

//declaring arduino and C++ libraries and using standard namespace
#include <Arduino.h>
#include "snakeSeg.h"
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <SD.h>
#include <TouchScreen.h>
#include <SPI.h>
#include <SD.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSerif12pt7b.h>
using namespace std;

// physical component initialization
MCUFRIEND_kbv tft;
#define JOY_VERT A9  // should connect A9 to pin VRx
#define JOY_HORIZ A8  // should connect A8 to pin VRy
#define JOY_SEL 53

// joystick and cursor initialization
#define JOY_CENTER 512
#define JOY_DEADZONE 64
#define CURSOR_SIZE 9
int16_t cursorX, cursorY;  // based on top left corner of cursor

// display dimensions and image/map initialization
#define DISPLAY_WIDTH 480
#define DISPLAY_HEIGHT 320
#define MAP_DISP_WIDTH (DISPLAY_WIDTH - 60)
#define MAP_DISP_HEIGHT DISPLAY_HEIGHT
#define BUTTON_HEIGHT DISPLAY_HEIGHT/2

// touch screen pins, obtained from the documentaion
#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9  // can be a digital pin
#define XP 8  // can be a digital pin

// calibration data for the touch screen, obtained from documentation
// the minimum/maximum possible readings from the touch point
#define TS_MINX 100
#define TS_MINY 120
#define TS_MAXX 940
#define TS_MAXY 920

// thresholds to determine if there was a touch
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// a multimeter reading says there are 300 ohms of resistance across the plate,
// so initialize with this to get more accurate readings
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define SD_CS 10

// different than SD
Sd2Card card;

//Declaring some of the functions at the start
void MainMenu();
void GameSetup();
void Play();
void score(int point);
SnakeSeg snake[600];

//direction of snake
int xDir;
int yDir;

//if false game ends
bool PLAY = true;

//has the player started?
bool STARTED;

//did the snake eat on this frame?
bool ATE;

//initial snake length
int len = 6;

//location of food
int FOODX;
int FOODY;


void setup() {
  /*
  This function checks the display and intialize the digital display

  Arguments:
    No arguments are provided.

  Returns:
    void
  */
  init();
  Serial.begin(9600);

  pinMode(JOY_SEL, INPUT_PULLUP);

  // TFT Display initialization
  uint16_t ID = tft.readID();  // read ID from display
  if (ID == 0xD3D3) ID = 0x9481;  // write-only shield
  tft.begin(ID);  // LCD gets ready to work

  // SD Card initialization
  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
  Serial.println("failed! Is it inserted properly?");
    while (true) {}
  }
  Serial.println("OK!");

  // SD card initialization for raw reads
  Serial.print("Initializing SPI communication for raw reads...");
  if (!card.init(SPI_HALF_SPEED, SD_CS)) {
    Serial.println("failed! Is the card inserted properly?");
    while (true) {}
  } else {
    Serial.println("OK!");
    }

  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  MainMenu();    
}


void MainMenu(){
  /*
  This function displays the opening menu to the user
  This menu is touch sensitive to the user and magic numbers are introduced
  in this function to make the menu appear symmetrical and good-looking to
  the user playing the game

  Arguments:
    No arguments are provided.

  Returns:
    void
  */

  //The description of the program is provided
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(100,100);
  tft.setTextColor(TFT_RED);
  tft.setTextSize(5);
  tft.println("SNAKE GAME");

  //This is the first option provided to start the game
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(3);
  tft.setCursor(DISPLAY_WIDTH/2 -30,DISPLAY_HEIGHT/2);
  tft.drawRect(DISPLAY_WIDTH/2 - 80, DISPLAY_HEIGHT/2 -5, 200, 35, TFT_RED);
  tft.println("START");

  //This provides the name of the option that lets user to view his score
  tft.setCursor(DISPLAY_WIDTH/2 -70,DISPLAY_HEIGHT/2 +32.5);
  tft.drawRect(DISPLAY_WIDTH/2 -80, DISPLAY_HEIGHT/2 +25, 200, 35, TFT_RED);
  tft.println("LAST SCORE");

  //This gives an option to the user to log out of the game/program
  tft.setCursor(DISPLAY_WIDTH/2 -20,DISPLAY_HEIGHT/2 +65);
  tft.drawRect(DISPLAY_WIDTH/2 -80, DISPLAY_HEIGHT/2 +55, 200, 35, TFT_RED);
  tft.println("QUIT");
}


void loop() {
  /*
  This function displays the opening menu to the user
  This menu is touch sensitive to the user

  Arguments:
    No arguments are provided.

  Returns:
    void
  */

  int buttonVal = digitalRead(JOY_SEL);

  if (buttonVal == LOW) {
    MainMenu();
  }
  // query touchscreen and then reset pin states
  TSPoint touch = ts.getPoint(); // (x, y, pressure)
  pinMode(YP, OUTPUT);
  pinMode(XM, OUTPUT);
  

  if (touch.z >= MINPRESSURE && touch.z <= MAXPRESSURE) {
    int ptx = map(touch.y, TS_MINX, TS_MAXX, 0, DISPLAY_WIDTH);
    int pty = map(touch.x, TS_MINY, TS_MAXY, 0, DISPLAY_HEIGHT);


    if (ptx > DISPLAY_WIDTH/2 - DISPLAY_WIDTH/2 && ptx < DISPLAY_WIDTH/2 + DISPLAY_WIDTH/2) {
      if(pty > (DISPLAY_HEIGHT/2 -35) && pty < (DISPLAY_HEIGHT/2 +5)){ 
        GameSetup();

        while(PLAY){
          Play();
        }
        tft.fillRect(DISPLAY_WIDTH/2 -100,DISPLAY_HEIGHT/2, 220, 40, TFT_WHITE);
        delay(3000);
        MainMenu();
      }

      else if(pty > (DISPLAY_HEIGHT/4 +20) && pty < DISPLAY_HEIGHT/2 - 35){
        int point = -1;
        score(point);
      }
      else if(pty > (DISPLAY_HEIGHT/4 -20) && pty < (DISPLAY_HEIGHT/4 +20)){
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(DISPLAY_WIDTH/2-200,DISPLAY_HEIGHT/4);
        tft.setTextColor(TFT_WHITE);
        tft.setTextSize(3);
        tft.println("Thank you for playing.");
        tft.setTextSize(2);
        tft.setTextColor(TFT_YELLOW);
        tft.setCursor(DISPLAY_WIDTH/2-50,DISPLAY_HEIGHT/2+30);
        tft.println("Created by:");
        tft.setCursor(DISPLAY_WIDTH/2-200,DISPLAY_HEIGHT/2+50);
        tft.println("Al Hisham Anik & Sean Rutherford");
        delay(5000);
        tft.fillScreen(TFT_BLACK);
      }
    }
  }
}


void score(int point) {
  /*
  This function displays the opening menu to the user
  This menu is touch sensitive to the user

  Arguments:
    No arguments are provided.

  Returns:
    void
  */
  int value = point ;
  if (point==-1){
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(DISPLAY_WIDTH,DISPLAY_WIDTH);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(3);
    tft.println(value);
  }
}


void GameSetup() {
  /*
  This function displays the opening menu to the user
  This menu is touch sensitive to the user

  Arguments:
    No arguments are provided.

  Returns:
    void
  */

  init();

  Serial.begin(9600);

  pinMode(JOY_SEL, INPUT_PULLUP);

  //tft.reset(); // hardware reset
  uint16_t ID = tft.readID();    // read ID from display
  Serial.print("ID = 0x");
  Serial.println(ID, HEX);
  if (ID == 0xD3D3) ID = 0x9481; // write-o
  tft.begin(ID);

  randomSeed(analogRead(15)); 

  tft.setRotation(1);

  //staring snake segments length 6
  snake[0].setPos(12, 6);
  snake[1].setPos(13, 6);
  snake[2].setPos(14, 6);
  snake[3].setPos(15, 6);
  snake[4].setPos(16, 6);
  snake[5].setPos(17, 6);

  tft.fillScreen(TFT_BLACK);
  //draw head
  tft.fillRect(snake[0].xPos, snake[0].yPos, 16,16, TFT_RED);
  tft.drawRect(snake[0].xPos, snake[0].yPos, 16,16, TFT_BLACK);
  //draw body
  for(int i = 1; i < len; i++){
    tft.fillRect(snake[i].xPos, snake[i].yPos, 16,16, TFT_RED);
    tft.drawRect(snake[i].xPos, snake[i].yPos, 16,16, TFT_BLACK);
  }
    
  //initial food position, ensuring food is not on snake 
  FOODX = random(0,29);
  FOODY = random(0,19);
  while(true){
    for(int i = 0; i < len; i++){
      if(FOODX*16 == snake[i].xPos && FOODY*16 == snake[i].yPos){
        FOODX = random(1,29);
        FOODY = random(1,19);
        break;
      }
    }
    break;
  }
  //draw food
  tft.drawRect(16*FOODX,16*FOODY,16,16,TFT_GREEN);
}



void Play() { 
  /*
  This function is called when the user presses the start game option
  and as long as there is signal coming from the joystick this keeps
  works

  Arguments:
    No arguments are provided.

  Returns:
    void
  */

  // obtaing the jostick orientation value
  int xVal = analogRead(JOY_HORIZ);
  int yVal = analogRead(JOY_VERT);
  int buttonVal = digitalRead(JOY_SEL);

  // Next block of if-else statements manages the movement of joystick and gives the movement speed for the snake
  if(xVal < JOY_CENTER - JOY_DEADZONE && yVal < JOY_CENTER + JOY_DEADZONE && yVal > JOY_CENTER - JOY_DEADZONE){
    STARTED = true;
    xDir = 1;
    yDir = 0;
  }
  else if(xVal > JOY_CENTER + JOY_DEADZONE && yVal < JOY_CENTER + JOY_DEADZONE && yVal > JOY_CENTER - JOY_DEADZONE){
    STARTED = true;
    xDir = -1;
    yDir = 0;
  }
  else if(yVal > JOY_CENTER + JOY_DEADZONE && xVal < JOY_CENTER + JOY_DEADZONE && xVal > JOY_CENTER - JOY_DEADZONE){
    STARTED = true;
    xDir = 0;
    yDir = 1;
  }
  else if(yVal < JOY_CENTER - JOY_DEADZONE && xVal < JOY_CENTER + JOY_DEADZONE && xVal > JOY_CENTER - JOY_DEADZONE ){
    STARTED = true;
    xDir = 0;
    yDir = -1;
  }
    
  if(STARTED){
  Serial.println((snake[0].xPos + xDir)/16);
  Serial.println((snake[0].yPos + yDir)/16);

  if(snake[0].yPos + yDir*16 == 16*FOODY && snake[0].xPos + xDir*16 == 16*FOODX){
    snake[len].xPos = snake[len - 1].xPos;
    snake[len].yPos = snake[len - 1].yPos;
    ATE = true;
  }
  else{
    tft.fillRect(snake[len - 1].xPos, snake[len - 1].yPos, 16, 16, TFT_BLACK);
  }
  for(int i = len - 1; i > 0; i--){
    snake[i].xPos = snake[i - 1].xPos;
    snake[i].yPos = snake[i - 1].yPos;
  }
  int point = 0;
  if(ATE){
    point++;
    score(point);
    len++;
    tft.drawRect(16*FOODX,16*FOODY,16,16,TFT_BLACK);
    FOODX = random(1,29);
    FOODY = random(1,19);

    while(true){
      for(int i = 0; i < len; i++){
        if(FOODX*16 == snake[i].xPos && FOODY*16 == snake[i].yPos){
          FOODX = random(1,29);
          FOODY = random(1,19);
          break;
        }
      }
      break;
      point++;
    }
    tft.drawRect(16*FOODX,16*FOODY,16,16,TFT_GREEN);
    ATE = false;
    }

   snake[0].move(xDir, yDir);

  for(int i = 1; i < len; i++){
    if(snake[0].xPos == snake[i].xPos && snake[0].yPos == snake[i].yPos){
      PLAY = false;
      return 0;
    }
  }

  if(snake[0].xPos > 29*16 || snake[0].xPos < 0 || snake[0].yPos > 19*16 || snake[0].yPos < 0){
    PLAY = false;
    return 0;
  }

    
  tft.fillRect(snake[0].xPos, snake[0].yPos, 16, 16, TFT_RED);
  tft.drawRect(snake[0].xPos, snake[0].yPos, 16, 16, TFT_BLACK);

  score(point);
  }
  delay(100);
}



int main() {
  /*
  Main function that receives the user response and
  displays the output to the TFT display
  */

  //TFT display is intialized
  setup();

  //Window 1(user interface) is setup
  while (true) {
    loop();
    delay(5);
  }

  Serial.flush();
  Serial.end();
  return 0;
}
