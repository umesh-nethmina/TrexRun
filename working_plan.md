# 🦖 Dinosaur Endless Runner Game (GLUT + C++)

## 📌 Project Overview
This project is a 2D Endless Runner Game developed using C++ and OpenGL (GLUT).  
It is divided into modular systems handled by different team members, ensuring a scalable and maintainable architecture while integrating fundamental computer graphics algorithms.

---

## 👥 1. Team Scope Division

### 👤 Member 1 — Player System (Movement, Transformations & Animations)
**Responsibilities**
- Dinosaur movement physics (run, jump, duck) and states (RUNNING, JUMPING, DUCKING, DEAD).
- Player input handling (keyboard/special keys) and horizontal bounds restriction.
- Integration of player animations and transformation effects.

**Algorithms & Where Used**
- **2D Transformations**:
  - **Translation**: Dynamic horizontal/vertical positioning of the player ($x, y$ coordinates).
  - **Rotation**: A jump flip animation (rotating $360^\circ$ around the dinosaur's center) when the player jumps.
  - **Scaling**: Shrinking the player's bounding box and model vertically during ducking ($S_y < 1.0$), and scaling up when receiving specific power-ups.
  - **Reflection**: Mirroring the dinosaur horizontally (reflection across the vertical axis) when facing left vs. right.
  - **Shearing**: Applying an X-shear ($Sh_x$ relative to ground) when running at high speeds to create a dynamic forward-tilt motion blur effect.
- **Animations**:
  - Frame-by-frame sprite indexing (`currentFrame = (currentFrame + 1) % totalFrames`).
  - Smooth animation interpolation (using timer functions to transition between rotation/shearing values).

---

### 👤 Member 2 — Obstacles, Collision & Clipping System
**Responsibilities**
- Procedural obstacle generation (cacti, flying obstacles).
- Hitbox calculation and AABB collision detection.
- Debug overlay graphics and bounding box clipping.

**Algorithms & Where Used**
- **Line Drawing (Bresenham / DDA)**:
  - Used to render outline borders for active debug hitboxes around the player and obstacles to visualize collision detection in real time.
- **Line Clipping (Cohen-Sutherland / Liang-Barsky)**:
  - Clipping debug lines and velocity vectors against the viewport/active screen boundaries.
  - Used in the **Graphics Lab Showcase** mode to demonstrate how wireframe shapes or lines are clipped when they cross the bounding screen margins.

---

### 👤 Member 3 — Environment & Graphics System
**Responsibilities**
- Infinite scrolling ground.
- Parallax background rendering (clouds, mountains, stars).
- Sky transitions (day/night cycles).

**Algorithms & Where Used**
- **Line Drawing (Bresenham's Line Algorithm)**:
  - Rendering the scrolling ground line pixel-by-pixel, avoiding standard OpenGL line primitives.
- **Circle Drawing (Mid-Point / Bresenham's Circle Algorithm)**:
  - Drawing the Sun and Moon in the sky. The background renderer uses custom circle rasterization (both filled circles and outline rays).
- **Line Clipping (Cohen-Sutherland)**:
  - Clipping background elements (such as background mountains, clouds, and stars) that scroll off the boundaries of the viewport or sky clipping box.

---

### 👤 Member 4 — UI, Audio & Menu System
**Responsibilities**
- Game menus (Start, Pause, Game Over, Graphics Lab selector).
- Score tracking and file-based high scores.
- Audio playback coordination.

**Algorithms & Where Used**
- **Line Drawing (DDA / Bresenham)**:
  - Drawing custom retro UI frames, menu borders, selection indicators, and button panels.
- **2D Transformations & Animations**:
  - **Scaling**: Creating a pulsing animation on the "GAME OVER" text and start menus (gradually scaling up and down around the center).
  - **Translation**: Slide-in menu transitions from the top or sides when the game is paused.

---

### 👤 Member 5 — Power-ups, Coins & Integration
**Responsibilities**
- Coin spawning and collection mechanics.
- Power-up systems (shield, magnet, speed boost).
- Final integration, test validation, and the interactive Graphics Lab mode.

**Algorithms & Where Used**
- **Circle Drawing (Mid-Point Circle Algorithm)**:
  - **Coins**: Rendering circular coins in the sky using the Midpoint Circle Algorithm (both outlines and filled discs).
  - **Shield**: Drawing a protective energy shield outline around the dinosaur using Bresenham's Circle Algorithm when the shield power-up is active.
- **2D Transformations**:
  - **Rotation**: Spinning the coins continuously about their vertical centers to give a 3D rotation feel.
  - **Scaling**: Pulsing the shield circle radius to warn the player when the power-up duration is about to expire.

---

## ⚙️ 2. Detailed Algorithms Description

### 1️⃣ Line Drawing Algorithms
Used to rasterize line segments between endpoints $(x_0, y_0)$ and $(x_1, y_1)$ pixel-by-pixel:
- **DDA (Digital Differential Analyzer)**: Calculates pixel coordinates by incrementing the coordinate with the larger delta and calculating the step value. Excellent for floating-point calculations.
- **Bresenham's Line Algorithm**: An integer-only line drawing algorithm using a decision parameter ($d$) to decide whether to increment the minor axis at each step. High performance and accurate.
- **Mid-Point Line Algorithm**: Compares the midpoint between two potential pixels against the line equation to choose the closest pixel.

### 2️⃣ Circle Drawing Algorithms
Used to rasterize circles of radius $R$ centered at $(X_c, Y_c)$ using 8-way symmetry:
- **Mid-Point Circle Algorithm**: Uses a decision parameter starting at $d = 1 - R$ to increment along the arc, plotting eight symmetric points simultaneously for each calculated point.
- **Filled Circle Algorithm**: Implemented by drawing horizontal lines connecting the symmetric boundary pixels on opposite sides of the circle.

### 3️⃣ 2D Geometric Transformations
Applied manually on 2D coordinates $[x, y]^T$ using matrix transformations:
- **Translation**: Translates vertices by $(T_x, T_y)$:
  $$x' = x + T_x, \quad y' = y + T_y$$
- **Rotation**: Rotates vertices by angle $\theta$ around a pivot $(X_p, Y_p)$:
  $$x' = X_p + (x - X_p)\cos\theta - (y - Y_p)\sin\theta$$
  $$y' = Y_p + (x - X_p)\sin\theta + (y - Y_p)\cos\theta$$
- **Scaling**: Scales vertices by factors $(S_x, S_y)$ relative to a pivot $(X_p, Y_p)$:
  $$x' = S_x(x - X_p) + X_p, \quad y' = S_y(y - Y_p) + Y_p$$
- **Reflection**: Reflects coordinates horizontally or vertically:
  $$x_{reflected} = 2 \cdot X_{ref} - x \quad \text{(Horizontal reflection across } x = X_{ref}\text{)}$$
- **Shearing**: Tilts coordinates along the X-axis:
  $$x' = x + Sh_x \cdot (y - Y_{ref}), \quad y' = y$$

### 4️⃣ Line Clipping Algorithms
Clips a line segment against a clipping rectangle defined by $[X_{min}, Y_{min}]$ and $[X_{max}, Y_{max}]$:
- **Cohen-Sutherland Line Clipping**: Divides the plane into 9 regions using 4-bit Outcodes (Top, Bottom, Right, Left). Uses bitwise AND/OR operations to perform trivial acceptance, trivial rejection, or intersection calculations.
- **Liang-Barsky Line Clipping**: Uses parametric representation $x = x_1 + t \cdot \Delta x$, $y = y_1 + t \cdot \Delta y$ and solves the inequalities $p_i \cdot t \le q_i$ to find the intersection parameters $t_1, t_2$.

### 5️⃣ Dynamic Animations
- **Frame-by-frame Sprite Animation**: Cycling through coordinate-based textures using dynamic updates.
- **Transformation-based animations**: Continuous updates to rotation angles, shear angles, and scale factors inside a time-based update loop (`glutTimerFunc`).

---

## 🔗 3. GLUT Integration Mapping
| System | GLUT Function |
|--------|---------------|
| Rendering | `glutDisplayFunc` |
| Game loop / Logic update | `glutTimerFunc` |
| Text Input & Mode Swap | `glutKeyboardFunc` |
| Special Key Controls (Jump/Duck) | `glutSpecialFunc` / `glutSpecialUpFunc` |
| Double Buffering Swap | `glutSwapBuffers` |

---

## 🔄 4. Game Execution Flow
1. **Initialize GLUT Window**: Create screen and set projection viewport.
2. **Setup OpenGL States**: Set background color, projection matrices, alpha blending.
3. **Register Callbacks**: Register display, keyboard, special keys, and update loops.
4. **Update Loop (`update`)**:
   - Update player position, gravity, jumping state, and animations.
   - Update obstacle positions and check AABB collisions.
   - Update background parallax values.
   - If in **Lab Mode**: Update current transformation angles, line clipping inputs, and active interactive shapes.
5. **Display Render (`display`)**:
   - Draw environment (ground, sun/moon, background mountains).
   - Draw active game entities (Dino, cacti, coins).
   - Draw menus (Main Menu, HUD, Pause screen, Game Over screen, Lab Showcase).
6. **Swap Buffers**: Render the frame using double-buffered graphics.

---

## 🧪 5. Testing & Integration
**Integration Tests**
- Verify that custom line drawing maps exactly to GLUT-based line coordinates.
- Ensure 2D matrix transformation functions properly tilt, scale, reflect, and rotate vertices around arbitrary pivot points.
- Verify that Cohen-Sutherland and Liang-Barsky algorithms correctly resolve the identical clipped lines.
- Test collision states using wireframe debugging boxes drawn with custom rasterization.
