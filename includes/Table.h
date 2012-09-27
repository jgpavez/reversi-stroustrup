#ifndef __TABLE__
#define __TABLE__

#include "ReversiGui.h"
#include "ReversiIncludes.h"
#include <gdkmm/dragcontext.h>


class Table : public Gtk::Widget{
public:

	Table(){};
	Table(ReversiGui *gui_);
	Table(Table *t);
	void Restart();//throw badplace

	bool move(int c, int r, int color);
	bool simulMove(int c, int r, int color);
	bool isValidMove(int c, int r, int color) const;
	bool hasValidMove(int color) const;
	bool isOutFlanking(int color, int col, int row, int dc, int dr) const;
	bool isOutFlankable(int c, int r) const;
	int getPositionContent(int c, int r) const;
	int getValidMovesCount(int color) const;
	void update();

	void boardCellSetPixmap(int c, int r, const gchar * const *data, int layer = 0, int alpha = 255);
	void boardCellSetPixmap(int c, int r, const char *filename, int layer = 0, int alpha = 255);

	void boardCellClearPixmap(int c, int r, int layer = -1);
	void boardCellColor(int column, int row, int color,int layer = MAX_LAYER_VALUE, int alpha = 255);

	void placePiece(int c, int r, int token, int alpha=255);
	void clearPiece(int c, int r);
	void highlightSquare(int c, int r, int color);
	void unhighlightSquare( int c, int r);
	const char *getPiecePixmap(int token);
	int getBlackCount() const{ return blackCount; };
	int getWhiteCount() const{ return whiteCount; };
	int getEmptyCount() const{ return emptyCount; };
	int getBlackFrontierCount() const{ return blackFrontierCount; };
	int getWhiteFrontierCount() const{ return whiteFrontierCount; };
	int getBlackSafeCount() const{ return blackSafeCount; };
	int getWhiteSafeCount() const{ return whiteSafeCount; };
	int getMoveNumber() const{ return moveNumber; }


private:
	 ReversiGui *gui;
	 int board[MAX_BOARD_COLUMNS][MAX_BOARD_ROWS];
	 // parameters used by ReversiAI
	 bool safe[MAX_BOARD_COLUMNS][MAX_BOARD_ROWS];
	 int moveNumber;
	 int blackCount;
	 int whiteCount;
	 int emptyCount;
	 int blackFrontierCount;
	 int whiteFrontierCount;
	 int blackSafeCount;
	 int whiteSafeCount;
};

#endif
