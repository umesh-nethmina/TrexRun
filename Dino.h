#ifndef DINO_H
#define DINO_H

struct Dino {
    float x;
    float y;
    float width;
    float height;
    float velocity;
    bool isJumping;

    Dino();
    void init();
    void update();
    void draw();
    void jump();
};

#endif
