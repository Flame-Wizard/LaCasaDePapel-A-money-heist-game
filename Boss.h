#ifndef BOSS_H
#define BOSS_H

/*
================================================================
Boss.h  --  Visual Studio 2013
Boss enemy system for Money Heist game.
- Appears every 5 normal enemy kills
- 20 HP, faster movement
- Fires 3 bullets in a spread pattern
- Large red enemy with health bar above

RULES:
1. Do NOT #include "iGraphics.h" here
2. #include "Boss.h" must come AFTER Ammo.h
3. Requires playerX, playerY, playerScore, playerHP,
ebullets[], bulletSpeed, SCREEN_WIDTH, SCREEN_HEIGHT,
gameState, STATE_OVER all defined in main.cpp before include
================================================================
*/

/* ================================================================
BOSS TUNING
================================================================ */
#define BOSS_MAX_HP         10      /* boss health points            */
#define BOSS_SPEED          1.5f    /* movement speed (faster than normal 2.5f) */
#define BOSS_SHOOT_RATE     50      /* frames between burst shots    */
#define BOSS_SPREAD_COUNT   3       /* bullets per burst             */
#define BOSS_SPREAD_ANGLE   0.15f   /* radians between spread bullets */
#define BOSS_SCORE_REWARD   50      /* score for killing boss        */
#define BOSS_SPAWN_KILLS    5       /* kills before boss appears     */
#define BOSS_DAMAGE_PLAYER  10      /* HP lost if boss touches player*/
#define BOSS_BULLET_SPEED   8       /* boss bullet travel speed      */
#define BOSS_W              80      /* visual width                  */
#define BOSS_HEIGHT             120     /* visual height                 */
#define BOSS_CONTACT_COOLDOWN 60    /* frames between contact damage */

/* ================================================================
BOSS STRUCT
================================================================ */
struct Boss
{
	bool  active;
	float x, y;
	int   hp;
	int   shootTimer;
	bool  facingRight;
	int   flashTimer;       /* red flash when hit                   */
	int   contactCooldown;  /* cooldown for contact damage          */
	int   killsAtSpawn;     /* playerScore snapshot when spawned    */
};

/* ================================================================
BOSS GLOBALS
================================================================ */
Boss boss;
int  bossImg = 0;
int  bossKillCounter = 0;   /* tracks kills since last boss spawn  */
int  totalKills = 0;   /* total enemies killed this session   */

/* last known kill count to detect new kills */
static int boss_lastScore = 0;

/* ================================================================
boss_init()
Call once at game start or level transition.
================================================================ */
void boss_init(void)
{
	boss.active = false;
	boss.x = -1000;
	boss.y = -1000;
	boss.hp = BOSS_MAX_HP;
	boss.shootTimer = BOSS_SHOOT_RATE;
	boss.facingRight = true;
	boss.flashTimer = 0;
	boss.contactCooldown = 0;
	bossKillCounter = 0;
	totalKills = 0;
	boss_lastScore = 0;
}

/* ================================================================
boss_spawn()
Spawns boss from a random screen edge.
================================================================ */
void boss_spawn(void)
{
	int side = rand() % 4;

	boss.active = true;
	boss.hp = BOSS_MAX_HP;
	boss.shootTimer = BOSS_SHOOT_RATE;
	boss.flashTimer = 0;
	boss.contactCooldown = 0;
	boss.facingRight = true;

	if (side == 0) { boss.x = -BOSS_W - 10;          boss.y = (float)(rand() % SCREEN_HEIGHT); }
	else if (side == 1) { boss.x = (float)(SCREEN_WIDTH + 10); boss.y = (float)(rand() % SCREEN_HEIGHT); }
	else if (side == 2) { boss.x = (float)(rand() % SCREEN_WIDTH); boss.y = (float)(SCREEN_HEIGHT + 10); }
	else{ boss.x = (float)(rand() % SCREEN_WIDTH); boss.y = -BOSS_HEIGHT - 10; }
}

/* ================================================================
boss_fireBullets()
Fires 3 spread bullets toward the player.
================================================================ */
void boss_fireBullets(void)
{
	int   i, slot;
	float baseAngle, angle, len, dx, dy;

	dx = (float)(playerX + PLAYER_W / 2) - (boss.x + BOSS_W / 2);
	dy = (float)(playerY + PLAYER_H / 2) - (boss.y + BOSS_HEIGHT / 2);
	len = (float)sqrt((double)(dx * dx + dy * dy));
	if (len < 0.001f) return;

	baseAngle = (float)atan2((double)dy, (double)dx);

	for (i = 0; i < BOSS_SPREAD_COUNT; i++)
	{
		/* find free bullet slot */
		for (slot = 0; slot < MAX_BULLETS; slot++)
		if (!ebullets[slot].active) break;
		if (slot >= MAX_BULLETS) break;

		angle = baseAngle + (float)(i - 1) * BOSS_SPREAD_ANGLE;

		ebullets[slot].x = boss.x + BOSS_W / 2;
		ebullets[slot].y = boss.y + BOSS_HEIGHT / 2;
		ebullets[slot].dx = (float)cos((double)angle);
		ebullets[slot].dy = (float)sin((double)angle);
		ebullets[slot].active = true;
	}
}

/* ================================================================
boss_checkBulletHit()
Check if any player bullet hits the boss.
Call from checkBulletEnemyCollisions area or boss_update.
================================================================ */
void boss_checkBulletHit(void)
{
	int bi;
	if (!boss.active) return;

	for (bi = 0; bi < MAX_BULLETS; bi++)
	{
		if (!bullets[bi].active) continue;

		if (bullets[bi].x + 20 > boss.x        &&
			bullets[bi].x      < boss.x + BOSS_W &&
			bullets[bi].y + 20 > boss.y        &&
			bullets[bi].y      < boss.y + BOSS_HEIGHT)
		{
			bullets[bi].active = false;
			boss.hp--;
			boss.flashTimer = 12;   /* red flash */

			if (boss.hp <= 0)
			{
				boss.active = false;
				playerScore += BOSS_SCORE_REWARD;
				bossKillCounter = 0;   /* reset kill counter after boss dies */
			}
			break;
		}
	}
}



void boss_loadImage(void)
{
    bossImg = iLoadImage("Images\\boss.png");  // ← change boss.png to your file name
}

/* ================================================================
boss_draw()
Draw the boss with health bar and flash effect.
Call in iDraw() during gameplay.
================================================================ */
void boss_draw(void)
{
	int  barX, barY, barW;
	char buf[16];

	if (!boss.active) return;

	/* ---- BOSS SPRITE using your own image ---- */
	/* ---- BOSS SPRITE ---- */
	if (bossImg > 0)
	{
		if (boss.facingRight)
			iShowImage((int)boss.x, (int)boss.y, BOSS_W, BOSS_HEIGHT, bossImg);
		else
			iShowImage((int)boss.x + BOSS_W, (int)boss.y, -BOSS_W, BOSS_HEIGHT, bossImg);
	}
	else
	{
		/* fallback if image missing - draws red rectangle */
		if (boss.flashTimer > 0) iSetColor(255, 255, 255);
		else                     iSetColor(180, 0, 0);
		iFilledRectangle((int)boss.x, (int)boss.y, BOSS_W, BOSS_HEIGHT);
		iSetColor(80, 0, 0);
		iRectangle((int)boss.x, (int)boss.y, BOSS_W, BOSS_HEIGHT);
		iSetColor(255, 220, 0);
		iText((int)boss.x + 10, (int)boss.y + BOSS_HEIGHT / 2,
			"BOSS", GLUT_BITMAP_HELVETICA_18);
	}

	/* health bar background */
	barW = 100;
	barX = (int)boss.x - 10;
	barY = (int)boss.y + BOSS_HEIGHT + 10;
	iSetColor(60, 0, 0);
	iFilledRectangle(barX, barY, barW, 12);

	/* health bar fill */
	iSetColor(255, 30, 30);
	iFilledRectangle(barX, barY,
		(barW * boss.hp) / BOSS_MAX_HP, 12);

	/* health bar border */
	iSetColor(255, 100, 100);
	iRectangle(barX, barY, barW, 12);

	/* HP numbers */
	iSetColor(255, 255, 255);
	sprintf_s(buf, sizeof(buf), "%d/%d", boss.hp, BOSS_MAX_HP);
	iText(barX + 28, barY + 1, buf, GLUT_BITMAP_HELVETICA_12);

	/* BOSS warning label */
	iSetColor(255, 0, 0);
	iText((int)boss.x + 5, (int)boss.y + BOSS_HEIGHT + 28,
		"!! BOSS !!", GLUT_BITMAP_HELVETICA_12);
}

/* ================================================================
boss_drawHUD()
Shows kill progress toward next boss spawn in top-left area.
Call in iDraw() during gameplay.
================================================================ */
void boss_drawHUD(void)
{
	char buf[48];
	int  barW = 160;
	int  filled;

	if (boss.active)
	{
		/* pulsing red warning */
		iSetColor(200, 0, 0);
		iFilledRectangle(360, 855, 200, 26);
		iSetColor(255, 80, 80);
		iRectangle(360, 855, 200, 26);
		iSetColor(255, 255, 0);
		iText(368, 863, "!! BOSS ACTIVE !!",
			GLUT_BITMAP_HELVETICA_18);
		return;
	}

	/* kills-to-boss progress */
	iSetColor(30, 30, 30);
	iFilledRectangle(360, 855, barW, 26);
	iSetColor(255, 100, 0);
	filled = (barW * bossKillCounter) / BOSS_SPAWN_KILLS;
	if (filled > barW) filled = barW;
	iFilledRectangle(360, 855, filled, 26);
	iSetColor(255, 150, 0);
	iRectangle(360, 855, barW, 26);
	iSetColor(255, 255, 255);
	sprintf_s(buf, sizeof(buf), "Boss in: %d kills",
		BOSS_SPAWN_KILLS - bossKillCounter);
	iText(366, 863, buf, GLUT_BITMAP_HELVETICA_12);
}

/* ================================================================
boss_update()
Call every frame from fixedUpdate() during STATE_PLAY / PLAY2.
Handles movement, shooting, kill detection, and contact damage.
================================================================ */
void boss_update(void)
{
	float dx, dy, dist;

	/* ---- detect new kills to increment bossKillCounter ----
	We track score increases caused by enemy kills.
	Normal kill = L2_SCORE_PER_KILL(10) or 5.
	We just check if score went up since last frame.        */
	if (playerScore > boss_lastScore && !boss.active)
	{
		bossKillCounter++;
		boss_lastScore = playerScore;
		if (bossKillCounter >= BOSS_SPAWN_KILLS)
		{
			boss_spawn();
			bossKillCounter = 0;
		}
	}
	else
	{
		boss_lastScore = playerScore;
	}

	if (!boss.active) return;

	/* flash timer */
	if (boss.flashTimer > 0) boss.flashTimer--;

	/* contact cooldown */
	if (boss.contactCooldown > 0) boss.contactCooldown--;

	/* movement toward player */
	dx = (float)(playerX + PLAYER_W / 2) - (boss.x + BOSS_W / 2);
	dy = (float)(playerY + PLAYER_H / 2) - (boss.y + BOSS_HEIGHT / 2);
	dist = (float)sqrt((double)(dx * dx + dy * dy));

	boss.facingRight = (dx >= 0);

	if (dist > 0.0f)
	{
		boss.x += (dx / dist) * BOSS_SPEED;
		boss.y += (dy / dist) * BOSS_SPEED;
	}

	/* shooting */
	boss.shootTimer--;
	if (boss.shootTimer <= 0)
	{
		boss_fireBullets();
		boss.shootTimer = BOSS_SHOOT_RATE;
	}

	/* bullet collision */
	boss_checkBulletHit();

	/* contact damage with player */
	if (boss.contactCooldown == 0)
	{
		if ((int)boss.x              < playerX + PLAYER_W &&
			(int)boss.x + BOSS_W     > playerX &&
			(int)boss.y              < playerY + PLAYER_H &&
			(int)boss.y + BOSS_HEIGHT     > playerY)
		{
			playerHP -= BOSS_DAMAGE_PLAYER;
			boss.contactCooldown = BOSS_CONTACT_COOLDOWN;
			if (playerHP <= 0)
			{
				playerHP = 0;
				gameState = STATE_OVER;
			}
		}
	}

	/* keep boss on screen (wraps with player tile movement) */
	if (boss.x < -BOSS_W - 50) boss.x = (float)(-BOSS_W - 50);
	if (boss.x > SCREEN_WIDTH + 50) boss.x = (float)(SCREEN_WIDTH + 50);
	if (boss.y < -BOSS_HEIGHT - 50) boss.y = (float)(-BOSS_HEIGHT - 50);
	if (boss.y > SCREEN_HEIGHT + 50) boss.y = (float)(SCREEN_HEIGHT + 50);
}

#endif /* BOSS_H */