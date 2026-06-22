#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Dino.h"
#include "Constants.h"
#include <GL/glut.h>
#include <stdio.h>

GLuint loadTex(const char* path) {
    int w, h, channels;
    unsigned char* data = stbi_load(path, &w, &h, &channels, 4); // force RGBA
    if (!data) {
        printf("Failed to load %s\n", path);
        return 0;
    }
    
    // Chroma key: replace white background with transparent
    for (int i = 0; i < w * h * 4; i += 4) {
        if (data[i] > 230 && data[i+1] > 230 && data[i+2] > 230) {
            data[i+3] = 0; // set alpha to 0 for white pixels
        }
    }
    
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    
    // Pixel-art filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // Wrap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    stbi_image_free(data);
    return tex;
}

void Dino::loadTextures() {
    runTextures[0] = loadTex("assets/t_rex/run1.png");
    runTextures[1] = loadTex("assets/t_rex/run3.png");
    runTextures[2] = loadTex("assets/t_rex/run2.png");
    
    duckTextures[0] = loadTex("assets/t_rex/duck1.png");
    duckTextures[1] = loadTex("assets/t_rex/duck2.png");
    duckTextures[2] = loadTex("assets/t_rex/duck3.png");
}

Dino::Dino() {
    for (int i = 0; i < 3; i++) {
        runTextures[i] = 0;
        duckTextures[i] = 0;
    }
    init();
}

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void Dino::init() {
    x = 100.0f;
    y = GROUND_Y;
    width = 40.0f;
    height = 40.0f;
    velocity = 0.0f;
    state = RUNNING;
    currentFrame = 0;
    frameCounter = 0;
    isMovingLeft = false;
    isMovingRight = false;
    hasDoubleJumped = false;
    facingLeft = false;

    // Initialize 2D Transformation states
    rotationAngle = 0.0f;
    shearFactor = 0.0f;
    scaleX = 1.0f;
    scaleY = 1.0f;

    // Running frame metadata
    runFrames[0] = { 800.0f, 591.0f, 640.0f };
    runFrames[1] = { 794.0f, 591.0f, 633.0f };
    runFrames[2] = { 818.0f, 605.0f, 657.0f };

    // Ducking frame metadata
    duckFrames[0] = { 673.0f, 512.0f, 477.0f };
    duckFrames[1] = { 855.0f, 393.0f, 481.0f };
    duckFrames[2] = { 825.0f, 410.0f, 578.0f };
}

void Dino::update(float gameSpeed) {
    if (state == DEAD) return;

    if (isMovingLeft) {
        x -= 10.0f;
        if (x < 0.0f) x = 0.0f;
        facingLeft = true;
    }
    if (isMovingRight) {
        x += 10.0f;
        if (x > WINDOW_WIDTH - width) x = WINDOW_WIDTH - width;
        facingLeft = false;
    }

    if (state == JUMPING) {
        velocity -= GRAVITY;
        y += velocity;
        
        // Spin rotation animation: rotate only during double jump
        if (hasDoubleJumped) {
            rotationAngle += 12.0f;
            if (rotationAngle >= 360.0f) {
                rotationAngle -= 360.0f;
            }
        } else {
            rotationAngle = 0.0f;
        }

        if (y <= GROUND_Y) {
            y = GROUND_Y;
            state = RUNNING;
            velocity = 0.0f;
            hasDoubleJumped = false;
            rotationAngle = 0.0f; // Reset rotation when landing
        }
    } else {
        rotationAngle = 0.0f;
    }

    // Ducking scaling: smooth interpolation
    if (state == DUCKING) {
        // Smoothly scale Y down to 0.5f
        if (scaleY > 0.5f) {
            scaleY -= 0.1f;
            if (scaleY < 0.5f) scaleY = 0.5f;
        }
    } else {
        // Smoothly scale Y back to 1.0f
        if (scaleY < 1.0f) {
            scaleY += 0.1f;
            if (scaleY > 1.0f) scaleY = 1.0f;
        }
    }

    // Shearing: tilt forward depending on running speed
    if (state == RUNNING) {
        // Shear factor proportional to gameSpeed (e.g. gameSpeed * 0.02)
        // Interpolate shear factor for smooth tilt changes
        float targetShear = gameSpeed * 0.02f;
        if (shearFactor < targetShear) {
            shearFactor += 0.005f;
            if (shearFactor > targetShear) shearFactor = targetShear;
        } else if (shearFactor > targetShear) {
            shearFactor -= 0.005f;
            if (shearFactor < targetShear) shearFactor = targetShear;
        }
    } else {
        // Return to 0 shear when jumping or ducking
        if (shearFactor > 0.0f) {
            shearFactor -= 0.02f;
            if (shearFactor < 0.0f) shearFactor = 0.0f;
        }
    }

    // Animation logic
    if (state == RUNNING || state == DUCKING) {
        frameCounter++;
        if (frameCounter > 3) { 
            currentFrame = (currentFrame + 1) % 3; 
            frameCounter = 0;
        }
    }
}

void Dino::draw() {
    // Reset color to white so texture renders with its native colors
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    
    GLuint currentTex = runTextures[currentFrame];
    SpriteFrame frame = runFrames[currentFrame];

    float scale = 40.0f / 591.0f; // Scale factor based on standard run height (591 pixels = 40 units)
    float drawWidth = frame.width * scale;
    float drawHeight = frame.height * scale;
    float drawX = x;
    float drawY = y - 1.0f * scale; // Shift down by 1 pixel to offset the bottom 1px padding

    if (facingLeft) {
        // Flipped texture: snout is on the left
        float snoutOffsetFromLeftFlipped = (frame.width - frame.snoutX) * scale;
        drawX = x - snoutOffsetFromLeftFlipped;
    } else {
        // Original texture: snout is on the right
        float snoutOffsetFromLeft = frame.snoutX * scale;
        drawX = (x + width) - snoutOffsetFromLeft;
    }
    
    if (state == DEAD) {
        glColor4f(1.0f, 0.5f, 0.5f, 1.0f); // Tint red when dead
    }
    
    if (currentTex != 0) {
        glBindTexture(GL_TEXTURE_2D, currentTex);
    }
    
    // 4 local corners relative to drawX, drawY:
    // Bottom-Left, Bottom-Right, Top-Right, Top-Left
    float localX[4] = { 0.0f, drawWidth, drawWidth, 0.0f };
    float localY[4] = { 0.0f, 0.0f, drawHeight, drawHeight };
    
    // We will transform these corners manually
    float transX[4], transY[4];
    
    // Convert angle to radians
    float rad = rotationAngle * (float)(M_PI / 180.0f);
    float cosA = cosf(rad);
    float sinA = sinf(rad);
    
    // Pivot points
    // For scaling and shearing: bottom-center of the local quad
    float pivotSX = drawWidth / 2.0f;
    float pivotSY = 0.0f;
    
    // For rotation: center of the scaled & sheared quad
    float pivotRX = (drawWidth / 2.0f) * scaleX;
    float pivotRY = (drawHeight / 2.0f) * scaleY;

    for (int i = 0; i < 4; i++) {
        float px = localX[i];
        float py = localY[i];
        
        // 1. Reflection (Horizontal reflection across local vertical center line)
        if (facingLeft) {
            px = drawWidth - px;
        }
        
        // 2. Scaling (relative to local bottom-center pivot)
        px = pivotSX + (px - pivotSX) * scaleX;
        py = pivotSY + (py - pivotSY) * scaleY;
        
        // 3. Shearing (X-shearing relative to local bottom pivot)
        px = px + shearFactor * (py - pivotSY);
        
        // 4. Rotation (relative to local center pivot rx, ry)
        float rx = px - pivotRX;
        float ry = py - pivotRY;
        float rotX = rx * cosA - ry * sinA;
        float rotY = rx * sinA + ry * cosA;
        px = rotX + pivotRX;
        py = rotY + pivotRY;
        
        // 5. Translation to world coordinates
        transX[i] = drawX + px;
        transY[i] = drawY + py;
    }

    // Draw the manually transformed vertices
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(transX[0], transY[0]);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(transX[1], transY[1]);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(transX[2], transY[2]);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(transX[3], transY[3]);
    glEnd();
    
    glBindTexture(GL_TEXTURE_2D, 0); // unbind
}

void Dino::jump() {
    if (state == RUNNING || state == DUCKING) {
        state = JUMPING;
        velocity = JUMP_STRENGTH;
        height = 40.0f; // Reset height if jumping from duck
        width = 40.0f;  // Reset width
        hasDoubleJumped = false;
    } else if (state == JUMPING && !hasDoubleJumped) {
        velocity = JUMP_STRENGTH;
        hasDoubleJumped = true;
    }
}

void Dino::duck() {
    if (state == RUNNING) {
        state = DUCKING;
        height = 20.0f;
        width = 40.0f;
    } else if (state == JUMPING) {
        velocity -= 5.0f;
    }
}

void Dino::unduck() {
    if (state == DUCKING) {
        state = RUNNING;
        height = 40.0f;
        width = 40.0f;
    }
}

void Dino::die() {
    state = DEAD;
}
