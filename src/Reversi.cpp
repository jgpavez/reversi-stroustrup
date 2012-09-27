#include "Reversi.h"
#include "Table.h"
#include <iostream>

#ifndef MIN_DRAG_SIZE
#define MIN_DRAG_SIZE 64
#endif

Reversi::Reversi(){
	gui.main_window_show();
	gui.main_window_set_title("REVERSI/OTELLO GAME");

	table = new Table(&gui);
	players[0] = new Machine(BLACK,table);
	players[1] = new Person(WHITE,table);
	actual_player = WHITE;

	gui.mouse_button_released.connect(sigc::mem_fun(*this,&Reversi::on_mouse_button_released));
	gui.main_window_close.connect(sigc::mem_fun(*this,&Reversi::on_delete_event));

	for ( int r = 0; r < MAX_BOARD_ROWS; r++ ){
		for ( int c = 0; c < MAX_BOARD_COLUMNS; c++ ){
			gui.board_cell_set_pixmap(c,r,GREEN,4);
			gui.board_cell_set_drag_dest(c,r);
		}
	}
}

void Reversi::run(Gtk::Main &myapp, int argc, char *argv[]){
	gtk_app = &myapp;
	// Start main loop to receive events
	myapp.run();
}

void Reversi::on_mouse_button_released(int c, int r, int button){
	// This is just for fun, call delete it for performance
	if ( table->getValidMovesCount(actual_player) == 0 ) {
		gui.reset_image();
		return;
	} //
	if ( !table->isValidMove(c,r,actual_player)) return;
	players[(actual_player>0)?actual_player:(actual_player+1)]->makeMove(c,r);
	players[(actual_player>0)?actual_player:(actual_player+1)]->endTurn();
	actual_player = -(actual_player);
	if ( players[(actual_player>0)?actual_player:(actual_player+1)]->startTurn() ){	// The player have nothing more to do
		actual_player = -(actual_player);
	}
}

int Reversi::on_delete_event(){
	exit(0);
}

int main (int argc, char *argv[]){
	Gtk::Main myapp(argc, argv);
	Reversi reversi;
	reversi.run(myapp, argc, argv);
	return 0;
}
