#include "ReversiGui.h"

#include <glibmm/ustring.h>

#ifndef CELL_SIZE
#define CELL_SIZE 32
#endif

#ifndef SCALING_METHOD
#define SCALING_METHOD Gdk::INTERP_BILINEAR
#endif

// Start gui, link the boxes and cells
ReversiGui::ReversiGui(){
	gdk_rgb_init();
	set_title("REVERSI/OTELLO Game");

	main_vbox.show();
	add(main_vbox);

	hbox.show();
	main_vbox.pack_start(hbox,Gtk::PACK_EXPAND_WIDGET,0);
	hbox.set_border_width(5);

	img = new Gtk::Image("images/trollface.png");
	img->show();
	hbox.pack_start(*img,Gtk::PACK_EXPAND_WIDGET,0);

	board_frame = new Gtk::AspectFrame(Glib::ustring(""),0.5,0.5,1,false);
	board_frame->show();
	board_frame->set_border_width(5);
	board_frame->set_shadow_type(Gtk::SHADOW_OUT);
	hbox.pack_start(*board_frame,Gtk::PACK_EXPAND_WIDGET,0);

	board_table.resize(MAX_BOARD_ROWS,MAX_BOARD_COLUMNS);
	board_table.set_homogeneous(true);
	board_table.set_spacings(0);
	board_table.show();
	board_frame->add(board_table);

	// Link the cells to the user events responses
	for ( int c = 0; c < MAX_BOARD_COLUMNS; c++ ){
		for ( int r = 0; r < MAX_BOARD_ROWS; r++ ){
			cell[c][r].set_size_request(CELL_SIZE,CELL_SIZE);
			cell[c][r].show();
			cell[c][r].set_events(Gdk::EXPOSURE_MASK
								  |Gdk::LEAVE_NOTIFY_MASK
								  |Gdk::BUTTON_PRESS_MASK
								  |Gdk::BUTTON_RELEASE_MASK
								  |Gdk::POINTER_MOTION_MASK
								  |Gdk::ENTER_NOTIFY_MASK
								  |Gdk::LEAVE_NOTIFY_MASK);
			cell[c][r].signal_configure_event().connect(sigc::bind(sigc::mem_fun(*this,&ReversiGui::on_cell_configure),c,r));
			cell[c][r].signal_expose_event().connect(sigc::bind(sigc::mem_fun(*this,&ReversiGui::on_cell_expose),c,r));
			cell[c][r].signal_motion_notify_event().connect(sigc::bind(sigc::mem_fun(*this,&ReversiGui::on_mouse_motion),c,r));
			cell[c][r].signal_button_release_event().connect(sigc::bind(sigc::mem_fun(*this,&ReversiGui::on_mouse_button_released),c,r));
			cell[c][r].signal_button_press_event().connect(sigc::bind(sigc::mem_fun(*this,&ReversiGui::on_mouse_button_pressed),c,r));
			cell[c][r].signal_drag_begin().connect(sigc::bind(sigc::mem_fun(*this,&ReversiGui::on_cell_drag_begin),c,r));
			cell[c][r].signal_drag_motion().connect(sigc::bind(sigc::mem_fun(*this,&ReversiGui::on_cell_drag_motion),c,r));
			cell[c][r].signal_drag_end().connect(sigc::bind(sigc::mem_fun(*this,&ReversiGui::on_cell_drag_end),c,r));
			cell[c][r].signal_drag_leave().connect(sigc::bind(sigc::mem_fun(*this,&ReversiGui::on_cell_drag_leave),c,r));
			cell[c][r].signal_drag_drop().connect(sigc::bind(sigc::mem_fun(*this,&ReversiGui::on_cell_drag_drop),c,r));

			board_table.attach(cell[c][r],c,c+1,r,r+1);

			for ( int i = 0; i < MAX_LAYERS; i++ ){
				cell_user_visible[c][r][i] = false;
				cell_color_alpha[c][r][i]   = 0;
			}
		}
	}

	chess_gui_target_entry.push_back( Gtk::TargetEntry(REVERSI_GUI_DND_TARGET, Gtk::TargetFlags(0), REVERSI_GUI_DND_INFO));

}

ReversiGui::~ReversiGui(){
	delete board_frame;
}

// Main window control
void ReversiGui::main_window_set_size(int width, int height){
	set_size_request(width,height);
}
void ReversiGui::main_window_set_title(const char *text){
	set_title(text);
}
void ReversiGui::main_window_set_active(bool active){
	set_sensitive(active);
}
void ReversiGui::main_window_hide(){
	hide();
}
void ReversiGui::main_window_show(){
	show();
}
void ReversiGui::reset_image(){
	img->set("images/trollface.gif");
}
bool ReversiGui::on_mouse_button_released(GdkEventButton *button, int c, int r){

	mouse_button_released.emit(c,r,button->button);
	return true;
}

// Events response
bool ReversiGui::on_mouse_button_pressed(GdkEventButton *button, int c, int r){
	int type;
	hot_x = (int)button->x;
	hot_y = (int)button->y;
	mouse_button_pressed.emit(c,r,type,button->button);
	return false;
}
bool ReversiGui::on_cell_expose(GdkEventExpose *event, int c, int r){
	int width,height;
	Glib::RefPtr<Gdk::GC> my_gc;
	my_gc = cell[c][r].get_style()->get_fg_gc(cell[c][r].get_state());
	cell[c][r].get_window()->get_size(width,height);

	cell_backing_store[c][r]->render_to_drawable(cell[c][r].get_window(),my_gc,0,0,0,0,width,height,Gdk::RGB_DITHER_NORMAL,0,0);
	return false;
}
bool ReversiGui::on_delete_event(GdkEventAny*){
	return main_window_close.emit();
}

bool ReversiGui::on_cell_configure( GdkEventConfigure *ev, int c, int r){
	Glib::RefPtr<Gdk::GC> my_gc;
	Gdk::Color my_color;
	int x, y;
	int width, height;
	int width_1, height_1;
	Glib::RefPtr<Gdk::Pixbuf> scaled;
	guint32 color;
	guchar *colordata;

	cell[c][r].get_window()->get_size(width,height);
	cell_backing_store[c][r] = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB,false,8,width,height);
	cell_backing_store[c][r]->fill(0);

	for ( int i = MAX_LAYERS-1; i >= 0; i-- ){
		if ( cell_user_pixmap[c][r][i] && cell_user_visible[c][r][i]){
			scaled = cell_user_pixmap[c][r][i]->scale_simple(width,height,Gdk::INTERP_NEAREST);
			scaled->composite(cell_backing_store[c][r],0,0,width,height,0,0,1,1,SCALING_METHOD,cell_user_pixmap_alpha[c][r][i]);
		}

	}
	//no agregue una parte
	if ( c == 0 && r == 0 ){
		cell[c][r].get_window()->get_size(width,height);
		cell_resized.emit(width,height);
	}
	return false;

}

bool ReversiGui::on_mouse_motion(GdkEventMotion *motion , int c, int r){
	return true;
}

// Cell properties control
void ReversiGui::board_cell_color(int c, int r, int color, int layer, int alpha){
	cell_color[c][r][layer] = color;
	cell_color_alpha[c][r][layer] = alpha;
	on_cell_configure(NULL,c,r);
	cell[c][r].queue_draw();
}
void ReversiGui::board_cell_set_pixmap( int c, int r, const gchar * const *data, int layer, int alpha){
	if ( layer >= MAX_LAYERS ) return;
	cell_user_pixmap[c][r][layer] = Gdk::Pixbuf::create_from_xpm_data ((const char **)data);
	cell_user_pixmap_alpha[c][r][layer] = alpha;
	cell_user_visible[c][r][layer] = true;

	on_cell_configure(NULL,c,r);
	cell[c][r].queue_draw();
}
void ReversiGui::board_cell_set_pixmap( int c, int r, const char *file, int layer, int alpha){
	if ( layer >= MAX_LAYERS ) return;

	cell_user_pixmap[c][r][layer] = Gdk::Pixbuf::create_from_file(file);
	cell_user_pixmap_alpha[c][r][layer]=alpha;
	cell_user_visible[c][r][layer]=true;
	on_cell_configure(NULL,c,r);
	cell[c][r].queue_draw();
}

void ReversiGui::board_cell_clear_pixmap( int c, int r, int layer ){
	if ( layer >= MAX_LAYERS ) return;

	if ( layer < 0 ){
		for ( int i = 0; i < MAX_LAYERS; i++ ){
			cell_user_visible[c][r][i] = false;
		}
	}else{
		cell_user_visible[c][r][layer] = false;
	}
	on_cell_configure(NULL,c,r);
	cell[c][r].queue_draw();
}

void ReversiGui::board_cell_pixmap_show( int c, int r, int layer ){
	if(layer >= MAX_LAYERS) return;

	if( layer < 0 ){
		for( int i = 0; i < MAX_LAYERS; i++){
	         cell_user_visible[c][r][i] = true;
	    }
	}else{
	    cell_user_visible[c][r][layer] = true;
	}

	on_cell_configure(NULL,c,r);
	cell[c][r].queue_draw();
}

void ReversiGui::board_cell_pixmap_hide(int c, int r, int layer){
	if(layer >= MAX_LAYERS) return;

	if( layer < 0 ){
		for( int i = 0; i < MAX_LAYERS; i++){
	         cell_user_visible[c][r][i] = false;
	    }
	}else{
	    cell_user_visible[c][r][layer] = false;
	}

	on_cell_configure(NULL,c,r);
	cell[c][r].queue_draw();
}

int ReversiGui::board_cell_get_width(int c, int r){
	int width, height;
	cell[c][r].get_window()->get_size(width,height);
	return width;
}

int ReversiGui::board_cell_get_height(int c, int r){
	int width, height;
	cell[c][r].get_window()->get_size(width,height);
	return height;
}

// Drag functions
void ReversiGui::on_cell_drag_begin(const Glib::RefPtr<Gdk::DragContext> &context, int c, int r){

   int hotx, hoty;

   drag_success = true;

   drag_source_c = c;
   drag_source_r = r;

   drag_target_c = drag_source_c;
   drag_target_r = drag_source_r;

   drag_context = context;

   cell_drag_begin.emit(c,r,hot_x, hot_y);

}

void ReversiGui::on_cell_drag_end(const Glib::RefPtr<Gdk::DragContext> &context, int c, int r){

	if(drag_source_c == drag_target_c && drag_source_r == drag_target_r){
      cell_drag_drop.emit(false,drag_source_c,drag_source_r,drag_target_c,drag_target_r);
	}else{
      cell_drag_drop.emit(true,drag_source_c,drag_source_r,drag_target_c,drag_target_r);
	}

}

void ReversiGui::on_cell_drag_leave(const Glib::RefPtr<Gdk::DragContext> &context, guint, int c, int r){

	drag_target_c=drag_source_c;
	drag_target_r=drag_source_r;

	cell_drag_leave.emit(c,r);

}

bool ReversiGui::on_cell_drag_motion( const Glib::RefPtr<Gdk::DragContext> &context, int x, int y, guint time, int c, int r){
	if ( c != drag_target_c || r != drag_target_r ){
		cell_drag_enter.emit(c,r);
	}
	cell_drag_motion.emit(c,r);
	drag_target_c = c;
	drag_target_r = r;
	return false;
}
bool ReversiGui::on_cell_drag_drop( const Glib::RefPtr<Gdk::DragContext> &context, int x, int y, guint time, int c, int r){
		drag_target_c = c;
		drag_target_r = r;
		return true;
}

void ReversiGui::board_cell_set_drag_source( int c, int r ){
	cell[c][r].drag_source_set(chess_gui_target_entry, Gdk::BUTTON1_MASK, Gdk::ACTION_COPY);
}
void ReversiGui::board_cell_unset_drag_source( int c, int r ){
	cell[c][r].drag_source_unset();
}
void ReversiGui::board_cell_set_drag_dest( int c, int r){
	cell[c][r].drag_dest_set(chess_gui_target_entry, Gtk::DEST_DEFAULT_ALL, Gdk::ACTION_COPY );
}
void ReversiGui::board_cell_unset_drag_dest(int c, int r){
   cell[c][r].drag_dest_unset();
}
void ReversiGui::board_cell_set_drag_cursor(int c, int r, int hot_spot_x, int hot_spot_y, const gchar *  const *pixmap, int width, int height){
	Glib::RefPtr<Gdk::Colormap> cm;
	int hotx, hoty;
	cm = get_window()->get_colormap();
	p  = Gdk::Pixmap::create_from_xpm(get_window()->get_colormap(),b,pixmap);
	int pwidth, pheight;
	p->get_size(pwidth, pheight);

	if ( width && height ){
		hotx = hot_spot_x - (width-pwidth)/2;
		hoty = hot_spot_y - (height-pheight)/2;
	} else {
		hotx = hot_spot_x;
		hoty = hot_spot_y;
	}
	g_print("%d,%d\n",hotx,hoty);

	drag_context->set_icon(cm,p,b,hotx,hoty);
}

void ReversiGui::board_cell_set_drag_cursor(int c, int r, int hot_spot_x, int hot_spot_y, const char *file, int width, int height){
	Glib::RefPtr<Gdk::Pixbuf> scaled;
	scaled = Gdk::Pixbuf::create_from_file(file);
	pb = scaled->scale_simple(width,height,SCALING_METHOD);
	drag_context->set_icon(pb,hot_spot_x,hot_spot_y);
}
void ReversiGui::board_cell_unset_drag_cursor(int hot_spot_x, int hot_spot_y, const gchar *const *pixmap){

}
