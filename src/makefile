CC = g++
FLAGS = -Wall -g
CFLAGS = `pkg-config gtkmm-2.4 --cflags` -Iimages
LIBS = `pkg-config gtkmm-2.4 --libs`
EXE_NAME = reversi
REVERSI_HOME = $HOME/Escritorio/reversi/
LIBRARY= $(REVERSI_HOME)lib/libreversi.so

OBJ_FILES = $(REVERSI_HOME)lib/reversigui.o \
SRC_FILES = $(REVERSI_HOME)src/Reversi.cpp \

INC_DIR = -I includes/

LIB_DIR = -I lib/

bin: $(EXE_NAME)

$(EXE_NAME): $(LIBRARY) $(REVERSI_HOME)src/Reversi.cpp $(REVERSI_HOME)includes/Reversi.h
	$(CC) $(FLAGS) -o $(EXE_NAME) $(SRC_FILES) $(LIBRARY) $(INC_DIR) $(LIB_DIR) $(CFLAGS) $(LIBS)

lib: $(LIBRARY)

$(LIBRARY): $(OBJ_FILES)
	g++ -shared -o $(LIBRARY) $(OBJ_FILES)
	
lib/reversigui.o:	$(REVERSI_HOME)lib/ReversiGui.cpp $(REVERSI_HOME)lib/ReversiGui.h
		$(CC) -c -fPIC $(REVERSI_HOME)lib/ReversiGui.cpp -o $(REVERSI_HOME)lib/ReversiGui.o $(CFLAGS)

clean:
	-rm $(EXE_NAME)
	-rm $(LIBRARY)
	-rm $(OBJ_FILES)
	-rm src/*~
	
# DO NOT DELETE

