#include "ReversiAI.h"
#include <stdlib.h>
#include <cmath>
#include <iostream>
using namespace std;

ReversiAI::ReversiAI(Table *board, int color):maxDepth(3),board_(board),color_(color){
	if ( board_->getMoveNumber() >= 55 - 3 ){ maxDepth = board_->getEmptyCount(); }
};

ComputerMove ReversiAI::bestMove(Table *board,int color, int depth, int alpha, int beta){
	// Alpha-Beta algorithm that calculates the best move for the computer player
	// Alpha-Beta algorithm derives from max-min, this algorithm look ahead the next moves in the board
	// Considering that the both players plays the move that maximize his gain( one maximize and the other
	// minimize the gain of the enemy), the alpha beta cutoff allow to eliminate some branches on the
	// tree that are not used. Because you cant look until the end of the game, only simulate some rounds
	// in future and calculate the quality on the board in that round, to make a choice, for this is used
	// some parameters like mobility, gained coins, etc.

	ComputerMove rbestMove(-1,-1);
	rbestMove.rank = -color * maxRank;
	int validMoves = board->getValidMovesCount(color);
	int rowStart = rand()%8;
	int colStart = rand()%8;
	for ( int i = 0; i < 8; i ++){
		for ( int j = 0; j < 8; j++){
			int col = (colStart + i)%8;
			int row = (rowStart + j)%8;
			if (board->isValidMove(col, row, color)){
				ComputerMove testMove(col,row);
				Table *testBoard = new Table(board);
				testBoard->simulMove(testMove.col,testMove.row,color);
				int score = testBoard->getWhiteCount()-testBoard->getBlackCount();
				int nextColor 	= -color;
				int forfeit 	= 0;
				bool isEndGame 	= false;
				int opponentValidMoves = testBoard->getValidMovesCount(nextColor);
				if (opponentValidMoves == 0){
					forfeit = color;
					nextColor = -nextColor;
					if	( !testBoard->hasValidMove(nextColor)) isEndGame = true;
				}
				if ( isEndGame || depth == maxDepth ){
					if ( isEndGame ){
						// Black win , positive score
						if ( score < 0 )
							testMove.rank = -maxRank + score;
						// White win, negative score
						else if ( score > 0 )
							testMove.rank = maxRank + score;
						// Draw
						else testMove.rank = 0;
					}
					// Game not ended, calculate the move rank ( rank of the actual state of the board )
					// Formula for rank in each movement
					else{
						testMove.rank = forfeitWeight * forfeit
									+ frontierWeight  * ( testBoard->getBlackFrontierCount() - testBoard->getWhiteFrontierCount())
									+ mobilityWeight  * color * ( validMoves - opponentValidMoves)
									+ stabilityWeight * ( testBoard->getWhiteSafeCount() - testBoard->getBlackSafeCount())
									+ score;
					}
				}
				else {
					ComputerMove nextMove = bestMove(testBoard,nextColor, depth + 1, alpha, beta);
					testMove.rank = nextMove.rank;
					// Forfeit is cumulative
					if ( forfeit != 0 && abs(testMove.rank) < maxRank)
						testMove.rank += forfeitWeight * forfeit;
					// Adjust the alpha and beta values
					if ( color == WHITE && testMove.rank > beta)
						beta = testMove.rank;
					if ( color == BLACK && testMove.rank < alpha)
						alpha = testMove.rank;
				}
				if ( color == WHITE && testMove.rank > alpha ){
					testMove.rank = alpha;
					delete testBoard;
					return testMove;
				}
				if ( color == BLACK && testMove.rank < beta ){
					testMove.rank = beta;
					delete testBoard;
					return testMove;
				}
				// First move tested, assume is the best
				if ( rbestMove.row < 0 ) rbestMove = testMove;
				// Otherwise take the best move between the best and the test
				else if ( color * testMove.rank > color * rbestMove.rank ){
					rbestMove = testMove;
				}
				delete testBoard;
			}
		}
	}
	return rbestMove;
}


void ReversiAI::getBestMove(int &c, int &r){
	int alpha =	maxRank + 64;
	int beta  = -alpha;
	ComputerMove temp =  bestMove(board_,color_, 1,alpha,beta);
	c = temp.col;
	r = temp.row;
}

