#ifndef OBSTACLES_H
#define OBSTACLES_H

struct Cactus {
    float x;
    float y;
    float width;
    float height;
    bool active;
    
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
