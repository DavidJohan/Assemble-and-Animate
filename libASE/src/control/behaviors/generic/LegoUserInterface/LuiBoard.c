#include <stdlib.h>
#include <stdbool.h>
#include <ase/control/behaviors/generic/LegoUserInterface/LuiBoard.h>

void LuiBoard_init(BoardState_t* board) {
  	board->start = 1;
	board->clear = 0; 
	board->record = 0;
	board->cTrain = 0;
	board->train = 0;
	board->behaviors[0] = 0;
	board->behaviors[1] = 0;
	board->behaviors[2] = 0;
	board->behaviors[3] = 0;
	board->behaviors[4] = 0;
}

bool LuiBoard_isEmpty(BoardState_t* board) {
	bool empty = true;
	if(board->clear!=0) 	empty = false;
	if(board->record!=0) 	empty = false;
	if(board->train!=0)	empty = false;
	if(board->cTrain!=0)	empty = false;
	int i;
	for(i=0;i<board->behaviors[i];i++) {
		if(board->behaviors[i] != 0) {
			empty = false;
		}
	}
	return empty;
}

bool LuiBoard_hasBehavior(BoardState_t* board) {
  	bool hasBehavior = false;
	int i;
	for(i=0;i<board->behaviors[i];i++) {
		if(board->behaviors[i] != 0) {
			hasBehavior = true;
		}
	}
	return hasBehavior;
}
char LuiBoard_behaviorOf(int index, BoardState_t* board) {
  	if(index<5) {
	  return board->behaviors[index];
	}
    	if(index==5) return board->start;
	if(index==6) return board->clear;
	if(index==7) return board->record;
	if(index==8) return board->train;
	if(index==9) return board->cTrain;
	return 255;
}
