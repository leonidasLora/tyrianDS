@echo off
REM ===========================================
REM Script de Compilacion - OpenTyrian DS
REM ===========================================

echo.
echo ============================================
echo   OpenTyrian DS - Compilador
echo ============================================
echo.

REM Configurar entorno
set DEVKITPRO=C:\tyrianDS\devkit\devkitARM
set DEVKITARM=%DEVKITPRO%
set LIBNDS=%DEVKITARM%\libnds
set PATH=%DEVKITARM%\bin;%PATH%

echo Entorno configurado:
echo   DEVKITARM=%DEVKITARM%
echo   LIBNDS=%LIBNDS%
echo.

REM Ir al directorio del proyecto
cd /d C:\tyrianDS

REM Limpiar
echo [1/3] Limpiando...
make clean
if errorlevel 1 (
    echo ERROR en limpieza
    pause
    exit /b 1
)
echo.

REM Compilar
echo [2/3] Compilando...
make
if errorlevel 1 (
    echo ERROR en compilacion
    pause
    exit /b 1
)
echo.

REM Verificar resultado
echo [3/3] Verificando...
if exist "tyrianDS.nds" (
    echo.
    echo ============================================
    echo   COMPILACION EXITOSA
    echo ============================================
    echo.
    echo Archivos generados:
    dir tyrianDS.* /b
    echo.
    echo Tamano del ROM: 
    for %%A in (tyrianDS.nds) do echo   %%~zA bytes
    echo.
    echo IMPORTANTE: Copia la carpeta 'tyrian/' con los datos del juego
    echo             a la raiz de tu tarjeta SD junto con tyrianDS.nds
) else (
    echo ERROR: No se genero tyrianDS.nds
)

echo.
pause
