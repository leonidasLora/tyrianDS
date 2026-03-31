@echo off
REM ===========================================
REM Script de Instalacion - OpenTyrian DS
REM ===========================================

echo.
echo ============================================
echo   OpenTyrian DS - Instalador de Librerias
echo ============================================
echo.

REM Verificar que estamos en la carpeta correcta
if not exist "devkit\libnds\include\nds.h" (
    echo ERROR: No se encontro la carpeta devkit\libnds
    echo Ejecuta este script desde la carpeta del proyecto
    pause
    exit /b 1
)

echo [1/3] Verificando devkitARM...
echo.

REM Buscar devkitARM en ubicaciones comunes
set DEVKITARM=
if exist "C:\devkitPro\devkitARM" (
    set DEVKITARM=C:\devkitPro\devkitARM
) else if exist "C:\Program Files\devkitPro\devkitARM" (
    set DEVKITARM=C:\Program Files\devkitPro\devkitARM
) else if exist "%USERPROFILE%\devkitPro\devkitARM" (
    set DEVKITARM=%USERPROFILE%\devkitPro\devkitARM
)

if "%DEVKITARM%"=="" (
    echo No se encontro devkitARM instalado.
    echo.
    echo Por favor, descarga e instala devkitARM desde:
    echo https://wii.leseratte10.de/devkitPro/devkitARM/r17/
    echo.
    echo O usa el instalador oficial:
    echo https://github.com/devkitPro/installer/releases
    echo.
    echo Despues de instalar, ejecuta este script de nuevo.
    pause
    exit /b 1
)

echo Encontrado: %DEVKITARM%
echo.

REM Verificar si devkitARM tiene libnds
if not exist "%DEVKITARM%\libnds" (
    echo Creando carpeta libnds...
    mkdir "%DEVKITARM%\libnds\include"
    mkdir "%DEVKITARM%\libnds\lib"
)

echo [2/3] Copiando librerias...
echo.

REM Copiar headers
echo   - Copiando headers...
xcopy /E /Y "devkit\libnds\include\*" "%DEVKITARM%\libnds\include\" >nul 2>&1

REM Copiar librerias
echo   - Copiando librerias...
xcopy /Y "devkit\libnds\lib\*.a" "%DEVKITARM%\libnds\lib\" >nul 2>&1

echo.

echo [3/3] Configurando variables de entorno...
echo.

REM Obtener ruta actual
set SCRIPT_DIR=%~dp0
set SCRIPT_DIR=%SCRIPT_DIR:~0,-1%

echo Para configurar el entorno, agrega estas variables:
echo.
echo   DEVKITARM=%DEVKITARM%
echo   LIBNDS=%%DEVKITARM%%\libnds
echo.
echo En Windows, puedes configurar esto en:
echo   Panel de Control ^> Sistema ^> Configuracion avanzada del sistema
echo   ^> Variables de entorno
echo.
echo O ejecuta estos comandos en CMD antes de compilar:
echo.
echo   set DEVKITARM=%DEVKITARM%
echo   set LIBNDS=%%DEVKITARM%%\libnds
echo   set PATH=%%DEVKITARM%%\bin;%%PATH%%
echo.

REM Verificar compilador
if exist "%DEVKITARM%\bin\arm-none-eabi-gcc.exe" (
    echo Verificando compilador...
    "%DEVKITARM%\bin\arm-none-eabi-gcc" --version | findstr /C:"gcc"
    if errorlevel 1 (
        echo.
        echo ADVERTENCIA: El compilador existe pero no responde correctamente.
    ) else (
        echo.
        echo [OK] Compilador encontrado.
    )
) else (
    echo.
    echo ADVERTENCIA: No se encontro arm-none-eabi-gcc.exe
    echo Verifica que devkitARM este correctamente instalado.
)

REM Verificar librerias copiadas
echo.
echo Verificando librerias...
if exist "%DEVKITARM%\libnds\include\nds.h" (
    echo   [OK] nds.h
) else (
    echo   [ERROR] nds.h no encontrado
)
if exist "%DEVKITARM%\libnds\include\SDL\SDL.h" (
    echo   [OK] SDL.h
) else (
    echo   [ERROR] SDL.h no encontrado
)
if exist "%DEVKITARM%\libnds\lib\libnds9.a" (
    echo   [OK] libnds9.a
) else (
    echo   [ERROR] libnds9.a no encontrado
)
if exist "%DEVKITARM%\libnds\lib\libSDL.a" (
    echo   [OK] libSDL.a
) else (
    echo   [ERROR] libSDL.a no encontrado
)
if exist "%DEVKITARM%\libnds\lib\libfat.a" (
    echo   [OK] libfat.a
) else (
    echo   [ERROR] libfat.a no encontrado
)

echo.
echo ============================================
echo   Instalacion completada
echo ============================================
echo.
echo Para compilar el proyecto:
echo.
echo   1. Abre una terminal CMD
echo   2. Ejecuta:
echo      set DEVKITARM=%DEVKITARM%
echo      set LIBNDS=%%DEVKITARM%%\libnds
echo      set PATH=%%DEVKITARM%%\bin;%%PATH%%
echo   3. Ve a la carpeta del proyecto
echo   4. Ejecuta: make
echo.
pause
