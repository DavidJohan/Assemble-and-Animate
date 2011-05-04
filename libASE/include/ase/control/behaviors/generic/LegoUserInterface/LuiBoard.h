
#ifndef LUIBOARD_H_
#define LUIBOARD_H_
typedef struct {
	char start;
	char clear;
	char record;
	char cTrain;
	char train;
	char behaviors[5];
} BoardState_t;


void LuiBoard_init(BoardState_t* board);
bool LuiBoard_isEmpty(BoardState_t* board);
bool LuiBoard_hasBehavior(BoardState_t* board) ;
char LuiBoard_behaviorOf(int index, BoardState_t* board);

#endif /* LUIBOARD_H_*/

