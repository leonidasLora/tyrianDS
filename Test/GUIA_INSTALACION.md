# Guía de Instalación - OpenTyrian DS con SDL Histórico

## ✅ Librerías Ya Descargadas

Este proyecto ya incluye las librerías compatibles en la carpeta `devkit/`:

```
devkit/
└── libnds/
    ├── include/
    │   ├── nds.h          ✅
    │   ├── nds/           ✅ (22 headers)
    │   ├── fat.h          ✅
    │   └── SDL/           ✅ (60+ headers)
    └── lib/
        ├── libnds9.a      ✅
        ├── libnds7.a      ✅
        ├── libfat.a       ✅
        ├── libSDL.a       ✅
        ├── libSDL_mixer.a ✅
        └── libdswifi*.a  ✅
```

**Archivos originales (ya no necesarios):**
- `sdl.tar.bz2` - Puede eliminarse (contenido ya en devkit/)

---

## Paso 1: Instalar devkitARM

Descarga devkitARM r17 (la versión de la época):

**Windows:**
```
https://wii.leseratte10.de/devkitPro/devkitARM/r17/devkitARM_r17-win32.exe
```

**Linux:**
```
https://wii.leseratte10.de/devkitPro/devkitARM/r17/devkitARM_r17-for-dslinux-built-on-debian-etch-..>
```

Instala en: `C:\devkitPro\devkitARM` o `/opt/devkitpro/devkitARM`

---

## Paso 2: Descargar librerías adicionales

Si devkitARM r17 no incluye libnds, libfat y dswifi, descárgalos del mirror:

```
# libnds 1.4.3 (2010)
https://wii.leseratte10.de/devkitPro/libnds/libnds_1.4.3%20(2010)/

# libfat 1.0.12
https://wii.leseratte10.de/devkitPro/libfat/libfat_1.0.12%20(2014)/

# dswifi
https://wii.leseratte10.de/devkitPro/dswifi/dswifi-0.4.0%20(2017)/dswifi-0.4.0.tar.bz2
```

---

## Paso 3: Organizar la estructura

El Makefile espera esta estructura:

```
devkitARM/
├── libnds/
│   ├── include/
│   │   ├── nds.h
│   │   ├── nds/           ← Headers de libnds
│   │   └── SDL/           ← ¡Headers de SDL van aquí!
│   └── lib/
│       ├── libnds9.a
│       ├── libnds7.a
│       ├── libfat.a
│       ├── libSDL.a        ← Librería SDL
│       └── libSDL_mixer.a  ← Librería de audio
└── bin/
    └── arm-none-eabi-gcc
```

---

## Paso 4: Copiar archivos del proyecto

```bash
# Asumiendo DEVKITARM=C:\devkitPro\devkitARM

# 1. Copiar headers de SDL al lugar correcto
cp -r include/SDL "$DEVKITARM/libnds/include/"

# 2. Copiar librerías SDL
cp lib/libSDL.a "$DEVKITARM/libnds/lib/"
cp lib/libSDL_mixer.a "$DEVKITARM/libnds/lib/"

# 3. Verificar que libnds, libfat y libndswifi estén en su lugar
# (descomprime los archivos descargados en pasos anteriores)
```

---

## Paso 5: Configurar variables de entorno

```bash
export DEVKITARM="C:/devkitPro/devkitARM"
export LIBNDS="$DEVKITARM/libnds"
export PATH="$DEVKITARM/bin:$PATH"
```

En Windows (CMD):
```cmd
set DEVKITARM=C:\devkitPro\devkitARM
set LIBNDS=%DEVKITARM%\libnds
set PATH=%DEVKITARM%\bin;%PATH%
```

---

## Paso 6: Compilar

```bash
cd "C:\Users\Leonidas\Documents\mis proyectos\tyrianDS"
make clean
make
```

---

## Estructura Actual del Proyecto

```
tyrianDS/
├── arm7/               ← Código ARM7
├── arm9/               ← Código ARM9
├── include/            ← Headers SDL (copiar a devkitARM)
│   └── SDL/
├── lib/                ← Librerías SDL (copiar a devkitARM)
│   ├── libSDL.a
│   └── libSDL_mixer.a
├── tyrian/             ← Datos del juego
├── Makefile            ← Makefile principal
├── sdl.tar.bz2         ← Archivo original
└── Test/               ← Documentación
```

---

## Posibles Errores y Soluciones

### Error: "nds.h not found"
```
Solución: Instalar libnds completo
```

### Error: "SDL.h not found"
```
Solución: Copiar include/SDL/ a $LIBNDS/include/SDL/
```

### Error: "Cannot find -lSDL"
```
Solución: Copiar libSDL.a a $LIBNDS/lib/
```

### Error: "Cannot find -lfat"
```
Solución: Instalar libfat desde el mirror
```

---

## Comandos de Verificación

```bash
# Verificar que devkitARM está instalado
arm-none-eabi-gcc --version

# Verificar estructura de libnds
ls $LIBNDS/include/
ls $LIBNDS/lib/

# Verificar que SDL está en su lugar
ls $LIBNDS/include/SDL/SDL.h
ls $LIBNDS/lib/libSDL.a
```

---

## Resumen de Archivos a Descargar

| Archivo | URL |
|---------|-----|
| devkitARM r17 (Windows) | wii.leseratte10.de/devkitPro/devkitARM/r17/devkitARM_r17-win32.exe |
| devkitARM r17 (Linux) | wii.leseratte10.de/devkitPro/devkitARM/r17/ |
| libnds 1.4.3 | wii.leseratte10.de/devkitPro/libnds/libnds_1.4.3%20(2010)/ |
| libfat 1.0.12 | wii.leseratte10.de/devkitPro/libfat/libfat_1.0.12%20(2014)/ |
| dswifi 0.4.0 | wii.leseratte10.de/devkitPro/dswifi/dswifi-0.4.0%20(2017)/dswifi-0.4.0.tar.bz2 |
