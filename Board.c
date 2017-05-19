#include "NXTServo-lib-UW.c"
#include "AI.c"
#include "NXT_FileIO.c"
#include "PC_FileIO.c"


//const float SVMY = 0.00891816783986232;
//const float SVMX = 0.00941395494556475;

void liftPen()
{
	setServoPosition(S4, 1, 20);
	wait1Msec(200);
}

void lowerPen()
{
	setServoPosition(S4, 1, -20);
	wait1Msec(200);
}

void motorOff(){
	motor[motorA] = 0;
	motor[motorB] = 0;
	motor[motorC] = 0;
}

void setYPower(int power){
	motor[motorA] = -power;
	motor[motorC] = -power;
}

void setXPower (int power){
	motor[motorB] = -power;
}

void resetX(float *pos){
	liftPen();
	setXPower(-100);
	while (SensorValue[S2]==0){}
	setXPower(0);
	nMotorEncoder[motorB]=0;
	pos[0]=0;
}

void resetY(float *pos){
	liftPen();
	setYPower(-100);
	while (SensorValue[S3]==0){}
	setYPower(0);
	nMotorEncoder[motorA]=0;
	pos[1]=0;
}

void zero(float *pos){
	resetX(pos);
	resetY(pos);
}

void moveDisplacement(float xCoord, float yCoord, float *pos, int power = 50){

	nMotorEncoder[motorA]=0;
	nMotorEncoder[motorB]=0;

	if (xCoord < 0)	setXPower(-power);
	else if (xCoord > 0) setXPower(power);
	if (yCoord < 0)	setYPower(-power);
	else if (yCoord > 0) setYPower(power);


	while (abs(nMotorEncoder[motorA]) < abs(yCoord) || abs(nMotorEncoder[motorB]) < abs(xCoord)){}
	motorOff();

	pos[0] += xCoord;
	pos[1] += yCoord;
}

void moveTo(float xCoord, float yCoord, float *pos){
	//moveDisplacement(xCoord-pos[0], yCoord-pos[1], pos);
	zero(pos);
	moveDisplacement(xCoord, 0, pos);
	moveDisplacement(0, yCoord, pos);
	pos[0] = xCoord;
	pos[1] = yCoord;
}
//Yuanpei
int checkColor(int x, int y, float *pos)
{
	moveTo(150+300*x+50, 150+300*y+220 ,pos);
	wait1Msec(200);
	if (SensorValue[S1] == 5){
		displayString(0,"red");
		wait1Msec(1000);
		eraseDisplay();
		return 1;
	}
	return 0;
}

void updateBoardColor(float *pos){
	for (int i = 0; i<3; i++){
		for (int j = 0; j<3; j++ ){
			if (readBoard[i][j] == 0) readBoard[i][j] = checkColor(i,j,pos);
		}
	}
}
/*

void drawCircleOnBoard(float x0, float y0, float *pos){	//abs location of centre of circle
	zero(pos);
	liftPen();
	float r = 50, dx = r/10;
	float x = -r, y = y0, dyArr[10];
	moveTo(x0-r, y0, pos);	//haven't tested this yet
	lowerPen();
	for (int i=0; i<10; i++){
		x = x + (r/10);
		float yi = y;
		y = sqrt( r*r - x*x);
		float dy = y - yi;
		dyArr[i] = dy;
		moveDisplacement(dx,dy, pos);
	}
	for (int i=0; i<10; i++)
		moveDisplacement(dx,-dyArr[9-i], pos);
	for (int i=0; i<10; i++)
		moveDisplacement(-dx,-dyArr[i], pos);
	for (int i=0; i<10; i++)
		moveDisplacement(-dx,dyArr[9-i], pos);

	liftPen();
}
*/

void drawLineAbs(float xCoord1, float yCoord1, float xCoord2, float yCoord2, float * pos){
	moveTo(xCoord1,yCoord1, pos);
	lowerPen();
	moveDisplacement(xCoord2 - xCoord1,yCoord2 - yCoord1,pos);
	liftPen();
	wait1Msec(300);
}

void drawLineRel(float xCoord, float yCoord, float * pos){
	lowerPen();
	moveDisplacement(xCoord, yCoord, pos);
	motorOff();
	liftPen();
	wait1Msec(300);
}

void square(float length, float * pos){
	liftPen();
	moveDisplacement(-length/2,length/2,pos);
	lowerPen();
	moveDisplacement(length,0,pos,20);
	wait1Msec(200);
	moveDisplacement(0,-length,pos,20);
	wait1Msec(200);
	moveDisplacement(-length,0,pos,20);
	wait1Msec(200);
	moveDisplacement(0,length,pos,20);
	wait1Msec(200);
	liftPen();
	moveDisplacement(length/2,-length/2,pos);
	wait1Msec(200);
}

void drawFrame(float *pos){
	zero(pos);
  	drawLineAbs(0,300,900,300,pos);
	drawLineAbs(0,600,900,600,pos);
	drawLineAbs(300,0,300,900,pos);
	drawLineAbs(600,0,600,900,pos);

	for (int i = 0; i<3; i++){
		for(int j = 0; j<3; j++){
			moveTo(150+300*i,150+300*j,pos);
			square(100,pos);
		}
	}
}

//Just for calibration
void goToBoxes(float *pos){
	zero(pos);
	for (int i = 0; i<3; i++){
		for(int j = 0; j<3; j++){
			moveTo(150+300*i+50, 150+300*j+220 ,pos);
			wait1Msec(1000);
		}
	}
}

void drawX(float length, float *pos){
	moveDisplacement(-length/2,-length/2,pos);
	drawLineRel(length,length,pos);
	moveDisplacement(0,-length,pos);
	drawLineRel(-length,length,pos);
	liftPen();
	moveDisplacement(length/2,-length/2,pos);
}

void plotMove(int box, float * pos){
	int i = box/3;
	int j = box%3;
	moveTo(150+300*i, 150+300*j, pos);
	drawX(100, pos);
	readBoard[i][j] = 2;
	board[i][j] = 2;
}

void drawEndCondition(float *pos, int endC)
{
	zero(pos);
	if (endC == 0)
	{
		// User Lost robot never loses
		moveTo(100, 1000, pos); // U
		drawLineRel(0, 200, pos);
		drawLineRel(100, 0, pos);
		drawLineRel(0, -200, pos);
		moveDisplacement(50, 0, pos); // L
		drawLineRel(0, 200, pos);
		drawLineRel(100, 0, pos);
		moveDisplacement(50, 0, pos); // O
		drawLineRel(0, -200, pos);
		drawLineRel(100, 0, pos);
		drawLineRel(0, 200, pos);
		drawLineRel(-100, 0, pos);
		moveDisplacement(250, -200, pos); // S
		drawLineRel(-100, 0, pos);
		drawLineRel(0, 100, pos);
		drawLineRel(100, 0, pos);
		drawLineRel(0, 100, pos);
		drawLineRel(-100, 0, pos);
		moveDisplacement(250, 0, pos); // E
		drawLineRel(-100, 0, pos);
		drawLineRel(0, -200, pos);
		drawLineRel(200, 0, pos);
		moveDisplacement(-100, 100, pos);
		drawLineRel(100, 0, pos);
	}
	else if (endC == 1)
	{
		// Cheat
		moveTo(450,450,pos);
		drawX(900,pos);
		drawEndCondition(pos, 0);
	}
	else if (endC == 2)
	{
		// Tie
		moveTo(100, 1000, pos); // T
		drawLineRel(100, 0, pos);
		moveDisplacement(-50, 0, pos);
		drawLineRel(0, 200, pos);
		moveDisplacement(100, 0, pos); // I
		drawLineRel(0, -200, pos);
		moveDisplacement(150, 200, pos); // E
		drawLineRel(-100, 0, pos);
		drawLineRel(0, -200, pos);
		drawLineRel(200, 0, pos);
		moveDisplacement(-100, 100, pos);
		drawLineRel(100, 0, pos);

	}
}

void init(float *pos)
{
	SensorType[S1] = sensorColorNxtFULL;
	SensorType[S2] = sensorTouch;
	SensorType[S3] = sensorTouch;
	SensorType[S4] = sensorI2CCustom9V;

	liftPen();

	// Goes to a distant location to draw a square for the user to put in the card
	zero(pos);
	moveTo(100,1500,pos);
	square(300,pos);
	zero(pos);
	displayString(0, "Place Game card");
	displayString(1, "in the square then");
	displayString(2, "press the middle button");
	while(nNxtButtonPressed != 3);
	moveTo(150,1720 ,pos);
	while(SensorValue[S1] != 5){}
	drawFrame(pos);

}

task main()
{
	bool gameStart = 1;
	while(gameStart){
		for (int i = 0; i < 3; i++){
			for (int j = 0; j < 3; j++){
				board[i][j] = 0;
				readBoard[i][j] = 0;
			}
		}
		float pos[2] = {0,0};
		init(pos);

		TFileHandle fout;
		word fileSize = 5000;
		bool fileOkay = openWritePC(fout, "fileWrite.txt", fileSize);

		int nextMove = -1, winner = -1;
		bool cheated = false;

		for (int i = 0; i < 9 && winner == -1 && !cheated; i++)
		{
			if (i % 2 == 0)
			{
				// Player move
				//zero(pos);
				moveTo(0,1800,pos);
				while(nNxtButtonPressed != 3){}
				updateBoardColor(pos);
				cheated = checkCheat();
				if (!cheated)
				{
					// set board to readBoard
					for (int i = 0; i < 3; i++)
					{
						for (int j = 0; j < 3; j++)
						{
							board[i][j] = readBoard[i][j];
						}
					}
				}
			}
			else
			{
				// Robot move
				nextMove = computeNextMove();
				plotMove(nextMove, pos);
			}
			winner = checkWin();

			printBoard(fout);
			writeEndlPC(fout);
		}
		zero(pos);
		if (!cheated){
			if (winner == 1)
				displayString(0, "Player won");
			else if (winner == 2){
				displayString(0,"Robot won");
				drawEndCondition(pos, 0);
			}
			else
			{
				displayString(0,"Tie");
				drawEndCondition(pos, 2);
			}
		}
		else
		{
			eraseDisplay();
			displayString(0, "You cheated. Robot won");
			drawEndCondition(pos, 1);
		}
		wait1Msec(5000);

		motorOff();

		eraseDisplay();
		displayString(0, "Erase the board");
		displayString(1, "Centre restart");
		displayString(2, "Other button quit");
		while (nNxtButtonPressed == -1){}
		if (nNxtButtonPressed!=3) gameStart = 0;

	}
}
