#ifndef LEVEL2_H
#define LEVEL2_H



/* ================================================================
LEVEL-2 STATE CONSTANTS
================================================================ */
#define STATE_PLAY2    10
#define STATE_PUZZLE2  11
#define STATE_WIN2     12

/* ================================================================
LEVEL-2 TUNING
================================================================ */
#define WORD_BANK_SIZE    100
#define WORDS_PER_PUZZLE  5
#define L2_TIME_PER_WORD  1500
#define L2_INPUT_MAX      20
#define L2_ENEMY_SPEED    3.2f
#define L2_ENEMY_HEALTH   4
#define L2_SCORE_PER_KILL 10

/* ================================================================
WORD BANK
================================================================ */
struct WordEntry
{
	char answer[32];
	char masked[32];
	char hint[64];
};

static WordEntry wordBank[WORD_BANK_SIZE] =
{
	{ "EAT", "E_T", "Hint: What you do with food" },
	{ "SIT", "S_T", "Hint: Rest on a chair" },
	{ "FLY", "F_Y", "Hint: Move through the air" },
	{ "CRY", "C_Y", "Hint: Tears fall from eyes" },
	{ "BUY", "B_Y", "Hint: Pay money for something" },
	{ "SKY", "S_Y", "Hint: Above the clouds" },
	{ "HOT", "H_T", "Hint: High temperature" },
	{ "WET", "W_T", "Hint: Covered in water" },
	{ "OLD", "O_D", "Hint: Not young anymore" },
	{ "SAD", "S_D", "Hint: Feeling unhappy" },
	{ "AIR", "A_R", "Hint: You breathe this" },
	{ "ART", "A_T", "Hint: Painting or drawing" },
	{ "ASK", "A_K", "Hint: Put a question to someone" },
	{ "BUS", "B_S", "Hint: Public road transport" },
	{ "CUT", "C_T", "Hint: Use scissors or knife" },
	{ "DAD", "D_D", "Hint: Your father" },
	{ "DAY", "D_Y", "Hint: 24 hours of time" },
	{ "EGG", "E_G", "Hint: Comes from a chicken" },
	{ "END", "E_D", "Hint: The final part" },
	{ "FUN", "F_N", "Hint: Enjoyable and entertaining" },
	{ "HUG", "H_G", "Hint: Wrap arms around someone" },
	{ "JOY", "J_Y", "Hint: Feeling of happiness" },
	{ "KID", "K_D", "Hint: A young child" },
	{ "LAW", "L_W", "Hint: Rules of the country" },
	{ "LIE", "L_E", "Hint: Not telling the truth" },
	{ "MOM", "M_M", "Hint: Your mother" },
	{ "MIX", "M_X", "Hint: Combine things together" },
	{ "NET", "N_T", "Hint: Used to catch fish" },
	{ "OIL", "O_L", "Hint: Used for cooking or engine" },
	{ "PAY", "P_Y", "Hint: Give money for something" },
	{ "RAW", "R_W", "Hint: Not cooked yet" },
	{ "SEA", "S_A", "Hint: Large body of salt water" },
	{ "SEW", "S_W", "Hint: Join cloth with needle" },
	{ "SON", "S_N", "Hint: Male child of parents" },
	{ "TOY", "T_Y", "Hint: Children play with this" },
	{ "WIN", "W_N", "Hint: Come first in a game" },
	{ "BOOK", "B__K", "Hint: You read this" },
	{ "FOOD", "F__D", "Hint: You eat this daily" },
	{ "HAND", "H__D", "Hint: End of your arm" },
	{ "HOME", "H_ME", "Hint: Where you live" },
	{ "LAKE", "L_KE", "Hint: Body of water inland" },
	{ "LAMP", "L_MP", "Hint: Gives light in a room" },
	{ "LOVE", "L_VE", "Hint: Deep caring feeling" },
	{ "MILK", "M_LK", "Hint: White drink from cow" },
	{ "MOON", "M__N", "Hint: Glows in night sky" },
	{ "NAME", "N_ME", "Hint: What people call you" },
	{ "RAIN", "R_IN", "Hint: Water falling from clouds" },
	{ "RICE", "R_CE", "Hint: Common everyday food grain" },
	{ "ROAD", "R_AD", "Hint: Cars drive on this" },
	{ "SALT", "S_LT", "Hint: Makes food tasty" },
	{ "SHOP", "SH_P", "Hint: Place to buy things" },
	{ "SNOW", "SN_W", "Hint: White frozen rain" },
	{ "SONG", "S_NG", "Hint: Music with words" },
	{ "STAR", "ST_R", "Hint: Shines in night sky" },
	{ "SWIM", "SW_M", "Hint: Move through water" },
	{ "TALK", "T_LK", "Hint: Speak with someone" },
	{ "TEAM", "T__M", "Hint: Group working together" },
	{ "TENT", "T_NT", "Hint: Shelter used for camping" },
	{ "TEST", "T_ST", "Hint: Exam to check knowledge" },
	{ "TIME", "T_ME", "Hint: Measured in hours and minutes" },
	{ "TREE", "TR_E", "Hint: Has trunk leaves and branches" },
	{ "TRIP", "TR_P", "Hint: A short journey" },
	{ "VOTE", "V_TE", "Hint: Choose in an election" },
	{ "WAKE", "W_KE", "Hint: Stop sleeping" },
	{ "WALK", "W_LK", "Hint: Move on foot slowly" },
	{ "WARM", "W_RM", "Hint: Slightly hot temperature" },
	{ "WASH", "W_SH", "Hint: Clean with water" },
	{ "WIND", "W_ND", "Hint: Moving air outside" },
	{ "WISH", "W_SH", "Hint: Hope for something" },
	{ "WOOD", "W__D", "Hint: Material from trees" },
	{ "WORD", "W_RD", "Hint: A unit of language" },
	{ "WORK", "W_RK", "Hint: Do a job or task" },
	{ "YEAR", "Y__R", "Hint: 365 days of time" },
	{ "BABY", "B_BY", "Hint: Newly born child" },
	{ "BACK", "B_CK", "Hint: Opposite side of front" },
	{ "BIRD", "B_RD", "Hint: Animal with wings and beak" },
	{ "BORN", "B_RN", "Hint: Come into the world" },
	{ "BOSS", "B_SS", "Hint: Person in charge" },
	{ "BURN", "B_RN", "Hint: Damaged by fire or heat" },
	{ "CAKE", "C_KE", "Hint: Sweet baked birthday treat" },
	{ "CALM", "C_LM", "Hint: Peaceful and not worried" },
	{ "CASH", "C_SH", "Hint: Paper money in hand" },
	{ "CHAT", "CH_T", "Hint: Casual friendly conversation" },
	{ "CHIN", "CH_N", "Hint: Bottom part of your face" },
	{ "CITY", "C_TY", "Hint: Large populated urban area" },
	{ "CLUE", "CL_E", "Hint: Helps solve a mystery" },
	{ "COAT", "C_AT", "Hint: Worn over clothes in cold" },
	{ "COIN", "C_IN", "Hint: Metal piece of money" },
	{ "COLD", "C_LD", "Hint: Low temperature weather" },
	{ "COPY", "C_PY", "Hint: Make an exact duplicate" },
	{ "CORN", "C_RN", "Hint: Yellow vegetable on cob" },
	{ "CUBE", "C_BE", "Hint: Six sided 3D shape" },
	{ "DARK", "D_RK", "Hint: No light present" },
	{ "DAWN", "D_WN", "Hint: Early morning sunrise time" },
	{ "DEAR", "D_AR", "Hint: Someone you love or care about" },
	{ "DEEP", "D__P", "Hint: Far down below surface" },
	{ "DESK", "D_SK", "Hint: Furniture for studying or working" },
	{ "DISH", "D_SH", "Hint: Plate used for serving food" },
	{ "DOOR", "D__R", "Hint: You enter through this" },
	{ "DRAW", "DR_W", "Hint: Make a picture with pencil" },
	
};

/* ================================================================
LEVEL-2 BACKGROUND IMAGE HANDLES
bg2[0..8] filled by loadLevel2Images()

Grid layout  (tileY*3 + tileX):
(0,2)=bg2[6]  (1,2)=bg2[7]  (2,2)=bg2[8]  <- top row
(0,1)=bg2[3]  (1,1)=bg2[4]  (2,1)=bg2[5]  <- middle row
(0,0)=bg2[0]  (1,0)=bg2[1]  (2,0)=bg2[2]  <- bottom row
bg2[8] = tile(2,2) = LOCKER ROOM (top-right)
================================================================ */
int bg2[9];

/* ================================================================
LEVEL-2 VARIABLES
================================================================ */
int  l2_selectedWords[WORDS_PER_PUZZLE];
int  l2_currentWordIndex = 0;
int  l2_solvedCount = 0;
int  l2_puzzleTime = L2_TIME_PER_WORD;
char l2_playerInput[L2_INPUT_MAX + 4];
int  l2_inputLen = 0;
int  l2_feedbackTimer = 0;
bool l2_wordCorrect = false;
bool l2_feedbackWrong = false;

bool l2_lockerActive = true;
int  l2_lockerTileX = 2;
int  l2_lockerTileY = 2;
int  l2_lockerX = 1580;
int  l2_lockerY = 750;

bool l2_doorActive = false;
int  l2_doorTileX = 0;
int  l2_doorTileY = 0;
int  l2_doorX = 0;
int  l2_doorY = 0;
int  l2_doorTimer = 0;
int  l2_doorDuration = 700;
int  l2_nextDoorTime = 1000;

bool l2_levelComplete = false;
int  l2_winTimer = 0;
int  l2_indicatorTarget = 0;   /* 0=locker  1=door */

bool inLevel2 = false;

/* ================================================================
loadLevel2Images()
Call this from allImg() in main.cpp
Change the file paths to your own Level-2 room images.
================================================================ */
void loadLevel2Images(void)
{
	bg2[0] = iLoadImage("Images\\l2_room1.png");
	bg2[1] = iLoadImage("Images\\l2_room2.png");
	bg2[2] = iLoadImage("Images\\l2_room3.png");
	bg2[3] = iLoadImage("Images\\l2_room4.png");
	bg2[4] = iLoadImage("Images\\l2_room5.png");
	bg2[5] = iLoadImage("Images\\l2_room6.png");
	bg2[6] = iLoadImage("Images\\l2_room7.png");
	bg2[7] = iLoadImage("Images\\l2_room8.png");
	bg2[8] = iLoadImage("Images\\l2_room9.png");
}

/* ================================================================
l2_generatePuzzleSession()
================================================================ */
void l2_generatePuzzleSession(void)
{
	bool used[WORD_BANK_SIZE];
	int  i, widx, tries;
	for (i = 0; i < WORD_BANK_SIZE; i++) used[i] = false;
	for (i = 0; i < WORDS_PER_PUZZLE; i++)
	{
		tries = 0;
		do { widx = rand() % WORD_BANK_SIZE; tries++; } while (used[widx] && tries < 500);
		used[widx] = true;
		l2_selectedWords[i] = widx;
	}
	l2_currentWordIndex = 0;
	l2_solvedCount = 0;
	l2_puzzleTime = L2_TIME_PER_WORD;
	l2_inputLen = 0;
	l2_playerInput[0] = '\0';
	l2_feedbackTimer = 0;
	l2_wordCorrect = false;
	l2_feedbackWrong = false;
}

/* ================================================================
l2_spawnDoor()
================================================================ */
void l2_spawnDoor(void)
{
	l2_doorActive = true;
	l2_doorTileX = rand() % 3;
	l2_doorTileY = rand() % 3;
	l2_doorX = 100 + rand() % (1700 - 200);
	l2_doorY = 100 + rand() % (900 - 200);
	l2_doorTimer = 0;
}

/* ================================================================
initLevel2()
Call when transitioning from Level-1 WIN state.
NOTE: resetAllEnemies() and spawnFruit() are defined in main.cpp
AFTER this header is included, so they are visible here.
================================================================ */
void initLevel2(void)
{
	playerX = 100;
	playerY = 100;
	tileX = 0;
	tileY = 0;

	l2_lockerActive = true;
	l2_lockerTileX = 2;
	l2_lockerTileY = 2;
	l2_lockerX = 1580;
	l2_lockerY = 750;

	l2_doorActive = false;
	l2_levelComplete = false;
	l2_winTimer = 0;
	l2_indicatorTarget = 0;
	l2_nextDoorTime = 1000;
	l2_doorTimer = 0;

	inLevel2 = true;

	resetAllEnemies();
	spawnFruit();
}

/* ================================================================
drawPuzzle2Screen()
================================================================ */
void drawPuzzle2Screen(void)
{
	int   i, wi, mlen, startX, letterY, dotX, dotY;
	int   panelX, panelY, panelW, panelH, ibX, ibY;
	float timePct;
	char  buf[128];
	char  ch[3];
	char  masked[32];

	iSetColor(10, 10, 30);
	iFilledRectangle(0, 0, 1700, 900);

	/* title bar */
	iSetColor(160, 0, 0);
	iFilledRectangle(0, 830, 1700, 70);
	iSetColor(255, 255, 0);
	iText(330, 850,
		"LEVEL 2  --  ENCRYPTION KEY: MISSING WORD PUZZLE",
		GLUT_BITMAP_HELVETICA_18);

	/* solved progress bar */
	iSetColor(40, 40, 40);
	iFilledRectangle(50, 780, 400, 30);
	iSetColor(0, 200, 100);
	iFilledRectangle(50, 780,
		(400 * l2_solvedCount) / WORDS_PER_PUZZLE, 30);
	iSetColor(255, 255, 255);
	sprintf_s(buf, sizeof(buf), "Words Solved: %d / %d",
		l2_solvedCount, WORDS_PER_PUZZLE);
	iText(55, 788, buf, GLUT_BITMAP_HELVETICA_18);

	/* timer bar */
	timePct = (float)l2_puzzleTime / (float)L2_TIME_PER_WORD;
	if (timePct < 0.0f) timePct = 0.0f;
	iSetColor(60, 0, 0);
	iFilledRectangle(1250, 780, 400, 30);
	if (timePct > 0.5f)  iSetColor(0, 220, 0);
	else if (timePct > 0.25f) iSetColor(255, 180, 0);
	else                      iSetColor(255, 30, 30);
	iFilledRectangle(1250, 780, (int)(400.0f * timePct), 30);
	iSetColor(255, 255, 255);
	sprintf_s(buf, sizeof(buf), "Time: %d sec", l2_puzzleTime / 60);
	iText(1255, 788, buf, GLUT_BITMAP_HELVETICA_18);

	/* word panel */
	panelX = 500; panelY = 430; panelW = 700; panelH = 300;
	iSetColor(20, 20, 50);
	iFilledRectangle(panelX, panelY, panelW, panelH);
	iSetColor(0, 180, 255);
	iRectangle(panelX, panelY, panelW, panelH);

	wi = l2_selectedWords[l2_currentWordIndex];

	iSetColor(180, 180, 255);
	iText(panelX + 20, panelY + panelH - 30,
		wordBank[wi].hint, GLUT_BITMAP_HELVETICA_18);

	/* masked word */
	strcpy_s(masked, sizeof(masked), wordBank[wi].masked);
	mlen = (int)strlen(masked);
	startX = panelX + panelW / 2 - (mlen * 40) / 2;
	letterY = panelY + 180;

	for (i = 0; i < mlen; i++)
	{
		ch[0] = masked[i]; ch[1] = '\0';
		if (masked[i] == '_')
		{
			iSetColor(200, 40, 40);
			iFilledRectangle(startX + i * 40, letterY - 5, 28, 35);
			iSetColor(255, 255, 255);
			iText(startX + i * 40 + 8, letterY + 5, "?",
				GLUT_BITMAP_TIMES_ROMAN_24);
		}
		else
		{
			iSetColor(255, 255, 100);
			iText(startX + i * 40 + 4, letterY + 5, ch,
				GLUT_BITMAP_TIMES_ROMAN_24);
		}
	}
	iSetColor(0, 180, 255);
	iFilledRectangle(startX, letterY - 10, mlen * 40, 3);

	iSetColor(200, 200, 200);
	iText(panelX + 20, panelY + 100,
		"Type the COMPLETE word (all letters) then press ENTER",
		GLUT_BITMAP_HELVETICA_12);

	/* input box */
	ibX = 650; ibY = 330;
	iSetColor(30, 30, 60);
	iFilledRectangle(ibX, ibY, 400, 60);
	iSetColor(0, 255, 150);
	iRectangle(ibX, ibY, 400, 60);
	iSetColor(255, 255, 255);
	if (l2_inputLen > 0)
		sprintf_s(buf, sizeof(buf), "%s_", l2_playerInput);
	else
		sprintf_s(buf, sizeof(buf), "_");
	iText(ibX + 15, ibY + 22, buf, GLUT_BITMAP_TIMES_ROMAN_24);

	/* feedback */
	if (l2_feedbackTimer > 0)
	{
		if (l2_wordCorrect)
		{
			iSetColor(0, 255, 100);
			iText(800, 290, "CORRECT!", GLUT_BITMAP_TIMES_ROMAN_24);
		}
		else if (l2_feedbackWrong)
		{
			iSetColor(255, 50, 50);
			iText(700, 290, "WRONG!  GAME OVER...",
				GLUT_BITMAP_TIMES_ROMAN_24);
		}
	}

	/* progress dots */
	for (i = 0; i < WORDS_PER_PUZZLE; i++)
	{
		dotX = 850 - (WORDS_PER_PUZZLE * 35) / 2 + i * 35;
		dotY = 250;
		if (i < l2_solvedCount)        iSetColor(0, 220, 80);
		else if (i == l2_currentWordIndex)  iSetColor(255, 200, 0);
		else                                iSetColor(80, 80, 80);
		iFilledCircle(dotX + 12, dotY + 12, 12);
	}

	iSetColor(150, 150, 150);
	iText(50, 30,
		"ESC = Menu   BACKSPACE = Delete   ENTER = Submit",
		GLUT_BITMAP_HELVETICA_12);
}

/* ================================================================
drawLevel2HUD()
Call in iDraw() when gameState == STATE_PLAY2
================================================================ */
void drawLevel2HUD(int score)
{
	char   buf[64];
	double X[3], Y[3];
	double ang;
	float  dx, dy, L, S, bx, by;
	int    tx, ty, tTileX, tTileY;
	bool   drawArrow;

	iSetColor(255, 100, 0);
	iText(20, 860, "LEVEL 2", GLUT_BITMAP_HELVETICA_18);
	sprintf_s(buf, sizeof(buf), "Score: %d", score);
	iSetColor(255, 255, 255);
	iText(135, 860, buf, GLUT_BITMAP_HELVETICA_18);

	/* gold locker */
	if (l2_lockerActive &&
		tileX == l2_lockerTileX && tileY == l2_lockerTileY)
	{
		iSetColor(180, 120, 0);
		iFilledRectangle(l2_lockerX, l2_lockerY, 70, 90);
		iSetColor(255, 220, 0);
		iRectangle(l2_lockerX, l2_lockerY, 70, 90);
		iSetColor(255, 255, 255);
		iFilledCircle(l2_lockerX + 35, l2_lockerY + 55, 10);
		iSetColor(255, 220, 0);
		iText(l2_lockerX + 2, l2_lockerY - 20,
			"L2 LOCKER", GLUT_BITMAP_HELVETICA_12);
	}

	/* cyan escape door */
	if (l2_doorActive &&
		tileX == l2_doorTileX && tileY == l2_doorTileY)
	{
		iSetColor(0, 60, 80);
		iFilledRectangle(l2_doorX, l2_doorY, 60, 80);
		iSetColor(0, 220, 255);
		iRectangle(l2_doorX, l2_doorY, 60, 80);
		iSetColor(0, 255, 220);
		iText(l2_doorX - 10, l2_doorY + 90,
			"ESCAPE!", GLUT_BITMAP_HELVETICA_12);
	}

	/* arrow indicator */
	tx = 0; ty = 0; tTileX = 0; tTileY = 0; drawArrow = false;
	if (l2_indicatorTarget == 0 && l2_lockerActive)
	{
		tx = l2_lockerX; ty = l2_lockerY;
		tTileX = l2_lockerTileX; tTileY = l2_lockerTileY;
		drawArrow = true;
	}
	else if (l2_doorActive)
	{
		tx = l2_doorX; ty = l2_doorY;
		tTileX = l2_doorTileX; tTileY = l2_doorTileY;
		drawArrow = true;
	}
	if (drawArrow)
	{
		dx = (float)((tTileX - tileX) * 1700 + (tx - playerX));
		dy = (float)((tTileY - tileY) * 900 + (ty - playerY));
		ang = atan2((double)dy, (double)dx);
		L = 60.0f; S = 25.0f;
		bx = 850.0f + (float)(cos(ang) * (double)L);
		by = 450.0f + (float)(sin(ang) * (double)L);
		X[0] = (double)bx + cos(ang)        * (double)S;
		X[1] = (double)bx + cos(ang + 1.57) * (double)S;
		X[2] = (double)bx - cos(ang + 1.57) * (double)S;
		Y[0] = (double)by + sin(ang)        * (double)S;
		Y[1] = (double)by + sin(ang + 1.57) * (double)S;
		Y[2] = (double)by - sin(ang + 1.57) * (double)S;
		if (l2_indicatorTarget == 0) iSetColor(255, 140, 0);
		else                         iSetColor(0, 220, 255);
		iFilledPolygon(X, Y, 3);
	}
}

/* ================================================================
updatePuzzle2Input()
Call every frame when gameState == STATE_PUZZLE2
================================================================ */
void updatePuzzle2Input(void)
{
	static bool alphaPressed[26];
	static bool apInited = false;
	if (!apInited)
	{
		int k; for (k = 0; k < 26; k++) alphaPressed[k] = false; apInited = true;
	}
	static bool backPressed = false;
	static bool enterPressed = false;

	int  i, wi;
	bool pressed;
	char cu, cl, c;
	char upper[L2_INPUT_MAX + 4];

	/* wait during feedback flash */
	if (l2_feedbackTimer > 0)
	{
		l2_feedbackTimer--;
		if (l2_wordCorrect) return;
	}

	/* letters A-Z */
	for (i = 0; i < 26; i++)
	{
		cu = (char)('A' + i);
		cl = (char)('a' + i);
		pressed = (isKeyPressed(cu) != 0) || (isKeyPressed(cl) != 0);
		if (pressed)
		{
			if (!alphaPressed[i] && l2_inputLen < L2_INPUT_MAX - 1)
			{
				l2_playerInput[l2_inputLen] = cu;
				l2_inputLen++;
				l2_playerInput[l2_inputLen] = '\0';
				alphaPressed[i] = true;
			}
		}
		else alphaPressed[i] = false;
	}

	/* backspace */
	if (isKeyPressed(8))
	{
		if (!backPressed && l2_inputLen > 0)
		{
			l2_inputLen--; l2_playerInput[l2_inputLen] = '\0'; backPressed = true;
		}
	}
	else backPressed = false;

	/* enter */
	if (isKeyPressed(13))
	{
		if (!enterPressed && l2_inputLen > 0)
		{
			for (i = 0; i < l2_inputLen; i++)
			{
				c = l2_playerInput[i];
				if (c >= 'a' && c <= 'z') c = (char)(c - 32);
				upper[i] = c;
			}
			upper[l2_inputLen] = '\0';

			wi = l2_selectedWords[l2_currentWordIndex];

			if (strcmp(upper, wordBank[wi].answer) == 0)
			{
				l2_solvedCount++;
				l2_wordCorrect = true;
				l2_feedbackWrong = false;
				l2_feedbackTimer = 60;
				l2_inputLen = 0;
				l2_playerInput[0] = '\0';
				l2_puzzleTime = L2_TIME_PER_WORD;
				if (l2_solvedCount >= WORDS_PER_PUZZLE)
				{
					gameState = STATE_WIN2; l2_winTimer = 180;
				}
				else
				{
					l2_currentWordIndex++;
				}
			}
			else
			{
				l2_feedbackWrong = true;
				l2_wordCorrect = false;
				l2_feedbackTimer = 50;
				gameState = STATE_OVER;
			}
			enterPressed = true;
		}
	}
	else enterPressed = false;

	/* countdown */
	l2_puzzleTime--;
	if (l2_puzzleTime <= 0) gameState = STATE_OVER;
}

/* ================================================================
level2FixedUpdate()
Call every frame when gameState == STATE_PLAY2
================================================================ */
void level2FixedUpdate(void)
{
	/* locker collision */
	if (l2_lockerActive &&
		tileX == l2_lockerTileX && tileY == l2_lockerTileY)
	{
		if (playerX       < l2_lockerX + 70 &&   // FIXED: was l2_doorX
			playerX + 150 > l2_lockerX      &&   // FIXED: was l2_doorX
			playerY       < l2_lockerY + 90 &&   // FIXED: was l2_doorY
			playerY + 200 > l2_lockerY)           // FIXED: was l2_doorY
		{
			l2_lockerActive = false;
			l2_indicatorTarget = 0;
			l2_generatePuzzleSession();           // ADDED: prepare word puzzle
			gameState = STATE_PUZZLE2;            // ADDED: trigger puzzle screen
		}
	}

	/* door timer */
	if (!l2_doorActive)
	{
		l2_nextDoorTime--;
		if (l2_nextDoorTime <= 0)
		{
			l2_spawnDoor(); l2_nextDoorTime = 1500 + rand() % 1000;
		}
	}
	else
	{
		l2_doorTimer++;
		if (l2_doorTimer > l2_doorDuration) l2_doorActive = false;

		if (tileX == l2_doorTileX && tileY == l2_doorTileY)
		{
			if (playerX       < l2_doorX + 60 &&
				playerX + 150 > l2_doorX      &&
				playerY       < l2_doorY + 80 &&
				playerY + 200 > l2_doorY)
			{
				l2_levelComplete = true;
				l2_doorActive = false;
				gameState = STATE_WIN2;
				l2_winTimer = 180;
			}
		}
	}
}

/* ================================================================
level2WinUpdate()
Call every frame when gameState == STATE_WIN2
================================================================ */
/* ================================================================
level2WinUpdate()
Call every frame when gameState == STATE_WIN2
================================================================ */

/* Forward declarations for Level 3 (defined in Level3.h, included after this file) */
#ifndef STATE_PLAY3
#define STATE_PLAY3    20
#endif
void initLevel3(void);

void level2WinUpdate(void)
{
	l2_winTimer--;
	if (l2_winTimer <= 0)
	{
		if (l2_levelComplete)
		{
			/* Level 2 door was entered -> go to Level 3 */
			playerHP = CHAR_MAX_HEALTH;
			initLevel3();
			gameState = STATE_PLAY3;
		}
		else
		{
			/* Puzzle solved, now spawn the escape door */
			gameState = STATE_PLAY2;
			l2_lockerActive = false;
			l2_indicatorTarget = 1;
			l2_levelComplete = false;
			l2_spawnDoor();
		}
	}
}
/* ================================================================
level2EnemyUpdate()
Call from change() when inLevel2 == true
================================================================ */
void level2EnemyUpdate(void)
{
	int   i;
	float dx, dy, dist;

	spawnTimer++;
	if (spawnTimer > nextSpawnTime)
	{
		for (i = 0; i < 10; i++)
		{
			if (!enemy[i].policeShow)
			{
				spawnEnemy(i);
				enemy[i].health = L2_ENEMY_HEALTH;
				break;
			}
		}
		spawnTimer = 0;
		nextSpawnTime = 30 + rand() % 60;
	}

	for (i = 0; i < 10; i++)
	{
		if (!enemy[i].policeShow) continue;
		dx = (float)(playerX - enemy[i].police_x);
		dy = (float)(playerY - enemy[i].police_y);
		dist = (float)sqrt((double)(dx * dx + dy * dy));
		enemy[i].shootTimer--;
		if (enemy[i].shootTimer <= 0)
		{
			eFireBullet(i); enemy[i].shootTimer = 60 + rand() % 60;
		}
		enemy[i].facingRight = (dx >= 0) ? true : false;
		if (dist > 0.0f)
		{
			enemy[i].police_x += (int)((dx / dist) * L2_ENEMY_SPEED);
			enemy[i].police_y += (int)((dy / dist) * L2_ENEMY_SPEED);
		}
	}
}

/* ================================================================
drawLevel2WinScreen()
================================================================ */
void drawLevel2WinScreen(void)
{
	iShowImage(0, 0, 1700, 900, winBg);
	iSetColor(0, 0, 0);
	iFilledRectangle(380, 320, 940, 260);
	iSetColor(0, 255, 150);
	iRectangle(380, 320, 940, 260);
	iSetColor(255, 255, 0);
	iText(500, 530, "LEVEL 2 PUZZLE SOLVED!",
		GLUT_BITMAP_TIMES_ROMAN_24);
	iSetColor(0, 255, 200);
	iText(420, 480,
		"Find the ESCAPE DOOR to complete the level!",
		GLUT_BITMAP_HELVETICA_18);
	iSetColor(200, 200, 200);
	iText(460, 435,
		"The cyan door has appeared somewhere on the map...",
		GLUT_BITMAP_HELVETICA_12);
}

/* ================================================================
drawLevel2CompleteScreen()
================================================================ */
void drawLevel2CompleteScreen(void)
{
	iSetColor(0, 0, 0);
	iFilledRectangle(350, 260, 1000, 320);
	iSetColor(255, 200, 0);
	iRectangle(350, 260, 1000, 320);
	iSetColor(255, 255, 0);
	iText(530, 540, "LEVEL 2 COMPLETE!",
		GLUT_BITMAP_TIMES_ROMAN_24);
	iSetColor(255, 255, 255);
	iText(430, 490,
		"You cracked the encryption and escaped!",
		GLUT_BITMAP_HELVETICA_18);
	iSetColor(0, 255, 150);
	iText(510, 440,
		"Congratulations, Heist Master!",
		GLUT_BITMAP_HELVETICA_18);
	iSetColor(150, 150, 150);
	iText(580, 390,
		"Press ESC to return to menu",
		GLUT_BITMAP_HELVETICA_12);
}

#endif /* LEVEL2_H */