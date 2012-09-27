#ifndef __PLAYER__
#define __PLAYER__

#include "ReversiGui.h"
#include "Table.h"

class Player{
public:
	Player(int color, Table *table);
	~Player(){};
	virtual int startTurn() = 0;
	virtual int endTurn() = 0;
	virtual bool makeMove(int x, int y) = 0; //Throw badplace
	void giveUp();
	bool test();
	int getColor(){ return this->color; }
	const char *getName();

protected:
	Table *table;
	int color;
};
inline const char *Player::getName(){
	switch(color){
	case WHITE:
		return "WHITE";
		break;
	case BLACK:
		return "BLACK";
		break;
	}
}

#endif
