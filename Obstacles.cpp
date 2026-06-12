#include "Obstacles.h"
#include "Constants.h"
#include <GL/glut.h>
#include <stdlib.h>

Cactus::Cactus() {
    x = 0;
    y = GROUND_Y;
    width = 30.0f;
    height = 50.0f;
    active = false;
}

ObstacleManager::ObstacleManager() {
    init();
}

void ObstacleManager::init() {
    for (int i = 0; i < MAX_CACTI; i++) {
        cacti[i].active = false;
    }
    spawnCactus();
}

void ObstacleManager::spawnCactus() {
    for (int i = 0; i < MAX_CACTI; i++) {
        if (!cacti[i].active) {
            cacti[i].x = WINDOW_WIDTH + (rand() % 400);
            cacti[i].height = 40.0f + (rand() % 30);
            cacti[i].active = true;
            break;
        }
    }
}

void ObstacleManager::update(float gameSpeed) {
    for (int i = 0; i < MAX_CACTI; i++) {
        if (cacti[i].active) {
            cacti[i].x -= gameSpeed;
            if (cacti[i].x + cacti[i].width < 0) {
                cacti[i].active = false;
            }
        }
    }

    if (rand() % 100 < 2) {
        bool canSpawn = true;
        for (int i = 0; i < MAX_CACTI; i++) {
            if (cacti[i].active && cacti[i].x > WINDOW_WIDTH - 200) {
                canSpawn = false;
                break;
            }
        }
        if (canSpawn) spawnCactus();
    }
}

void ObstacleManager::draw() {
    glColor3f(0.1f, 0.8f, 0.1f);
    for (int i = 0; i < MAX_CACTI; i++) {
        if (cacti[i].active) {
            glBegin(GL_QUADS);
            glVertex2f(cacti[i].x, cacti[i].y);
            glVertex2f(cacti[i].x + cacti[i].width, cacti[i].y);
            glVertex2f(cacti[i].x + cacti[i].width, cacti[i].y + cacti[i].height);
            glVertex2f(cacti[i].x, cacti[i].y + cacti[i].height);
            glEnd();
        }
    }
}
