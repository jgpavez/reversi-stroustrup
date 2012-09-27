#include "Table.h"


Table::Table(ReversiGui *gui_){
	gui = gui_;
	for ( int i = 0; i < MAX_BOARD_COLUMNS; i++ ){
		for ( int j = 0; j < MAX_BOARD_ROWS; j++ ){
			board[i][j] = EMPTY;
			safe[i][j] 	= false;
		}
	}
	moveNumber = 1;
	placePiece(MAX_BOARD_COLUMNS/2-1,MAX_BOARD_ROWS/2-1,WHITE);
	board[MAX_BOARD_COLUMNS/2-1][MAX_BOARD_ROWS/2-1] = WHITE;
	placePiece(MAX_BOARD_COLUMNS/2,MAX_BOARD_ROWS/2,WHITE);
	board[MAX_BOARD_COLUMNS/2][MAX_BOARD_ROWS/2] = WHITE;
	placePiece(MAX_BOARD_COLUMNS/2,MAX_BOARD_ROWS/2-1,BLACK);
	board[MAX_BOARD_COLUMNS/2][MAX_BOARD_ROWS/2-1] = BLACK;
	placePiece(MAX_BOARD_COLUMNS/2-1,MAX_BOARD_ROWS/2,BLACK);
	board[MAX_BOARD_COLUMNS/2-1][MAX_BOARD_ROWS/2] = BLACK;
	update();

}

Table::Table(Table *t){
	for ( int i = 0; i < MAX_BOARD_COLUMNS; i++ ){
		for ( int j = 0; j < MAX_BOARD_ROWS; j++ ){
			board[i][j] = t->board[i][j];
			safe[i][j]  = t->safe[i][j];
		}
	}
	blackCount 			= t->getBlackCount();
	whiteCount 			= t->getWhiteCount();
	emptyCount 			= t->getEmptyCount();
	blackSafeCount 		= t->getBlackSafeCount();
	whiteSafeCount 		= t->getWhiteSafeCount();
	blackFrontierCount 	= t->getBlackFrontierCount();
	whiteFrontierCount 	= t->getWhiteFrontierCount();
}

void Table::placePiece(int c, int r, int token, int alpha){
	gui->board_cell_set_pixmap(c, r, getPiecePixmap(token),0,alpha);
	gui->board_cell_unset_drag_dest(c,r);
	gui->board_cell_set_drag_source(c,r);
}
void Table::clearPiece( int c, int r ){
	board[c][r] = EMPTY;
	gui->board_cell_clear_pixmap(c,r,0);
}

void Table::highlightSquare(int c, int r, int color){
	gui->board_cell_color(c,r,color,1,150);
}
void Table::unhighlightSquare(int c, int r){
	gui->board_cell_color(c,r,0,1,0);
}

const char *Table::getPiecePixmap(int token){
	switch(token){
	case WHITE: return WHITE_TOKEN;
	case BLACK: return BLACK_TOKEN;
	}
	return 0;
}
int Table::getPositionContent(int c, int r) const{
	return board[c][r];
}
int Table::getValidMovesCount(int color) const{
	int n = 0;
	for (int i = 0; i < MAX_BOARD_COLUMNS; i++){
		for ( int j = 0; j < MAX_BOARD_ROWS; j++){
			if (isValidMove(i,j,color)) n++;
		}
	}
	return n;
}
void Table::boardCellSetPixmap(int c, int r, const gchar * const *data,int layer, int alpha){
	gui->board_cell_set_pixmap(c, r, data, layer, alpha);
}

void Table::boardCellSetPixmap(int c, int r, const char *filename, int layer, int alpha){
	gui->board_cell_set_pixmap(c, r, filename, layer, alpha);
}

void Table::boardCellClearPixmap (int c, int r, int layer){
	gui->board_cell_clear_pixmap(c, r, layer);
}

void Table::boardCellColor(int column, int row, int color, int layer, int alpha){
	gui->board_cell_color(column, row, color, layer, alpha);
}
bool Table::simulMove(int c, int r, int color){
	board[c][r] = color;
		int dc,dr;
		int col,row;
		// Flip oponent coins
		for ( dc = -1; dc <= 1; dc++ ){
			for ( dr = -1; dr <= 1; dr++ ){
				if ( !(dc == 0 && dr == 0) && isOutFlanking(color,c,r,dc,dr)){ // If your coin is outflankable in that direction
					col = c + dc;
					row = r + dr;
					while ( board[col][row] == -color ){
					board[col][row] = color;
					col += dc;
					row += dr;
					}
				}
			}
		}
		moveNumber++;
		update();
		return true;
}
bool Table::move(int c, int r, int color){
	board[c][r] = color;
	placePiece(c, r, color);
	int dc,dr;
	int col,row;
	// Flip oponent coins
	for ( dc = -1; dc <= 1; dc++ ){
		for ( dr = -1; dr <= 1; dr++ ){
			if ( !(dc == 0 && dr == 0) && isOutFlanking(color,c,r,dc,dr)){ // If your coin is outflankable in that direction
				col = c + dc;
				row = r + dr;
				while ( board[col][row] == -color ){
				board[col][row] = color;
				placePiece(col,row,color);
				col += dc;
				row += dr;
				}
			}
		}
	}
	moveNumber++;
	update();
	return true;
}

bool Table::hasValidMove(int color) const{
	for (int i = 0; i < MAX_BOARD_COLUMNS; i++){
			for ( int j = 0; j < MAX_BOARD_ROWS; j++){
				if (isValidMove(i,j,color)) return true;
			}
		}
	return false;
}
bool Table::isValidMove(int c, int r, int color) const{
	if ( board[c][r] != EMPTY )
		return false;							// square must be empty
	for ( int dc = -1; dc <= 1; dc++ ){
		for ( int dr = -1; dr <= 1; dr++ ){
			if (!( dc == 0 && dr == 0 ) && isOutFlanking(color, c, r, dc, dr))// Must be able to flip at least one opponent disc
				return true;
		}
	}
	return false;
}

bool Table::isOutFlanking(int color, int col, int row, int dc, int dr) const{
	// If flip an or more enemy coin, so is out flanking
	int c = col + dc;
	int r = row + dr;
	// Move in the direction as long as we stay on the board and land on a disc of opossite color
	while ( c >= 0 && c < 8 && r >= 0 && r < 8 && board[c][r] == -color ){
		c += dc;
		r += dr;
	}
	// if we ran out of board, moved one space or didn't land on a disc of the same color, false
	if ( c < 0 || c > 7 || r < 0 || r > 7 || (c - dc == col && r - dr == row ) || board[c][r] != color)
		return false;

	return true;
}
bool Table::isOutFlankable(int c, int r) const{
	// check if a disk is outflankable
	// Is outflankable if there is an empty square in both sides, or if there is an empty square on one
	// side and an opponent or unsafe disc of the same color on the other side

	int color = board[c][r];
	bool space1,space2,unsafe1,unsafe2;

	// first check the horizontal line
	space1 = false; space2 = false; unsafe1 = false; unsafe2 = false;
	// West
	for ( int j = 0; j < c && !space1; j++ ){
		if (board[j][r] == EMPTY) space1 = true;
		else if (board[j][r] != color || !safe[j][r]) unsafe1 = true;
	}
	// East
	for ( int j = c + 1; j < 8 && !space2; j++ ){
		if (board[j][r] == EMPTY) space2 = true;
		else if (board[j][r] != color || !safe[j][r]) unsafe2 = true;
	}
	if ((space1 && space2) || (space1 && unsafe2) || (unsafe1 && space2)) return true;

	// Now the vertical line
	space1 = false; space2 = false; unsafe1 = false; unsafe2 = false;
	// North
	for ( int i = 0; i < r && !space1; i++ ){
		if ( board[c][i] == EMPTY ) space1 = true;
		else if ( board[c][i] != color || !safe[c][i]) unsafe1 = true;
	}
	// South
	for ( int i = r + 1; i < 8 && !space2; i++ ){
		if ( board[c][i] == EMPTY ) space2 = true;
		else if ( board[c][i] != color || !safe[c][i]) unsafe2 = true;
	}
	if ((space1 && space2) || (space1 && unsafe2) || (unsafe1 && space2)) return true;

	// NorthWest-Southeast diagonal
	space1 = false; space2 = false; unsafe1 = false; unsafe1 = false;
	// NorthWest
	int i = c-1;
	int j = r-1;
	while ( i >= 0 && j >= 0 && !space1 ){
		if (board[i][j] == EMPTY) space1 = true;
		else if (board[i][j] != color || !safe[i][j]) unsafe1 = true;
		i--;
		j--;
	}
	// Southeast
	i = c + 1;
	j = r + 1;
	while ( i < 8 && j < 8 && !space2){
		if (board[i][j] == EMPTY) space2 = true;
		else if (board[i][j] != color || !safe[i][j]) unsafe2 = true;
		i++;
		j++;
	}
	if ((space1 && space2) || (space1 && unsafe2) || (unsafe1 && space2)) return true;

	// Northeast-southwest diagonal line
	space1 = false; space2 = false; unsafe1 = false; unsafe2 = false;
	// Northeast
	i = c - 1;
	j = r + 1;
	while (i >= 0 && j < 8 && !space1){
		if ( board[i][j] == EMPTY) space1 = true;
		else if ( board[i][j] != color || !safe[i][j] ) unsafe1 = true;
		i--;
		j++;
	}
	// SouthWest
	i = c + 1;
	j = r - 1;
	while ( i < 8 && j >= 0 && !space2 ){
		if ( board[i][j] == EMPTY ) space2 = true;
		else if ( board[i][j] != color || !safe[i][j]) unsafe2 = true;
		i++;
		j--;
	}
	if ((space1 && space2) || (space1 && unsafe2) || (unsafe1 && space2)) return true;

	// cannot be outflanked
	return false;
}

void Table::update(){
	// Update Counts on the board, mainly used by reversiAI
	blackCount 			= 0;
	whiteCount 			= 0;
	emptyCount 			= 0;
	blackFrontierCount 	= 0;
	whiteFrontierCount 	= 0;
	blackSafeCount  	= 0;
	whiteSafeCount 		= 0;
	bool statusChanged = true;
	// first check the unsafe discs, if there are still outflankable. Those are not are marked as safe.
	// If new safe discs were found, the process is repeated, because this can change the state of other discs
	// repeat until no safe discs are found
	while ( statusChanged ){
		statusChanged = false;
		for ( int i = 0; i < 8; i++ ){
			for ( int j = 0; j < 8; j++ ){
				if ( board[i][j] != EMPTY && !safe[i][j] && !isOutFlankable(i,j)){
					safe[i][j] = true;
					statusChanged = true;
				}
			}
		}
	}
	for ( int i = 0; i < 8; i++ ){
		for ( int j = 0; j < 8; j++ ){
			bool isFrontier = false;
			if ( board[i][j] != EMPTY ){
				for ( int dc = -1; dc <= 1; dc++ ){
					for ( int dr = -1; dr <= 1; dr++ ){
						if ( !( dr == 0 && dc == 0 ) && i + dc >= 0 && i + dc < 8 && j + dr >= 0 && j + dr < 8
								&& board[i + dc][j + dr] == EMPTY)
							isFrontier = true;
					}
				}
			}
			if ( board[i][j] == BLACK ){
				blackCount++;
				if ( isFrontier ) blackFrontierCount++;
				if ( safe[i][j] ) blackSafeCount++;
			}
			else if ( board[i][j] == WHITE ){
				whiteCount++;
				if ( isFrontier ) whiteFrontierCount++;
				if ( safe[i][j] ) whiteSafeCount++;
			}
			else emptyCount++;
		}
	}
}
