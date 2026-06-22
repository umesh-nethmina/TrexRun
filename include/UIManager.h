#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <GL/glut.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// ─────────────────────────────────────────────
//  MENU STATES
// ─────────────────────────────────────────────
enum MenuState {
    MENU_START,
    MENU_PAUSE,
    MENU_GAMEOVER,
    MENU_NONE
};

// ─────────────────────────────────────────────
//  DDA Line Drawing (floating-point precision)
// ─────────────────────────────────────────────
void ddaLine(float x0, float y0, float x1, float y1);

// ─────────────────────────────────────────────
//  Bresenham Line Drawing (integer precision)
// ─────────────────────────────────────────────
void bresenhamLine(int x0, int y0, int x1, int y1);

// ─────────────────────────────────────────────
//  Draw a retro rectangular border using Bresenham
// ─────────────────────────────────────────────
void drawRetroBorder(float x, float y, float w, float h,
                     float r, float g, float b, float thickness);

// ─────────────────────────────────────────────
//  Draw a double-lined decorative box (DDA inner, Bresenham outer)
// ─────────────────────────────────────────────
void drawDecorativeBox(float x, float y, float w, float h,
                       float r, float g, float b);

// ─────────────────────────────────────────────
//  Bitmap text helper
// ─────────────────────────────────────────────
void uiDrawText(float x, float y, void* font, const char* text,
                float r, float g, float b);

// ─────────────────────────────────────────────
//  Scaled text using manual pixel quads (Translation + Scaling)
// ─────────────────────────────────────────────
void drawScaledChar(float cx, float cy, char c, float scale,
                    float r, float g, float b);
void drawScaledText(float cx, float cy, const char* text, float scale,
                    float r, float g, float b);

// ─────────────────────────────────────────────
//  Score display (HUD)
// ─────────────────────────────────────────────
void drawScoreHUD(int score, int highScore);

// ─────────────────────────────────────────────
//  High Score file I/O
// ─────────────────────────────────────────────
void saveHighScore(int score);
int  loadHighScore();

// ─────────────────────────────────────────────
//  UIManager class
// ─────────────────────────────────────────────
class UIManager {
public:
    // Slide-in animation state (Translation)
    float slideOffset;       // current Y offset for slide animation
    float slideTarget;       // target Y offset (0 = fully visible)
    float slideSpeed;        // units per frame

    // Pulse/scale animation state (Scaling)
    float pulseScale;        // current scale factor for GAME OVER text
    float pulseDir;          // +1 growing, -1 shrinking
    float pulseMin, pulseMax;
    float pulseSpeed;

    // Flash animation for score milestone
    int   scoreFlashTimer;
    bool  scoreFlashVisible;

    // Selector blink
    int   blinkTimer;
    bool  blinkVisible;

    MenuState activeMenu;

    UIManager();
    void init();
    void update();           // call every frame to advance animations

    void showStartMenu();
    void showPauseMenu();
    void showGameOverScreen(int score, int highScore);
    void drawHUD(int score, int highScore, bool isPaused);

    // Internal drawers
    void drawStartMenuContent();
    void drawPauseMenuContent();
    void drawGameOverContent(int score, int highScore);
};

// Global instance
extern UIManager uiManager;

#endif // UIMANAGER_H