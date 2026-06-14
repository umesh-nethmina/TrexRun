# Ensure the 64-bit MSYS2 compiler is prioritized in the PATH
$env:PATH = "C:\msys64\ucrt64\bin;" + $env:PATH


# Copy the 64-bit freeglut.dll to the project directory so it runs correctly
Copy-Item ".\freeglut\freeglut\bin\x64\libfreeglut.dll" -Destination ".\libfreeglut.dll" -Force

# Compile source files
# -I points to the include directory
# -L points to the library directory (using 64-bit libs)
# -lfreeglut, -lglu32, -lopengl32 are the required libraries
Write-Host "Compiling source files..."
g++ src\main.cpp src\Dino.cpp src\Obstacles.cpp src\UIManager.cpp src\Environment.cpp -o DinosaurGame.exe -I.\include -I.\freeglut\freeglut\include -L.\freeglut\freeglut\lib\x64 -lfreeglut -lglu32 -lopengl32

if ($LASTEXITCODE -eq 0) {
    Write-Host "Compilation successful! Running DinosaurGame.exe..."
    .\DinosaurGame.exe
} else {
    Write-Host "Build failed."
}
