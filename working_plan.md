# 🦖 Dinosaur Endless Runner Game (GLUT + C++)

## 📌 Project Overview
This project is a 2D Endless Runner Game developed using C++ and OpenGL (GLUT).  
It is divided into modular systems handled by different team members, ensuring scalable and maintainable architecture.

---

## 👥 1. Team Scope Division

### 👤 Member 1 — Player System
**Responsibilities**
- Dinosaur movement (run, jump, duck)
- Animation system
- Input handling
- Player state management

**Modules**
- Player Controller
- Animation Manager
- Input Handler
- State Machine

---

### 👤 Member 2 — Obstacles & Collision System
**Responsibilities**
- Obstacle generation (cactus, bird)
- Collision detection
- Difficulty scaling

**Modules**
- Obstacle Manager
- Spawn System
- Collision Engine
- Object Pooling System

---

### 👤 Member 3 — Environment & Graphics System
**Responsibilities**
- Infinite scrolling ground
- Parallax background
- Sky transitions (day/night)
- Weather effects (optional)

**Modules**
- Environment Renderer
- Background System
- Lighting System

---

### 👤 Member 4 — UI & Audio System
**Responsibilities**
- Game menus (Start, Pause, Game Over)
- Score system
- Audio effects
- Save/Load system

**Modules**
- UI Manager
- Score Manager
- Audio Controller
- File Manager

---

### 👤 Member 5 — Power-ups, Coins & Integration
**Responsibilities**
- Coins system
- Power-ups (shield, magnet, boost)
- Statistics tracking
- Final integration testing

**Modules**
- Power-up System
- Coin Manager
- Stats Tracker
- Integration Tester

---

## ⚙️ 2. Algorithms Used

### 🎮 Player System
**Gravity Simulation**
```cpp
velocityY += gravity;
playerY += velocityY;
```

**Finite State Machine (FSM)**
States:
- RUNNING
- JUMPING
- DUCKING
- DEAD

**Sprite Animation**
```cpp
currentFrame = (currentFrame + 1) % totalFrames;
```

---

### 🚧 Obstacles & Collision System
**AABB Collision Detection**
```cpp
if (player.intersects(obstacle)) {
    gameOver();
}
```

**Random Spawn Algorithm**
```cpp
spawnDistance = random(200, 500);
```

**Object Pooling**
Reuse objects instead of creating new ones.

---

### 🌄 Environment System
**Infinite Scrolling**
```cpp
groundX -= speed;
if (groundX < -width) groundX = 0;
```

**Parallax Scrolling**
```cpp
cloudSpeed = speed * 0.2;
mountainSpeed = speed * 0.5;
```

**LERP (Day/Night Transition)**
```cpp
color = lerp(dayColor, nightColor, t);
```

---

### 🧩 UI & Game Logic
**Score System**
```cpp
score += deltaTime * multiplier;
```

**Save/Load System**
- File-based high score storage

**Event Handling**
- Keyboard input
- Menu navigation
- Button clicks

---

### 💎 Power-ups & Coins System
**Circle Collision**
```cpp
(x1 - x2)^2 + (y1 - y2)^2 < r^2
```

**Power-up FSM**
- NORMAL
- SHIELD
- BOOST
- MAGNET

**Object Pooling**
Used for:
- Coins
- Particles
- Effects

---

## 🔗 3. GLUT Integration Mapping
| System | GLUT Function |
|--------|---------------|
| Rendering | `glutDisplayFunc` |
| Game loop | `glutIdleFunc` |
| Input | `glutKeyboardFunc` |
| Animation timing | `glutTimerFunc` |
| Screen update | `glutSwapBuffers` |

---

## 🔄 4. Game Execution Flow
1. Initialize GLUT window
2. Load assets (textures, audio)
3. Start game loop (`glutIdleFunc`)
4. Update:
   - Player physics
   - Obstacles movement
   - Collision detection
   - Score system
5. Render frame (`glutDisplayFunc`)
6. Handle input events
7. Repeat loop continuously

---

## 🧪 5. Testing & Integration
**Integration Tests**
- Player + Obstacles collision
- Player + Coins collection
- Power-up activation
- Score update validation
- UI + Audio synchronization

---

## 🚀 6. Key Features Summary
- Endless running system
- Physics-based movement
- Smooth animations
- Procedural obstacle generation
- Parallax environment
- Power-up mechanics
- Modular architecture (team-based design)

---

## 📌 Conclusion
This project demonstrates core computer graphics and game development principles using GLUT, including:
- Physics simulation
- State machines
- Rendering pipelines
- Real-time interaction systems
