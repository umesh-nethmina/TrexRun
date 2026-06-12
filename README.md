# Dinosaur Game

A 2D clone of the classic Google Chrome Dinosaur Game (T-Rex Runner) built in C++ using the OpenGL Utility Toolkit (GLUT) via FreeGLUT.

## Group Project Structure
The code has been split into multiple files so group members can collaborate without merge conflicts:
- `Constants.h`: Contains game constants (screen size, gravity, jump strength).
- `Dino.h` & `Dino.cpp`: Handles the Dinosaur's physics, jumping, and rendering.
- `Obstacles.h` & `Obstacles.cpp`: Handles the spawning, movement, and rendering of the Cacti.
- `main.cpp`: Manages the main game loop, collision detection, and window setup.

## Prerequisites
- A Windows environment.
- A C++ compiler (the provided scripts are tailored for MinGW `g++`).
- FreeGLUT binaries are included locally within the project, so no downloads are required!

## How to Build and Run

A PowerShell script is provided to compile and run the application automatically.

1. Open a PowerShell terminal.
2. Navigate to the project directory: `d:\PROJECTS\Dinosaur_Game`
3. Run the build script:
   ```powershell
   .\build_and_run.ps1
   ```

The script will compile all `.cpp` files using your local `g++` compiler, automatically link the required OpenGL and local FreeGLUT libraries, and launch `DinosaurGame.exe`.

## Controls
- `SPACE` or `UP ARROW` : Jump.
- `R` : Restart when Game Over.
- `ESC` : Exit the application.
