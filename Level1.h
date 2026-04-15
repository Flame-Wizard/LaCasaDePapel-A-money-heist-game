#ifndef LEVEL1_H
#define LEVEL1_H



/* ================================================================
LEVEL-1 DEFINES
================================================================ */
#define TARGET_LOCKER 0
#define TARGET_DOOR   1

/* ================================================================
LEVEL-1 VARIABLES
================================================================ */
int  winTimer = 0;
int  winBg;
int  indicatorTarget = TARGET_LOCKER;

/* level-1 locker */
bool lockerActive = true;
int  lockerTileX = MAP_MAX_X;
int  lockerTileY = MAP_MAX_Y;
int  lockerX = SCREEN_WIDTH - 120;
int  lockerY = SCREEN_HEIGHT - 150;

/* level-1 puzzle */
int  patternA = 0, patternMul = 0, puzzleAnswer = 0;
char puzzleInput[32] = "";
int  puzzleLen = 0;
int  puzzleTime = 600;

/* level-1 door */
bool doorActive = false;
int  doorTileX = 0, doorTileY = 0;
int  doorX = 0, doorY = 0;
int  doorTimer = 0;
int  doorDuration = 600;
int  nextDoorTime = 1000;
bool levelComplete = false;
bool enteringLevel2 = false;

/* ================================================================
LEVEL-1 FUNCTIONS
================================================================ */
void generatePattern(void)
{
	patternA = 2 + rand() % 5;
	patternMul = 2 + rand() % 3;
	puzzleAnswer = patternA * patternMul * patternMul * patternMul;
	puzzleLen = 0;
	puzzleInput[0] = '\0';
	puzzleTime = 600;
}
void spawnDoor(void)
{
	doorActive = true;
	doorTileX = rand() % 3;
	doorTileY = rand() % 3;
	doorX = 100 + rand() % (SCREEN_WIDTH - 200);
	doorY = 100 + rand() % (SCREEN_HEIGHT - 200);
	doorTimer = 0;
}

#endif /* LEVEL1_H */