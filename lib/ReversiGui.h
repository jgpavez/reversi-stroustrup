/**********************************************************************************************
 *  This class represent the graphic interface of the reversi game, is based in another interface
 *  of chess i found in the web, and modified for Gtk2.4.
 *  Use Gtk2.4, and sigc for events response, the number of cells is defined in ReversiIncludes
 *
 ************************************************************************************************/

#ifndef __REVERSIGUI_H__
#define __REVERSIGUI_H__

#include <sigc++/sigc++.h>
#include <list>
#include "ReversiIncludes.h"

using namespace std;
using namespace sigc;


#include <gtkmm/base.h>
#include <gtkmm/main.h>
#include <gtkmm/style.h>
#include <gtkmm/window.h>
#include <gtkmm/button.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/entry.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/entry.h>
#include <gtkmm/frame.h>
#include <gtkmm/table.h>
#include <gtkmm/textview.h>
#include <gtkmm/textbuffer.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/separator.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/aspectframe.h>

#include <gdkmm/dragcontext.h>
#include <gdkmm/pixmap.h>
#include <gdkmm/bitmap.h>

#include <gdkmm/pixbuf.h>

#define MAX_LAYERS        5
#define MAX_LAYER_VALUE   4

#define REVERSI_GUI_DND_TARGET "//jgpavezreversi//"
#define REVERSI_GUI_DND_INFO 0xdeadbeef

/*
 * Class for reversi graphic interface representation
 */
class ReversiGui : public Gtk::Window {
private:

	bool drag_success;
	int drag_target_c;
	int drag_target_r;
	int drag_source_c;
	int drag_source_r;
	int hot_x;
	int hot_y;

	Glib::RefPtr<Gdk::DragContext> drag_context;
	Glib::RefPtr<Gdk::Pixmap> p;
	Glib::RefPtr<Gdk::Bitmap> b;
	Glib::RefPtr<Gdk::Pixbuf> pb;

	std::list<Gtk::TargetEntry> chess_gui_target_entry;

	Gtk::VBox main_vbox;
	Gtk::Statusbar status_bar;
	Gtk::HBox hbox;

	Gtk::VBox left_panel;
	Gtk::Label label1;
	Gtk::Image *img;

	// This ones are for the board, controlled by the table class
	Gtk::AspectFrame *board_frame;
	Gtk::Table board_table;
	Gtk::DrawingArea cell[MAX_BOARD_COLUMNS][MAX_BOARD_ROWS];
	//-----------------------------------------------------------
	// Some properties of the cell
	Glib::RefPtr<Gdk::Pixbuf> cell_backing_store[MAX_BOARD_COLUMNS][MAX_BOARD_ROWS];
	Glib::RefPtr<Gdk::Pixbuf> cell_user_pixmap[MAX_BOARD_COLUMNS][MAX_BOARD_ROWS][MAX_LAYERS];
	int cell_user_pixmap_alpha[MAX_BOARD_COLUMNS][MAX_BOARD_ROWS][MAX_LAYERS];
	int cell_color_alpha[MAX_BOARD_COLUMNS][MAX_BOARD_ROWS][MAX_LAYERS];
	int cell_color[MAX_BOARD_COLUMNS][MAX_BOARD_ROWS][MAX_LAYERS];
	bool cell_user_visible[MAX_BOARD_COLUMNS][MAX_BOARD_ROWS][MAX_LAYERS];

protected:

	// This functions work in response of events of the user
	bool on_mouse_button_released(GdkEventButton *button, int c, int r);
	bool on_mouse_button_pressed(GdkEventButton *button, int c, int r);
	bool on_cell_expose(GdkEventExpose *event, int c, int r);
	bool on_delete_event(GdkEventAny*);
	bool on_cell_configure(GdkEventConfigure *ev, int c, int r);
	bool on_mouse_motion(GdkEventMotion *motion , int c, int r);

	// Drag is when the user moves a piece to other place, dragging, i think is not needed this time
	// but leave it just in case
	void on_cell_drag_begin(const Glib::RefPtr<Gdk::DragContext> &, int c, int r);
	void on_cell_drag_end  (const Glib::RefPtr<Gdk::DragContext> &, int c, int r);
	void on_cell_drag_leave (const Glib::RefPtr<Gdk::DragContext> &, guint, int c, int r);
	bool on_cell_drag_motion(const Glib::RefPtr<Gdk::DragContext> &, int, int, guint, int c, int r);
	bool on_cell_drag_drop  (const Glib::RefPtr<Gdk::DragContext> &, int, int, guint, int c, int r);

public:
	ReversiGui();
	~ReversiGui();

	void set_cells(Gtk::DrawingArea *cells);

	// Main Window Control
	void main_window_set_size(int width, int height);
	void main_window_set_title(const char *text);
	void main_window_set_active(bool active);
	void main_window_hide();
	void main_window_show();
	void reset_image();

	// Board cell control
	void board_cell_color( int column, int row, int color, int layer = MAX_LAYER_VALUE, int alpha = 255);
	void board_cell_set_pixmap( int c, int r, const gchar * const *data, int layer = 0, int alpha = 255);
	void board_cell_set_pixmap( int c, int r, const char *file, int layer = 0, int alpha = 255);

	void board_cell_clear_pixmap( int c, int r, int layer = -1);
	void board_cell_pixmap_show( int c, int r, int layer = -1);
	void board_cell_pixmap_hide( int c, int r, int layer = -1);
	int board_cell_get_width( int c, int r);
	int board_cell_get_height( int c, int r);

	// Drag response
	void board_cell_set_drag_source( int column, int row);
	void board_cell_unset_drag_source( int column, int row);
	void board_cell_set_drag_dest( int column, int row);
	void board_cell_unset_drag_dest( int column, int row);
	void board_cell_set_drag_cursor( int source_c, int source_r, int hot_spot_x, int hot_spot_y,
									 const gchar * const *pixmap, int width = 0, int height = 0);
	void board_cell_set_drag_cursor( int source_c, int source_r, int hot_spot_x, int hot_spot_y,
			 	 	 	 	 	 	 const char *pixmap, int width = 0, int height = 0);

	void board_cell_unset_drag_cursor( int hot_spot_x, int hot_spot_y, const gchar * const *pixmap);

	// This are signal to catch the events of the user
	signal0<int> 						main_window_close;
	signal2<void,int,int> 				cell_resized;
	signal3<void,int,int,int> 				mouse_button_released;
	signal4<void,int,int,int,int> 		mouse_button_pressed;
	signal2<void,int,int> 				mouse_motion;
	signal4<void,int,int,int,int> 		cell_drag_begin;
	signal2<void,int,int> 				cell_drag_enter;
	signal2<void,int,int> 				cell_drag_leave;
	signal2<void,int,int> 			   	cell_drag_motion;
	signal5<void,bool,int,int,int,int> 	cell_drag_drop;

};

#endif
