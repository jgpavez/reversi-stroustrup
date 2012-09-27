CC = g++
FLAGS = -Wall -g
CFLAGS = `pkg-config gtkmm-2.4 --cflags` -Iimages
LIBS = `pkg-config gtkmm-2.4 --libs`
EXE_NAME = reversi
LIBRARY= lib/libreversi.so

OBJ_FILES = lib/reversigui.o \

INC_DIR = -Iincludes/

LIB_DIR = -Ilib/

SRC_FILES = src/Reversi.cpp src/Table.cpp src/Player.cpp src/Machine.cpp src/Person.cpp src/ReversiAI.cpp

bin: $(EXE_NAME)

$(EXE_NAME): $(LIBRARY) $(SRC_FILES)
	$(CC) $(FLAGS) -o $(EXE_NAME) $(SRC_FILES) $(LIBRARY) $(INC_DIR) $(LIB_DIR) $(CFLAGS) $(LIBS)

lib: $(LIBRARY)

$(LIBRARY): $(OBJ_FILES)
	g++ -shared -o $(LIBRARY) $(OBJ_FILES) 
	
lib/reversigui.o:	lib/ReversiGui.cpp lib/ReversiGui.h
		$(CC) -c -fPIC $(INC_DIR) lib/ReversiGui.cpp -o lib/reversigui.o $(CFLAGS)

clean:
	-rm $(EXE_NAME)
	-rm $(LIBRARY)
	-rm $(OBJ_FILES)
	-rm src/*~
	
# DO NOT DELETE

