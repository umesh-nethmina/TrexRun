#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "Constants.h"
#include "Dino.h"
#include "Obstacles.h"
#include "UIManager.h"

// ── Game State
bool gameOver    = false;
bool isPaused    = false;
bool gameStarted = false;
int  score       = 0;
int  highScore   = 0;
float gameSpeed  = 5.0f;
int  frameCount  = 0;

Dino            dino;
ObstacleManager obstacleManager;

// ── Collision (AABB)
bool checkCollision(Dino& d, Cactus& c) {
    // Shrink the T-Rex hitbox so it matches the physical character outline, 
    // ignoring the long tail and snout edges that make it feel unfair.
    float shrinkX = 10.0f; 
    float shrinkY = 10.0f; 
    
    float dinoLeft = d.x + shrinkX;
    float dinoRight = d.x + d.width - shrinkX;
    float dinoBottom = d.y + shrinkY;
    float dinoTop = d.y + d.height - shrinkY;
    
    // Also slightly shrink the cactus hitbox
    float cactusLeft = c.x + 4.0f;
    float cactusRight = c.x + c.width - 4.0f;
    float cactusBottom = c.y;
    float cactusTop = c.y + c.height - 4.0f;

    return (dinoLeft < cactusRight &&
            dinoRight > cactusLeft &&
            dinoBottom < cactusTop &&
            dinoTop > cactusBottom);
}

// ── initGame
void initGame() {
    dino.init();
    dino.loadTextures();
    obstacleManager.init();
    score      = 0;
    gameSpeed  = 5.0f;
    gameOver   = false;
    isPaused   = false;
    frameCount = 0;
}

// ── Update loop (~60 FPS)
void update(int value) {
    uiManager.update();   // advance UI animations every frame

    if (gameStarted && !gameOver && !isPaused) {
        frameCount++;
        if (frameCount % 10 == 0) {
            score++;
            if (score % 100 == 0) gameSpeed += 0.5f;
        }
        dino.update(gameSpeed);
        obstacleManager.update(gameSpeed);

        for (int i = 0; i < ObstacleManager::MAX_CACTI; i++) {
            if (obstacleManager.cacti[i].active &&
                checkCollision(dino, obstacleManager.cacti[i])) {
                gameOver = true;
                dino.die();
                if (score > highScore) highScore = score;
                uiManager.showGameOverScreen(score, highScore);
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// ── Display
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Ground line
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_LINES);
    glVertex2f(0.0f, GROUND_Y);
    glVertex2f((float)WINDOW_WIDTH, GROUND_Y);
    glEnd();

    dino.draw();
    obstacleManager.draw();

    if (!gameStarted) {
        // Start screen
        uiManager.drawStartMenuContent();
    } else if (gameOver) {
        // Game over screen  
        uiManager.drawGameOverContent(score, highScore);
    } else {
        // In-game HUD (score + optional pause overlay)
        uiManager.drawHUD(score, highScore, isPaused);
    }

    glutSwapBuffers();
}

// ── Keyboard
void keyboard(unsigned char key, int x, int y) {
    if (key == 27) exit(0); // ESC

    if (key == ' ' && gameStarted && !gameOver) {
        isPaused = !isPaused;
        if (isPaused) uiManager.showPauseMenu();
    }

    if (key == 13) { // ENTER
        if (!gameStarted) {
            gameStarted = true;
            uiManager.activeMenu = MENU_NONE;
        } else if (gameOver) {
            initGame();
            gameStarted = true;
            uiManager.activeMenu = MENU_NONE;
        }
    }
}

void keyboardUp(unsigned char key, int x, int y) {}

void specialKeys(int key, int x, int y) {
    if (gameStarted && !isPaused && !gameOver) {
        if (key == GLUT_KEY_UP   && dino.state != DEAD) dino.jump();
        if (key == GLUT_KEY_DOWN && dino.state != DEAD) dino.duck();
        if (key == GLUT_KEY_LEFT && dino.state != DEAD) dino.isMovingLeft  = true;
        if (key == GLUT_KEY_RIGHT&& dino.state != DEAD) dino.isMovingRight = true;
    }
}

void specialKeysUp(int key, int x, int y) {
    if (key == GLUT_KEY_DOWN  && !gameOver) dino.unduck();
    if (key == GLUT_KEY_LEFT)               dino.isMovingLeft  = false;
    if (key == GLUT_KEY_RIGHT)              dino.isMovingRight = false;
}

// ── initGL
void initGL() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// ── main
int main(int argc, char** argv) {
    srand((unsigned int)time(NULL));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Chrome Dinosaur Game Clone");

    initGL();
    initGame();

    // Load high score from file
    highScore = loadHighScore();

    // Init UI — start menu slides in
    uiManager.init();
    uiManager.showStartMenu();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialKeys);
    glutSpecialUpFunc(specialKeysUp);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}