#include "Machine.h"
#include "ReversiAI.h"

Machine::Machine(int color , Table *table):Player(color,table){
}
bool Machine::makeMove(int c , int r){
	table->move(c,r,color);
	return true;
}
void Machine::getNextMove(int &c, int &r){
	ReversiAI ai(table,color);
	//ComputerMove move = ai.bestMove();
}
int Machine::startTurn(){
	int lc, lr;
	ReversiAI ai(table,color);
	ai.getBestMove(lc,lr);
	makeMove(lc,lr);
	return 1;
}
int Machine::endTurn(){
	return 0;
}
