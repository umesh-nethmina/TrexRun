# Copy the 32-bit freeglut.dll to the project directory so it runs correctly
if (-not (Test-Path "libfreeglut.dll")) {
    Copy-Item ".\freeglut\freeglut\bin\libfreeglut.dll" -Destination ".\libfreeglut.dll"
}

# Compile source files
# -I points to the include directory
# -L points to the library directory (using 32-bit libs since MinGW is 32-bit)
# -lfreeglut, -lglu32, -lopengl32 are the required libraries
Write-Host "Compiling source files..."
g++ src/main.cpp src/Dino.cpp src/Obstacles.cpp src/UIManager.cpp src/Environment.cpp -o DinosaurGame.exe -I.\freeglut\freeglut\include -I.\include -L.\freeglut\freeglut\lib -lfreeglut -lglu32 -lopengl32

if ($LASTEXITCODE -eq 0) {
    Write-Host "Compilation successful! Running DinosaurGame.exe..."
    .\DinosaurGame.exe
} else {
    Write-Host "Build failed."
}
