#ifndef OBSTACLES_H
#define OBSTACLES_H

struct Cactus {
    float x;
    float y;
    float width;
    float height;
    bool active;
    
    // Falling meteor physics and aesthetics
    float vx;
    float vy;
    bool isStuck;
    int type;
    float rotation;
    float rotSpeed;
    float trailX[8];
    float trailY[8];
    int trailCount;
    
    Cactus();
};

class ObstacleManager {
public:
    static const int MAX_CACTI = 5;
    Cactus cacti[MAX_CACTI];

    ObstacleManager();
    void init();
    void spawnCactus();
    void update(float gameSpeed);
    void draw();
};

#endif
