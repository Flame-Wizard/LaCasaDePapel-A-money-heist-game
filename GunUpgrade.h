#ifndef GUN_UPGRADE_H
#define GUN_UPGRADE_H

/* ================================================================
GUN UPGRADE SYSTEM
================================================================ */
#define STATE_GUN_PUZZLE 30

bool gun_foundPassword = false;
bool gun_isUnlocked = false;
int  gun_feedbackTimer = 0;
bool gun_wrongPass = false;

char gun_password[] = "ARMORY";
char gun_input[20] = "";
int  gun_inputLen = 0;

/* Positions for the items in the bottom right corners */
#define ITEM_X 1450
#define ITEM_Y 150
#define ITEM_W 80
#define ITEM_H 60

/* ================================================================
gun_drawRoomItems()
Draws the password note in Room 2 and the Weapon Crate in Room 5
================================================================ */
void gun_drawRoomItems()
{
	/* Room 2 (Tile 1,0): The Password Note */
	if (tileX == 1 && tileY == 0 && !gun_foundPassword)
	{
		/* Glowing folder */
		iSetColor(255, 200, 0);
		iFilledRectangle(ITEM_X, ITEM_Y, 50, 40);
		iSetColor(255, 255, 255);
		iRectangle(ITEM_X, ITEM_Y, 50, 40);
		iText(ITEM_X - 10, ITEM_Y + 50, "INTEL", GLUT_BITMAP_HELVETICA_12);
	}

	/* Room 5 (Tile 1,1): The Weapon Crate */
	if (tileX == 1 && tileY == 1 && !gun_isUnlocked)
	{
		/* Heavy Crate */
		iSetColor(40, 40, 40);
		iFilledRectangle(ITEM_X, ITEM_Y, ITEM_W, ITEM_H);
		iSetColor(150, 150, 150);
		iRectangle(ITEM_X, ITEM_Y, ITEM_W, ITEM_H);
		iSetColor(255, 50, 50);
		iFilledRectangle(ITEM_X + 10, ITEM_Y + 25, 60, 10); /* Red stripe */
		iSetColor(255, 255, 255);
		iText(ITEM_X - 20, ITEM_Y + 70, "LOCKED WEAPON CRATE", GLUT_BITMAP_HELVETICA_12);
	}

	/* HUD Element: Always show password once found */
	if (gun_foundPassword && !gun_isUnlocked)
	{
		iSetColor(0, 0, 0);
		iFilledRectangle(20, 740, 250, 30);
		iSetColor(255, 200, 0);
		iRectangle(20, 740, 250, 30);
		iText(30, 748, "Crate Password: ARMORY", GLUT_BITMAP_HELVETICA_18);
	}
}

/* ================================================================
gun_update()
Call in fixedUpdate() to check collisions
================================================================ */
void gun_update()
{
	if (gun_isUnlocked) return;

	/* Check collision with Note in Room 2 */
	if (tileX == 1 && tileY == 0 && !gun_foundPassword)
	{
		if (playerX < ITEM_X + 50 && playerX + PLAYER_W > ITEM_X &&
			playerY < ITEM_Y + 40 && playerY + PLAYER_H > ITEM_Y)
		{
			gun_foundPassword = true;
		}
	}

	/* Check collision with Crate in Room 5 */
	if (tileX == 1 && tileY == 1)
	{
		if (playerX < ITEM_X + ITEM_W && playerX + PLAYER_W > ITEM_X &&
			playerY < ITEM_Y + ITEM_H && playerY + PLAYER_H > ITEM_Y)
		{
			/* Only allow opening if they found the password first */
			if (gun_foundPassword) {
				gameState = STATE_GUN_PUZZLE;
				gun_inputLen = 0;
				gun_input[0] = '\0';
			}
		}
	}
}

/* ================================================================
gun_drawPuzzleScreen()
The UI for unlocking the gun
================================================================ */
void gun_drawPuzzleScreen()
{
	iSetColor(15, 15, 20);
	iFilledRectangle(0, 0, 1700, 900);

	iSetColor(255, 50, 50);
	iFilledRectangle(0, 830, 1700, 70);
	iSetColor(255, 255, 255);
	iText(650, 850, "ARMORY TERMINAL", GLUT_BITMAP_TIMES_ROMAN_24);

	int panelX = 500, panelY = 350, panelW = 700, panelH = 300;
	iSetColor(30, 30, 40);
	iFilledRectangle(panelX, panelY, panelW, panelH);
	iSetColor(255, 50, 50);
	iRectangle(panelX, panelY, panelW, panelH);

	iSetColor(200, 200, 200);
	iText(panelX + 150, panelY + 220, "ENTER OVERRIDE CODE TO UNLOCK M249", GLUT_BITMAP_HELVETICA_18);

	/* Input Box */
	iSetColor(10, 10, 15);
	iFilledRectangle(650, 420, 400, 60);
	iSetColor(255, 255, 255);
	iRectangle(650, 420, 400, 60);

	char buf[64];
	sprintf_s(buf, sizeof(buf), "%s_", gun_input);
	iSetColor(255, 200, 0);
	iText(670, 440, buf, GLUT_BITMAP_TIMES_ROMAN_24);

	if (gun_feedbackTimer > 0)
	{
		if (gun_wrongPass) {
			iSetColor(255, 0, 0);
			iText(750, 370, "ACCESS DENIED", GLUT_BITMAP_TIMES_ROMAN_24);
		}
		else {
			iSetColor(0, 255, 100);
			iText(700, 370, "WEAPON UPGRADED!", GLUT_BITMAP_TIMES_ROMAN_24);
		}
	}
}

/* ================================================================
gun_updateInput()
Handles typing the password
================================================================ */
void gun_updateInput()
{
	static bool alphaPressed[26];
	static bool apInited = false;
	if (!apInited) { for (int k = 0; k < 26; k++) alphaPressed[k] = false; apInited = true; }

	static bool backPressed = false;
	static bool enterPressed = false;
	static bool escPressed = false;

	if (gun_feedbackTimer > 0) {
		gun_feedbackTimer--;
		if (gun_feedbackTimer == 0 && !gun_wrongPass) {
			/* Apply Upgrades and return to game */
			gun_isUnlocked = true;
			cooldownMax = 3; /* Double fire rate! */
			gameState = inLevel3 ? 20 /*STATE_PLAY3*/ : (inLevel2 ? 10 /*STATE_PLAY2*/ : 3 /*STATE_PLAY*/);
		}
		return;
	}

	/* Letters */
	for (int i = 0; i < 26; i++) {
		char cu = 'A' + i;
		char cl = 'a' + i;
		if (isKeyPressed(cu) || isKeyPressed(cl)) {
			if (!alphaPressed[i] && gun_inputLen < 15) {
				gun_input[gun_inputLen++] = cu;
				gun_input[gun_inputLen] = '\0';
				alphaPressed[i] = true;
			}
		}
		else alphaPressed[i] = false;
	}

	/* Backspace */
	if (isKeyPressed(8)) {
		if (!backPressed && gun_inputLen > 0) {
			gun_input[--gun_inputLen] = '\0'; backPressed = true;
		}
	}
	else backPressed = false;

	/* Enter */
	if (isKeyPressed(13)) {
		if (!enterPressed && gun_inputLen > 0) {
			if (strcmp(gun_input, gun_password) == 0) {
				gun_wrongPass = false;
				gun_feedbackTimer = 90; /* Pause to show success */
			}
			else {
				gun_wrongPass = true;
				gun_feedbackTimer = 60; /* Pause to show error */
				gun_inputLen = 0;
				gun_input[0] = '\0';
			}
			enterPressed = true;
		}
	}
	else enterPressed = false;

	/* Escape to exit terminal */
	if (isKeyPressed(27)) {
		if (!escPressed) {
			gameState = inLevel3 ? 20 : (inLevel2 ? 10 : 3);
			escPressed = true;
			playerY -= 20; // Move player slightly away so they don't immediately re-trigger
		}
	}
	else escPressed = false;
}

#endif