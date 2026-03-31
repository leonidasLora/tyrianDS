@echo off
set DEVKITARM=C:\Users\Leonidas\Documents\mis proyectos\tyrianDS\devkit\devkitARM
set LIBNDS=%DEVKITARM%\libnds
set PATH=%DEVKITARM%\bin;%PATH%

cd /d "C:\Users\Leonidas\Documents\mis proyectos\tyrianDS"

echo Limpiando...
make clean

echo Compilando...
make

echo.
echo Listo. Si hubo errores, apareceran arriba.
pause
