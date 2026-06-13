#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "Constants.h"
#include <vector>

struct GroundTile {
    float x;        // left‑most x of the tile
    float y;        // y‑position (top of tile)
    float width;
    float height;
};

struct ParallaxLayer {
    float x;
    float y;
    float width;
    float height;
    float speedFactor; // multiplied by gameSpeed
};

class Environment {
public:
    Environment();
    void init();                     // call once at start / after reset
    void update(float gameSpeed, int frameCount);    // call each frame
    void draw();                     // call each frame after clearing buffer
private:
    void recycleTile(GroundTile& tile);
    void drawGround();               // draws scrolling ground using tiles
    void drawClouds();               // draws mid parallax layer (clouds)
    void drawMountains();            // draws far parallax layer (mountain silhouette) using Bresenham
    void drawSky(float dayNightFactor); // draws gradient sky, sun/moon using Bresenham circle
    void drawStars(float dayNightFactor); // draws stars (more visible at night)
    // Bresenham helpers
    static void drawLineBresenham(int x0, int y0, int x1, int y1);
    static void drawCircleBresenham(int xc, int yc, int radius, bool filled = false);

    GroundTile tiles[3];             // enough tiles to cover screen + a little extra
    ParallaxLayer farLayer;          // far parallax layer (mountains) – only x used for scroll
    ParallaxLayer midLayer;          // e.g., clouds
    float timeOfDay;                 // 0.0 = night, 1.0 = day, loops
    float currentGameSpeed;          // latest gameSpeed passed to update
    int    currentFrame;             // latest frameCount passed to update

    // Mountain silhouette data
    struct Point { int x; int y; };  // x: horizontal offset from pattern start, y: height above baseline (>=0)
    std::vector<Point> mountainRidge; // control points of the ridge (sorted by x, covering [0, patternWidth])
    float mountainBaseY;             // Y coordinate of the mountain base (where fill stops)
    float patternWidth;              // width of one repeating pattern (in world units)

    // Helper: get ridge height (above base) at a given localX within [0, patternWidth]
    float getRidgeHeight(float localX) const;
};

#endif // ENVIRONMENT_H