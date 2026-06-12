# Dinosaur Game

A C++ project utilizing the OpenGL Utility Toolkit (GLUT) via FreeGLUT. Currently, this project displays a test harness with spinning 3D shapes (sphere, cone, and torus).

## Prerequisites
- A Windows environment.
- A C++ compiler (the provided scripts are tailored for MinGW `g++`).
- FreeGLUT binaries are included locally within the project.

## How to Build and Run

A PowerShell script is provided to compile and run the application automatically.

1. Open a PowerShell terminal.
2. Navigate to the project directory: `d:\PROJECTS\Dinosaur_Game`
3. Run the build script:
   ```powershell
   .\build_and_run.ps1
   ```

The script will compile `main.cpp` using your local `g++` compiler, automatically link the required OpenGL and local FreeGLUT libraries, and launch `DinosaurGame.exe`.

## Controls
- `+` : Increase the number of slices and stacks in the 3D shapes.
- `-` : Decrease the number of slices and stacks in the 3D shapes.
- `q` or `ESC` : Exit the application.
