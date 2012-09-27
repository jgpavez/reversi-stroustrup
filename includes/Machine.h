#ifndef __MACHINE__
#define __MACHINE__
#include "Player.h"
#include "ReversiIncludes.h"
#include "Table.h"

class Machine:public Player{
public:
	Machine(int color, Table *table);
	~Machine(){};
	bool makeMove(int c, int r);
	void getNextMove(int &c, int &r);
	int startTurn();
	int endTurn();
};
#endif
