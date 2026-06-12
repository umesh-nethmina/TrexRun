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
    obstacleManager.init();
    
    score = 0;
    gameSpeed = 5.0f;
    gameOver = false;
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
    if (!gameOver) {
        frameCount++;
        // Increase score every 10 frames
        if (frameCount % 10 == 0) {
            score++;
            // Speed up game gradually
            if (score % 100 == 0) {
                gameSpeed += 0.5f; 
            }
        }

        dino.update();
        obstacleManager.update(gameSpeed);

        // Check collisions
        for (int i = 0; i < ObstacleManager::MAX_CACTI; i++) {
            if (obstacleManager.cacti[i].active && checkCollision(dino, obstacleManager.cacti[i])) {
                gameOver = true;
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

    if (gameOver) {
        drawText(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2, GLUT_BITMAP_HELVETICA_18, "G A M E   O V E R");
        drawText(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 - 30, GLUT_BITMAP_HELVETICA_12, "Press 'R' to Restart");
    }

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 27) { // ESC key
        exit(0);
    }
    
    if (key == ' ' && !dino.isJumping && !gameOver) {
        dino.jump();
    }
    
    if ((key == 'r' || key == 'R') && gameOver) {
        initGame();
    }
}

void specialKeys(int key, int x, int y) {
    if (key == GLUT_KEY_UP && !dino.isJumping && !gameOver) {
        dino.jump();
    }
}

void initGL() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // White background
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    srand((unsigned int)time(NULL));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Chrome Dinosaur Game Clone");

    initGL();
    
    // Enable alpha blending for transparent PNG backgrounds
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    dino.loadTextures();
    initGame();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}
