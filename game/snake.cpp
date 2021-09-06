#define SD_CS 10
#define JOY_VERT  A9 // should connect A9 to pin VRx
#define JOY_HORIZ A8 // should connect A8 to pin VRy
#define JOY_SEL   53

#include "snakeSeg.h"

#include <Arduino.h>

// core graphics library (written by Adafruit)
#include <Adafruit_GFX.h>

// Hardware-specific graphics library for MCU Friend 3.5" TFT LCD shield
#include <MCUFRIEND_kbv.h>

MCUFRIEND_kbv tft;

#define DISPLAY_WIDTH  480
#define DISPLAY_HEIGHT 320

#define JOY_CENTER   512
#define JOY_DEADZONE 32

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
	init();

  	Serial.begin(9600);

	pinMode(JOY_SEL, INPUT_PULLUP);

	//    tft.reset();             // hardware reset
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

	//long snake for testing
	/*snake[0].setPos(0, 6);
	snake[1].setPos(1, 6);
	snake[2].setPos(2, 6);
	snake[3].setPos(3, 6);
	snake[4].setPos(4, 6);
	snake[5].setPos(5, 6);
	snake[6].setPos(6, 6);
	snake[7].setPos(7, 6);
	snake[8].setPos(8, 6);
	snake[9].setPos(9, 6);
	snake[10].setPos(10, 6);
	snake[11].setPos(11, 6);
	snake[12].setPos(12, 6);
	snake[13].setPos(13, 6);
	snake[14].setPos(14, 6);
	snake[15].setPos(15, 6);
	snake[16].setPos(16, 6);
	snake[17].setPos(17, 6);
	snake[18].setPos(18, 6);
	snake[19].setPos(19, 6);
	snake[20].setPos(20, 6);
	snake[21].setPos(21, 6);
	snake[22].setPos(22, 6);
	snake[23].setPos(23, 6);
	snake[24].setPos(24, 6);
	snake[25].setPos(25, 6);
	snake[26].setPos(26, 6);
	snake[27].setPos(27, 6);
	snake[28].setPos(28, 6);
	snake[29].setPos(29, 6);*/



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
  				//tft.drawCircle(200,200,9,TFT_RED);
  				break;
  			}
  		}
  		break;
  	}

  	tft.drawRect(16*FOODX,16*FOODY,16,16,TFT_GREEN);








}


void Play() {
	


	int xVal = analogRead(JOY_HORIZ);
  	int yVal = analogRead(JOY_VERT);
  	int buttonVal = digitalRead(JOY_SEL);


  	if(xVal < JOY_CENTER - JOY_DEADZONE && yVal < JOY_CENTER + JOY_DEADZONE && yVal > JOY_CENTER - JOY_DEADZONE){
  		STARTED = true;//tft.fillRect(snake.xPos, snake.yPos, 16, 16, TFT_BLACK);
  		xDir = 1;
  		yDir = 0;

  	}
  	else if(xVal > JOY_CENTER + JOY_DEADZONE && yVal < JOY_CENTER + JOY_DEADZONE && yVal > JOY_CENTER - JOY_DEADZONE){
  		STARTED = true;//tft.fillRect(snake.xPos, snake.yPos, 16, 16, TFT_BLACK);
  		xDir = -1;
  		yDir = 0;

  	}
  	else if(yVal > JOY_CENTER + JOY_DEADZONE && xVal < JOY_CENTER + JOY_DEADZONE && xVal > JOY_CENTER - JOY_DEADZONE){
  		STARTED = true;//tft.fillRect(snake.xPos, snake.yPos, 16, 16, TFT_BLACK);
  		xDir = 0;
  		yDir = 1;
  	}
  	else if(yVal < JOY_CENTER - JOY_DEADZONE && xVal < JOY_CENTER + JOY_DEADZONE && xVal > JOY_CENTER - JOY_DEADZONE ){
  		STARTED = true;//tft.fillRect(snake.xPos, snake.yPos, 16, 16, TFT_BLACK);
  		xDir = 0;
  		yDir = -1;

  	}

  	
  	if(STARTED){

  	//snake[0].move(xDir, yDir);
  	Serial.println((snake[0].xPos + xDir)/16);
  	Serial.println((snake[0].yPos + yDir)/16);

  	if(snake[0].yPos + yDir*16 == 16*FOODY && snake[0].xPos + xDir*16 == 16*FOODX){
  		snake[len].xPos = snake[len - 1].xPos;
  		snake[len].yPos = snake[len - 1].yPos;
  		ATE = true;
  		//Serial.println("ATE");//len++;
  	}
  	else{
  		tft.fillRect(snake[len - 1].xPos, snake[len - 1].yPos, 16, 16, TFT_BLACK);

  	}
  	//tft.fillRect(snake[5].xPos, snake[5].yPos, 16, 16, TFT_BLACK);
  	
  	//snake[0].move(xDir, yDir);
  	for(int i = len - 1; i > 0; i--){
  		snake[i].xPos = snake[i - 1].xPos;
  		snake[i].yPos = snake[i - 1].yPos;

  	}

  	if(ATE){
  		len++;
  		tft.drawRect(16*FOODX,16*FOODY,16,16,TFT_BLACK);
  		FOODX = random(1,29);
  		FOODY = random(1,19);
  		while(true){
  			for(int i = 0; i < len; i++){
  				if(FOODX*16 == snake[i].xPos && FOODY*16 == snake[i].yPos){
  					FOODX = random(1,29);
  					FOODY = random(1,19);
  					//tft.drawCircle(200,200,9,TFT_RED);
  					break;
  				}
  			}
  			break;
  		}
  		tft.drawRect(16*FOODX,16*FOODY,16,16,TFT_GREEN);


  		//to print score(do later)
  		/*tft.setCursor(20, );
  		setTextColor(uint16_t color);
  		setTextColor(uint16_t color, uint16_t backgroundcolor);
  		setTextSize(uint8_t size);void setTextWrap(boolean w);
  		tft.print("(len - 6)*10")*/
  		ATE = false;
  	}

  	//for(int i = 0; i < 6; i++){
  	//Serial.print("X: ");
  	//Serial.println(snake[i].xPos);
  	//Serial.print("Y: ");
  	//Serial.println(snake[i].yPos);
  	
    //}

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

  	
  	//tft.drawRect(16*FOODX,16*FOODY,16,16,TFT_GREEN);
  	tft.fillRect(snake[0].xPos, snake[0].yPos, 16, 16, TFT_RED);
  	tft.drawRect(snake[0].xPos, snake[0].yPos, 16, 16, TFT_BLACK);

  	

  	

  }
  	




  	delay(100);
}




int main() {
	setup();

	while(PLAY){
		Play();
	}
	tft.drawCircle(200,100,9,TFT_BLUE);



  

	Serial.end();
	return 0;
}


