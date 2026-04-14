#ifndef AMMO_H
#define AMMO_H

/*
================================================================
Ammo.h  --  Visual Studio 2013
Limited ammo system for Money Heist game.
- Player starts with 30 bullets
- Cannot shoot when ammo = 0
- Ammo pickups spawn like fruit system
- Pickup gives 15 bullets

RULES:
1. Do NOT #include "iGraphics.h" here
2. #include "Ammo.h" must come AFTER Level2.h and Laser.h
3. Requires playerScore, playerX, playerY, tileX, tileY,
PLAYER_W, PLAYER_H, SCREEN_WIDTH, SCREEN_HEIGHT
all defined in main.cpp before this include
================================================================
*/

/* ================================================================
AMMO TUNING
================================================================ */
#define AMMO_START          50      /* bullets at game start         */
#define AMMO_PER_PICKUP     50      /* bullets gained per pickup     */
#define AMMO_LOW_THRESHOLD  8       /* below this = flash warning    */
#define AMMO_PICKUP_W       120      /* pickup sprite width           */
#define AMMO_PICKUP_H       120      /* pickup sprite height          */
#define AMMO_RESPAWN_MIN    800     /* min frames before new pickup  */
#define AMMO_RESPAWN_MAX    1400    /* max frames before new pickup  */

/* ================================================================
AMMO GLOBALS
================================================================ */
int  playerAmmo = AMMO_START;
bool ammoPickupActive = false;
int  ammoPickupTileX = 0;
int  ammoPickupTileY = 0;
int  ammoPickupX = 0;
int  ammoPickupY = 0;
int  ammoPickupTimer = 0;     /* counts up to respawn threshold   */
int  ammoNextSpawn = AMMO_RESPAWN_MIN;
int  ammoFlashTimer = 0;     /* flashes HUD when ammo is low     */
int  ammoPickupImg = 0;     /* loaded image handle              */

/* ================================================================
ammo_init()
Call once when game starts or resets.
================================================================ */
void ammo_init(void)
{
	playerAmmo = AMMO_START;
	ammoPickupActive = false;
	ammoPickupTimer = 0;
	ammoNextSpawn = AMMO_RESPAWN_MIN +
		rand() % (AMMO_RESPAWN_MAX - AMMO_RESPAWN_MIN);
	ammoFlashTimer = 0;
}

/* ================================================================
ammo_loadImage()
Call from allImg() in main.cpp.
Uses a simple drawn box if image not found.
================================================================ */
void ammo_loadImage(void)
{
	ammoPickupImg = iLoadImage("Images\\ammo.png");
}

/* ================================================================
ammo_spawnPickup()
Spawns an ammo box on a random tile.
================================================================ */
void ammo_spawnPickup(void)
{
	int oldTX = ammoPickupTileX;
	int oldTY = ammoPickupTileY;

	ammoPickupActive = true;
	do
	{
		ammoPickupTileX = rand() % 3;
		ammoPickupTileY = rand() % 3;
	} while (ammoPickupTileX == oldTX && ammoPickupTileY == oldTY);

	ammoPickupX = 120 + rand() % (SCREEN_WIDTH - 240);
	ammoPickupY = 120 + rand() % (SCREEN_HEIGHT - 240);
	ammoPickupTimer = 0;
}

/* ================================================================
ammo_canShoot()
Returns true if player has bullets remaining.
Use this to gate fireBullet() calls in main.cpp.
================================================================ */
bool ammo_canShoot(void)
{
	return (playerAmmo > 0);
}

/* ================================================================
ammo_consume()
Call once per bullet fired.
================================================================ */
void ammo_consume(void)
{
	if (playerAmmo > 0) playerAmmo--;
}

/* ================================================================
ammo_drawPickup()
Draw the ammo pickup box on screen.
Call in iDraw() during gameplay.
================================================================ */
void ammo_drawPickup(void)
{
	char buf[16];

	if (!ammoPickupActive) return;
	if (tileX != ammoPickupTileX || tileY != ammoPickupTileY) return;

	/* try image first, fallback to coloured rectangle */
	if (ammoPickupImg > 0)
	{
		iShowImage(ammoPickupX, ammoPickupY,
			AMMO_PICKUP_W, AMMO_PICKUP_H, ammoPickupImg);
	}
	else
	{
		/* bright green ammo crate */
		iSetColor(0, 160, 0);
		iFilledRectangle(ammoPickupX, ammoPickupY,
			AMMO_PICKUP_W, AMMO_PICKUP_H);
		iSetColor(0, 255, 0);
		iRectangle(ammoPickupX, ammoPickupY,
			AMMO_PICKUP_W, AMMO_PICKUP_H);
		/* bullet icon lines */
		iSetColor(255, 255, 100);
		iLine(ammoPickupX + 12, ammoPickupY + 8,
			ammoPickupX + 12, ammoPickupY + 28);
		iLine(ammoPickupX + 20, ammoPickupY + 8,
			ammoPickupX + 20, ammoPickupY + 28);
		iLine(ammoPickupX + 28, ammoPickupY + 8,
			ammoPickupX + 28, ammoPickupY + 28);
	}

	/* label above */
	iSetColor(200, 255, 200);
	sprintf_s(buf, sizeof(buf), "+%d AMMO", AMMO_PER_PICKUP);
	iText(ammoPickupX - 4, ammoPickupY + AMMO_PICKUP_H + 6,
		buf, GLUT_BITMAP_HELVETICA_12);
}

/* ================================================================
ammo_drawHUD()
Draws ammo counter on screen.
Call in iDraw() during gameplay (after drawLevel2HUD or score).
================================================================ */
void ammo_drawHUD(void)
{
	char buf[32];
	int  i;
	bool flash = (ammoFlashTimer / 8) % 2 == 0;

	/* background */
	if (playerAmmo <= AMMO_LOW_THRESHOLD && flash)
		iSetColor(180, 0, 0);
	else
		iSetColor(30, 30, 30);
	iFilledRectangle(20, 790, 200, 28);

	/* border */
	if (playerAmmo <= AMMO_LOW_THRESHOLD && flash)
		iSetColor(255, 80, 80);
	else
		iSetColor(0, 200, 255);
	iRectangle(20, 790, 200, 28);

	/* text */
	if (playerAmmo == 0)
	{
		iSetColor(255, 50, 50);
		iText(28, 798, "NO AMMO! Find pickup!", GLUT_BITMAP_HELVETICA_12);
	}
	else
	{
		iSetColor(255, 255, 255);
		sprintf_s(buf, sizeof(buf), "AMMO: %d", playerAmmo);
		iText(28, 798, buf, GLUT_BITMAP_HELVETICA_18);
	}

	/* small yellow bullet icons */
	int show = playerAmmo > 10 ? 10 : playerAmmo;
	for (i = 0; i < show; i++)
	{
		iSetColor(255, 220, 0);
		iFilledRectangle(230 + i * 12, 795, 6, 18);
	}
}

/* ================================================================
ammo_update()
Call every frame from fixedUpdate() during STATE_PLAY / PLAY2.
Handles pickup collision and respawning.
================================================================ */
void ammo_update(void)
{
	/* flash timer for low ammo warning */
	if (playerAmmo <= AMMO_LOW_THRESHOLD)
		ammoFlashTimer++;
	else
		ammoFlashTimer = 0;

	/* pickup collision */
	if (ammoPickupActive &&
		tileX == ammoPickupTileX && tileY == ammoPickupTileY)
	{
		if (playerX              < ammoPickupX + AMMO_PICKUP_W &&
			playerX + PLAYER_W   > ammoPickupX                  &&
			playerY              < ammoPickupY + AMMO_PICKUP_H  &&
			playerY + PLAYER_H   > ammoPickupY)
		{
			playerAmmo += AMMO_PER_PICKUP;
			if (playerAmmo > 99) playerAmmo = 99;   /* cap at 99 */
			ammoPickupActive = false;
			ammoPickupTimer = 0;
			ammoNextSpawn = AMMO_RESPAWN_MIN +
				rand() % (AMMO_RESPAWN_MAX - AMMO_RESPAWN_MIN);
		}
	}

	/* respawn timer */
	if (!ammoPickupActive)
	{
		ammoPickupTimer++;
		if (ammoPickupTimer >= ammoNextSpawn)
			ammo_spawnPickup();
	}
}

#endif /* AMMO_H */



