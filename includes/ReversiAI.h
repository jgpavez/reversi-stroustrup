#ifndef __REVERSI_AI__
#define __REVERSI_AI__

#include "Table.h"
#include "limits.h"

struct ComputerMove{
	int row,col;
	int rank;
	ComputerMove(int c, int r, int rank_ = 0){
		col 	= c;
		row 	= r;
		rank 	= rank_;
	};
};
class ReversiAI{
public:
	ReversiAI(){};
	ReversiAI(Table *board,int color);
	void calculateComputerMove(int &r, int &c);
	void getBestMove(int &c, int &r);
	ComputerMove bestMove(Table *board,int color, int depth, int alpha, int beta);
private:
	static const int forfeitWeight 		= 35;
	static const int frontierWeight 	= 10;
	static const int mobilityWeight 	= 5;
	static const int stabilityWeight 	= 50;
	static const int maxRank 			= INT_MAX - 64;
	int maxDepth;
	Table *board_;
	int color_;
};



#endif
