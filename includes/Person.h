#ifndef __PERSON__
#define __PERSON__
#include "Player.h"
#include "ReversiIncludes.h"
#include "ReversiGui.h"
#include "Table.h"

class Person:public Player{
public:
	Person(int color, Table *table);
	~Person(){};
	bool makeMove(int c, int r);
	int startTurn();
	int endTurn();

};
#endif
