#ifndef __REVERSI__
#define __REVERSI__
#include "ReversiGui.h"
#include "ReversiIncludes.h"
#include "Table.h"
#include "Player.h"
#include "Person.h"
#include "Machine.h"
#include <stdio.h>

class Reversi : public Gtk::Widget{
public:
	Reversi();
	void run( Gtk::Main &myapp, int argc, char *argv[]);
	bool Test();

protected:
	Gtk::Main *gtk_app;
	ReversiGui gui;

	void on_mouse_button_released( int c, int r, int button );
	int on_delete_event();

private:
	Player *players[2];
	Table  *table;
	int actual_player;

	friend class Table;
};

#endif
