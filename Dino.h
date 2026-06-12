#ifndef DINO_H
#define DINO_H

#include "Constants.h"

struct Dino {
    float x;
    float y;
    float width;
    float height;
    float velocity;
    bool isJumping;
    unsigned int texture; // Only one texture now
    int currentFrame;
    int frameTimer;

    Dino();
    void init();
    void loadTextures();
    void update();
    void draw();
    void jump();
};

#endif
