#include "Obstacles.h"
#include "Constants.h"
#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Drawing helper functions
static void drawSolidCircle(float cx, float cy, float r) {
  glBegin(GL_TRIANGLE_FAN);
  glVertex2f(cx, cy);
  int numSegments = 12;
  for (int i = 0; i <= numSegments; i++) {
    float theta = 2.0f * (float)M_PI * float(i) / float(numSegments);
    glVertex2f(cx + r * cosf(theta), cy + r * sinf(theta));
  }
  glEnd();
}

static void drawSolidDiamond(float cx, float cy, float w, float h) {
  glBegin(GL_QUADS);
  glVertex2f(cx, cy + h / 2.0f);
  glVertex2f(cx + w / 2.0f, cy);
  glVertex2f(cx, cy - h / 2.0f);
  glVertex2f(cx - w / 2.0f, cy);
  glEnd();
}

Cactus::Cactus() {
  x = 0;
  y = GROUND_Y;
  width = 30.0f;
  height = 50.0f;
  active = false;
  vx = 0.0f;
  vy = 0.0f;
  isStuck = false;
  type = 0;
  rotation = 0.0f;
  rotSpeed = 0.0f;
  trailCount = 0;
  for (int i = 0; i < 8; i++) {
    trailX[i] = 0.0f;
    trailY[i] = 0.0f;
  }
}

ObstacleManager::ObstacleManager() { init(); }

void ObstacleManager::init() {
  for (int i = 0; i < MAX_CACTI; i++) {
    cacti[i].active = false;
  }
  spawnCactus();
}

void ObstacleManager::spawnCactus() {
  for (int i = 0; i < MAX_CACTI; i++) {
    if (!cacti[i].active) {
      // Spawn meteors high up in the sky, off-screen to the right
      cacti[i].x = WINDOW_WIDTH + (rand() % 200);
      cacti[i].y = WINDOW_HEIGHT - 60.0f + (rand() % 40);
      
      // Random width and height (dimensions of the meteor)
      cacti[i].width = 30.0f + (rand() % 15);
      cacti[i].height = cacti[i].width; // Keep them mostly square-ish initially
      
      // Horizontal speed is faster than the gameSpeed to make it fly in
      // Vertical speed is negative to fall down
      cacti[i].vx = -7.0f - (rand() % 4);
      cacti[i].vy = -3.0f - (rand() % 3);
      
      cacti[i].isStuck = false;
      cacti[i].type = rand() % 3; // 3 distinct styles
      cacti[i].rotation = 0.0f;
      cacti[i].rotSpeed = (float)((rand() % 200) - 100) / 10.0f; // Spin speed
      
      // Initialize trail position
      cacti[i].trailCount = 0;
      float startCX = cacti[i].x + cacti[i].width / 2.0f;
      float startCY = cacti[i].y + cacti[i].height / 2.0f;
      for (int j = 0; j < 8; j++) {
        cacti[i].trailX[j] = startCX;
        cacti[i].trailY[j] = startCY;
      }
      
      cacti[i].active = true;
      break;
    }
  }
}

void ObstacleManager::update(float gameSpeed) {
  for (int i = 0; i < MAX_CACTI; i++) {
    if (cacti[i].active) {
      if (!cacti[i].isStuck) {
        // Falling meteor update
        // We add some extra vx depending on speed
        float speedScale = gameSpeed / 5.0f;
        cacti[i].x += cacti[i].vx * speedScale;
        cacti[i].y += cacti[i].vy * speedScale;
        cacti[i].rotation += cacti[i].rotSpeed;

        // Shift and update trail
        for (int j = 7; j > 0; j--) {
          cacti[i].trailX[j] = cacti[i].trailX[j - 1];
          cacti[i].trailY[j] = cacti[i].trailY[j - 1];
        }
        cacti[i].trailX[0] = cacti[i].x + cacti[i].width / 2.0f;
        cacti[i].trailY[0] = cacti[i].y + cacti[i].height / 2.0f;
        if (cacti[i].trailCount < 8) cacti[i].trailCount++;

        // Check ground hit
        if (cacti[i].y <= GROUND_Y) {
          cacti[i].y = GROUND_Y;
          cacti[i].isStuck = true;
          cacti[i].vx = -gameSpeed;
          cacti[i].vy = 0.0f;
          cacti[i].rotation = 0.0f;
          
          // Re-initialize trail for smoke rising effect
          float startCX = cacti[i].x + cacti[i].width / 2.0f;
          float startCY = cacti[i].y + cacti[i].height;
          for (int j = 0; j < 8; j++) {
            cacti[i].trailX[j] = startCX;
            cacti[i].trailY[j] = startCY;
          }
        }
      } else {
        // Stuck on ground update: moves with the ground speed
        cacti[i].x -= gameSpeed;

        // Scroll and drift the smoke trail upwards
        for (int j = 0; j < 8; j++) {
          cacti[i].trailX[j] -= gameSpeed;
          cacti[i].trailY[j] += 0.8f; // Smoke drifts up
          cacti[i].trailX[j] += (rand() % 10 - 5) * 0.15f; // Small horizontal drift
        }

        // Spawn new smoke/spark particles at the top of the obstacle
        if (rand() % 10 < 4) {
          for (int j = 7; j > 0; j--) {
            cacti[i].trailX[j] = cacti[i].trailX[j - 1];
            cacti[i].trailY[j] = cacti[i].trailY[j - 1];
          }
          cacti[i].trailX[0] = cacti[i].x + cacti[i].width / 2.0f;
          cacti[i].trailY[0] = cacti[i].y + cacti[i].height;
        }
      }

      // Check off-screen boundary
      if (cacti[i].x + cacti[i].width < -50.0f) {
        cacti[i].active = false;
      }
    }
  }

  // Spawning frequency logic
  if (rand() % 100 < 3) {
    bool canSpawn = true;
    for (int i = 0; i < MAX_CACTI; i++) {
      if (cacti[i].active && cacti[i].x > WINDOW_WIDTH - 280) {
        canSpawn = false;
        break;
      }
    }
    if (canSpawn) {
      spawnCactus();
    }
  }
}

void ObstacleManager::draw() {
  for (int i = 0; i < MAX_CACTI; i++) {
    if (cacti[i].active) {
      float cx = cacti[i].x + cacti[i].width / 2.0f;
      float cy = cacti[i].y + cacti[i].height / 2.0f;
      float rad = cacti[i].rotation * (float)M_PI / 180.0f;
      float r = cacti[i].width / 2.0f;

      if (cacti[i].type == 0) {
        // === TYPE 0: FIRE METEOR ===
        if (!cacti[i].isStuck) {
          // Draw fire trail
          for (int j = cacti[i].trailCount - 1; j >= 0; j--) {
            float t = (float)j / 8.0f;
            float size = cacti[i].width * (0.8f - t * 0.6f);
            float alpha = 1.0f - t;
            glColor4f(1.0f, 0.2f + 0.8f * (1.0f - t), 0.0f, alpha * 0.6f);
            drawSolidCircle(cacti[i].trailX[j], cacti[i].trailY[j], size / 2.0f);
          }

          // Draw outer fire aura
          glColor4f(1.0f, 0.5f, 0.0f, 0.3f);
          drawSolidCircle(cx, cy, r * 1.3f);

          // Draw spinning rocky body
          glBegin(GL_POLYGON);
          glColor3f(0.28f, 0.24f, 0.24f); // dark brown-gray rock
          for (int v = 0; v < 8; v++) {
            float angle = (v * 45.0f) * (float)M_PI / 180.0f + rad;
            float jaggedR = r * (0.85f + 0.15f * sinf(v * 2.3f));
            glVertex2f(cx + jaggedR * cosf(angle), cy + jaggedR * sinf(angle));
          }
          glEnd();

          // Draw fiery glowing cracks in the rock
          glBegin(GL_LINES);
          glColor3f(1.0f, 0.6f, 0.0f);
          for (int v = 0; v < 4; v++) {
            float angle1 = (v * 90.0f) * (float)M_PI / 180.0f + rad;
            float angle2 = ((v * 90.0f) + 120.0f) * (float)M_PI / 180.0f + rad;
            glVertex2f(cx + r * 0.1f * cosf(angle1), cy + r * 0.1f * sinf(angle1));
            glVertex2f(cx + r * 0.6f * cosf(angle2), cy + r * 0.6f * sinf(angle2));
          }
          glEnd();
        } else {
          // Stuck Fire Meteor on the ground
          // Draw smoke particles rising
          for (int j = 0; j < cacti[i].trailCount; j++) {
            float alpha = 1.0f - (float)j / 8.0f;
            float size = 4.0f * (1.0f - (float)j / 8.0f);
            glColor4f(0.3f, 0.3f, 0.3f, alpha * 0.5f); // Gray smoke
            drawSolidCircle(cacti[i].trailX[j], cacti[i].trailY[j], size);
          }

          // Draw cooling rock sitting on ground
          float by = cacti[i].y;
          float h = cacti[i].height;
          glBegin(GL_POLYGON);
          glColor3f(0.2f, 0.18f, 0.18f); // Even darker rock
          glVertex2f(cx - r * 0.8f, by);                   // Bottom-left
          glVertex2f(cx - r * 1.0f, by + h * 0.3f);        // Mid-left
          glVertex2f(cx - r * 0.7f, by + h * 0.7f);        // Upper-mid-left
          glVertex2f(cx - r * 0.1f, by + h * 1.0f);        // Top peak
          glVertex2f(cx + r * 0.6f, by + h * 0.8f);        // Upper-mid-right
          glVertex2f(cx + r * 1.0f, by + h * 0.4f);        // Mid-right
          glVertex2f(cx + r * 0.9f, by + h * 0.15f);       // Lower-mid-right
          glVertex2f(cx + r * 0.8f, by);                   // Bottom-right
          glEnd();

          // Draw faint orange/red ember cracks relative to bottom of rock
          glBegin(GL_LINES);
          glColor3f(0.8f, 0.3f, 0.0f);
          // Crack 1
          glVertex2f(cx - r * 0.2f, by + h * 0.1f);
          glVertex2f(cx - r * 0.4f, by + h * 0.3f);
          // Crack 2
          glVertex2f(cx + r * 0.1f, by + h * 0.2f);
          glVertex2f(cx + r * 0.3f, by + h * 0.4f);
          // Crack 3
          glVertex2f(cx - r * 0.1f, by + h * 0.5f);
          glVertex2f(cx + r * 0.2f, by + h * 0.6f);
          glEnd();
        }
      }
      else if (cacti[i].type == 1) {
        // === TYPE 1: PURPLE ALIEN CRYSTAL ===
        if (!cacti[i].isStuck) {
          // Draw neon purple trail
          for (int j = cacti[i].trailCount - 1; j >= 0; j--) {
            float t = (float)j / 8.0f;
            float size = cacti[i].width * (0.7f - t * 0.5f);
            float alpha = 1.0f - t;
            glColor4f(0.8f, 0.0f, 1.0f, alpha * 0.5f);
            drawSolidDiamond(cacti[i].trailX[j], cacti[i].trailY[j], size, size * 1.3f);
          }

          // Draw spinning crystal core
          glBegin(GL_QUADS);
          glColor3f(0.5f, 0.0f, 0.8f); // Purple
          float w = cacti[i].width / 2.0f;
          float h = cacti[i].height / 2.0f;
          float ptsX[4] = { 0.0f, w, 0.0f, -w };
          float ptsY[4] = { h, 0.0f, -h, 0.0f };
          for (int v = 0; v < 4; v++) {
            float rx = ptsX[v] * cosf(rad) - ptsY[v] * sinf(rad);
            float ry = ptsX[v] * sinf(rad) + ptsY[v] * cosf(rad);
            glVertex2f(cx + rx, cy + ry);
          }
          glEnd();

          // Draw inner bright highlight core
          glBegin(GL_QUADS);
          glColor3f(1.0f, 0.4f, 1.0f); // Magenta glow
          for (int v = 0; v < 4; v++) {
            float rx = (ptsX[v] * 0.4f) * cosf(rad) - (ptsY[v] * 0.4f) * sinf(rad);
            float ry = (ptsX[v] * 0.4f) * sinf(rad) + (ptsY[v] * 0.4f) * cosf(rad);
            glVertex2f(cx + rx, cy + ry);
          }
          glEnd();
        } else {
          // Stuck Crystal Spike on the ground
          // Draw purple floating sparkles
          for (int j = 0; j < cacti[i].trailCount; j++) {
            float alpha = 1.0f - (float)j / 8.0f;
            glColor4f(0.9f, 0.2f, 1.0f, alpha * 0.7f);
            glPointSize(3.0f);
            glBegin(GL_POINTS);
            glVertex2f(cacti[i].trailX[j], cacti[i].trailY[j]);
            glEnd();
          }

          // Draw crystal spike standing on ground
          float bx = cacti[i].x + cacti[i].width / 2.0f;
          float by = cacti[i].y; // GROUND_Y
          float ch = cacti[i].height;
          float cw = cacti[i].width;

          glBegin(GL_TRIANGLES);
          glColor3f(0.4f, 0.0f, 0.7f);
          glVertex2f(cacti[i].x, by);
          glVertex2f(bx, by + ch);
          glVertex2f(bx, by);

          glColor3f(0.6f, 0.1f, 0.8f);
          glVertex2f(bx, by);
          glVertex2f(bx, by + ch);
          glVertex2f(cacti[i].x + cw, by);
          glEnd();

          glBegin(GL_TRIANGLES);
          glColor3f(0.9f, 0.4f, 1.0f);
          glVertex2f(bx - cw * 0.12f, by);
          glVertex2f(bx, by + ch);
          glVertex2f(bx + cw * 0.12f, by);
          glEnd();
        }
      }
      else {
        // === TYPE 2: ICE METEOR / BLUE COMET ===
        if (!cacti[i].isStuck) {
          // Draw frosty cyan trail
          for (int j = cacti[i].trailCount - 1; j >= 0; j--) {
            float t = (float)j / 8.0f;
            float size = cacti[i].width * (0.8f - t * 0.6f);
            float alpha = 1.0f - t;
            glColor4f(0.0f, 0.7f, 1.0f, alpha * 0.5f);
            drawSolidCircle(cacti[i].trailX[j], cacti[i].trailY[j], size / 2.0f);
          }

          // Draw icy core body (spinning hexagon)
          glBegin(GL_POLYGON);
          glColor3f(0.1f, 0.5f, 0.8f); // Ice blue
          for (int v = 0; v < 6; v++) {
            float angle = (v * 60.0f) * (float)M_PI / 180.0f + rad;
            float R = r * (0.9f + 0.1f * sinf(v * 3.1f));
            glVertex2f(cx + R * cosf(angle), cy + R * sinf(angle));
          }
          glEnd();

          // Draw bright inner ice structure
          glBegin(GL_POLYGON);
          glColor3f(0.7f, 0.95f, 1.0f); // Bright frost
          for (int v = 0; v < 6; v++) {
            float angle = (v * 60.0f) * (float)M_PI / 180.0f + rad;
            glVertex2f(cx + r * 0.5f * cosf(angle), cy + r * 0.5f * sinf(angle));
          }
          glEnd();
        } else {
          // Stuck Ice Shards on the ground
          // Draw light blue/cyan condensation particles
          for (int j = 0; j < cacti[i].trailCount; j++) {
            float alpha = 1.0f - (float)j / 8.0f;
            glColor4f(0.6f, 0.85f, 1.0f, alpha * 0.5f);
            drawSolidCircle(cacti[i].trailX[j], cacti[i].trailY[j], 1.5f);
          }

          // Draw overlapping ice shards
          float bx = cacti[i].x + cacti[i].width / 2.0f;
          float by = cacti[i].y; // GROUND_Y
          float ch = cacti[i].height;
          float cw = cacti[i].width;

          glBegin(GL_TRIANGLES);
          glColor3f(0.2f, 0.6f, 0.9f);
          glVertex2f(cacti[i].x + cw * 0.2f, by);
          glVertex2f(bx, by + ch);
          glVertex2f(cacti[i].x + cw * 0.8f, by);
          
          glColor3f(0.1f, 0.5f, 0.8f);
          glVertex2f(cacti[i].x, by);
          glVertex2f(cacti[i].x + cw * 0.35f, by + ch * 0.6f);
          glVertex2f(bx, by);

          glColor3f(0.3f, 0.7f, 1.0f);
          glVertex2f(bx, by);
          glVertex2f(cacti[i].x + cw * 0.65f, by + ch * 0.5f);
          glVertex2f(cacti[i].x + cw, by);
          glEnd();

          glBegin(GL_TRIANGLES);
          glColor3f(0.85f, 0.95f, 1.0f);
          glVertex2f(bx - cw * 0.06f, by);
          glVertex2f(bx, by + ch);
          glVertex2f(bx + cw * 0.06f, by);
          glEnd();
        }
      }
    }
  }
}
