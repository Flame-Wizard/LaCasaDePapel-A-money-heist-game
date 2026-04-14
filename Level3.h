#ifndef LEVEL3_H
#define LEVEL3_H

/* ================================================================
LEVEL-3 STATE CONSTANTS
================================================================ */
#define STATE_PLAY3    20
#define STATE_PUZZLE3  21
#define STATE_WIN3     22

/* ================================================================
LEVEL-3 TUNING & VAULT TERMINAL
================================================================ */
#define L3_TIME_PER_QUESTION 1800  /* 30 seconds per question */
#define L3_INPUT_MAX         20
#define L3_ENEMY_SPEED       3.2f
#define L3_ENEMY_HEALTH      4
#define L3_SCORE_PER_KILL    10

struct UraniumQuestion {
	char question[128];
	char answer[32];
};

/* The 3 High-Stakes Questions */
UraniumQuestion l3_questions[3] = {
	{ "Enter the primary fissile isotope of Uranium used in reactors:", "235" },
	{ "What is the atomic number of Uranium?", "92" },
	{ "What color does Uranium glass glow under UV light?", "GREEN" }
};

int bg3[9];

/* ================================================================
LEVEL-3 VARIABLES
================================================================ */
int  l3_currentQuestionIndex = 0;
int  l3_solvedCount = 0;
int  l3_puzzleTime = L3_TIME_PER_QUESTION;
char l3_playerInput[L3_INPUT_MAX + 4];
int  l3_inputLen = 0;
int  l3_feedbackTimer = 0;
bool l3_wordCorrect = false;
bool l3_feedbackWrong = false;

/* Move the locker to Room 6, Top-Left Corner */
bool l3_lockerActive = true;
int  l3_lockerTileX = 2;       /* Room 6 is tile (2,1) */
int  l3_lockerTileY = 1;
int  l3_lockerX = 120;         /* Top left coordinate */
int  l3_lockerY = 700;

bool l3_doorActive = false;
int  l3_doorTileX = 0;
int  l3_doorTileY = 0;
int  l3_doorX = 0;
int  l3_doorY = 0;
int  l3_doorTimer = 0;
int  l3_doorDuration = 700;
int  l3_nextDoorTime = 1000;

bool l3_levelComplete = false;
int  l3_winTimer = 0;
int  l3_indicatorTarget = 0;   /* 0=locker  1=door */

bool inLevel3 = false;

/* ================================================================
loadLevel3Images()
Call this from allImg() in main.cpp
================================================================ */
void loadLevel3Images(void)
{
	bg3[0] = iLoadImage("Images\\L3_room1.png");
	bg3[1] = iLoadImage("Images\\L3_room2.png");
	bg3[2] = iLoadImage("Images\\L3_room3.png");
	bg3[3] = iLoadImage("Images\\L3_room4.png");
	bg3[4] = iLoadImage("Images\\L3_room5.png");
	bg3[5] = iLoadImage("Images\\L3_room6.png");
	bg3[6] = iLoadImage("Images\\L3_room7.png");
	bg3[7] = iLoadImage("Images\\L3_room8.png");
	bg3[8] = iLoadImage("Images\\L3_room9.png");
}

/* ================================================================
l3_generatePuzzleSession()
================================================================ */
void l3_generatePuzzleSession(void)
{
	l3_currentQuestionIndex = 0;
	l3_solvedCount = 0;
	l3_puzzleTime = L3_TIME_PER_QUESTION;
	l3_inputLen = 0;
	l3_playerInput[0] = '\0';
	l3_feedbackTimer = 0;
	l3_wordCorrect = false;
	l3_feedbackWrong = false;
}

/* ================================================================
l3_spawnDoor()
================================================================ */
void l3_spawnDoor(void)
{
	l3_doorActive = true;
	l3_doorTileX = rand() % 3;
	l3_doorTileY = rand() % 3;
	l3_doorX = 100 + rand() % (1700 - 200);
	l3_doorY = 100 + rand() % (900 - 200);
	l3_doorTimer = 0;
}

/* ================================================================
initLevel3()
Call when transitioning to Level 3.
================================================================ */
void initLevel3(void)
{
	playerX = 100;
	playerY = 100;
	tileX = 0;
	tileY = 0;

	l3_lockerActive = true;
	l3_lockerTileX = 2;
	l3_lockerTileY = 1;
	l3_lockerX = 120;
	l3_lockerY = 700;

	l3_doorActive = false;
	l3_levelComplete = false;
	l3_winTimer = 0;
	l3_indicatorTarget = 0;
	l3_nextDoorTime = 1000;
	l3_doorTimer = 0;

	inLevel3 = true;
	inLevel2 = false;   // clear L2 

	// reset Level 2 flags
	l2_levelComplete = false;
	l2_doorActive = false;

	resetAllEnemies();
	spawnFruit();
}

/* ================================================================
drawPuzzle3Screen()
================================================================ */
void drawPuzzle3Screen(void)
{
	int   panelX, panelY, panelW, panelH, ibX, ibY;
	float timePct;
	char  buf[128];

	/* Deep black background for terminal vibe */
	iSetColor(5, 10, 5);
	iFilledRectangle(0, 0, 1700, 900);

	/* Title bar */
	iSetColor(0, 80, 0);
	iFilledRectangle(0, 830, 1700, 70);
	iSetColor(0, 255, 100);
	iText(500, 850, "LEVEL 3  --  ISOTOPE VAULT TERMINAL", GLUT_BITMAP_TIMES_ROMAN_24);

	/* Progress indicator */
	iSetColor(0, 255, 100);
	sprintf_s(buf, sizeof(buf), "SECURITY CLEARANCE: %d / 3", l3_solvedCount);
	iText(50, 788, buf, GLUT_BITMAP_HELVETICA_18);

	/* Timer bar (shrinks down) */
	timePct = (float)l3_puzzleTime / (float)L3_TIME_PER_QUESTION;
	if (timePct < 0.0f) timePct = 0.0f;
	iSetColor(40, 0, 0);
	iFilledRectangle(1250, 780, 400, 30);
	if (timePct > 0.5f)  iSetColor(0, 255, 0);
	else if (timePct > 0.25f) iSetColor(255, 200, 0);
	else                      iSetColor(255, 0, 0);
	iFilledRectangle(1250, 780, (int)(400.0f * timePct), 30);

	iSetColor(255, 255, 255);
	sprintf_s(buf, sizeof(buf), "SYSTEM LOCKDOWN IN: %d sec", l3_puzzleTime / 60);
	iText(1260, 788, buf, GLUT_BITMAP_HELVETICA_18);

	/* Main Terminal Panel */
	panelX = 350; panelY = 350; panelW = 1000; panelH = 300;
	iSetColor(10, 30, 10);
	iFilledRectangle(panelX, panelY, panelW, panelH);
	iSetColor(0, 255, 50);
	iRectangle(panelX, panelY, panelW, panelH);

	/* Draw Current Question */
	iSetColor(0, 255, 150);
	iText(panelX + 40, panelY + panelH - 80,
		l3_questions[l3_currentQuestionIndex].question, GLUT_BITMAP_TIMES_ROMAN_24);

	/* Input box */
	ibX = 650; ibY = 400;
	iSetColor(5, 15, 5);
	iFilledRectangle(ibX, ibY, 400, 60);
	iSetColor(0, 255, 100);
	iRectangle(ibX, ibY, 400, 60);

	/* Blinking cursor effect */
	if (l3_inputLen > 0)
		sprintf_s(buf, sizeof(buf), "> %s_", l3_playerInput);
	else
		sprintf_s(buf, sizeof(buf), "> _");

	iSetColor(0, 255, 0);
	iText(ibX + 20, ibY + 22, buf, GLUT_BITMAP_TIMES_ROMAN_24);

	/* Feedback Flash */
	if (l3_feedbackTimer > 0)
	{
		if (l3_wordCorrect)
		{
			iSetColor(0, 255, 100);
			iText(760, 320, "ACCESS GRANTED", GLUT_BITMAP_TIMES_ROMAN_24);
		}
		else if (l3_feedbackWrong)
		{
			iSetColor(255, 0, 0);
			iText(700, 320, "ACCESS DENIED - ALARM TRIGGERED", GLUT_BITMAP_TIMES_ROMAN_24);
		}
	}

	iSetColor(0, 150, 50);
	iText(50, 30, "ESC = Abort   BACKSPACE = Delete   ENTER = Submit Code", GLUT_BITMAP_HELVETICA_12);
}

/* ================================================================
drawLevel3HUD()
Call in iDraw() when gameState == STATE_PLAY3
================================================================ */
void drawLevel3HUD(int score)
{
	char   buf[64];
	double X[3], Y[3];
	double ang;
	float  dx, dy, L, S, bx, by;
	int    tx, ty, tTileX, tTileY;
	bool   drawArrow;

	iSetColor(255, 100, 0);
	iText(20, 860, "LEVEL 3", GLUT_BITMAP_HELVETICA_18);
	sprintf_s(buf, sizeof(buf), "Score: %d", score);
	iSetColor(255, 255, 255);
	iText(135, 860, buf, GLUT_BITMAP_HELVETICA_18);

	/* gold locker */
	if (l3_lockerActive &&
		tileX == l3_lockerTileX && tileY == l3_lockerTileY)
	{
		iSetColor(180, 120, 0);
		iFilledRectangle(l3_lockerX, l3_lockerY, 70, 90);
		iSetColor(255, 220, 0);
		iRectangle(l3_lockerX, l3_lockerY, 70, 90);
		iSetColor(255, 255, 255);
		iFilledCircle(l3_lockerX + 35, l3_lockerY + 55, 10);
		iSetColor(255, 220, 0);
		iText(l3_lockerX + 2, l3_lockerY - 20,
			"VAULT TERMINAL", GLUT_BITMAP_HELVETICA_12);
	}

	/* cyan escape door */
	if (l3_doorActive &&
		tileX == l3_doorTileX && tileY == l3_doorTileY)
	{
		iSetColor(0, 60, 80);
		iFilledRectangle(l3_doorX, l3_doorY, 60, 80);
		iSetColor(0, 220, 255);
		iRectangle(l3_doorX, l3_doorY, 60, 80);
		iSetColor(0, 255, 220);
		iText(l3_doorX - 10, l3_doorY + 90,
			"ESCAPE!", GLUT_BITMAP_HELVETICA_12);
	}

	/* arrow indicator */
	tx = 0; ty = 0; tTileX = 0; tTileY = 0; drawArrow = false;
	if (l3_indicatorTarget == 0 && l3_lockerActive)
	{
		tx = l3_lockerX; ty = l3_lockerY;
		tTileX = l3_lockerTileX; tTileY = l3_lockerTileY;
		drawArrow = true;
	}
	else if (l3_doorActive)
	{
		tx = l3_doorX; ty = l3_doorY;
		tTileX = l3_doorTileX; tTileY = l3_doorTileY;
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
		if (l3_indicatorTarget == 0) iSetColor(255, 140, 0);
		else                         iSetColor(0, 220, 255);
		iFilledPolygon(X, Y, 3);
	}
}

/* ================================================================
updatePuzzle3Input()
Call every frame when gameState == STATE_PUZZLE3
================================================================ */
void updatePuzzle3Input(void)
{
	static bool alphaPressed[26];
	static bool numPressed[10];
	static bool apInited = false;
	if (!apInited)
	{
		int k;
		for (k = 0; k < 26; k++) alphaPressed[k] = false;
		for (k = 0; k < 10; k++) numPressed[k] = false;
		apInited = true;
	}

	static bool backPressed = false;
	static bool enterPressed = false;

	int  i;
	bool pressed;
	char cu, cl, c;
	char upper[L3_INPUT_MAX + 4];

	/* wait during feedback flash */
	if (l3_feedbackTimer > 0)
	{
		l3_feedbackTimer--;
		if (l3_wordCorrect) return;
	}

	/* letters A-Z */
	for (i = 0; i < 26; i++)
	{
		cu = (char)('A' + i);
		cl = (char)('a' + i);
		pressed = (isKeyPressed(cu) != 0) || (isKeyPressed(cl) != 0);
		if (pressed)
		{
			if (!alphaPressed[i] && l3_inputLen < L3_INPUT_MAX - 1)
			{
				l3_playerInput[l3_inputLen] = cu;
				l3_inputLen++; l3_playerInput[l3_inputLen] = '\0';
				alphaPressed[i] = true;
			}
		}
		else alphaPressed[i] = false;
	}

	/* numbers 0-9 */
	for (i = 0; i < 10; i++)
	{
		cu = (char)('0' + i);
		pressed = (isKeyPressed(cu) != 0);
		if (pressed)
		{
			if (!numPressed[i] && l3_inputLen < L3_INPUT_MAX - 1)
			{
				l3_playerInput[l3_inputLen] = cu;
				l3_inputLen++; l3_playerInput[l3_inputLen] = '\0';
				numPressed[i] = true;
			}
		}
		else numPressed[i] = false;
	}

	/* backspace */
	if (isKeyPressed(8))
	{
		if (!backPressed && l3_inputLen > 0)
		{
			l3_inputLen--; l3_playerInput[l3_inputLen] = '\0'; backPressed = true;
		}
	}
	else backPressed = false;

	/* enter */
	if (isKeyPressed(13))
	{
		if (!enterPressed && l3_inputLen > 0)
		{
			for (i = 0; i < l3_inputLen; i++)
			{
				c = l3_playerInput[i];
				if (c >= 'a' && c <= 'z') c = (char)(c - 32);
				upper[i] = c;
			}
			upper[l3_inputLen] = '\0';

			if (strcmp(upper, l3_questions[l3_currentQuestionIndex].answer) == 0)
			{
				l3_solvedCount++;
				l3_wordCorrect = true;
				l3_feedbackWrong = false;
				l3_feedbackTimer = 60;
				l3_inputLen = 0;
				l3_playerInput[0] = '\0';
				l3_puzzleTime = L3_TIME_PER_QUESTION;

				if (l3_solvedCount >= 3) /* 3 Questions total */
				{
					gameState = STATE_WIN3; l3_winTimer = 180;
				}
				else
				{
					l3_currentQuestionIndex++;
				}
			}
			else
			{
				l3_feedbackWrong = true;
				l3_wordCorrect = false;
				l3_feedbackTimer = 50;
				gameState = STATE_OVER;
			}
			enterPressed = true;
		}
	}
	else enterPressed = false;

	/* countdown */
	l3_puzzleTime--;
	if (l3_puzzleTime <= 0) gameState = STATE_OVER;
}

/* ================================================================
level3FixedUpdate()
Call every frame when gameState == STATE_PLAY3
================================================================ */
/* ================================================================
level3FixedUpdate()
Call every frame when gameState == STATE_PLAY3
================================================================ */

struct Firework {
	float x, y, dx, dy;
	int r, g, b;
	bool active;
	int life;
};
#define MAX_PARTICLES 100
Firework particles[MAX_PARTICLES];
float bounceAnim = 0;

void initFireworks() {
	for (int i = 0; i < MAX_PARTICLES; i++) {
		particles[i].active = true;
		particles[i].x = 850;
		particles[i].y = 450;
		float angle = (rand() % 360) * 3.14159f / 180.0f;
		float speed = (rand() % 100) / 10.0f + 2.0f;
		particles[i].dx = cos(angle) * speed;
		particles[i].dy = sin(angle) * speed;
		particles[i].r = rand() % 256;
		particles[i].g = rand() % 256;
		particles[i].b = rand() % 256;
		particles[i].life = 100 + rand() % 100;
	}
}


/* ================================================================
level3FixedUpdate()
Call every frame when gameState == STATE_PLAY3
================================================================ */
void level3FixedUpdate(void)
{
	/* locker collision */
	if (l3_lockerActive &&
		tileX == l3_lockerTileX && tileY == l3_lockerTileY)
	{
		if (playerX       < l3_lockerX + 70 &&
			playerX + 150 > l3_lockerX      &&
			playerY       < l3_lockerY + 90 &&
			playerY + 200 > l3_lockerY)
		{
			l3_lockerActive = false;
			l3_indicatorTarget = 0;
			l3_generatePuzzleSession();
			gameState = STATE_PUZZLE3;
		}
	}

	/* door timer */
	if (!l3_doorActive)
	{
		l3_nextDoorTime--;
		if (l3_nextDoorTime <= 0)
		{
			l3_spawnDoor(); l3_nextDoorTime = 1500 + rand() % 1000;
		}
	}
	else
	{
		l3_doorTimer++;
		if (l3_doorTimer > l3_doorDuration) l3_doorActive = false;

		if (tileX == l3_doorTileX && tileY == l3_doorTileY)
		{
			if (playerX       < l3_doorX + 60 &&
				playerX + 150 > l3_doorX      &&
				playerY       < l3_doorY + 80 &&
				playerY + 200 > l3_doorY)
			{
				l3_levelComplete = true;
				initFireworks();
				l3_doorActive = false;
				gameState = STATE_WIN3; /* Trigger the final win state */
			}
		}
	}
}

/* ================================================================
level3WinUpdate()
Call every frame when gameState == STATE_WIN3
================================================================ */
/* ================================================================
level3WinUpdate()
Call every frame when gameState == STATE_WIN3
================================================================ */
void level3WinUpdate(void)
{
	/* If they entered the final door, freeze everything so the fireworks play forever. */
	if (l3_levelComplete) {
		return;
	}

	/* Otherwise, they just got the Uranium. Run the timer then spawn the door. */
	l3_winTimer--;
	if (l3_winTimer <= 0)
	{
		gameState = STATE_PLAY3;
		l3_lockerActive = false;
		l3_indicatorTarget = 1;
		l3_levelComplete = false;
		l3_spawnDoor();
	}
}

/* ================================================================
level3EnemyUpdate()
Call from change() when inLevel3 == true
================================================================ */
void level3EnemyUpdate(void)
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
				enemy[i].health = L3_ENEMY_HEALTH;
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
			enemy[i].police_x += (int)((dx / dist) * L3_ENEMY_SPEED);
			enemy[i].police_y += (int)((dy / dist) * L3_ENEMY_SPEED);
		}
	}
}

void drawLevel3CompleteScreen(void);
/* ================================================================
drawLevel3WinScreen()
Called immediately after answering the 3 Uranium questions.
================================================================ */
/* ================================================================
drawLevel3WinScreen()
Called immediately after answering the 3 Uranium questions.
================================================================ */
void drawLevel3WinScreen(void)
{
	/* If they hit the escape door, show the fireworks instead! */
	if (l3_levelComplete) {
		drawLevel3CompleteScreen();
		return;
	}

	/* Deep black/green background for the mid-level terminal message */
	iSetColor(5, 10, 5);
	iFilledRectangle(0, 0, 1700, 900);

	/* Terminal style text box */
	iSetColor(0, 30, 0);
	iFilledRectangle(380, 320, 940, 260);
	iSetColor(0, 255, 100);
	iRectangle(380, 320, 940, 260);

	/* The requested text */
	iSetColor(0, 255, 0);
	iText(650, 500, "URANIUM IS ALL YOURS", GLUT_BITMAP_TIMES_ROMAN_24);

	/* Subtext telling the player what to do next */
	iSetColor(200, 255, 200);
	iText(530, 430, "Find the cyan ESCAPE DOOR to exit the facility!", GLUT_BITMAP_HELVETICA_18);
}

/* ================================================================
drawLevel3CompleteScreen()
Called when the player steps into the escape door.
================================================================ */
/* ================================================================
ANIMATION VARIABLES & SETUP
================================================================ */


/* ================================================================
drawLevel3CompleteScreen()
Called when the player steps into the escape door.
================================================================ */
void drawLevel3CompleteScreen(void)
{
	/* Darken background */
	iSetColor(10, 10, 15);
	iFilledRectangle(0, 0, 1700, 900);

	/* Update and Draw Particles (Fireworks) */
	for (int i = 0; i < MAX_PARTICLES; i++) {
		if (particles[i].active) {
			iSetColor(particles[i].r, particles[i].g, particles[i].b);
			iFilledCircle(particles[i].x, particles[i].y, 4);

			particles[i].x += particles[i].dx;
			particles[i].y += particles[i].dy;
			particles[i].dy -= 0.1f; /* Gravity */
			particles[i].life--;

			if (particles[i].life <= 0) {
				/* Respawn particle to keep animation going */
				particles[i].x = rand() % 1700;
				particles[i].y = rand() % 400 + 400;
				particles[i].active = true;
				particles[i].life = 100;
			}
		}
	}

	/* Bouncing Text Logic */
	bounceAnim += 0.05f;
	float yOffset = sin(bounceAnim) * 20.0f;

	/* Overlay Box */
	iSetColor(0, 0, 0);
	iFilledRectangle(350, 260 + (int)yOffset, 1000, 320);
	iSetColor(0, 255, 100);
	iRectangle(350, 260 + (int)yOffset, 1000, 320);

	/* "HEIST COMPLETE" with shadow */
	iSetColor(0, 100, 0);
	iText(550 + 4, 540 + (int)yOffset - 4, "HEIST COMPLETE!", GLUT_BITMAP_TIMES_ROMAN_24);
	iSetColor(0, 255, 100);
	iText(550, 540 + (int)yOffset, "HEIST COMPLETE!", GLUT_BITMAP_TIMES_ROMAN_24);

	iSetColor(255, 255, 255);
	iText(460, 490 + (int)yOffset, "You secured the Uranium and escaped the facility.", GLUT_BITMAP_HELVETICA_18);

	/* Flashing Color for "Congratulations" */
	int flash = (int)(sin(bounceAnim * 2) * 127) + 128;
	iSetColor(flash, 255, flash);
	iText(510, 440 + (int)yOffset, "Congratulations, Mastermind!", GLUT_BITMAP_HELVETICA_18);

	iSetColor(150, 150, 150);
	iText(580, 390 + (int)yOffset, "Press ESC to return to menu", GLUT_BITMAP_HELVETICA_12);
}

#endif /* LEVEL3_H */