# Configuracion del Entorno - OpenTyrian DS
# Copia este archivo y ajustalo a tu ruta de devkitARM

# ===========================================
# INSTRUCCIONES DE INSTALACION
# ===========================================

# 1. DESCARGAR DEVKITARM
#    Descarga devkitARM r17 desde:
#    https://wii.leseratte10.de/devkitPro/devkitARM/r17/
#    (busca la version para tu SO)
#
#    Instala en: C:\devkitPro\devkitARM  (Windows)
#    O en: /opt/devkitpro/devkitARM      (Linux)

# 2. COPIAR LIBRERIAS
#    Este proyecto ya incluye las librerias compatibles en la carpeta "devkit/"
#    Copia el contenido a tu devkitARM:

#    En Windows (ejecuta como administrador):
#    xcopy /E /Y devkit\libnds\* C:\devkitPro\devkitARM\libnds\

#    En Linux:
#    sudo cp -r devkit/libnds/* /opt/devkitpro/devkitARM/libnds/

# 3. CONFIGURAR VARIABLES DE ENTORNO

# ===========================================
# VARIABLES DE ENTORNO (ajusta segun tu instalacion)
# ===========================================

# WINDOWS - Agrega esto en Variables de Entorno del Sistema
# DEVKITARM=C:\devkitPro\devkitARM
# LIBNDS=%DEVKITARM%\libnds
# PATH=%DEVKITARM%\bin;%PATH%

# LINUX/MAC - Agrega esto en tu ~/.bashrc o ~/.zshrc
# export DEVKITARM=/opt/devkitpro/devkitARM
# export LIBNDS=${DEVKITARM}/libnds
# export PATH=${DEVKITARM}/bin:${PATH}

# ===========================================
# CONTENIDO DE LA CARPETA devkit/
# ===========================================

# devkit/
# └── libnds/
#     ├── include/
#     │   ├── nds.h              (header principal)
#     │   ├── nds/              (headers de nds)
#     │   │   ├── arm7/
#     │   │   ├── arm9/
#     │   │   ├── bios.h
#     │   │   ├── card.h
#     │   │   ├── debug.h
#     │   │   ├── disc_io.h
#     │   │   ├── dma.h
#     │   │   ├── fifocommon.h
#     │   │   ├── input.h
#     │   │   ├── interrupts.h
#     │   │   ├── ipc.h
#     │   │   ├── memory.h
#     │   │   ├── system.h
#     │   │   ├── timers.h
#     │   │   └── touch.h
#     │   ├── fat.h              (soporte FAT)
#     │   ├── SDL/               (SDL para NDS)
#     │   │   ├── SDL.h
#     │   │   ├── SDL_video.h
#     │   │   ├── SDL_events.h
#     │   │   ├── SDL_audio.h
#     │   │   ├── SDL_joystick.h
#     │   │   ├── SDL_keyboard.h
#     │   │   ├── SDL_keysym.h
#     │   │   ├── SDL_timer.h
#     │   │   ├── SDL_mixer.h
#     │   │   └── ... (60+ headers)
#     │   └── dswifi*.h         (wifi)
#     └── lib/
#         ├── libnds9.a          (libnds ARM9)
#         ├── libnds7.a          (libnds ARM7)
#         ├── libfat.a           (soporte FAT)
#         ├── libSDL.a           (SDL)
#         ├── libSDL_mixer.a     (SDL audio)
#         ├── libdswifi9.a       (WiFi ARM9)
#         └── libdswifi7.a       (WiFi ARM7)

# ===========================================
# VERIFICACION
# ===========================================

# Ejecuta estos comandos para verificar:

# Verificar compilador
# arm-none-eabi-gcc --version

# Verificar librerias
# ls $LIBNDS/include/nds.h
# ls $LIBNDS/include/SDL/SDL.h
# ls $LIBNDS/lib/libnds9.a

# ===========================================
# COMPILACION
# ===========================================

# Desde la carpeta del proyecto:
# make clean
# make

# Output: opentyrian.nds
