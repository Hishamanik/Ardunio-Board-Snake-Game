#include "snakeSeg.h"

SnakeSeg::SnakeSeg(){

};

void SnakeSeg::setPos(int X, int Y){
	xPos = 16*X;
	yPos = 16*Y;
};

void SnakeSeg::move(int X, int Y){

	xPos = xPos + X*16;
	yPos = yPos + Y*16;


};
