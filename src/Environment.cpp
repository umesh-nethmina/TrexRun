#include "Environment.h"
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>

Environment::Environment() {
    // ctor does nothing; init() does the setup
    currentGameSpeed = 0.0f;
    currentFrame = 0;

    // Define an irregular mountain ridge (x offset, height above baseline)
    // Points are chosen to create a natural-looking silhouette with varied peaks and valleys.
    // X values increase from 0 to patternWidth.
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

    // Ensure points are sorted by x (they are)
    // Compute pattern width as distance from first to last point
    if (!mountainRidge.empty()) {
        patternWidth = mountainRidge.back().x - mountainRidge.front().x;
    } else {
        patternWidth = 0.0f;
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

    // ----- Far parallax layer (silhouette mountains) -----
    farLayer.x = 0.0f;
    farLayer.y = WINDOW_HEIGHT * 0.35f;   // place mountains above ground, below clouds
    farLayer.width = (float)WINDOW_WIDTH * 2.0f; // wide enough to scroll
    farLayer.height = 200.0f;            // height of the mountain area (not used directly)
    farLayer.speedFactor = 0.04f;        // very slow parallax

    // ----- Mid parallax layer (clouds) -----
    midLayer.x = 0.0f;
    midLayer.y = WINDOW_HEIGHT * 0.60f;  // a bit higher than mountains
    midLayer.width = (float)WINDOW_WIDTH * 2.0f;
    midLayer.height = WINDOW_HEIGHT * 0.25f;
    midLayer.speedFactor = 0.12f;        // slower than ground

    // ----- Time of day -----
    timeOfDay = 0.0f; // start at midnight

    // Set mountain base Y from farLayer.y
    mountainBaseY = farLayer.y;
}

void Environment::update(float gameSpeed, int frameCount) {
    // Store for use in draw
    currentGameSpeed = gameSpeed;
    currentFrame = frameCount;

    // Update ground tiles
    for (int i = 0; i < 3; ++i) {
        tiles[i].x -= gameSpeed;
        if (tiles[i].x + tiles[i].width < 0.0f) {
            recycleTile(tiles[i]);
        }
    }

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

    // Draw clouds (mid layer) - lighter quads
    drawClouds();

    // Draw mountains (far layer) using Bresenham line silhouette (filled)
    drawMountains();

    // Draw ground tiles
    drawGround();
}

void Environment::drawGround() {
    glColor3f(0.2f, 0.2f, 0.2f);
    for (int i = 0; i < 3; ++i) {
        glBegin(GL_QUADS);
            glVertex2f(tiles[i].x, tiles[i].y);
            glVertex2f(tiles[i].x + tiles[i].width, tiles[i].y);
            glVertex2f(tiles[i].x + tiles[i].width, tiles[i].y + tiles[i].height);
            glVertex2f(tiles[i].x, tiles[i].y + tiles[i].height);
        glEnd();
    }
}

void Environment::recycleTile(GroundTile& tile) {
    // Find the tile that is furthest to the right and place this one after it
    float furthestRight = tiles[0].x;
    for (int i = 1; i < 3; ++i) {
        if (tiles[i].x > furthestRight) furthestRight = tiles[i].x;
    }
    tile.x = furthestRight + GROUND_TILE_WIDTH;
    // y, width, height stay the same
}

void Environment::drawClouds() {
    // Draw a simple cloud layer as a set of puffy quads (or just a single quad for simplicity)
    // We'll draw a few ellipsoid-like quads to represent clouds.
    // For simplicity, we'll just draw a light gray stretched quad.
    glColor3f(0.9f, 0.9f, 0.95f); // almost white
    glBegin(GL_QUADS);
        glVertex2f(midLayer.x, midLayer.y);
        glVertex2f(midLayer.x + midLayer.width, midLayer.y);
        glVertex2f(midLayer.x + midLayer.width, midLayer.y + midLayer.height);
        glVertex2f(midLayer.x, midLayer.y + midLayer.height);
    glEnd();
}

void Environment::drawMountains() {
    if (mountainRidge.empty()) return;

    // Determine how many pattern repeats we need to cover the visible width plus some margin
    int patternCount = (int)ceil((farLayer.width + patternWidth) / patternWidth) + 1;
    int startOffset = (int)farLayer.x;

    // Choose mountain color based on day/night (dark gray, slightly tinted by sky)
    float baseIntensity = 0.15f; // mountain base darkness
    float dayFactor = timeOfDay; // 0 = night, 1 = day
    // At night mountains are a bit darker; at day they are slightly lighter (still dark)
    float intensity = baseIntensity + 0.05f * dayFactor; // range ~0.15-0.20
    glColor3f(intensity, intensity, intensity);

    for (int i = -1; i <= patternCount; ++i) {
        float patternStartX = startOffset + i * patternWidth;
        // Draw filled silhouette for this pattern repeat
        // We'll iterate over integer x positions within the pattern width
        int startX = (int)patternStartX;
        int endX   = (int)(patternStartX + patternWidth);
        for (int x = startX; x < endX; ++x) {
            float localX = x - patternStartX; // in [0, patternWidth]
            float ridgeY = getRidgeHeight(localX); // height above base (>=0)
            int yTop = (int)(mountainBaseY + ridgeY);
            int yBase = (int)mountainBaseY;
            // Draw vertical line from base to top (inclusive)
            drawLineBresenham(x, yBase, x, yTop);
        }
    }
}

/* Helper: linear interpolation to get ridge height at localX */
float Environment::getRidgeHeight(float localX) const {
    if (mountainRidge.size() < 2) return 0.0f;
    // Ensure localX is within [0, patternWidth]; wrap if needed (should already be within due to loop)
    if (localX < 0.0f) localX = 0.0f;
    if (localX > patternWidth) localX = patternWidth;

    // Find segment where localX lies between points[i].x and points[i+1].x
    for (size_t i = 0; i + 1 < mountainRidge.size(); ++i) {
        const Point& p0 = mountainRidge[i];
        const Point& p1 = mountainRidge[i+1];
        if (localX >= p0.x && localX <= p1.x) {
            // Avoid division by zero (should not happen as x's are increasing)
            float segLen = (float)(p1.x - p0.x);
            if (segLen == 0.0f) return (float)p0.y;
            float t = (localX - p0.x) / segLen;
            return (float)p0.y + t * ((float)p1.y - (float)p0.y);
        }
    }
    // If localX is exactly at the last point, return its y
    if (!mountainRidge.empty())
        return (float)mountainRidge.back().y;
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