// ============================================================
//  Highscore.h  -  MoneyHeist High Score System
//  Screen size: 1700 x 900
// ============================================================

#pragma once
#include <stdio.h>
#include <string.h>

// ── Configuration ────────────────────────────────────────────
#define MAX_HIGH_SCORES   5
#define SCORE_FILE        "highscores.txt"
#define HS_NAME_LEN       50

// ── Data ─────────────────────────────────────────────────────
struct HighScoreEntry {
	char name[HS_NAME_LEN];
	int  score;
};

HighScoreEntry highScores[MAX_HIGH_SCORES];
int            highScoreCount = 0;

// ── Forward declarations ──────────────────────────────────────
void hs_load();
void hs_save();
void hs_tryInsert(const char* name, int score);
bool hs_isNewHighScore(int score);
void hs_draw();

// =============================================================
//  hs_load  -  read scores from disk (call once at startup)
// =============================================================
void hs_load() {
	highScoreCount = 0;
	memset(highScores, 0, sizeof(highScores));

	FILE* fp = NULL;
	fopen_s(&fp, SCORE_FILE, "r");   // safe version - fixes C4996
	if (!fp) return;                  // file doesn't exist yet - that's fine

	while (highScoreCount < MAX_HIGH_SCORES) {
		HighScoreEntry e;
		if (fscanf_s(fp, "%d ", &e.score) != 1) break;
		if (fgets(e.name, HS_NAME_LEN, fp) == NULL) break;

		// strip trailing newline left by fgets
		int len = (int)strlen(e.name);
		if (len > 0 && e.name[len - 1] == '\n')
			e.name[len - 1] = '\0';

		highScores[highScoreCount++] = e;
	}
	fclose(fp);
}

// =============================================================
//  hs_save  -  write scores to disk
// =============================================================
void hs_save() {
	FILE* fp = NULL;
	fopen_s(&fp, SCORE_FILE, "w");   // safe version - fixes C4996
	if (!fp) return;

	for (int i = 0; i < highScoreCount; i++)
		fprintf_s(fp, "%d %s\n", highScores[i].score, highScores[i].name);

	fclose(fp);
}

// =============================================================
//  hs_isNewHighScore
// =============================================================
bool hs_isNewHighScore(int score) {
	if (highScoreCount < MAX_HIGH_SCORES) return true;
	return score > highScores[highScoreCount - 1].score;
}

// =============================================================
//  hs_tryInsert  -  insert, sort descending, save
// =============================================================
void hs_tryInsert(const char* name, int score) {
	if (!hs_isNewHighScore(score)) return;

	int idx = highScoreCount < MAX_HIGH_SCORES
		? highScoreCount++
		: MAX_HIGH_SCORES - 1;

	strncpy_s(highScores[idx].name, HS_NAME_LEN, name, _TRUNCATE);  // safe version
	highScores[idx].score = score;

	// insertion sort descending
	for (int i = idx; i > 0; i--) {
		if (highScores[i].score > highScores[i - 1].score) {
			HighScoreEntry tmp = highScores[i];
			highScores[i] = highScores[i - 1];
			highScores[i - 1] = tmp;
		}
		else break;
	}

	hs_save();
}

// =============================================================
//  hs_draw  -  render scoreboard  (1700 x 900 window)
// =============================================================
void hs_draw() {

	// ── Dark panel centred on 1700x900 ──────────────────────
	iSetColor(0, 0, 0);
	iFilledRectangle(400, 150, 900, 600);

	// Red border
	iSetColor(180, 0, 0);
	iRectangle(400, 150, 900, 600);

	// Inner gold border
	iSetColor(180, 140, 0);
	iRectangle(410, 160, 880, 580);

	// ── Title ────────────────────────────────────────────────
	iSetColor(220, 30, 30);
	iText(695, 710, "HIGH SCORES", GLUT_BITMAP_TIMES_ROMAN_24);

	// ── Column headers ───────────────────────────────────────
	iSetColor(200, 160, 0);
	iText(460, 668, "RANK", GLUT_BITMAP_HELVETICA_18);
	iText(600, 668, "NAME", GLUT_BITMAP_HELVETICA_18);
	iText(1100, 668, "SCORE", GLUT_BITMAP_HELVETICA_18);

	// divider line under headers
	iSetColor(180, 0, 0);
	iLine(420, 658, 1280, 658);

	// ── Rows ─────────────────────────────────────────────────
	if (highScoreCount == 0) {
		iSetColor(200, 200, 200);
		iText(680, 430, "No scores yet!", GLUT_BITMAP_HELVETICA_18);
	}

	for (int i = 0; i < highScoreCount; i++) {
		int rowY = 580 - i * 90;

		if (i == 0) iSetColor(255, 215, 0);   // gold
		else if (i == 1) iSetColor(192, 192, 192);   // silver
		else if (i == 2) iSetColor(205, 127, 50);   // bronze
		else             iSetColor(230, 230, 230);   // white

		char rankStr[8];
		sprintf_s(rankStr, "#%d", i + 1);
		iText(460, rowY, rankStr, GLUT_BITMAP_HELVETICA_18);

		iText(600, rowY, highScores[i].name, GLUT_BITMAP_HELVETICA_18);

		char scoreStr[32];
		sprintf_s(scoreStr, "%d", highScores[i].score);
		iText(1100, rowY, scoreStr, GLUT_BITMAP_HELVETICA_18);

		// row separator line
		iSetColor(80, 0, 0);
		iLine(420, rowY - 15, 1280, rowY - 15);
	}

	// ── ESC hint ─────────────────────────────────────────────
	iSetColor(180, 180, 180);
	iText(715, 170, "Press ESC to return", GLUT_BITMAP_HELVETICA_12);
}