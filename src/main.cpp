#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "Constants.h"
#include "Dino.h"
#include "Obstacles.h"

// Game State
bool gameOver = false;
bool isPaused = false;
bool gameStarted = false;
int score = 0;
int highScore = 0;
float gameSpeed = 5.0f;
int frameCount = 0;

Dino dino;
ObstacleManager obstacleManager;

// Drawing Text
void drawText(float x, float y, void* font, const char* string) {
    glRasterPos2f(x, y);
    int len = (int)strlen(string);
    for (int i = 0; i < len; i++) {
        glutBitmapCharacter(font, string[i]);
    }
}

void initGame() {
    dino.init();
    dino.loadTextures();
    obstacleManager.init();
    
    score = 0;
    gameSpeed = 5.0f;
    gameOver = false;
    isPaused = false;
    frameCount = 0;
}

bool checkCollision(Dino& d, Cactus& c) {
    // Simple Axis-Aligned Bounding Box (AABB) collision
    if (d.x < c.x + c.width &&
        d.x + d.width > c.x &&
        d.y < c.y + c.height &&
        d.y + d.height > c.y) {
        return true;
    }
    return false;
}

void update(int value) {
    if (gameStarted && !gameOver && !isPaused) {
        frameCount++;
        // Increase score every 10 frames
        if (frameCount % 10 == 0) {
            score++;
            // Speed up game gradually
            if (score % 100 == 0) {
                gameSpeed += 0.5f; 
            }
        }

        dino.update(gameSpeed);
        obstacleManager.update(gameSpeed);

        // Check collisions
        for (int i = 0; i < ObstacleManager::MAX_CACTI; i++) {
            if (obstacleManager.cacti[i].active && checkCollision(dino, obstacleManager.cacti[i])) {
                gameOver = true;
                dino.die();
                if (score > highScore) {
                    highScore = score;
                }
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0); // ~60 FPS
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw Ground
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_LINES);
    glVertex2f(0.0f, GROUND_Y);
    glVertex2f((float)WINDOW_WIDTH, GROUND_Y);
    glEnd();

    dino.draw();
    obstacleManager.draw();

    // Draw Scores
    glColor3f(0.0f, 0.0f, 0.0f); // Black text
    char scoreStr[50];
    sprintf(scoreStr, "HI %05d  %05d", highScore, score);
    drawText(WINDOW_WIDTH - 200, WINDOW_HEIGHT - 30, GLUT_BITMAP_HELVETICA_18, scoreStr);

    if (!gameStarted) {
        drawText(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2, GLUT_BITMAP_HELVETICA_18, "PRESS ENTER TO START");
    } else if (gameOver) {
        drawText(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2, GLUT_BITMAP_HELVETICA_18, "G A M E   O V E R");
        drawText(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 - 30, GLUT_BITMAP_HELVETICA_12, "Press ENTER to Restart");
    } else if (isPaused) {
        drawText(WINDOW_WIDTH / 2 - 30, WINDOW_HEIGHT / 2, GLUT_BITMAP_HELVETICA_18, "PAUSED");
    }

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 27) { // ESC key
        exit(0);
    }
    
    // Space for pause and play
    if (key == ' ' && gameStarted && !gameOver) {
        isPaused = !isPaused;
    }
    
    // Enter for start and restart
    if (key == 13) {
        if (!gameStarted) {
            gameStarted = true;
        } else if (gameOver) {
            initGame();
            gameStarted = true;
        }
    }
}

void keyboardUp(unsigned char key, int x, int y) {
    // Unused but needed for callback
}

void specialKeys(int key, int x, int y) {
    if (gameStarted && !isPaused && !gameOver) {
        if (key == GLUT_KEY_UP && dino.state != DEAD) {
            dino.jump();
        }
        if (key == GLUT_KEY_DOWN && dino.state != DEAD) {
            dino.duck();
        }
        if (key == GLUT_KEY_LEFT && dino.state != DEAD) {
            dino.isMovingLeft = true;
        }
        if (key == GLUT_KEY_RIGHT && dino.state != DEAD) {
            dino.isMovingRight = true;
        }
    }
}

void specialKeysUp(int key, int x, int y) {
    if (key == GLUT_KEY_DOWN && !gameOver) {
        dino.unduck();
    }
    if (key == GLUT_KEY_LEFT) {
        dino.isMovingLeft = false;
    }
    if (key == GLUT_KEY_RIGHT) {
        dino.isMovingRight = false;
    }
}

void initGL() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // White background
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int main(int argc, char** argv) {
    srand((unsigned int)time(NULL));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Chrome Dinosaur Game Clone");

    initGL();
    initGame();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialKeys);
    glutSpecialUpFunc(specialKeysUp);
    
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}
