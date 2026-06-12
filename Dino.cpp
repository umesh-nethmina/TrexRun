#include "Dino.h"
#include "Constants.h"
#include <GL/glut.h>

Dino::Dino() {
    init();
}

void Dino::init() {
    x = 100.0f;
    y = GROUND_Y;
    width = 40.0f;
    height = 40.0f;
    velocity = 0.0f;
    isJumping = false;
}

void Dino::update() {
    if (isJumping) {
        velocity -= GRAVITY;
        y += velocity;
        if (y <= GROUND_Y) {
            y = GROUND_Y;
            isJumping = false;
            velocity = 0.0f;
        }
    }
}

void Dino::draw() {
    glColor3f(0.4f, 0.4f, 0.4f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

void Dino::jump() {
    if (!isJumping) {
        isJumping = true;
        velocity = JUMP_STRENGTH;
    }
}
