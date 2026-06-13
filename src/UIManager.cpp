// ═══════════════════════════════════════════════════════════════════════════
//  UIManager.cpp  ─  Member 4: UI, Audio & Menu System
//
//  Algorithms implemented:
//    • DDA Line Drawing         (menu borders, dividers)
//    • Bresenham Line Drawing   (retro box frames, selection arrows)
//    • 2D Translation           (slide-in / slide-out menu transitions)
//    • 2D Scaling               (pulsing GAME OVER text)
//    • High Score File I/O      (highscore.dat)
// ═══════════════════════════════════════════════════════════════════════════

#include "UIManager.h"
#include "Constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Global instance
UIManager uiManager;

// ─────────────────────────────────────────────────────────────────
//  DDA Line Drawing
// ─────────────────────────────────────────────────────────────────
void ddaLine(float x0, float y0, float x1, float y1) {
    float dx = x1 - x0, dy = y1 - y0;
    float steps = (fabsf(dx) > fabsf(dy)) ? fabsf(dx) : fabsf(dy);
    if (steps == 0.0f) { glBegin(GL_POINTS); glVertex2f(x0,y0); glEnd(); return; }
    float xInc = dx / steps, yInc = dy / steps;
    float x = x0, y = y0;
    glBegin(GL_POINTS);
    for (int i = 0; i <= (int)steps; i++) {
        glVertex2f(roundf(x), roundf(y));
        x += xInc; y += yInc;
    }
    glEnd();
}

// ─────────────────────────────────────────────────────────────────
//  Bresenham Line Drawing
// ─────────────────────────────────────────────────────────────────
void bresenhamLine(int x0, int y0, int x1, int y1) {
    int dx = abs(x1-x0), dy = abs(y1-y0);
    int sx = (x0<x1)?1:-1, sy = (y0<y1)?1:-1;
    int err = dx - dy;
    glBegin(GL_POINTS);
    while (true) {
        glVertex2f((float)x0,(float)y0);
        if (x0==x1 && y0==y1) break;
        int e2 = 2*err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 <  dx) { err += dx; y0 += sy; }
    }
    glEnd();
}

// ─────────────────────────────────────────────────────────────────
//  Retro border using Bresenham
// ─────────────────────────────────────────────────────────────────
void drawRetroBorder(float x, float y, float w, float h,
                     float r, float g, float b, float thickness) {
    glColor3f(r, g, b);
    glPointSize(thickness);
    int x0=(int)x, y0=(int)y, x1=(int)(x+w), y1=(int)(y+h);
    bresenhamLine(x0,y0,x1,y0);
    bresenhamLine(x1,y0,x1,y1);
    bresenhamLine(x1,y1,x0,y1);
    bresenhamLine(x0,y1,x0,y0);
    glPointSize(1.0f);
}

// ─────────────────────────────────────────────────────────────────
//  Decorative double box (Bresenham outer + DDA inner + corner squares)
// ─────────────────────────────────────────────────────────────────
void drawDecorativeBox(float x, float y, float w, float h,
                       float r, float g, float b) {
    // Outer (Bresenham)
    glColor3f(r, g, b);
    glPointSize(2.0f);
    drawRetroBorder(x, y, w, h, r, g, b, 2.0f);

    // Inner (DDA) — 6px inset
    float p = 6.0f;
    glColor3f(r*0.6f, g*0.6f, b*0.6f);
    glPointSize(1.0f);
    ddaLine(x+p,   y+p,   x+w-p, y+p);
    ddaLine(x+w-p, y+p,   x+w-p, y+h-p);
    ddaLine(x+w-p, y+h-p, x+p,   y+h-p);
    ddaLine(x+p,   y+h-p, x+p,   y+p);

    // Corner accent squares
    glColor3f(r, g, b);
    float cs = 5.0f;
    for (int i = 0; i < 4; i++) {
        float cx2 = (i==1||i==3) ? x+w-cs : x;
        float cy2 = (i==2||i==3) ? y+h-cs : y;
        glBegin(GL_QUADS);
        glVertex2f(cx2,    cy2);
        glVertex2f(cx2+cs, cy2);
        glVertex2f(cx2+cs, cy2+cs);
        glVertex2f(cx2,    cy2+cs);
        glEnd();
    }
}
// ─────────────────────────────────────────────────────────────────
//  Bitmap text helper
// ─────────────────────────────────────────────────────────────────
void uiDrawText(float x, float y, void* font, const char* text,
                float r, float g, float b) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    for (int i = 0; text[i]; i++) glutBitmapCharacter(font, text[i]);
}

// ─────────────────────────────────────────────────────────────────
//  5×7 pixel-art font bitmaps (A-Z, 0-9, space, !, ., :)
// ─────────────────────────────────────────────────────────────────
static const unsigned char FONT5x7[][7] = {
    {0x0E,0x11,0x11,0x1F,0x11,0x11,0x00}, // A
    {0x1E,0x11,0x11,0x1E,0x11,0x1E,0x00}, // B
    {0x0F,0x10,0x10,0x10,0x10,0x0F,0x00}, // C
    {0x1E,0x11,0x11,0x11,0x11,0x1E,0x00}, // D
    {0x1F,0x10,0x10,0x1E,0x10,0x1F,0x00}, // E
    {0x1F,0x10,0x10,0x1E,0x10,0x10,0x00}, // F
    {0x0F,0x10,0x10,0x13,0x11,0x0F,0x00}, // G
    {0x11,0x11,0x11,0x1F,0x11,0x11,0x00}, // H
    {0x0E,0x04,0x04,0x04,0x04,0x0E,0x00}, // I
    {0x07,0x02,0x02,0x02,0x12,0x0C,0x00}, // J
    {0x11,0x12,0x14,0x18,0x14,0x12,0x11}, // K
    {0x10,0x10,0x10,0x10,0x10,0x1F,0x00}, // L
    {0x11,0x1B,0x15,0x11,0x11,0x11,0x00}, // M
    {0x11,0x19,0x15,0x13,0x11,0x11,0x00}, // N
    {0x0E,0x11,0x11,0x11,0x11,0x0E,0x00}, // O
    {0x1E,0x11,0x11,0x1E,0x10,0x10,0x00}, // P
    {0x0E,0x11,0x11,0x15,0x12,0x0D,0x00}, // Q
    {0x1E,0x11,0x11,0x1E,0x14,0x13,0x11}, // R
    {0x0F,0x10,0x10,0x0E,0x01,0x01,0x1E}, // S
    {0x1F,0x04,0x04,0x04,0x04,0x04,0x00}, // T
    {0x11,0x11,0x11,0x11,0x11,0x0E,0x00}, // U
    {0x11,0x11,0x11,0x11,0x0A,0x04,0x00}, // V
    {0x11,0x11,0x11,0x15,0x1B,0x11,0x00}, // W
    {0x11,0x11,0x0A,0x04,0x0A,0x11,0x00}, // X
    {0x11,0x11,0x0A,0x04,0x04,0x04,0x00}, // Y
    {0x1F,0x01,0x02,0x04,0x08,0x1F,0x00}, // Z
    {0x0E,0x13,0x15,0x19,0x11,0x0E,0x00}, // 0
    {0x04,0x0C,0x04,0x04,0x04,0x0E,0x00}, // 1
    {0x0E,0x11,0x01,0x06,0x08,0x1F,0x00}, // 2
    {0x1F,0x02,0x04,0x02,0x11,0x0E,0x00}, // 3
    {0x02,0x06,0x0A,0x12,0x1F,0x02,0x00}, // 4
    {0x1F,0x10,0x1E,0x01,0x01,0x1E,0x00}, // 5
    {0x06,0x08,0x10,0x1E,0x11,0x0E,0x00}, // 6
    {0x1F,0x01,0x02,0x04,0x08,0x08,0x00}, // 7
    {0x0E,0x11,0x11,0x0E,0x11,0x0E,0x00}, // 8
    {0x0E,0x11,0x11,0x0F,0x01,0x0E,0x00}, // 9
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // SPACE (36)
    {0x04,0x04,0x04,0x04,0x00,0x04,0x00}, // !     (37)
    {0x00,0x00,0x00,0x00,0x00,0x04,0x00}, // .     (38)
    {0x00,0x04,0x00,0x00,0x04,0x00,0x00}, // :     (39)
};

static int charIdx(char c) {
    if (c>='A'&&c<='Z') return c-'A';
    if (c>='a'&&c<='z') return c-'a';
    if (c>='0'&&c<='9') return 26+(c-'0');
    if (c==' ') return 36;
    if (c=='!') return 37;
    if (c=='.') return 38;
    if (c==':') return 39;
    return 36;
}

// Draw one scaled character — Scaling transformation around pivot (cx,cy)
// x' = cx + (col - 2) * pixSize * scale
void drawScaledChar(float cx, float cy, char c, float scale,
                    float r, float g, float b) {
    const unsigned char* bits = FONT5x7[charIdx(c)];
    float ps = 2.0f * scale; // pixel size
    glColor3f(r, g, b);
    for (int row = 0; row < 7; row++) {
        for (int col = 0; col < 5; col++) {
            if (bits[row] & (0x10 >> col)) {
                float px = cx + (col - 2.0f) * ps;
                float py = cy + (3.0f - row)  * ps;
                float h  = ps * 0.5f;
                glBegin(GL_QUADS);
                glVertex2f(px-h, py-h); glVertex2f(px+h, py-h);
                glVertex2f(px+h, py+h); glVertex2f(px-h, py+h);
                glEnd();
            }
        }
    }
}

void drawScaledText(float cx, float cy, const char* text, float scale,
                    float r, float g, float b) {
    int len = (int)strlen(text);
    float spacing = 7.0f * scale;
    float totalW  = len * spacing - 2.0f * scale;
    float startX  = cx - totalW * 0.5f + spacing * 0.5f;
    for (int i = 0; i < len; i++)
        drawScaledChar(startX + i * spacing, cy, text[i], scale, r, g, b);
}

// ─────────────────────────────────────────────────────────────────
//  Score HUD
// ─────────────────────────────────────────────────────────────────
void drawScoreHUD(int score, int highScore) {
    char buf[64];
    sprintf(buf, "HI %05d", highScore);
    uiDrawText(WINDOW_WIDTH - 200.0f, WINDOW_HEIGHT - 28.0f,
               GLUT_BITMAP_HELVETICA_18, buf, 0.45f, 0.45f, 0.45f);
    sprintf(buf, "%05d", score);
    uiDrawText(WINDOW_WIDTH - 78.0f, WINDOW_HEIGHT - 28.0f,
               GLUT_BITMAP_HELVETICA_18, buf, 0.1f, 0.1f, 0.1f);
    // DDA separator
    glColor3f(0.75f, 0.75f, 0.75f);
    glPointSize(1.0f);
    ddaLine(WINDOW_WIDTH-210.0f, WINDOW_HEIGHT-34.0f,
            WINDOW_WIDTH-8.0f,  WINDOW_HEIGHT-34.0f);
}

// ─────────────────────────────────────────────────────────────────
//  High Score File I/O
// ─────────────────────────────────────────────────────────────────
static const char* HISCORE_FILE = "highscore.dat";
void saveHighScore(int score) {
    FILE* f = fopen(HISCORE_FILE, "w");
    if (f) { fprintf(f, "%d", score); fclose(f); }
}
int loadHighScore() {
    FILE* f = fopen(HISCORE_FILE, "r");
    if (!f) return 0;
    int v = 0; fscanf(f, "%d", &v); fclose(f); return v;
}

// ─────────────────────────────────────────────────────────────────
//  UIManager
// ─────────────────────────────────────────────────────────────────
UIManager::UIManager()
    : slideOffset(0.0f), slideTarget(0.0f), slideSpeed(12.0f),
      pulseScale(1.0f), pulseDir(1.0f),
      pulseMin(0.82f), pulseMax(1.28f), pulseSpeed(0.016f),
      scoreFlashTimer(0), scoreFlashVisible(true),
      blinkTimer(0), blinkVisible(true),
      activeMenu(MENU_START) {}

void UIManager::init() {
    slideOffset = (float)WINDOW_HEIGHT;
    slideTarget = 0.0f;
    pulseScale  = 1.0f;
    pulseDir    = 1.0f;
    blinkTimer  = 0;
    blinkVisible= true;
    activeMenu  = MENU_START;
}

void UIManager::update() {
    // Translation slide animation
    if (fabsf(slideOffset - slideTarget) > 0.5f) {
        float dir = (slideTarget > slideOffset) ? 1.0f : -1.0f;
        slideOffset += dir * slideSpeed;
        if (dir > 0 && slideOffset > slideTarget) slideOffset = slideTarget;
        if (dir < 0 && slideOffset < slideTarget) slideOffset = slideTarget;
    }
    // Scaling pulse for GAME OVER
    if (activeMenu == MENU_GAMEOVER) {
        pulseScale += pulseDir * pulseSpeed;
        if (pulseScale >= pulseMax) { pulseScale = pulseMax; pulseDir = -1.0f; }
        if (pulseScale <= pulseMin) { pulseScale = pulseMin; pulseDir =  1.0f; }
    }
    // Blink
    blinkTimer++;
    if (blinkTimer >= 32) { blinkTimer = 0; blinkVisible = !blinkVisible; }
}

void UIManager::showStartMenu() {
    activeMenu  = MENU_START;
    slideOffset = (float)WINDOW_HEIGHT;
    slideTarget = 0.0f;
    pulseScale  = 1.0f;
}

void UIManager::showPauseMenu() {
    activeMenu  = MENU_PAUSE;
    slideOffset = -(float)WINDOW_HEIGHT;
    slideTarget = 0.0f;
}

void UIManager::showGameOverScreen(int score, int highScore) {
    activeMenu  = MENU_GAMEOVER;
    slideOffset = (float)WINDOW_HEIGHT;
    slideTarget = 0.0f;
    pulseScale  = 1.0f;
    pulseDir    = 1.0f;
    if (score > highScore) saveHighScore(score);
}

// ─────────────────────────────────────────────────────────────────
//  drawHUD
// ─────────────────────────────────────────────────────────────────
void UIManager::drawHUD(int score, int highScore, bool isPaused) {
    drawScoreHUD(score, highScore);
    if (isPaused) drawPauseMenuContent();
}

// ─────────────────────────────────────────────────────────────────
//  Start Menu
// ─────────────────────────────────────────────────────────────────
void UIManager::drawStartMenuContent() {
    // Translation: slide in from top
    glPushMatrix();
    glTranslatef(0.0f, slideOffset, 0.0f);

    float cx = WINDOW_WIDTH * 0.5f, cy = WINDOW_HEIGHT * 0.5f;
    float bW = 450.0f, bH = 234.0f;
    float bX = cx - bW*0.5f, bY = cy - bH*0.5f;

    // Dark panel — ensures all text is visible against white game bg
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.08f, 0.08f, 0.12f, 0.88f);
    glBegin(GL_QUADS);
    glVertex2f(bX,bY); glVertex2f(bX+bW,bY);
    glVertex2f(bX+bW,bY+bH); glVertex2f(bX,bY+bH);
    glEnd();

    // Green decorative border
    drawDecorativeBox(bX, bY, bW, bH, 0.2f, 0.85f, 0.3f);

    // Title "DINO RUN" — large sharp bitmap font, bright green
    uiDrawText(cx - 82, bY+bH-44, GLUT_BITMAP_TIMES_ROMAN_24,
               "D I N O   R U N", 0.2f, 1.0f, 0.35f);

    // DDA divider
    glColor3f(0.25f, 0.75f, 0.35f); glPointSize(1.0f);
    ddaLine(bX+18, bY+bH-70, bX+bW-18, bY+bH-70);

    // Controls — bright white text
    uiDrawText(cx-128, bY+bH-94, GLUT_BITMAP_HELVETICA_12,
               "UP / SPACE = Jump          DOWN = Duck",
               1.0f, 1.0f, 1.0f);
    uiDrawText(cx-100, bY+bH-112, GLUT_BITMAP_HELVETICA_12,
               "LEFT / RIGHT = Move Horizontally",
               1.0f, 1.0f, 1.0f);
    uiDrawText(cx-88, bY+bH-130, GLUT_BITMAP_HELVETICA_12,
               "DOUBLE-TAP UP = Double Jump",
               0.8f, 0.8f, 0.8f);

    // Second divider
    glColor3f(0.2f, 0.6f, 0.3f);
    ddaLine(bX+18, bY+74, bX+bW-18, bY+74);

    // Blinking start prompt — bright yellow, GLUT bitmap for sharpness
    if (blinkVisible)
        uiDrawText(cx - 120, bY + 48,
                   GLUT_BITMAP_HELVETICA_18,
                   "PRESS  ENTER  TO  START",
                   1.0f, 0.95f, 0.2f);

    // Arrow decorations (Bresenham)
    glColor3f(0.2f, 0.9f, 0.35f); glPointSize(2.0f);
    bresenhamLine((int)bX+20,(int)(bY+52),(int)bX+34,(int)(bY+58));
    bresenhamLine((int)bX+20,(int)(bY+52),(int)bX+34,(int)(bY+46));
    bresenhamLine((int)(bX+bW)-20,(int)(bY+52),(int)(bX+bW)-34,(int)(bY+58));
    bresenhamLine((int)(bX+bW)-20,(int)(bY+52),(int)(bX+bW)-34,(int)(bY+46));
    glPointSize(1.0f);

    glPopMatrix();
}

// ─────────────────────────────────────────────────────────────────
//  Pause Menu
// ─────────────────────────────────────────────────────────────────
void UIManager::drawPauseMenuContent() {
    // Translation: slide in from bottom
    glPushMatrix();
    glTranslatef(0.0f, slideOffset, 0.0f);

    float cx = WINDOW_WIDTH*0.5f, cy = WINDOW_HEIGHT*0.5f;
    float bW = 310.0f, bH = 140.0f;
    float bX = cx-bW*0.5f, bY = cy-bH*0.5f;

    // Dark panel
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.06f, 0.06f, 0.18f, 0.90f);
    glBegin(GL_QUADS);
    glVertex2f(bX,bY); glVertex2f(bX+bW,bY);
    glVertex2f(bX+bW,bY+bH); glVertex2f(bX,bY+bH);
    glEnd();

    // Blue border
    drawDecorativeBox(bX, bY, bW, bH, 0.3f, 0.45f, 1.0f);

    // "PAUSED" — shadow + bright blue-white
    drawScaledText(cx+2, bY+bH-43, "PAUSED", 2.0f, 0.0f, 0.0f, 0.0f);
    drawScaledText(cx,   bY+bH-41, "PAUSED", 2.0f, 0.5f, 0.7f, 1.0f);

    // DDA divider
    glColor3f(0.35f, 0.5f, 0.9f); glPointSize(1.0f);
    ddaLine(bX+14, bY+bH-60, bX+bW-14, bY+bH-60);

    // Bright white instructions
    if (blinkVisible)
        uiDrawText(cx-107, bY+52, GLUT_BITMAP_HELVETICA_18,
                   "SPACE  TO  RESUME", 1.0f, 0.95f, 0.2f);
    uiDrawText(cx-55, bY+28, GLUT_BITMAP_HELVETICA_12,
               "ESC TO QUIT", 0.7f, 0.7f, 0.7f);

    glPopMatrix();
}

// ─────────────────────────────────────────────────────────────────
//  Game Over Screen
// ─────────────────────────────────────────────────────────────────
void UIManager::drawGameOverContent(int score, int highScore) {
    // Translation: slide in from top
    glPushMatrix();
    glTranslatef(0.0f, slideOffset, 0.0f);

    float cx = WINDOW_WIDTH*0.5f, cy = WINDOW_HEIGHT*0.5f;
    float bW = 460.0f, bH = 250.0f;
    float bX = cx-bW*0.5f, bY = cy-bH*0.5f;

    // Dark semi-transparent backdrop for contrast
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.12f, 0.05f, 0.05f, 0.82f);
    glBegin(GL_QUADS);
    glVertex2f(bX,bY); glVertex2f(bX+bW,bY);
    glVertex2f(bX+bW,bY+bH); glVertex2f(bX,bY+bH);
    glEnd();

    // Red decorative border
    drawDecorativeBox(bX, bY, bW, bH, 0.90f, 0.25f, 0.25f);

    // ── "GAME OVER" — large pixel-art + Scaling pulse animation
    // Scaling pivot at text centre: glTranslate to pivot, scale, translate back
   // "GAME OVER" — large sharp bitmap font with pulse via spacing trick
    // Shadow
    uiDrawText(cx - 108, bY+bH-54, GLUT_BITMAP_TIMES_ROMAN_24,
               "G A M E   O V E R", 0.0f, 0.0f, 0.0f);
    // Main bright red on top
    uiDrawText(cx - 110, bY+bH-52, GLUT_BITMAP_TIMES_ROMAN_24,
               "G A M E   O V E R", 1.0f, 0.18f, 0.18f);

    // DDA divider under title
    glColor3f(0.85f, 0.3f, 0.3f); glPointSize(1.0f);
    ddaLine(bX+20, bY+bH-76, bX+bW-20, bY+bH-76);

    // ── Score — large, bright white
    char buf[64];
    sprintf(buf, "SCORE :  %d", score);
    uiDrawText(cx - 72, bY + bH - 108,
               GLUT_BITMAP_HELVETICA_18, buf, 1.0f, 1.0f, 1.0f);

    // ── High score / new record line
    if (score > 0 && score >= highScore) {
        uiDrawText(cx - 85, bY + bH - 134,
                   GLUT_BITMAP_HELVETICA_18,
                   "*** NEW HIGH SCORE ! ***", 1.0f, 0.85f, 0.0f);
    } else {
        sprintf(buf, "BEST  :  %d", highScore);
        uiDrawText(cx - 65, bY + bH - 134,
                   GLUT_BITMAP_HELVETICA_18, buf, 0.75f, 0.75f, 0.75f);
    }

    // Second DDA divider above prompt
    glColor3f(0.7f, 0.35f, 0.35f);
    ddaLine(bX+20, bY+80, bX+bW-20, bY+80);

    // ── Blinking "PRESS ENTER TO RETRY" — bright yellow, readable
    if (blinkVisible) {
        uiDrawText(cx - 118, bY + 52,
                   GLUT_BITMAP_HELVETICA_18,
                   "PRESS  ENTER  TO  RETRY", 1.0f, 0.95f, 0.2f);
    }

    // Bresenham accent diagonals on top corners
    glColor3f(0.9f, 0.4f, 0.4f); glPointSize(2.0f);
    bresenhamLine((int)bX+10,        (int)(bY+bH)-10,
                  (int)bX+28,        (int)(bY+bH)-28);
    bresenhamLine((int)(bX+bW)-10,   (int)(bY+bH)-10,
                  (int)(bX+bW)-28,   (int)(bY+bH)-28);
    glPointSize(1.0f);

    glPopMatrix();
}