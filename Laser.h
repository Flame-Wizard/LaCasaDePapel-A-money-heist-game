#ifndef LASER_H
#define LASER_H

/*
================================================================
Laser.h  --  Visual Studio 2013
Laser beam system for Money Heist game.
Appears on every tile/room, 5-6 lasers per screen.
Player takes 30 HP damage on contact.

RULES:
1. Do NOT #include "iGraphics.h" here  (already in main.cpp)
2. #include "Laser.h" must come AFTER struct battle and Level2.h
in main.cpp
3. Add two lines in main.cpp (marked with LASER_INTEGRATION below)
================================================================
*/

/* ================================================================
LASER TUNING
================================================================ */
#define LASER_MAX          8        /* max lasers on screen at once  */
#define LASER_MIN_COUNT    2        /* min visible at one time       */
#define LASER_MAX_COUNT    3        /* max visible at one time       */
#define LASER_DAMAGE       5       /* HP lost on touch              */
#define LASER_MIN_LIFE     180      /* frames a laser stays (3 sec)  */
#define LASER_MAX_LIFE     420      /* frames a laser stays (7 sec)  */
#define LASER_SPAWN_RATE   50       /* frames between spawn checks   */
#define LASER_THICKNESS    4        /* visual thickness in pixels    */
#define LASER_WARN_FRAMES  30       /* orange warning flash before live */
#define LASER_DAMAGE_COOLDOWN 60    /* frames of invincibility after hit */

/* ================================================================
LASER ORIENTATION
================================================================ */
#define LASER_HORIZONTAL  0
#define LASER_VERTICAL    1
#define LASER_DIAGONAL_R  2        /* top-left  to bottom-right     */
#define LASER_DIAGONAL_L  3        /* top-right to bottom-left      */

/* ================================================================
LASER STRUCT
================================================================ */
struct Laser
{
	bool  active;
	int   type;          /* LASER_HORIZONTAL / VERTICAL / DIAGONAL  */
	int   x1, y1;        /* start point                             */
	int   x2, y2;        /* end point                               */
	int   lifeTimer;     /* counts down to 0 then deactivates       */
	int   maxLife;
	bool  isWarning;     /* true during first LASER_WARN_FRAMES     */
	int   warnTimer;
	float r, g, b;       /* beam colour                             */
};

/* ================================================================
LASER GLOBALS
================================================================ */
Laser  lasers[LASER_MAX];
int    laserSpawnTimer = 0;
int    laserDamageCooldown = 0;   /* player invincibility after hit  */

/* ================================================================
laser_init()
Call once at game start or level start.
================================================================ */
void laser_init(void)
{
	int i;
	for (i = 0; i < LASER_MAX; i++)
	{
		lasers[i].active = false;
		lasers[i].lifeTimer = 0;
		lasers[i].isWarning = false;
		lasers[i].warnTimer = 0;
	}
	laserSpawnTimer = 0;
	laserDamageCooldown = 0;
}

/* ================================================================
laser_randomColor()
Gives lasers a red/pink/white heist aesthetic.
================================================================ */
void laser_randomColor(Laser *lz)
{
	int roll = rand() % 4;
	if (roll == 0) { lz->r = 255; lz->g = 0;   lz->b = 0; } /* red      */
	else if (roll == 1) { lz->r = 255; lz->g = 0;   lz->b = 180; } /* pink     */
	else if (roll == 2) { lz->r = 255; lz->g = 80;  lz->b = 0; } /* orange   */
	else                { lz->r = 200; lz->g = 0;   lz->b = 255; } /* violet   */
}

/* ================================================================
laser_spawn()
Spawns one laser at a free slot.
================================================================ */
void laser_spawn(void)
{
	int  i, slot = -1;
	int  type;
	int  margin = 60;

	/* find free slot */
	for (i = 0; i < LASER_MAX; i++)
	{
		if (!lasers[i].active) { slot = i; break; }
	}
	if (slot == -1) return;   /* all slots used */

	type = rand() % 4;
	lasers[slot].type = type;
	lasers[slot].active = true;
	lasers[slot].isWarning = true;
	lasers[slot].warnTimer = LASER_WARN_FRAMES;
	lasers[slot].maxLife = LASER_MIN_LIFE + rand() % (LASER_MAX_LIFE - LASER_MIN_LIFE);
	lasers[slot].lifeTimer = lasers[slot].maxLife;
	laser_randomColor(&lasers[slot]);

	switch (type)
	{
	case LASER_HORIZONTAL:
		lasers[slot].y1 = margin + rand() % (900 - 2 * margin);
		lasers[slot].x1 = 0;
		lasers[slot].x2 = 1700;
		lasers[slot].y2 = lasers[slot].y1;
		break;

	case LASER_VERTICAL:
		lasers[slot].x1 = margin + rand() % (1700 - 2 * margin);
		lasers[slot].y1 = 0;
		lasers[slot].x2 = lasers[slot].x1;
		lasers[slot].y2 = 900;
		break;

	case LASER_DIAGONAL_R:
		/* from left edge to right edge, randomise vertical offset */
		lasers[slot].x1 = 0;
		lasers[slot].y1 = rand() % 900;
		lasers[slot].x2 = 1700;
		lasers[slot].y2 = rand() % 900;
		break;

	case LASER_DIAGONAL_L:
		lasers[slot].x1 = 1700;
		lasers[slot].y1 = rand() % 900;
		lasers[slot].x2 = 0;
		lasers[slot].y2 = rand() % 900;
		break;
	}
}

/* ================================================================
laser_countActive()
Returns number of currently active (non-warning) lasers.
================================================================ */
int laser_countActive(void)
{
	int i, cnt = 0;
	for (i = 0; i < LASER_MAX; i++)
	if (lasers[i].active) cnt++;
	return cnt;
}

/* ================================================================
laser_pointToSegmentDistSq()
Squared distance from point (px,py) to segment (x1,y1)-(x2,y2).
Used for collision instead of pixel-perfect check.
================================================================ */
float laser_pointToSegmentDistSq(float px, float py,
	float x1, float y1, float x2, float y2)
{
	float dx = x2 - x1, dy = y2 - y1;
	float lenSq = dx*dx + dy*dy;
	float t;
	float nearX, nearY, ex, ey;
	if (lenSq < 0.0001f) { nearX = x1; nearY = y1; }
	else
	{
		t = ((px - x1)*dx + (py - y1)*dy) / lenSq;
		if (t < 0.0f) t = 0.0f;
		if (t > 1.0f) t = 1.0f;
		nearX = x1 + t*dx;
		nearY = y1 + t*dy;
	}
	ex = px - nearX; ey = py - nearY;
	return ex*ex + ey*ey;
}

/* ================================================================
laser_checkPlayerCollision()
Returns true if player rectangle intersects laser beam.
Uses center-point distance to segment with a hit radius.
================================================================ */
bool laser_checkPlayerCollision(Laser *lz)
{
	float cx, cy, distSq;
	float hitRadius = (float)(LASER_THICKNESS + 18); /* generous hit box */

	if (!lz->active || lz->isWarning) return false;

	cx = (float)(playerX + PLAYER_W / 2);
	cy = (float)(playerY + PLAYER_H / 2);

	distSq = laser_pointToSegmentDistSq(
		cx, cy,
		(float)lz->x1, (float)lz->y1,
		(float)lz->x2, (float)lz->y2);

	return (distSq <= hitRadius * hitRadius);
}

/* ================================================================
laser_drawSingle()
Draws one laser beam with a glow effect (3 layered lines).
================================================================ */
void laser_drawSingle(Laser *lz)
{
	int k, offset;
	float alpha;

	if (!lz->active) return;

	if (lz->isWarning)
	{
		/* orange flicker warning */
		if ((lz->warnTimer / 5) % 2 == 0)
		{
			iSetColor(255, 140, 0);
			for (k = -1; k <= 1; k++)
			{
				iLine(lz->x1, lz->y1 + k, lz->x2, lz->y2 + k);
				iLine(lz->x1 + k, lz->y1, lz->x2 + k, lz->y2);
			}
		}
		return;
	}

	/* outer glow (darker, wider) */
	iSetColor((int)(lz->r * 0.4f), (int)(lz->g * 0.4f), (int)(lz->b * 0.4f));
	for (offset = -LASER_THICKNESS - 2; offset <= LASER_THICKNESS + 2; offset++)
	{
		iLine(lz->x1, lz->y1 + offset, lz->x2, lz->y2 + offset);
		iLine(lz->x1 + offset, lz->y1, lz->x2 + offset, lz->y2);
	}

	/* mid glow */
	iSetColor((int)(lz->r * 0.7f), (int)(lz->g * 0.7f), (int)(lz->b * 0.7f));
	for (offset = -LASER_THICKNESS; offset <= LASER_THICKNESS; offset++)
	{
		iLine(lz->x1, lz->y1 + offset, lz->x2, lz->y2 + offset);
		iLine(lz->x1 + offset, lz->y1, lz->x2 + offset, lz->y2);
	}

	/* bright core */
	iSetColor((int)lz->r, (int)lz->g, (int)lz->b);
	for (offset = -1; offset <= 1; offset++)
	{
		iLine(lz->x1, lz->y1 + offset, lz->x2, lz->y2 + offset);
		iLine(lz->x1 + offset, lz->y1, lz->x2 + offset, lz->y2);
	}

	/* white hot centre */
	iSetColor(255, 255, 255);
	iLine(lz->x1, lz->y1, lz->x2, lz->y2);
}

/* ================================================================
laser_drawAll()
Draw all active lasers.  Call in iDraw() during STATE_PLAY / PLAY2.
================================================================ */
void laser_drawAll(void)
{
	int i;
	for (i = 0; i < LASER_MAX; i++)
	if (lasers[i].active)
		laser_drawSingle(&lasers[i]);
}

/* ================================================================
laser_update()
Call every frame from fixedUpdate() during STATE_PLAY / PLAY2.
Handles spawning, timers, and player damage.
================================================================ */
void laser_update(void)
{
	int  i, active;
	bool hit;

	/* damage cooldown tick */
	if (laserDamageCooldown > 0) laserDamageCooldown--;

	/* ---- update existing lasers ---- */
	for (i = 0; i < LASER_MAX; i++)
	{
		if (!lasers[i].active) continue;

		/* warning phase */
		if (lasers[i].isWarning)
		{
			lasers[i].warnTimer--;
			if (lasers[i].warnTimer <= 0)
				lasers[i].isWarning = false;
			continue;
		}

		/* life countdown */
		lasers[i].lifeTimer--;
		if (lasers[i].lifeTimer <= 0)
		{
			lasers[i].active = false;
			continue;
		}

		/* player collision */
		if (laserDamageCooldown == 0 && laser_checkPlayerCollision(&lasers[i]))
		{
			playerHP -= LASER_DAMAGE;
			laserDamageCooldown = LASER_DAMAGE_COOLDOWN;
			if (playerHP <= 0)
			{
				playerHP = 0;
				gameState = STATE_OVER;
			}
		}
	}

	/* ---- spawn new lasers to keep 5-6 on screen ---- */
	laserSpawnTimer++;
	if (laserSpawnTimer >= LASER_SPAWN_RATE)
	{
		laserSpawnTimer = 0;
		active = laser_countActive();
		if (active < LASER_MIN_COUNT)
		{
			/* burst-spawn to reach minimum */
			int toSpawn = LASER_MIN_COUNT - active;
			int s;
			for (s = 0; s < toSpawn; s++) laser_spawn();
		}
		else if (active < LASER_MAX_COUNT)
		{
			/* chance to add one more */
			if (rand() % 2 == 0) laser_spawn();
		}
	}
}

/* ================================================================
laser_drawHUD()
Small on-screen indicator showing laser danger level.
Optional - call in iDraw() near your HUD.
================================================================ */
void laser_drawHUD(void)
{
	char buf[32];
	int  active = laser_countActive();

	iSetColor(180, 0, 0);
	iFilledRectangle(1560, 20, 120, 22);
	iSetColor(255, 60, 60);
	iRectangle(1560, 20, 120, 22);
	iSetColor(255, 255, 255);
	sprintf_s(buf, sizeof(buf), "LASERS: %d", active);
	iText(1566, 27, buf, GLUT_BITMAP_HELVETICA_12);
}

#endif /* LASER_H */