#include "iGraphics.h"
#include <cstdio>
#include <cstring>
#include <windows.h>
#include <mmsystem.h>
#include <math.h>
#include <cstdlib>
#include <ctime>
#include "Highscore.h"
#pragma comment(lib, "winmm.lib")

/* ================================================================
GAME STATES
================================================================ */
#define STATE_MENU         0
#define STATE_NAME         1
#define STATE_STORY        2
#define STATE_PLAY         3
#define STATE_SCORE        4
#define STATE_PUZZLE       5
#define STATE_OVER         6
#define STATE_WIN          7
#define STATE_INSTRUCTIONS 8
#define STATE_ABOUT        9
/* STATE_PLAY2=10  STATE_PUZZLE2=11  STATE_WIN2=12  defined in Level2.h */
/* STATE_PLAY3=20  STATE_PUZZLE3=21  STATE_WIN3=22  defined in Level3.h */

/* ================================================================
SCREEN
================================================================ */
#define SCREEN_WIDTH    1700
#define SCREEN_HEIGHT   900
#define MAX_BULLETS     50
#define CHAR_MAX_HEALTH 400
float displayHP = CHAR_MAX_HEALTH;
#define PLAYER_W        150
#define PLAYER_H        200

#define UPGRADED_W      80
#define UPGRADED_H      150

#define MOVE_SPEED      3
#define FRUIT_HEAL      100
#define POLICENUMBER    10
#define ENEMY_MAX_HEALTH 3
#define MAP_MIN_X 0
#define MAP_MAX_X 2
#define MAP_MIN_Y 0
#define MAP_MAX_Y 2
int mouseX = 0;
int mouseY = 0;

/* ================================================================
GLOBAL VARIABLES
================================================================ */
int  gameState = STATE_MENU;
int  previousGameState = STATE_MENU;
int  spawnTimer = 0;
int  nextSpawnTime = 50;

/* player */
bool playerFacingRight = true;
int  playerX = 100;
int  playerY = 100;
int  playerHP = CHAR_MAX_HEALTH;
int  playerScore = 0;
int  idx = 0;

int  charImg[3];
int  charImgUpgraded[3];
int  aimImg;
int  newBulletImg;
int  playerShootTimer = 0;
extern bool gun_isUnlocked;

float mouseTargetX = 0;
float mouseTargetY = 0;
bool  hasTarget = false;


void playerHealth() {
	int totalW = 250;
	int barH = 22;
	int barX = playerX - 50;
	int barY = playerY + PLAYER_H + 15;

	if (displayHP < playerHP) displayHP += 2.5f;
	if (displayHP > playerHP) displayHP -= 1.5f;
	if (displayHP > CHAR_MAX_HEALTH) displayHP = (float)CHAR_MAX_HEALTH;
	if (displayHP < 0) displayHP = 0;

	float hpPercent = displayHP / CHAR_MAX_HEALTH;

	iSetColor(20, 20, 20);
	iFilledRectangle(barX - 2, barY - 2, totalW + 4, barH + 4);

	iSetColor(160, 0, 0);
	iFilledRectangle(barX, barY, totalW, barH);

	if (hpPercent > 0.6f)
		iSetColor(0, 220, 0);
	else if (hpPercent > 0.3f)
		iSetColor(255, 200, 0);
	else
		iSetColor(255, 40, 40);

	iFilledRectangle(barX, barY, (int)(totalW * hpPercent), barH);

	iSetColor(255, 255, 255);
	iRectangle(barX, barY, totalW, barH);

	iSetColor(255, 255, 255);
	char hpText[30];
	sprintf_s(hpText, "HP: %d / %d", playerHP, CHAR_MAX_HEALTH);
	iText(barX + 60, barY + 4, hpText, GLUT_BITMAP_HELVETICA_12);
}

/* map */
int  bg[9];
int  tileX = 0, tileY = 0;

/* buttons / UI */
struct Button { int x, y, w, h, img, imgHover; };
Button menuButtons[4];

int btnStart, btnScores, btnInstructions, btnAbout;
int btnStartHover, btnScoresHover, btnInstructionsHover, btnAboutHover;

#define HOME_ANIM_FRAMES 20
int homeBgFrames[HOME_ANIM_FRAMES];
int homeAnimIdx = 0;
int homeAnimTimer = 0;
#define HOME_ANIM_SPEED 100

char playerName[50] = "";
int  nameIndex = 0;
int  storyImg[5];
int  currentStory = 0;
int  nameBgImg, scoreBgImg, instructionsImg, aboutImg;

/* bullets */
int bullet;
struct Bullet { float x, y, dx, dy; bool active; };
Bullet  bullets[MAX_BULLETS];
Bullet  ebullets[MAX_BULLETS];
int     bulletSpeed = 10;
int     bulletCooldown = 0;
int     cooldownMax = 6;

/* enemies */
int Police[3];

/* ================================================================
struct battle  -- MUST be defined here, BEFORE #include "Level1.h"
================================================================ */
struct battle
{
	int  police_x;
	int  police_y;
	int  policeIndex;
	bool policeShow;
	int  spawnSide;
	bool facingRight;
	int  health;
	int  shootTimer;
};
battle enemy[POLICENUMBER];

/* fruit */
bool fruitActive = false;
int  fruitImg;
int  fruitTileX = 0, fruitTileY = 0;
int  fruitX = 0, fruitY = 0;

/* audio */
bool introMusicPlaying = false;
bool mainMusicPlaying = false;

/* ================================================================
FORWARD DECLARATIONS
================================================================ */
void resetAllEnemies(void);
void spawnFruit(void);
void spawnEnemy(int i);
void eFireBullet(int ei);

/* ================================================================
INCLUDES
================================================================ */
#include "RoomCollision.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "Laser.h"
#include "Ammo.h"
#include "Boss.h"
#include "GunUpgrade.h"

/* ================================================================
HELPER: call this everywhere the game ends
================================================================ */
void gameOver(void)
{
	hs_tryInsert(playerName, playerScore);
	gameState = STATE_OVER;
}

/* ================================================================
AUDIO
================================================================ */
void playMenuMusic(void)
{
	PlaySound("Audio\\intro.wav", NULL,
		SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NODEFAULT);
	introMusicPlaying = true;
	mainMusicPlaying = false;
}
void playGameplayMusic(void)
{
	PlaySound("Audio\\mainsong.wav", NULL,
		SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NODEFAULT);
	mainMusicPlaying = true;
	introMusicPlaying = false;
}
void playClickSound(void)
{
	PlaySound("Audio\\click.wav", NULL,
		SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
}
void updateMusic(void)
{
	if (gameState != previousGameState)
	{
		if (gameState == STATE_MENU || gameState == STATE_NAME ||
			gameState == STATE_STORY || gameState == STATE_SCORE ||
			gameState == STATE_INSTRUCTIONS || gameState == STATE_ABOUT)
			playMenuMusic();
		else if (gameState == STATE_PLAY || gameState == STATE_PLAY2 || gameState == STATE_PLAY3)
			playGameplayMusic();
		previousGameState = gameState;
	}
}

/* ================================================================
BULLETS
================================================================ */
void initBullets(void)
{
	int i;
	for (i = 0; i < MAX_BULLETS; i++)
	{
		bullets[i].active = false; bullets[i].dx = 0; bullets[i].dy = 0;
	}
}
void initEbullets(void)
{
	int i;
	for (i = 0; i < MAX_BULLETS; i++)
	{
		ebullets[i].active = false; ebullets[i].dx = 0; ebullets[i].dy = 0;
	}
}
void fireBullet(void)
{
	int i; float len;
	for (i = 0; i < MAX_BULLETS; i++)
	{
		if (!bullets[i].active)
		{
			bullets[i].x = (float)(playerX + 25);
			bullets[i].y = (float)(playerY + 50);
			bullets[i].dx = mouseTargetX - (float)(playerX + PLAYER_W / 2);
			bullets[i].dy = mouseTargetY - (float)(playerY + PLAYER_H / 2);
			len = (float)sqrt((double)(bullets[i].dx*bullets[i].dx +
				bullets[i].dy*bullets[i].dy));
			if (len > 0.0f) { bullets[i].dx /= len; bullets[i].dy /= len; }
			bullets[i].active = true;
			break;
		}
	}
}
void updateBullets(void)
{
	int i;
	if (!hasTarget) return;
	for (i = 0; i < MAX_BULLETS; i++)
	{
		if (!bullets[i].active) continue;
		bullets[i].x += bullets[i].dx * (float)bulletSpeed;
		bullets[i].y += bullets[i].dy * (float)bulletSpeed;
		if (bullets[i].x < 0 || bullets[i].x > SCREEN_WIDTH ||
			bullets[i].y < 0 || bullets[i].y > SCREEN_HEIGHT)
			bullets[i].active = false;
	}
}
void drawBullets(void)
{
	int i;
	float angle;
	for (i = 0; i < MAX_BULLETS; i++)
	{
		if (bullets[i].active)
		{
			angle = (float)(atan2(bullets[i].dy, bullets[i].dx) * 180.0 / 3.14159265);
			if (gun_isUnlocked)
			{
				iRotate(bullets[i].x + 20, bullets[i].y + 15, angle);
				iShowImage((int)bullets[i].x, (int)bullets[i].y, 40, 35, newBulletImg);
				iUnRotate();
			}
			else
			{
				iRotate(bullets[i].x + 10, bullets[i].y + 10, angle);
				iShowImage((int)bullets[i].x, (int)bullets[i].y, 20, 20, bullet);
				iUnRotate();
			}
		}
	}
}
void eUpdateBullets(void)
{
	int i;
	for (i = 0; i < MAX_BULLETS; i++)
	{
		if (!ebullets[i].active) continue;
		ebullets[i].x += ebullets[i].dx * (float)bulletSpeed;
		ebullets[i].y += ebullets[i].dy * (float)bulletSpeed;
		if (ebullets[i].x < 0 || ebullets[i].x > SCREEN_WIDTH ||
			ebullets[i].y < 0 || ebullets[i].y > SCREEN_HEIGHT)
			ebullets[i].active = false;
	}
}
void eDrawBullets(void)
{
	int i;
	float angle;
	for (i = 0; i < MAX_BULLETS; i++)
	{
		if (ebullets[i].active)
		{
			angle = (float)(atan2(ebullets[i].dy, ebullets[i].dx) * 180.0 / 3.14159265);
			iRotate(ebullets[i].x + 10, ebullets[i].y + 10, angle);
			iShowImage((int)ebullets[i].x, (int)ebullets[i].y, 20, 20, bullet);
			iUnRotate();
		}
	}
}

/* ================================================================
ENEMIES
================================================================ */
void enemyMovement(void)
{
	int i, barX, barY;
	for (i = 0; i < POLICENUMBER; i++)
	{
		if (!enemy[i].policeShow) continue;
		if (enemy[i].facingRight)
			iShowImage(enemy[i].police_x, enemy[i].police_y,
			50, 100, Police[enemy[i].policeIndex]);
		else
			iShowImage(enemy[i].police_x + 50, enemy[i].police_y,
			-50, 100, Police[enemy[i].policeIndex]);
		barX = enemy[i].police_x;
		barY = enemy[i].police_y + 105;
		iSetColor(150, 0, 0);
		iFilledRectangle(barX, barY, 50, 8);
		iSetColor(0, 200, 0);
		iFilledRectangle(barX, barY,
			(50 * enemy[i].health) / ENEMY_MAX_HEALTH, 8);
	}
}
void spawnEnemy(int i)
{
	int side = rand() % 4;
	enemy[i].policeShow = true;
	enemy[i].spawnSide = side;
	if (side == 0)      { enemy[i].police_x = -50;               enemy[i].police_y = rand() % SCREEN_HEIGHT; }
	else if (side == 1) { enemy[i].police_x = SCREEN_WIDTH + 50; enemy[i].police_y = rand() % SCREEN_HEIGHT; }
	else if (side == 2) { enemy[i].police_x = rand() % SCREEN_WIDTH; enemy[i].police_y = SCREEN_HEIGHT + 50; }
	else                { enemy[i].police_x = rand() % SCREEN_WIDTH; enemy[i].police_y = -50; }
	enemy[i].policeIndex = rand() % 3;
	enemy[i].facingRight = true;
	enemy[i].health = ENEMY_MAX_HEALTH;
	enemy[i].shootTimer = 60 + rand() % 60;
}
void setEnemyVariables(void)
{
	int i;
	for (i = 0; i < POLICENUMBER; i++) spawnEnemy(i);
}
void resetAllEnemies(void)
{
	int i;
	for (i = 0; i < POLICENUMBER; i++)
	{
		enemy[i].policeShow = false;
		enemy[i].police_x = -1000;
		enemy[i].police_y = -1000;
		enemy[i].policeIndex = rand() % 3;
		enemy[i].facingRight = true;
		enemy[i].health = ENEMY_MAX_HEALTH;
		enemy[i].shootTimer = 60 + rand() % 60;
	}
	spawnTimer = 0;
	nextSpawnTime = 40 + rand() % 80;
}
void eFireBullet(int ei)
{
	int i; float dx, dy, len;
	for (i = 0; i < MAX_BULLETS; i++)
	{
		if (!ebullets[i].active)
		{
			ebullets[i].x = (float)(enemy[ei].police_x + 25);
			ebullets[i].y = (float)(enemy[ei].police_y + 50);
			dx = (float)(playerX + PLAYER_W / 2) - ebullets[i].x;
			dy = (float)(playerY + PLAYER_H / 2) - ebullets[i].y;
			len = (float)sqrt((double)(dx*dx + dy*dy));
			if (len > 0.0f) { ebullets[i].dx = dx / len; ebullets[i].dy = dy / len; }
			ebullets[i].active = true;
			break;
		}
	}
}

/* ================================================================
COLLISION
================================================================ */
bool isBulletHittingEnemy(int bi, int ei)
{
	return (bullets[bi].x + 20 > enemy[ei].police_x &&
		bullets[bi].x      < enemy[ei].police_x + 50 &&
		bullets[bi].y + 20 > enemy[ei].police_y &&
		bullets[bi].y      < enemy[ei].police_y + 100);
}
void checkBulletEnemyCollisions(void)
{
	int bi, ei;
	for (bi = 0; bi < MAX_BULLETS; bi++)
	{
		if (!bullets[bi].active) continue;
		for (ei = 0; ei < POLICENUMBER; ei++)
		{
			if (!enemy[ei].policeShow) continue;
			if (isBulletHittingEnemy(bi, ei))
			{
				bullets[bi].active = false;
				enemy[ei].health--;
				if (enemy[ei].health <= 0)
				{
					enemy[ei].policeShow = false;
					playerScore += inLevel3 ? L3_SCORE_PER_KILL : (inLevel2 ? L2_SCORE_PER_KILL : 5);
					enemy[ei].health = ENEMY_MAX_HEALTH;
				}
				break;
			}
		}
	}
}
bool isBulletHittingPlayer(int bi)
{
	return (ebullets[bi].x + 20 > playerX &&
		ebullets[bi].x      < playerX + PLAYER_W &&
		ebullets[bi].y + 20 > playerY &&
		ebullets[bi].y      < playerY + PLAYER_H);
}
void checkBulletPlayerCollisions(void)
{
	int bi;
	for (bi = 0; bi < MAX_BULLETS; bi++)
	{
		if (!ebullets[bi].active) continue;
		if (isBulletHittingPlayer(bi))
		{
			ebullets[bi].active = false;
			playerHP -= 5;
			if (playerHP <= 0)
				gameOver();   // ← saves score then sets STATE_OVER
			break;
		}
	}
}

/* ================================================================
MISC
================================================================ */
void spawnFruit(void)
{
	int oldTX = fruitTileX, oldTY = fruitTileY;
	fruitActive = true;
	do { fruitTileX = rand() % 3; fruitTileY = rand() % 3; } while (fruitTileX == oldTX && fruitTileY == oldTY);
	fruitX = 100 + rand() % (SCREEN_WIDTH - 200);
	fruitY = 100 + rand() % (SCREEN_HEIGHT - 200);
}

void drawHomePage(void)
{
	int i;
	homeAnimTimer++;
	if (homeAnimTimer >= HOME_ANIM_SPEED)
	{
		homeAnimTimer = 0;
		homeAnimIdx++;
		if (homeAnimIdx >= HOME_ANIM_FRAMES)
			homeAnimIdx = 0;
	}
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, homeBgFrames[homeAnimIdx]);
	for (i = 0; i < 4; i++)
	{
		bool hovered = (mouseX >= menuButtons[i].x &&
			mouseX <= menuButtons[i].x + menuButtons[i].w &&
			mouseY >= menuButtons[i].y &&
			mouseY <= menuButtons[i].y + menuButtons[i].h);
		if (hovered)
			iShowImage(menuButtons[i].x, menuButtons[i].y,
			menuButtons[i].w, menuButtons[i].h, menuButtons[i].imgHover);
		else
			iShowImage(menuButtons[i].x, menuButtons[i].y,
			menuButtons[i].w, menuButtons[i].h, menuButtons[i].img);
	}
}

/* ================================================================
IMAGE LOADING
================================================================ */
void allImg(void)
{
	int  i;
	char a[100];

	for (int i = 0; i < HOME_ANIM_FRAMES; i++)
	{
		char path[100];
		sprintf_s(path, sizeof(path), "Images//home_bg_%d.png", i + 1);
		homeBgFrames[i] = iLoadImage(path);
	}

	btnStart = iLoadImage("Images//start.png");
	btnScores = iLoadImage("Images//scores.png");
	btnInstructions = iLoadImage("Images//instructions.png");
	btnAbout = iLoadImage("Images//about.png");
	btnStartHover = iLoadImage("Images//start_hover.png");
	btnScoresHover = iLoadImage("Images//scores_hover.png");
	btnInstructionsHover = iLoadImage("Images//instructions_hover.png");
	btnAboutHover = iLoadImage("Images//about_hover.png");

	nameBgImg = iLoadImage("Images//namebg.png");
	scoreBgImg = iLoadImage("Images//scoreboard.png");
	instructionsImg = iLoadImage("Images//instruction.png");
	aboutImg = iLoadImage("Images//crew.png");
	fruitImg = iLoadImage("Images//fruits.png");
	winBg = iLoadImage("Images//room10(2).png");
	bullet = iLoadImage("Images//bullet.png");

	for (i = 0; i < 5; i++)
	{
		sprintf_s(a, sizeof(a), "Images//Story%d.png", i + 1);
		storyImg[i] = iLoadImage(a);
	}

	menuButtons[0].x = 150;  menuButtons[0].y = 40;
	menuButtons[0].w = 300;  menuButtons[0].h = 300;
	menuButtons[0].img = btnStart; menuButtons[0].imgHover = btnStartHover;

	menuButtons[1].x = 500;  menuButtons[1].y = 40;
	menuButtons[1].w = 300;  menuButtons[1].h = 300;
	menuButtons[1].img = btnScores; menuButtons[1].imgHover = btnScoresHover;

	menuButtons[2].x = 850;  menuButtons[2].y = 40;
	menuButtons[2].w = 300;  menuButtons[2].h = 300;
	menuButtons[2].img = btnInstructions; menuButtons[2].imgHover = btnInstructionsHover;

	menuButtons[3].x = 1200; menuButtons[3].y = 40;
	menuButtons[3].w = 300;  menuButtons[3].h = 300;
	menuButtons[3].img = btnAbout; menuButtons[3].imgHover = btnAboutHover;

	bg[0] = iLoadImage("Images\\room1.png");
	bg[1] = iLoadImage("Images\\room2.png");
	bg[2] = iLoadImage("Images\\lobby.png");
	bg[3] = iLoadImage("Images\\room4.png");
	bg[4] = iLoadImage("Images\\room5.png");
	bg[5] = iLoadImage("Images\\room6.png");
	bg[6] = iLoadImage("Images\\room7.png");
	bg[7] = iLoadImage("Images\\room8.png");
	bg[8] = iLoadImage("Images\\room10.png");

	loadLevel2Images();
	loadLevel3Images();

	ammo_loadImage();
	ammo_init();

	boss_loadImage();
	boss_init();

	Police[0] = iLoadImage("Images//police1.png");
	Police[1] = iLoadImage("Images//police2.png");
	Police[2] = iLoadImage("Images//police3.png");

	for (i = 0; i < 3; i++)
	{
		sprintf_s(a, sizeof(a), "Images//pic%d.png", i + 1);
		charImg[i] = iLoadImage(a);
	}

	charImgUpgraded[0] = iLoadImage("Images\\new_walk1.png");
	charImgUpgraded[1] = iLoadImage("Images\\new_walk2.png");
	charImgUpgraded[2] = iLoadImage("Images\\new_walk3.png");
	aimImg = iLoadImage("Images\\aim.png");
	newBulletImg = iLoadImage("Images\\new_bullet.png");

	playMenuMusic();
}

/* ================================================================
iDraw
================================================================ */
void iDraw(void)
{
	int    bgIdx;
	char   buf[128];
	char   line[128];
	double X[3], Y[3];
	float  dx, dy, ang, L, S, bx, by;
	int    tx, ty, tTX, tTY;

	iClear();
	iSetColor(255, 255, 255);
	updateMusic();

	/* MENU */
	if (gameState == STATE_MENU)
	{
		drawHomePage(); return;
	}

	/* NAME */
	if (gameState == STATE_NAME)
	{
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, nameBgImg);
		iText(600, 500, "Enter Your Name:", GLUT_BITMAP_TIMES_ROMAN_24);
		iRectangle(550, 445, 400, 50);
		iText(570, 470, playerName, GLUT_BITMAP_HELVETICA_18);
		iText(550, 400, "Press ENTER to continue", GLUT_BITMAP_HELVETICA_18);
		return;
	}

	/* STORY */
	if (gameState == STATE_STORY)
	{
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, storyImg[currentStory]);
		iText(50, 50, "Press ENTER to continue", GLUT_BITMAP_HELVETICA_18);
		return;
	}

	/* SCORE */
	if (gameState == STATE_SCORE)
	{
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, scoreBgImg);
		hs_draw();
		return;
	}

	/* INSTRUCTIONS */
	if (gameState == STATE_INSTRUCTIONS)
	{
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, instructionsImg);
		iText(50, 50, "Press ESC to return");
		return;
	}

	/* ABOUT */
	if (gameState == STATE_ABOUT)
	{
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, aboutImg);
		iText(50, 50, "Press ESC to return");
		return;
	}

	/* GAME OVER */
	if (gameState == STATE_OVER)
	{
		iSetColor(0, 0, 0);
		iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		iSetColor(255, 0, 0);
		iText(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 50,
			"GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);
		iSetColor(255, 255, 255);
		iText(SCREEN_WIDTH / 2 - 160, SCREEN_HEIGHT / 2,
			"Press ESC to return to Menu", GLUT_BITMAP_HELVETICA_18);
		return;
	}

	/* LEVEL-1 PUZZLE */
	if (gameState == STATE_PUZZLE)
	{
		iSetColor(20, 20, 20);
		iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		iSetColor(0, 255, 0);
		iText(SCREEN_WIDTH / 2 - 200, 820,
			"LOCKER NUMBER PATTERN PUZZLE", GLUT_BITMAP_HELVETICA_18);
		sprintf_s(line, sizeof(line), "Sequence: %d, %d, %d, ?",
			patternA, patternA*patternMul, patternA*patternMul*patternMul);
		iSetColor(255, 255, 255);
		iText(SCREEN_WIDTH / 2 - 100, 700, line, GLUT_BITMAP_TIMES_ROMAN_24);
		iSetColor(255, 255, 0);
		iText(SCREEN_WIDTH / 2 - 140, 640, "Enter Password:");
		iSetColor(50, 50, 50);
		iFilledRectangle(SCREEN_WIDTH / 2 - 150, 580, 300, 50);
		iSetColor(0, 255, 0);
		iRectangle(SCREEN_WIDTH / 2 - 150, 580, 300, 50);
		iSetColor(255, 255, 255);
		iText(SCREEN_WIDTH / 2 - 130, 600,
			puzzleLen > 0 ? puzzleInput : "_",
			GLUT_BITMAP_TIMES_ROMAN_24);
		sprintf_s(buf, sizeof(buf), "Time: %d sec", puzzleTime / 60);
		iSetColor(255, 50, 50);
		iText(SCREEN_WIDTH / 2 - 100, 500, buf);
		return;
	}

	/* LEVEL-2 PUZZLE */
	if (gameState == STATE_PUZZLE2) { drawPuzzle2Screen(); return; }

	/* LEVEL-3 PUZZLE */
	if (gameState == STATE_PUZZLE3) { drawPuzzle3Screen(); return; }

	/* GUN UPGRADE PUZZLE */
	if (gameState == STATE_GUN_PUZZLE) { gun_drawPuzzleScreen(); return; }

	/* LEVEL-1 WIN */
	if (gameState == STATE_WIN)
	{
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, winBg);
		iSetColor(0, 0, 0);
		iFilledRectangle(330, 330, 1040, 240);
		iSetColor(0, 255, 0);
		iRectangle(330, 330, 1040, 240);
		if (enteringLevel2)
		{
			iSetColor(255, 220, 0);
			iText(450, 530, "LEVEL 1 COMPLETE!", GLUT_BITMAP_TIMES_ROMAN_24);
			iSetColor(255, 255, 255);
			iText(400, 470, "Entering Level 2... Get Ready!", GLUT_BITMAP_HELVETICA_18);
			sprintf_s(buf, sizeof(buf), "Loading in %d ...", winTimer / 60 + 1);
			iSetColor(200, 200, 200);
			iText(560, 420, buf, GLUT_BITMAP_HELVETICA_12);
		}
		else
		{
			iSetColor(255, 255, 0);
			iText(490, 530, "PUZZLE SOLVED!", GLUT_BITMAP_TIMES_ROMAN_24);
			iSetColor(255, 255, 255);
			iText(390, 480, "Find the ESCAPE DOOR to complete Level 1!", GLUT_BITMAP_HELVETICA_18);
			iSetColor(0, 255, 100);
			iText(430, 435, "The green door has appeared somewhere on the map...", GLUT_BITMAP_HELVETICA_12);
			sprintf_s(buf, sizeof(buf), "Returning to game in %d ...", winTimer / 60 + 1);
			iSetColor(200, 200, 200);
			iText(560, 390, buf, GLUT_BITMAP_HELVETICA_12);
		}
		return;
	}

	/* LEVEL-2 WIN */
	if (gameState == STATE_WIN2) { drawLevel2WinScreen(); return; }

	/* LEVEL-3 WIN */
	if (gameState == STATE_WIN3) { drawLevel3WinScreen(); return; }

	/* ============================================================
	GAMEPLAY
	============================================================ */
	bgIdx = tileY * 3 + tileX;
	if (bgIdx >= 0 && bgIdx < 9)
	{
		if (inLevel3)       iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bg3[bgIdx]);
		else if (inLevel2)  iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bg2[bgIdx]);
		else                iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bg[bgIdx]);
	}

	if (tileX == fruitTileX && tileY == fruitTileY)
		iShowImage(fruitX, fruitY, 50, 50, fruitImg);

	if (!inLevel2 && !inLevel3 && lockerActive &&
		tileX == lockerTileX && tileY == lockerTileY)
	{
		iSetColor(80, 80, 80);
		iFilledRectangle(lockerX, lockerY, 70, 90);
		iSetColor(255, 255, 0);
		iRectangle(lockerX, lockerY, 70, 90);
	}

	drawBullets();
	laser_drawAll();
	laser_drawHUD();
	ammo_drawPickup();
	ammo_drawHUD();
	boss_draw();
	boss_drawHUD();
	gun_drawRoomItems();
	enemyMovement();
	eDrawBullets();
	playerHealth();

	sprintf_s(buf, sizeof(buf), "Score: %d", playerScore);
	iSetColor(255, 255, 255);
	iText(20, SCREEN_HEIGHT - 40, buf, GLUT_BITMAP_HELVETICA_18);

	if (playerShootTimer > 0 && gun_isUnlocked)
	{
		if (playerFacingRight) iShowImage(playerX, playerY, UPGRADED_W, UPGRADED_H, aimImg);
		else                   iShowImage(playerX + UPGRADED_W, playerY, -UPGRADED_W, UPGRADED_H, aimImg);
	}
	else
	{
		if (playerFacingRight) {
			if (gun_isUnlocked) iShowImage(playerX, playerY, UPGRADED_W, UPGRADED_H, charImgUpgraded[idx]);
			else                iShowImage(playerX, playerY, PLAYER_W, PLAYER_H, charImg[idx]);
		}
		else {
			if (gun_isUnlocked) iShowImage(playerX + UPGRADED_W, playerY, -UPGRADED_W, UPGRADED_H, charImgUpgraded[idx]);
			else                iShowImage(playerX + PLAYER_W, playerY, -PLAYER_W, PLAYER_H, charImg[idx]);
		}
	}

	if (!inLevel2 && !inLevel3 && doorActive &&
		tileX == doorTileX && tileY == doorTileY)
	{
		iSetColor(0, 80, 0);
		iFilledRectangle(doorX, doorY, 60, 80);
		iSetColor(0, 255, 0);
		iRectangle(doorX, doorY, 60, 80);
		iSetColor(0, 255, 0);
		iText(doorX - 10, doorY + 90, "EXIT!", GLUT_BITMAP_HELVETICA_12);
	}

	if (!inLevel2 && !inLevel3)
	{
		bool drawArrow = false;
		tx = 0; ty = 0; tTX = 0; tTY = 0;
		if (indicatorTarget == TARGET_LOCKER && lockerActive)
		{
			tx = lockerX; ty = lockerY; tTX = lockerTileX; tTY = lockerTileY; drawArrow = true;
		}
		else if (doorActive)
		{
			tx = doorX; ty = doorY; tTX = doorTileX; tTY = doorTileY; drawArrow = true;
		}
		if (drawArrow)
		{
			dx = (float)((tTX - tileX)*SCREEN_WIDTH + (tx - playerX));
			dy = (float)((tTY - tileY)*SCREEN_HEIGHT + (ty - playerY));
			ang = (float)atan2((double)dy, (double)dx);
			L = 60.0f; S = 25.0f;
			bx = (float)(SCREEN_WIDTH / 2) + (float)cos((double)ang)*L;
			by = (float)(SCREEN_HEIGHT / 2) + (float)sin((double)ang)*L;
			X[0] = (double)bx + cos((double)ang)        *(double)S;
			X[1] = (double)bx + cos((double)ang + 1.57) *(double)S;
			X[2] = (double)bx - cos((double)ang + 1.57) *(double)S;
			Y[0] = (double)by + sin((double)ang)        *(double)S;
			Y[1] = (double)by + sin((double)ang + 1.57) *(double)S;
			Y[2] = (double)by - sin((double)ang + 1.57) *(double)S;
			iSetColor(255, 80, 40);
			iFilledPolygon(X, Y, 3);
		}
	}

	if (inLevel2) drawLevel2HUD(playerScore);
	if (l2_levelComplete && gameState != STATE_WIN2) drawLevel2CompleteScreen();
	if (inLevel3) drawLevel3HUD(playerScore);
	if (l3_levelComplete && gameState != STATE_WIN3) drawLevel3CompleteScreen();
}

/* ================================================================
MOUSE
================================================================ */
void iMouseMove(int mx, int my)        { mouseX = mx; mouseY = my; }
void iPassiveMouseMove(int mx, int my) { mouseX = mx; mouseY = my; }

void iMouse(int button, int state, int mx, int my)
{
	int i;
	if (gameState == STATE_MENU &&
		button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		for (i = 0; i < 4; i++)
		{
			if (mx >= menuButtons[i].x &&
				mx <= menuButtons[i].x + menuButtons[i].w &&
				my >= menuButtons[i].y &&
				my <= menuButtons[i].y + menuButtons[i].h)
			{
				playClickSound();
				if (i == 0) { nameIndex = 0; playerName[0] = '\0'; gameState = STATE_NAME; }
				else if (i == 1) gameState = STATE_SCORE;
				else if (i == 2) gameState = STATE_INSTRUCTIONS;
				else if (i == 3) gameState = STATE_ABOUT;
			}
		}
	}
	if ((gameState == STATE_PLAY || gameState == STATE_PLAY2 || gameState == STATE_PLAY3) &&
		button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && bulletCooldown == 0)
	{
		mouseTargetX = (float)mx;
		mouseTargetY = (float)my;
		playerFacingRight = (mouseTargetX >= (float)(playerX + PLAYER_W / 2));
		if (ammo_canShoot())
		{
			fireBullet();
			ammo_consume();
		}
		bulletCooldown = cooldownMax;
		hasTarget = true;
		playerShootTimer = 15;
	}
}

/* ================================================================
KEYBOARD
================================================================ */
void iKeyboard(unsigned char key)
{
	if (gameState == STATE_NAME)
	{
		if (key == 13) { playClickSound(); playerName[nameIndex] = '\0'; currentStory = 0; gameState = STATE_STORY; }
		else if (key == 8 && nameIndex > 0) { nameIndex--; playerName[nameIndex] = '\0'; }
		else if (nameIndex < 49 && key >= 32 && key <= 126) { playerName[nameIndex++] = key; playerName[nameIndex] = '\0'; }
		return;
	}
	if (gameState == STATE_STORY)
	{
		if (key == 13)
		{
			playClickSound(); currentStory++;
			if (currentStory >= 5)
			{
				playerX = 100; playerY = 100; playerHP = CHAR_MAX_HEALTH; playerScore = 0;
				tileX = 0; tileY = 0; lockerActive = true; doorActive = false; levelComplete = false;
				resetAllEnemies(); gameState = STATE_PLAY;
			}
		}
		return;
	}
	if (gameState == STATE_PUZZLE)
	{
		if (key >= '0' && key <= '9')
		{
			if (puzzleLen < 10) { puzzleInput[puzzleLen++] = key; puzzleInput[puzzleLen] = '\0'; }
		}
		else if (key == 8 && puzzleLen > 0) { puzzleLen--; puzzleInput[puzzleLen] = '\0'; }
		else if (key == 13 && puzzleLen > 0)
		{
			if (atoi(puzzleInput) == puzzleAnswer)
			{
				gameState = STATE_WIN; winTimer = 180; indicatorTarget = TARGET_DOOR;
			}
			else
			{
				puzzleLen = 0; puzzleInput[0] = '\0';
				gameOver();   // ← FIX: saves score then sets STATE_OVER
			}
		}
		else if (key == 27) gameState = STATE_MENU;
		return;
	}
	if (key == 27) { playClickSound(); inLevel2 = false; inLevel3 = false; gameState = STATE_MENU; }
}
void iSpecialKeyboard(unsigned char key) { (void)key; }

/* ================================================================
change()  -- enemy AI timer
================================================================ */
void change(void)
{
	int i; float dx, dy, dist;

	if (inLevel2 && gameState == STATE_PLAY2) { level2EnemyUpdate(); return; }
	if (inLevel3 && gameState == STATE_PLAY3) { level3EnemyUpdate(); return; }
	if (gameState != STATE_PLAY) return;

	spawnTimer++;
	if (spawnTimer > nextSpawnTime)
	{
		for (i = 0; i < POLICENUMBER; i++)
		if (!enemy[i].policeShow) { spawnEnemy(i); break; }
		spawnTimer = 0;
		nextSpawnTime = 50 + rand() % 100;
	}
	for (i = 0; i < POLICENUMBER; i++)
	{
		if (!enemy[i].policeShow) continue;
		dx = (float)(playerX - enemy[i].police_x);
		dy = (float)(playerY - enemy[i].police_y);
		dist = (float)sqrt((double)(dx*dx + dy*dy));
		enemy[i].shootTimer--;
		if (enemy[i].shootTimer <= 0)
		{
			eFireBullet(i); enemy[i].shootTimer = 80 + rand() % 80;
		}
		enemy[i].facingRight = (dx >= 0) ? true : false;
		if (dist > 0.0f)
		{
			enemy[i].police_x += (int)((dx / dist) * 2.5f);
			enemy[i].police_y += (int)((dy / dist) * 2.5f);
		}
	}
}

/* ================================================================
fixedUpdate()  -- main logic timer
================================================================ */
void fixedUpdate(void)
{
	bool isMoving;
	int  fruitW = 50, fruitH = 50;

	/* ESC */
	{ static bool ep = false;
	if (isKeyPressed(27)) { if (!ep) { playClickSound(); inLevel2 = false; inLevel3 = false; gameState = STATE_MENU; ep = true; } }
	else ep = false; }

	/* NAME input */
	if (gameState == STATE_NAME)
	{
		static bool lp[128]; static bool lpI = false;
		if (!lpI) { int k; for (k = 0; k < 128; k++) lp[k] = false; lpI = true; }
		static bool bp = false, enp = false; char c;
		for (c = 'A'; c <= 'Z'; c++) { if (isKeyPressed(c)){ if (!lp[(int)c] && nameIndex<49){ playerName[nameIndex++] = c; playerName[nameIndex] = '\0'; lp[(int)c] = true; } } else lp[(int)c] = false; }
		for (c = 'a'; c <= 'z'; c++) { if (isKeyPressed(c)){ if (!lp[(int)c] && nameIndex<49){ playerName[nameIndex++] = c; playerName[nameIndex] = '\0'; lp[(int)c] = true; } } else lp[(int)c] = false; }
		if (isKeyPressed(' ')){ if (!lp[' '] && nameIndex<49){ playerName[nameIndex++] = ' '; playerName[nameIndex] = '\0'; lp[' '] = true; } }
		else lp[' '] = false;
		if (isKeyPressed(8)){ if (!bp&&nameIndex>0){ nameIndex--; playerName[nameIndex] = '\0'; bp = true; } }
		else bp = false;
		if (isKeyPressed(13)){ if (!enp){ playClickSound(); playerName[nameIndex] = '\0'; currentStory = 0; gameState = STATE_STORY; enp = true; } }
		else enp = false;
		return;
	}

	/* STORY */
	if (gameState == STATE_STORY)
	{
		static bool enp = false;
		if (isKeyPressed(13)){
			if (!enp){
				playClickSound(); currentStory++;
				if (currentStory >= 5){
					playerX = 100; playerY = 100; playerHP = CHAR_MAX_HEALTH; playerScore = 0;
					tileX = 0; tileY = 0; lockerActive = true; doorActive = false; levelComplete = false;
					resetAllEnemies(); spawnFruit();
					gameState = STATE_PLAY;
					ammo_init(); boss_init();
				} enp = true;
			}
		}
		else enp = false;
		return;
	}

	/* LEVEL-1 PUZZLE */
	if (gameState == STATE_PUZZLE)
	{
		static bool np[10]; static bool npI = false;
		if (!npI){ int k; for (k = 0; k < 10; k++) np[k] = false; npI = true; }
		static bool bp = false, enp = false; int i; char c;
		for (i = 0; i <= 9; i++){ c = (char)('0' + i); if (isKeyPressed(c)){ if (!np[i] && puzzleLen<10){ puzzleInput[puzzleLen++] = c; puzzleInput[puzzleLen] = '\0'; np[i] = true; } } else np[i] = false; }
		if (isKeyPressed(8)){ if (!bp&&puzzleLen>0){ puzzleLen--; puzzleInput[puzzleLen] = '\0'; bp = true; } }
		else bp = false;
		if (isKeyPressed(13)){
			if (!enp&&puzzleLen>0){
				if (atoi(puzzleInput) == puzzleAnswer)
				{
					levelComplete = true;
					gameState = STATE_WIN;
					winTimer = 180;
					indicatorTarget = TARGET_LOCKER;
				}
				else
				{
					puzzleLen = 0; puzzleInput[0] = '\0';
					gameOver();   // ← FIX: saves score then sets STATE_OVER
				}
				enp = true;
			}
		}
		else enp = false;

		puzzleTime--;
		if (puzzleTime <= 0)
			gameOver();   // ← FIX: puzzle timeout also saves score

		return;
	}

	/* LEVEL-2 PUZZLE */
	if (gameState == STATE_PUZZLE2) { updatePuzzle2Input(); return; }

	/* LEVEL-3 PUZZLE */
	if (gameState == STATE_PUZZLE3) { updatePuzzle3Input(); return; }

	/* GUN PUZZLE */
	if (gameState == STATE_GUN_PUZZLE) { gun_updateInput(); return; }

	/* LEVEL-1 WIN TIMER */
	if (gameState == STATE_WIN)
	{
		winTimer--;
		if (winTimer <= 0)
		{
			if (enteringLevel2)
			{
				enteringLevel2 = false;
				playerHP = CHAR_MAX_HEALTH;
				initLevel2();
				gameState = STATE_PLAY2;
			}
			else
			{
				spawnDoor();
				indicatorTarget = TARGET_DOOR;
				gameState = STATE_PLAY;
			}
		}
		return;
	}

	if (gameState == STATE_WIN2) { level2WinUpdate(); return; }
	if (gameState == STATE_WIN3) { level3WinUpdate(); return; }

	if (gameState != STATE_PLAY && gameState != STATE_PLAY2 && gameState != STATE_PLAY3) return;

	if (playerShootTimer > 0) playerShootTimer--;

	if (bulletCooldown > 0) bulletCooldown--;
	if (isKeyPressed(' ') && bulletCooldown == 0)
	{
		if (ammo_canShoot())
		{
			hasTarget = true; fireBullet(); ammo_consume();
			bulletCooldown = cooldownMax; playerShootTimer = 15;
		}
	}

	updateBullets();
	eUpdateBullets();
	checkBulletEnemyCollisions();
	checkBulletPlayerCollisions();
	laser_update();
	ammo_update();
	boss_update();
	gun_update();

	/* movement */
	isMoving = false;
	if (isKeyPressed('w') || isSpecialKeyPressed(GLUT_KEY_UP))    { playerY += MOVE_SPEED; isMoving = true; }
	if (isKeyPressed('a') || isSpecialKeyPressed(GLUT_KEY_LEFT))  { playerX -= MOVE_SPEED; playerFacingRight = false; isMoving = true; }
	if (isKeyPressed('s') || isSpecialKeyPressed(GLUT_KEY_DOWN))  { playerY -= MOVE_SPEED; isMoving = true; }
	if (isKeyPressed('d') || isSpecialKeyPressed(GLUT_KEY_RIGHT)) { playerX += MOVE_SPEED; playerFacingRight = true;  isMoving = true; }

	if (isMoving) { static int fd = 0; fd++; if (fd>5){ idx++; if (idx >= 3)idx = 0; fd = 0; } }
	else idx = 0;

	if (inLevel2 || inLevel3) applyRoomCollision(playerX, playerY);

	/* tile switching */
	if (playerX > SCREEN_WIDTH)  { if (tileX<MAP_MAX_X){ playerX = 0; tileX++; resetAllEnemies(); } else playerX = SCREEN_WIDTH - PLAYER_W; }
	if (playerX < 0)             { if (tileX>MAP_MIN_X){ playerX = SCREEN_WIDTH - PLAYER_W; tileX--; resetAllEnemies(); } else playerX = 0; }
	if (playerY > SCREEN_HEIGHT) { if (tileY<MAP_MAX_Y){ playerY = 0; tileY++; resetAllEnemies(); } else playerY = SCREEN_HEIGHT - PLAYER_H; }
	if (playerY < 0)             { if (tileY>MAP_MIN_Y){ playerY = SCREEN_HEIGHT - PLAYER_H; tileY--; resetAllEnemies(); } else playerY = 0; }

	/* fruit */
	if (tileX == fruitTileX && tileY == fruitTileY)
	{
		if (playerX < fruitX + fruitW && playerX + PLAYER_W > fruitX &&
			playerY < fruitY + fruitH && playerY + PLAYER_H > fruitY)
		{
			playerHP += FRUIT_HEAL; if (playerHP>CHAR_MAX_HEALTH) playerHP = CHAR_MAX_HEALTH;
			spawnFruit();
		}
	}

	/* LEVEL-1 locker */
	if (lockerActive && tileX == lockerTileX && tileY == lockerTileY)
	{
		if (playerX < lockerX + 70 && playerX + PLAYER_W > lockerX &&
			playerY < lockerY + 90 && playerY + PLAYER_H > lockerY)
		{
			gameState = STATE_PUZZLE; lockerActive = false; generatePattern();
		}
	}

	/* LEVEL-1 door */
	if (!doorActive)
	{
		if (!levelComplete)
		{
			nextDoorTime--;
			if (nextDoorTime <= 0) { spawnDoor(); nextDoorTime = 1500 + rand() % 1000; }
		}
	}
	else
	{
		doorTimer++;
		if (doorTimer > doorDuration)
		{
			doorActive = false;
			if (levelComplete) spawnDoor();
		}
		if (tileX == doorTileX && tileY == doorTileY)
		{
			if (playerX < doorX + 60 && playerX + PLAYER_W > doorX &&
				playerY < doorY + 80 && playerY + PLAYER_H > doorY)
			{
				if (levelComplete)
				{
					doorActive = false;
					enteringLevel2 = true;
					winTimer = 180;
					gameState = STATE_WIN;
				}
				else doorActive = false;
			}
		}
	}

	if (gameState == STATE_PLAY2) { level2FixedUpdate(); return; }
	if (gameState == STATE_PLAY3) { level3FixedUpdate(); return; }
}

/* ================================================================
MAIN
================================================================ */
int main(void)
{
	srand((unsigned int)time(NULL));
	nextSpawnTime = 50 + rand() % 100;
	iInitialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Money Heist");
	allImg();
	setEnemyVariables();
	hs_load();   // load high scores from disk at startup

	iSetTimer(16, change);
	iSetTimer(16, fixedUpdate);
	initBullets();
	initEbullets();


	/* ==========================================
	DEBUG BYPASS: JUMP STRAIGHT TO LEVEL 3
	========================================== */
	initLevel3();
	gameState = STATE_PLAY3;
	ammo_init();
	boss_init();
	/* ========================================== */

	iStart();
	return 0;
}