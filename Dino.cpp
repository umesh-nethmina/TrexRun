#include "Dino.h"
#include "Constants.h"
#include <GL/glut.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Dino::Dino() {
    texture = 0;
    currentFrame = 0;
    frameTimer = 0;
    init();
}

void Dino::loadTextures() {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    // Load the spritesheet containing 2 frames
    unsigned char *data = stbi_load("assets/trex_spritesheet.png", &width, &height, &nrChannels, STBI_rgb_alpha);
    if (data) {
        // Chroma keying: remove white background
        for (int p = 0; p < width * height * 4; p += 4) {
            if (data[p] > 240 && data[p+1] > 240 && data[p+2] > 240) {
                data[p+3] = 0;
            }
        }

        // Bounding box logic
        int minX = width, minY = height, maxX = 0, maxY = 0;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int p = (y * width + x) * 4;
                if (data[p+3] > 0) {
                    if (x < minX) minX = x;
                    if (x > maxX) maxX = x;
                    if (y < minY) minY = y;
                    if (y > maxY) maxY = y;
                }
            }
        }

        int cropWidth = maxX - minX + 1;
        int cropHeight = maxY - minY + 1;
        
        unsigned char* croppedData = new unsigned char[cropWidth * cropHeight * 4];
        for (int cy = 0; cy < cropHeight; cy++) {
            for (int cx = 0; cx < cropWidth; cx++) {
                int srcX = minX + cx;
                int srcY = minY + cy;
                int srcP = (srcY * width + srcX) * 4;
                int destP = (cy * cropWidth + cx) * 4;
                croppedData[destP] = data[srcP];
                croppedData[destP+1] = data[srcP+1];
                croppedData[destP+2] = data[srcP+2];
                croppedData[destP+3] = data[srcP+3];
            }
        }

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cropWidth, cropHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, croppedData);
        
        delete[] croppedData;
        stbi_image_free(data);
    } else {
        printf("Failed to load texture assets/trex_spritesheet.png\n");
    }
}

void Dino::init() {
    x = 100.0f;
    y = GROUND_Y;
    width = 60.0f;
    height = 60.0f;
    velocity = 0.0f;
    isJumping = false;
    currentFrame = 0;
    frameTimer = 0;
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
        currentFrame = 0; // Freeze animation while jumping
    } else {
        // Animate legs when running
        frameTimer++;
        if (frameTimer >= 6) { // Swap every 6 frames (adjust speed here)
            currentFrame = (currentFrame == 0) ? 1 : 0;
            frameTimer = 0;
        }
    }
}

void Dino::draw() {
    glColor3f(1.0f, 1.0f, 1.0f);
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glBegin(GL_QUADS);
    
    // Texture coordinates map to different halves of the spritesheet depending on the frame
    // No longer flipping horizontally, assuming the AI image already faces right
    float tx1, tx2;
    if (currentFrame == 0) {
        // Frame 1 is the left half of the image
        tx1 = 0.0f; tx2 = 0.5f; 
    } else {
        // Frame 2 is the right half of the image
        tx1 = 0.5f; tx2 = 1.0f; 
    }
    
    glTexCoord2f(tx1, 0.0f); glVertex2f(x, y);
    glTexCoord2f(tx2, 0.0f); glVertex2f(x + width, y);
    glTexCoord2f(tx2, 1.0f); glVertex2f(x + width, y + height);
    glTexCoord2f(tx1, 1.0f); glVertex2f(x, y + height);
    
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void Dino::jump() {
    if (!isJumping) {
        isJumping = true;
        velocity = JUMP_STRENGTH;
    }
}
