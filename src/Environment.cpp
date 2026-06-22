#include "Environment.h"
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>

Environment::Environment() {
    currentGameSpeed = 0.0f;
    currentFrame = 0;

    // Near mountain ridge (foreground silhouette)
    mountainRidge = {
        {   0,  20},
        {  30,  45},
        {  80,  30},
        { 130,  70},
        { 180,  55},
        { 230,  80},
        { 280,  40},
        { 340,  90},
        { 390,  60},
        { 440, 100},
        { 500,  50},
        { 560,  85},
        { 620,  45},
        { 680,  95},
        { 750,  55},
        { 820,  75},
        { 900,  40},
        { 980,  85},
        {1060,  30},
        {1150,  70}
    };

    if (!mountainRidge.empty()) {
        patternWidth = mountainRidge.back().x - mountainRidge.front().x;
    } else {
        patternWidth = 0.0f;
    }

    // Far mountain ridge (broader and taller background silhouette)
    farMountainRidge = {
        {   0,  50},
        { 100, 120},
        { 220,  70},
        { 350, 150},
        { 480,  80},
        { 600, 170},
        { 720,  90},
        { 850, 140},
        { 980,  60},
        {1100, 110},
        {1250,  50}
    };

    if (!farMountainRidge.empty()) {
        farPatternWidth = farMountainRidge.back().x - farMountainRidge.front().x;
    } else {
        farPatternWidth = 0.0f;
    }
}

void Environment::init() {
    // ----- Ground tiles -----
    float startX = 0.0f;
    for (int i = 0; i < 3; ++i) {
        tiles[i].x = startX + i * GROUND_TILE_WIDTH;
        tiles[i].y = GROUND_Y - GROUND_HEIGHT;   // bottom‑aligned
        tiles[i].width = GROUND_TILE_WIDTH;
        tiles[i].height = GROUND_HEIGHT;
    }

    // ----- Far parallax layer (Far Mountains) -----
    farLayer.x = 0.0f;
    farLayer.y = GROUND_Y;                // base Y sits exactly on the ground platform
    farLayer.width = (float)WINDOW_WIDTH * 2.0f; 
    farLayer.height = 200.0f;            
    farLayer.speedFactor = 0.02f;        // very slow scrolling for depth

    // ----- Mid parallax layer (Near Mountains) -----
    midLayer.x = 0.0f;
    midLayer.y = GROUND_Y;                // base Y sits exactly on the ground platform
    midLayer.width = (float)WINDOW_WIDTH * 2.0f;
    midLayer.height = 200.0f;
    midLayer.speedFactor = 0.05f;        // faster than farLayer

    // ----- Time of day -----
    timeOfDay = 0.0f; // start at midnight

    // Set mountain base Y
    mountainBaseY = GROUND_Y;
}

void Environment::update(float gameSpeed, int frameCount) {
    // Store for use in draw
    currentGameSpeed = gameSpeed;
    currentFrame = frameCount;

    // Ground tiles scrolling is disabled as the platform is now a solid continuous block
    /*
    for (int i = 0; i < 3; ++i) {
        tiles[i].x -= gameSpeed;
        if (tiles[i].x + tiles[i].width < 0.0f) {
            recycleTile(tiles[i]);
        }
    }
    */

    // Update parallax layers
    farLayer.x -= gameSpeed * farLayer.speedFactor;
    if (farLayer.x + farLayer.width < 0.0f) {
        farLayer.x = 0.0f;
    }
    midLayer.x -= gameSpeed * midLayer.speedFactor;
    if (midLayer.x + midLayer.width < 0.0f) {
        midLayer.x = 0.0f;
    }

    // Update time of day: full cycle every 30 seconds (adjust as desired)
    // Assume ~60 FPS -> 1800 frames per cycle
    const int CYCLE_FRAMES = 1800;
    timeOfDay = fmodf((float)frameCount / CYCLE_FRAMES, 1.0f);
}

void Environment::draw() {
    // Draw sky gradient and sun/moon based on timeOfDay
    float dayNight = timeOfDay; // 0 = night, 1 = day
    // Sky color: interpolate between night (dark blue) and day (light blue)
    float r = 0.5f + 0.5f * dayNight; // 0.5 (night) -> 1.0 (day)
    float g = 0.6f + 0.4f * dayNight; // 0.6 -> 1.0
    float b = 0.8f + 0.2f * dayNight; // 0.8 -> 1.0
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT); // clear with sky color

    // Draw stars (more visible at night)
    if (dayNight < 0.8f) { // only show when not too bright
        glPointSize(2.0f);
        glBegin(GL_POINTS);
        glColor3f(1.0f, 1.0f, 1.0f);
        const int starCount = 50;
        int starX[starCount] = { 100, 250, 400, 550, 700, 120, 300, 450, 600, 750,
                                 180, 330, 480, 630, 780, 220, 370, 520, 670, 720,
                                 90, 240, 390, 540, 690, 140, 290, 440, 590, 740,
                                 160, 310, 460, 610, 760, 110, 260, 410, 560, 710,
                                 130, 280, 430, 580, 730, 150, 320, 470, 620, 770 };
        int starY[starCount] = { 350, 360, 370, 380, 390, 340, 350, 360, 370, 380,
                                 330, 340, 350, 360, 370, 320, 330, 340, 350, 360,
                                 310, 320, 330, 340, 350, 300, 310, 320, 330, 340,
                                 290, 300, 310, 320, 330, 280, 290, 300, 310, 320,
                                 270, 280, 290, 300, 310, 260, 270, 280, 290, 300 };
        for (int i = 0; i < starCount; ++i) {
            // Apply parallax scroll for stars (very slow)
            float sx = starX[i] - (currentGameSpeed * 0.02f * currentFrame);
            if (sx < 0) sx += WINDOW_WIDTH * 2;
            if (sx > WINDOW_WIDTH * 2) sx -= WINDOW_WIDTH * 2;
            glVertex2f(sx, (float)starY[i]);
        }
        glEnd();
    }

    // Draw sun or moon using Bresenham circle
    int sunRadius = 30;
    int sunX = (int)(WINDOW_WIDTH * 0.8f);
    int sunY = (int)(WINDOW_HEIGHT * 0.8f);
    if (dayNight > 0.5f) { // day -> sun
        glColor3f(1.0f, 0.8f, 0.0f); // yellow
        drawCircleBresenham(sunX, sunY, sunRadius, true);
        // draw rays
        glColor3f(1.0f, 0.9f, 0.5f);
        for (int a = 0; a < 360; a += 30) {
            int dx = (int)(sunRadius * 1.5f * cosf(a * M_PI / 180.0f));
            int dy = (int)(sunRadius * 1.5f * sinf(a * M_PI / 180.0f));
            drawLineBresenham(sunX, sunY, sunX + dx, sunY + dy);
        }
    } else { // night -> moon
        glColor3f(0.9f, 0.9f, 1.0f); // pale yellow
        drawCircleBresenham(sunX, sunY, sunRadius, true);
        // moon craters (simple)
        glColor3f(0.7f, 0.7f, 0.8f);
        drawCircleBresenham(sunX - 10, sunY - 10, 5, true);
        drawCircleBresenham(sunX + 12, sunY - 8, 4, true);
        drawCircleBresenham(sunX - 5, sunY + 12, 6, true);
    }

    // Draw far mountains (background parallax layer)
    drawFarMountains();

    // Draw near mountains (foreground parallax layer)
    drawNearMountains();

    // Draw ground platform
    drawGround();
}

void Environment::drawGround() {
  // Draw main ground platform body with vertical gradient (from Y=0 to Y=GROUND_Y)
  // Deep warm brown to dark charcoal brown
  glBegin(GL_QUADS);
  glColor3f(0.28f, 0.18f, 0.12f); // Earthy warm brown at top
  glVertex2f(0.0f, GROUND_Y);
  glVertex2f((float)WINDOW_WIDTH, GROUND_Y);
  glColor3f(0.12f, 0.08f, 0.06f); // Dark charcoal brown at bottom
  glVertex2f((float)WINDOW_WIDTH, 0.0f);
  glVertex2f(0.0f, 0.0f);
  glEnd();
}

void Environment::recycleTile(GroundTile& tile) {
    float furthestRight = tiles[0].x;
    for (int i = 1; i < 3; ++i) {
        if (tiles[i].x > furthestRight) furthestRight = tiles[i].x;
    }
    tile.x = furthestRight + GROUND_TILE_WIDTH;
}

void Environment::drawFarMountains() {
    if (farMountainRidge.empty()) return;

    // Use farLayer properties
    int patternCount = (int)ceil((farLayer.width + farPatternWidth) / farPatternWidth) + 1;
    int startOffset = (int)farLayer.x;

    float dayFactor = timeOfDay;
    // Deep magical violet-indigo
    float r = 0.10f + 0.08f * dayFactor;
    float g = 0.06f + 0.06f * dayFactor;
    float b = 0.18f + 0.10f * dayFactor;
    glColor3f(r, g, b);

    for (int i = -1; i <= patternCount; ++i) {
        float patternStartX = startOffset + i * farPatternWidth;
        int startX = (int)patternStartX;
        int endX   = (int)(patternStartX + farPatternWidth);
        for (int x = startX; x < endX; ++x) {
            float localX = x - patternStartX;
            float ridgeY = getFarRidgeHeight(localX);
            int yTop = (int)(mountainBaseY + ridgeY);
            int yBase = (int)mountainBaseY;
            drawLineBresenham(x, yBase, x, yTop);
        }
    }
}

void Environment::drawNearMountains() {
    if (mountainRidge.empty()) return;

    // Use midLayer properties
    int patternCount = (int)ceil((midLayer.width + patternWidth) / patternWidth) + 1;
    int startOffset = (int)midLayer.x;

    float dayFactor = timeOfDay;
    // Dark charcoal-purple
    float r = 0.06f + 0.04f * dayFactor;
    float g = 0.05f + 0.03f * dayFactor;
    float b = 0.09f + 0.06f * dayFactor;
    glColor3f(r, g, b);

    for (int i = -1; i <= patternCount; ++i) {
        float patternStartX = startOffset + i * patternWidth;
        int startX = (int)patternStartX;
        int endX   = (int)(patternStartX + patternWidth);
        for (int x = startX; x < endX; ++x) {
            float localX = x - patternStartX;
            float ridgeY = getRidgeHeight(localX);
            int yTop = (int)(mountainBaseY + ridgeY);
            int yBase = (int)mountainBaseY;
            drawLineBresenham(x, yBase, x, yTop);
        }
    }
}

float Environment::getRidgeHeight(float localX) const {
    if (mountainRidge.size() < 2) return 0.0f;
    if (localX < 0.0f) localX = 0.0f;
    if (localX > patternWidth) localX = patternWidth;

    for (size_t i = 0; i + 1 < mountainRidge.size(); ++i) {
        const Point& p0 = mountainRidge[i];
        const Point& p1 = mountainRidge[i+1];
        if (localX >= p0.x && localX <= p1.x) {
            float segLen = (float)(p1.x - p0.x);
            if (segLen == 0.0f) return (float)p0.y;
            float t = (localX - p0.x) / segLen;
            return (float)p0.y + t * ((float)p1.y - (float)p0.y);
        }
    }
    if (!mountainRidge.empty())
        return (float)mountainRidge.back().y;
    return 0.0f;
}

float Environment::getFarRidgeHeight(float localX) const {
    if (farMountainRidge.size() < 2) return 0.0f;
    if (localX < 0.0f) localX = 0.0f;
    if (localX > farPatternWidth) localX = farPatternWidth;

    for (size_t i = 0; i + 1 < farMountainRidge.size(); ++i) {
        const Point& p0 = farMountainRidge[i];
        const Point& p1 = farMountainRidge[i+1];
        if (localX >= p0.x && localX <= p1.x) {
            float segLen = (float)(p1.x - p0.x);
            if (segLen == 0.0f) return (float)p0.y;
            float t = (localX - p0.x) / segLen;
            return (float)p0.y + t * ((float)p1.y - (float)p0.y);
        }
    }
    if (!farMountainRidge.empty())
        return (float)farMountainRidge.back().y;
    return 0.0f;
}

/* Bresenham line algorithm */
void Environment::drawLineBresenham(int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2; /* error value e_xy */
    while (true) {
        glBegin(GL_POINTS);
            glVertex2i(x0, y0);
        glEnd();
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
        if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
    }
}

/* Bresenham circle algorithm (midpoint) */
void Environment::drawCircleBresenham(int xc, int yc, int radius, bool filled) {
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;
    if (!filled) {
        while (y >= x) {
            // draw the 8 octants
            glBegin(GL_POINTS);
                glVertex2i(xc + x, yc + y);
                glVertex2i(xc - x, yc + y);
                glVertex2i(xc + x, yc - y);
                glVertex2i(xc - x, yc - y);
                glVertex2i(xc + y, yc + x);
                glVertex2i(xc - y, yc + x);
                glVertex2i(xc + y, yc - x);
                glVertex2i(xc - y, yc - x);
            glEnd();
            if (d < 0) d = d + 4 * x + 6;
            else {
                d = d + 4 * (x - y) + 10;
                --y;
            }
            ++x;
        }
    } else {
        // filled circle: draw horizontal lines for each y
        while (y >= x) {
            // for each scan line
            glBegin(GL_LINES);
                glVertex2i(xc - x, yc + y);
                glVertex2i(xc + x, yc + y);
                glVertex2i(xc - x, yc - y);
                glVertex2i(xc + x, yc - y);
                glVertex2i(xc - y, yc + x);
                glVertex2i(xc + y, yc + x);
                glVertex2i(xc - y, yc - x);
                glVertex2i(xc + y, yc - x);
            glEnd();
            if (d < 0) d = d + 4 * x + 6;
            else {
                d = d + 4 * (x - y) + 10;
                --y;
            }
            ++x;
        }
    }
}