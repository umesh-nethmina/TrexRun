#ifndef DINO_H
#define DINO_H

#include <GL/glut.h>

enum DinoState {
    RUNNING,
    JUMPING,
    DUCKING,
    DEAD
};

struct SpriteFrame {
    float width;
    float height;
    float snoutX;
};

struct Dino {
    float x;
    float y;
    float width;
    float height;
    float velocity;
    DinoState state;
    int currentFrame;
    int frameCounter;
    bool isMovingLeft;
    bool isMovingRight;
    bool hasDoubleJumped;
    bool facingLeft;

    GLuint runTextures[3];
    GLuint duckTextures[3];
    SpriteFrame runFrames[3];
    SpriteFrame duckFrames[3];

    // 2D Transformation states
    float rotationAngle;
    float shearFactor;
    float scaleX;
    float scaleY;

    Dino();
    void init();
    void update(float gameSpeed = 5.0f);
    void draw();
    void jump();
    void duck();
    void unduck();
    void die();
    
    void loadTextures();
};

#endif
