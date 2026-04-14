#ifndef ROOM_COLLISION_H
#define ROOM_COLLISION_H

/* ================================================================
RoomCollision.h
----------------------------------------------------------------
Wall + door collision for Level 2 and Level 3.

Looking at the room image (L3_room7.png) there are 4 doors:
LEFT   -- center-left  wall
RIGHT  -- center-right wall
TOP    -- center-top   wall
BOTTOM -- center-bottom wall

The player can ONLY leave a tile by walking through one of
these 4 door openings (each 160 px wide/tall, centered on
their wall).  Everywhere else the player slides along the wall.

----------------------------------------------------------------
SCREEN DIMENSIONS  (matches iMain.cpp)
SCREEN_WIDTH  = 1700
SCREEN_HEIGHT =  900

PLAYER SIZE  (matches iMain.cpp)
PLAYER_W = 150
PLAYER_H = 200

----------------------------------------------------------------
WALL THICKNESS
RC_WALL_T -- how deep the solid wall is on each edge.
60 px matches the pixel-art border in the room image.
Raise this value if the player still clips into the wall art.

DOOR OPENING  (160 px, centered on each wall edge)
Horizontal walls (top / bottom): gap centered at x = 850
gap spans x in [ 770 , 930 ]
Vertical walls   (left / right): gap centered at y = 450
gap spans y in [ 370 , 530 ]
================================================================ */

/* ---- tuning ---------------------------------------------------- */
#define RC_WALL_T    60          /* wall thickness on every edge   */
#define RC_DOOR_HALF 80          /* half of 160 px door opening    */

/* door centres -- change if your pixel-art doors are off-centre   */
#define RC_DOOR_CX   (SCREEN_WIDTH  / 2)   /* 850  horizontal      */
#define RC_DOOR_CY   (SCREEN_HEIGHT / 2)   /* 450  vertical        */

/* ----------------------------------------------------------------
rc_inHorizDoor(px)
Returns true if the player's horizontal centre is inside the
top-wall or bottom-wall door gap.
Used when the player hits the TOP or BOTTOM wall.
---------------------------------------------------------------- */
static inline bool rc_inHorizDoor(int px)
{
	int cx = px + PLAYER_W / 2;
	return (cx >= RC_DOOR_CX - RC_DOOR_HALF) &&
		(cx <= RC_DOOR_CX + RC_DOOR_HALF);
}

/* ----------------------------------------------------------------
rc_inVertDoor(py)
Returns true if the player's vertical centre is inside the
left-wall or right-wall door gap.
Used when the player hits the LEFT or RIGHT wall.
---------------------------------------------------------------- */
static inline bool rc_inVertDoor(int py)
{
	int cy = py + PLAYER_H / 2;
	return (cy >= RC_DOOR_CY - RC_DOOR_HALF) &&
		(cy <= RC_DOOR_CY + RC_DOOR_HALF);
}

/* ================================================================
applyRoomCollision(playerX, playerY)
----------------------------------------------------------------
Call every frame AFTER moving the player and BEFORE the
tile-switch checks in fixedUpdate().

Blocks the player against all four walls but leaves the door
gaps open so the tile-switch code can still fire when the
player walks through a door.

Slide behaviour:
Only the axis that hit the wall is corrected.  The other
axis is untouched, so the player glides smoothly along
the wall face.
================================================================ */
static inline void applyRoomCollision(int &px, int &py)
{
	/* ---- BOTTOM wall  (y = 0 edge) ----------------------------
	Player bottom edge = py.
	Solid region: py in [0, RC_WALL_T).
	Door gap: player may pass only if centred on x = 850.     */
	if (py < RC_WALL_T)
	{
		if (!rc_inHorizDoor(px))
			py = RC_WALL_T;
		/* else: in door gap -- let tile-switch fire naturally    */
	}

	/* ---- TOP wall  (y = SCREEN_HEIGHT edge) -------------------
	Player top edge = py + PLAYER_H.
	Solid region: py + PLAYER_H in (SCREEN_HEIGHT-RC_WALL_T,
	SCREEN_HEIGHT].          */
	if (py + PLAYER_H > SCREEN_HEIGHT - RC_WALL_T)
	{
		if (!rc_inHorizDoor(px))
			py = SCREEN_HEIGHT - RC_WALL_T - PLAYER_H;
	}

	/* ---- LEFT wall  (x = 0 edge) ------------------------------
	Player left edge = px.
	Solid region: px in [0, RC_WALL_T).
	Door gap: player may pass only if centred on y = 450.     */
	if (px < RC_WALL_T)
	{
		if (!rc_inVertDoor(py))
			px = RC_WALL_T;
	}

	/* ---- RIGHT wall  (x = SCREEN_WIDTH edge) ------------------
	Player right edge = px + PLAYER_W.
	Solid region: px + PLAYER_W in (SCREEN_WIDTH-RC_WALL_T,
	SCREEN_WIDTH].           */
	if (px + PLAYER_W > SCREEN_WIDTH - RC_WALL_T)
	{
		if (!rc_inVertDoor(py))
			px = SCREEN_WIDTH - RC_WALL_T - PLAYER_W;
	}
}

#endif /* ROOM_COLLISION_H */