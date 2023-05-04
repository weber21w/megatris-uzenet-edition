#ifndef MEGATRIS_H
#define MEGATRIS_H
struct tetraminoStruct {
  s8 size;
  const char blocks[4][16];

  };

/*
About this program:
-------------------

The game's main complication is the "multi-tasking" aspect. 2 players (game fields)
must play simultaneously, at there own speed, plus all the
animations. To avoid using a RTOS or develop some complex task
scheduler, a simpler approach was taken. A global "active field" variable
"f" is used by all functions implicated in parallel execution. That
way, both field code can be executed sequentially. A master loop call in turn
both fields. A state machine enable to have "non-blocking" animations. That
is, if player 1 "Tetris" animation is playing (and the next block is frozen), 
player 2 continues playing, unnafected. 

Concerning graphics, the main screen uses a special map, called the main map.
Its a map the size of the VRAM (40x28) and is used with a special function
called RestoreTile. When drawing a map data is copied from ROM to RAM, During
gameplay a tetramino is drawn in RAM and overwrites the previous map data. To
avoid needing more RAM to save previous image data, the RestoreTile function 
restores a tile from its map at a specific (x,y) location.

*/

const struct tetraminoStruct tetraminos[] PROGMEM={

	//Tetrimino: S
	{3,
	{{0,1,1,
	 1,1,0,
	 0,0,0},

	 {0,1,0,
	  0,1,1,
	  0,0,1},

	 {0,0,0,
	  0,1,1,
	  1,1,0},

	 {1,0,0,
	  1,1,0,
	  0,1,0}}
	},

	//J
	{3,

	{{2,0,0,
	 2,2,2,
	 0,0,0},

	 {0,2,2,
	  0,2,0,
	  0,2,0},

	 {0,0,0,
	  2,2,2,
	  0,0,2},

	 {0,2,0,
	  0,2,0,
	  2,2,0}}
	},

	//Tetrimino: L
 	{3,
	{{0,0,3,
	 3,3,3,
	 0,0,0},

	 {0,3,0,
	  0,3,0,
	  0,3,3},

	 {0,0,0,
	  3,3,3,
	  3,0,0},

	 {3,3,0,
	  0,3,0,
	  0,3,0}}
	},

	//Tetrimino: O
	{2,
	{{4,4,
	 4,4},

	{4,4,
	 4,4},

	{4,4,
	 4,4},

	{4,4,
	 4,4}}
	},

	//Tetrimino: T	
	{3,
	{{0,5,0,
	 5,5,5,
	 0,0,0},

	 {0,5,0,
	  0,5,5,
	  0,5,0},

	 {0,0,0,
	  5,5,5,
	  0,5,0},

	 {0,5,0,
	  5,5,0,
	  0,5,0}}
	},

	//I
	{4,
   {{0,0,0,0,
	 6,6,6,6,
	 0,0,0,0,
	 0,0,0,0},

	{0,0,6,0,
	 0,0,6,0,
	 0,0,6,0,
	 0,0,6,0},
	
	{0,0,0,0,
	 0,0,0,0,
	 6,6,6,6,
	 0,0,0,0},

	{0,6,0,0,
	 0,6,0,0,
	 0,6,0,0,
	 0,6,0,0}}

	},


	//Tetrimino: Z
	{3,
	{{7,7,0,
	 0,7,7,
	 0,0,0},

	 {0,0,7,
	  0,7,7,
	  0,7,0},

	 {0,0,0,
	  7,7,0,
	  0,7,7},

	 {0,7,0,
	  7,7,0,
	  7,0,0}}
	}
	
			


  };

#define O_TETRAMINO 3
#define T_TETRAMINO 4
#define I_TRETRAMINO 5
#define BG_TILE 21
#define CURSOR_TILE 11

#define LOCK_DELAY 15*2
#define SOFT_DROP_DELAY 1*2
#define ANIMATE_LOCK 1
#define ANIMATE_NONE 0

#define FIELD_HEIGHT 22
#define FIELD_WIDTH 10
#define NEW_BLOCK -1
#define NO_BLOCK -1
#define UNDEFINED -1
#define INACTIVE -1
#define GHOST_TILE 19
#define LINES_PER_LEVEL 10

#define ANIM_BACK_TO_BACK 0
#define ANIM_T_SPIN 1
#define ANIM_TETRIS 2

#define GARBAGE_ALIGNMENT_PROBABILITY 70*2

#define DEF_AUTO_REPEAT_DELAY 3*2 //doubled due to change in FAST_VSYNC...
#define MIN_AUTO_REPEAT_DELAY 1
#define MAX_AUTO_REPEAT_DELAY 12*2

#define DEF_INTER_REPEAT_DELAY 1*2
#define MIN_INTER_REPEAT_DELAY 1
#define MAX_INTER_REPEAT_DELAY 6*2
 
#define DEF_START_LEVEL 10 //halved due to change in FAST_VSYNC...
#define MIN_START_LEVEL 1
#define MAX_START_LEVEL 30

//declare custom assembly functions
extern void RestoreTile(char x,char y);
extern void LoadMap(void);
extern void SetTileMap(const int *data);

void StartSongNo(u8 songNo);
void drawTetramino(s8 x,s8 y,s8 tetramino,s8 rotation,s8 forceTile,u8 restore,u8 clipToField);
u8 processGravity(u8 p);
u8 processControls(u8 p);
u8 updateField(u8 p);
void initFields(void);
u8 moveBlock(u8 p, s8 dispX,s8 dispY);
void issueNewBlock(u8 p);
u8 lockBlock(u8 p);
void doGameOver(u8 p);
u8 rotateBlock(u8 p, s8 newRotation);
u8 lineCleared(u8 p, s8 y);
u8 fitCheck(u8 p, s8 x,s8 y,s8 block,s8 rotation);
void hardDrop(u8 p);
void fillFieldLine(u8 p, s8 y,u8 tile);
void copyFieldLines(u8 p, s8 rowSource,s8 rowDest,s8 len);
void fill(s8 x,s8 y,s8 width,s8 height,u8 tile);
void updateStats(s16 lines,s16 softDropTiles,s16 hardDropTiles,s16 tSpin);
void hold(u8 p);
void updateGhostPiece(u8 p, u8 restore);
void restore(s16 x,s16 y,s16 width,s16 height);
s8 randomize(void);
u8 processGarbage(u8 p);
void runStateMachine(u8 p);
u8 animField(u8 p);
void WaitVsyncAndProcessAnimations(u8 count);
u8 doGameContinue(void);
u8 MainMenu(void);
u8 OptionsMenu();
void restoreFields(void);
int ConnectMenu();

void processAnimations(u8 f);

const u8 sin_tablex[] PROGMEM = {
0x12,0x15,0x18,0x1b,0x1e,0x20,0x22,0x23,
0x24,0x24,0x24,0x23,0x22,0x20,0x1e,0x1b,
0x18,0x15,0x12,0xf,0xc,0x9,0x6,0x4,
0x2,0x1,0x0,0x0,0x0,0x1,0x2,0x4,
0x6,0x9,0xc,0xf,0x12,
};

const u8 sin_tabley[] PROGMEM = {
0xc,0xf,0x12,0x14,0x16,0x18,0x18,0x18,
0x16,0x14,0x12,0xf,0xc,0x9,0x6,0x4,
0x2,0x0,0x0,0x0,0x2,0x4,0x6,0x9,
};
struct fieldStruct {
	u8 currentState;
	u8 subState;	
	u8 lastClearCount;
	u8 hardDroppped;
	s8 nextBlock;
	u8 currBlock;
	s8 holdBlock;
	s8 currBlockSize;
	s8 currBlockX;
	s8 currBlockY;
	u8 currBlockRotation;
	s8 ghostBlockY;
	s8 left;
	s8 right;
	s8 bottom;
	s8 top;
	u8 locking;
	s8 currLockDelay;
	s8 gravity;			//the delay in frames between blocks moving down
	s8 currGravity;
	unsigned long score;
	unsigned int lines;
	unsigned int nextLevel;
	u8 height;
	u8 level;
	u8 startLevel;
	u8 gameOver;
	u8 canHold;
	unsigned int lastButtons;
	unsigned int currButtons;
	u8 autoRepeatDelay;
	u8 interRepeatDelay;
	u8 maxAutoRepeatDelay;
	u8 maxInterRepeatDelay;
	u8 softDropDelay;
	u8 kickUp;
	u8 noGhostBlock;
	u8 backToBack;
	u8 garbageQueue;
	u8 surface[22][10];
	u8 nextBlockPosX;
	u8 nextBlockPosY;
	u8 holdBlockPosX;
	u8 holdBlockPosY;
	u8 bag[7];			//the random bag of pieces
	u8 bagPos;
	u8 tSpin;		   //t-spin detected on last lock 1,2,3 (or zero=no t-spin)
	u8 lastOpIsRotation;		//true if last move was a rotation (gravity, or move -> 0)
	u8 tSpinAnimFrame;
	u8 backToBackAnimFrame;
	u8 tetrisAnimFrame;
	u8 lastHole;
	unsigned int padState;
	unsigned int lastPadState;
};

struct fieldStruct fields[2];

const char strGameOver[] PROGMEM ="GAME OVER!";
const char strYouWin[] PROGMEM ="YOU WIN!";
const char strYouLose[] PROGMEM ="YOU LOSE!!";

//import logo data
#include "data/uzeboxlogo_6x8.pic.inc"
#include "data/uzeboxlogo_6x8.map.inc"	

//import tunes
#include "data/music.inc"

//import tiles & maps
#include "data/font.inc"
#include "data/graphics.inc"

//import patches
#include "data/patches.inc"

//Global config
u8 vsMode;
u8 restart;

//u8 f=0; 			//active field

u8 songNo=0;	//default song
#endif
