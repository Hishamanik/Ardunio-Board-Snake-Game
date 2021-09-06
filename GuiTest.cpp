#include <Arduino.h>
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

// restuarant data and struct initialization

void MainMenu();
void setup() {
    /*
        Initializes physical components and serial communication
        Initializes display with black screen to "clear"
        Followed by YEG map and initial cursor displayed
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
    tft.setCursor(100,100);
    tft.setTextColor(TFT_RED);
    tft.setTextSize(5);
    tft.println("SNAKE GAME");

    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(3);

    tft.setCursor(DISPLAY_WIDTH/2 -30,DISPLAY_HEIGHT/2);
    tft.drawRect(DISPLAY_WIDTH/2 - 80, DISPLAY_HEIGHT/2 -5, 200, 35 , TFT_RED);
    tft.println("START");

    tft.setCursor(DISPLAY_WIDTH/2 -70,DISPLAY_HEIGHT/2 +32.5);
    tft.drawRect(DISPLAY_WIDTH/2 -80, DISPLAY_HEIGHT/2 +25, 200, 35 , TFT_RED);
    tft.println("DIFFICULTY");

    tft.setCursor(DISPLAY_WIDTH/2 -20,DISPLAY_HEIGHT/2 +65);
    tft.drawRect(DISPLAY_WIDTH/2 -80, DISPLAY_HEIGHT/2 +55, 200, 35 , TFT_RED);
    tft.println("QUIT");
}

void DifficultyMenu(){
	tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(3);

    tft.setCursor(3*DISPLAY_WIDTH/4,3*DISPLAY_HEIGHT/4);
    tft.drawRect(3*DISPLAY_WIDTH/4,3*DISPLAY_HEIGHT/4, 200, 35 , TFT_RED);
    tft.println("EASY");

    tft.setCursor(DISPLAY_WIDTH/2 -70,3*DISPLAY_HEIGHT/4);
    tft.drawRect(DISPLAY_WIDTH/2 -70,3*DISPLAY_HEIGHT/4, 200, 35 , TFT_RED);
    tft.println("MEDIUM");

    tft.setCursor(DISPLAY_WIDTH/4,3*DISPLAY_HEIGHT/4);
    tft.drawRect(DISPLAY_WIDTH/4,3*DISPLAY_HEIGHT/4, 200, 35 , TFT_RED);
    tft.println("HARD");
}

void loop() {

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

    /*Serial.print("x = ");
    Serial.println(ptx);
    Serial.print("y = ");
    Serial.println(pty);*/
    if (ptx > DISPLAY_WIDTH/2 - DISPLAY_WIDTH/2 && ptx < DISPLAY_WIDTH/2 + DISPLAY_WIDTH/2) {
        if(pty > (DISPLAY_HEIGHT/2 -35) && pty < (DISPLAY_HEIGHT/2 +5)){ 
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(100,100);
            tft.setTextColor(TFT_RED);
            tft.setTextSize(5);
            tft.println("SNAKE GAME");
        }
        else if(pty > (DISPLAY_HEIGHT/4 +20) && pty < DISPLAY_HEIGHT/2 - 35){
            DifficultyMenu();
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




int main() {
    setup();
    while (true) {
     loop();
     delay(5);
    }

    Serial.flush();
    Serial.end();
    return 0;
}