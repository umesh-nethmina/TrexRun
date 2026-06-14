# Ensure the 64-bit MSYS2 compiler is prioritized in the PATH (if it exists)
if (Test-Path "C:\msys64\ucrt64\bin") {
    $env:PATH = "C:\msys64\ucrt64\bin;" + $env:PATH
}

# Verify g++ is available
if (-not (Get-Command g++ -ErrorAction SilentlyContinue)) {
    Write-Error "g++ compiler not found in PATH. Please install MinGW or MSYS2."
    Exit 1
}

# Determine compiler target architecture
$target = (g++ -dumpmachine).Trim()
Write-Host "Detected compiler target architecture: $target"

# Resolve absolute paths to handle spaces in folder names correctly
$includeDir1 = (Resolve-Path ".\include").Path
$includeDir2 = (Resolve-Path ".\freeglut\freeglut\include").Path

if ($target -like "*x86_64*") {
    Write-Host "Configuring for 64-bit build..."
    $dllPath = (Resolve-Path ".\freeglut\freeglut\bin\x64\libfreeglut.dll").Path
    $libDir = (Resolve-Path ".\freeglut\freeglut\lib\x64").Path
} else {
    Write-Host "Configuring for 32-bit build..."
    $dllPath = (Resolve-Path ".\freeglut\freeglut\bin\libfreeglut.dll").Path
    $libDir = (Resolve-Path ".\freeglut\freeglut\lib").Path
}

# Copy the appropriate freeglut.dll to the project directory so it runs correctly
Copy-Item $dllPath -Destination ".\libfreeglut.dll" -Force

# Compile source files using dynamic library paths and absolute includes
Write-Host "Compiling source files..."
g++ src\main.cpp src\Dino.cpp src\Obstacles.cpp src\UIManager.cpp src\Environment.cpp `
    -o DinosaurGame.exe `
    "-I$includeDir1" `
    "-I$includeDir2" `
    "-L$libDir" `
    -lfreeglut -lglu32 -lopengl32

if ($LASTEXITCODE -eq 0) {
    Write-Host "Compilation successful! Running DinosaurGame.exe..."
    .\DinosaurGame.exe
} else {
    Write-Host "Build failed."
}
