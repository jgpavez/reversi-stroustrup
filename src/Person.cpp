#include "Person.h"
#include <sigc++/sigc++.h>

Person::Person(int color, Table *table):Player(color,table){
}
int Person::startTurn(){
	return 0;
}
bool Person::makeMove(int c, int r){
	return table->move(c,r,color);
}
int Person::endTurn(){
	return 1;
}
