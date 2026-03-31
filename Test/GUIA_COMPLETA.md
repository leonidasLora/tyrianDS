# Guía Completa de Compilación - OpenTyrian DS

## Resumen

Este documento explica cómo compilar OpenTyrian DS v0.3 desde el código fuente.

**Estado:** ✅ COMPILADO EXITOSAMENTE

---

## Requisitos del Sistema

### 1. Sistema Operativo
- Windows 10/11 (o cualquier Windows con soporte para cmd.exe)
- Alternativa: Linux con MSYS2 o Git Bash

### 2. Espacio en Disco
- ~500 MB para el entorno de desarrollo
- ~100 MB para el proyecto compilado

---

## Paso 1: Descargar el Código Fuente

El código fuente está disponible en:
```
https://vespenegas.com/tyrian.html
```

Archivos necesarios:
- `opentyriands-source.tar.bz2` - Código fuente del juego
- `sdl.tar.bz2` - Librería SDL para NDS (port de GPF)

---

## Paso 2: Estructura del Proyecto

```
tyrianDS/
├── arm7/               # Código ARM7 (audio, entrada)
├── arm9/               # Código ARM9 (gráficos, lógica)
├── devkit/             # Entorno de desarrollo
│   └── devkitARM/
│       └── libnds/     # Librerías necesarias
│           ├── include/
│           │   ├── nds.h
│           │   ├── SDL/
│           │   └── ...
│           └── lib/
│               ├── libnds7.a
│               ├── libnds9.a
│               ├── libSDL.a
│               └── ...
├── tyrian/             # DATOS DEL JUEGO (requerido)
├── Makefile
└── tyrianDS.nds        # Output (generado)
```

---

## Paso 3: Obtener los Datos del Juego

**IMPORTANTE:** El código fuente NO incluye los datos del juego Tyrian.

### Opción 1: Descargar Tyrian Original
1. Descarga Tyrian 2.1 de dominio público
2. Extrae los archivos .TUS, .HSH, .FLY, etc.
3. Crea la carpeta `tyrian/` en la raíz del proyecto
4. Copia los archivos de datos a `tyrian/`

### Opción 2: OpenTyrian PC
1. Descarga OpenTyrian de: https://github.com/opentyrian/opentyrian
2. Copia la carpeta `tyrian/` al proyecto

### Archivos mínimos necesarios en `tyrian/`:
- Archivos de datos del juego (TUS, FLY, etc.)
- `tyrian.snd` o similar
- `song_*.wav` (opcional, para música)

---

## Paso 4: Variables de Entorno

### Windows (CMD)

```cmd
set DEVKITPRO=C:\tyrianDS\devkit\devkitARM
set DEVKITARM=%DEVKITPRO%
set LIBNDS=%DEVKITARM%\libnds
set PATH=%DEVKITARM%\bin;%PATH%
```

### Linux/Bash

```bash
export DEVKITPRO=/path/to/tyrianDS/devkit/devkitARM
export DEVKITARM=$DEVKITPRO
export LIBNDS=$DEVKITARM/libnds
export PATH=$DEVKITARM/bin:$PATH
```

---

## Paso 5: Compilar

### Limpiar compilaciones anteriores:
```cmd
cd C:\tyrianDS
make clean
```

### Compilar:
```cmd
make
```

### Salida esperada:
```
arm7/tyrianDS.elf
arm9/tyrianDS.elf
tyrianDS.nds        ← Archivo ROM final (531 KB)
```

---

## Problemas Comunes y Soluciones

### Error: "nds.h not found"
**Causa:** LIBNDS no apunta a la ubicación correcta
**Solución:** Verificar que `LIBNDS` apunte a `devkitARM/libnds`

### Error: "SDL_NDS_ALLOW_HWSURFACE undeclared"
**Causa:** Falta constante del port de SDL de GPF
**Solución:** Agregar en `vga256d.c`:
```c
#ifndef SDL_NDS_ALLOW_HWSURFACE
#define SDL_NDS_ALLOW_HWSURFACE 0
#endif
```

### Error: "touchReadXY too few arguments"
**Causa:** Versión de libnds incompatible (API cambió)
**Solución:** Usar libnds de 2007-2008

---

## Estructura del Entorno de Desarrollo

El proyecto incluye un entorno pre-configurado con:

### Librerías Incluidas:
| Librería | Versión | Propósito |
|----------|---------|-----------|
| libnds | 20071023 | API base de NDS |
| dswifi | 0.3.3 | Soporte WiFi |
| libfat | 20070127 | Acceso a tarjeta SD |
| SDL | GPF port | Gráficos y entrada |

### Compilador:
- devkitARM r23b (2008)
- GCC 4.2.1

---

## Instalación en Nintendo DS

### 1. DLDI Patch
El archivo `.nds` necesita ser "parcheado" para funcionar con tu tarjeta de homebrew.

Usa una herramienta como:
- DLDITool
- ROM Patcher

### 2. Archivos Requeridos
```
/tarjeta SD/
├── tyrianDS.nds
└── tyrian/           ← Carpeta con datos del juego
    ├── *.tus
    ├── *.fth
    ├── *.hsd
    ├── *.lvl
    └── song_*.wav     (opcional)
```

---

## Archivos Generados

| Archivo | Descripción |
|---------|-------------|
| `tyrianDS.nds` | ROM principal de Nintendo DS |
| `tyrianDS.arm7` | Binario ARM7 (26 KB) |
| `tyrianDS.arm9` | Binario ARM9 (504 KB) |
| `tyrianDS.ds.gba` | Versión GBA (usada por dsbuild) |

---

## Notas Técnicas

### Procesadores
- **ARM7:** Procesador secundario - maneja audio y entrada
- **ARM9:** Procesador principal - maneja gráficos y lógica del juego

### Resolución
- El juego usa 320x200 (escala a la pantalla de la DS)
- Esto hace el texto más pequeño pero mantiene la jugabilidad

### Controles
```
D-Pad: Mover nave / Navegar menús
A: Confirmar
B: Cambiar arma trasera / Volver
Y: Disparar
L: Disparar sidekick izquierdo
R: Disparar sidekick derecho
Select: Menú en juego
Start: Confirmar
Touch: Mover cursor / nave
```

---

## Recursos

- Página oficial: https://vespenegas.com/tyrian.html
- GameBrew: https://www.gamebrew.org/wiki/OpenTyrian_DS
- OpenTyrian (PC): https://github.com/opentyrian/opentyrian
- DevkitPro: https://devkitpro.org/
- Mirror librerías: https://wii.leseratte10.de/devkitPro/

---

## Changelog de la Compilación

### Fixes aplicados al código original:

1. **vga256d.c** - Añadido define para `SDL_NDS_ALLOW_HWSURFACE`
   (El port de SDL de GPF usa constantes que no están en mirrors públicos)

### Librerías utilizadas:
- libnds: 20071023 (Octubre 2007)
- dswifi: 0.3.3 (2007)
- libfat: 20070127 (Enero 2007)
- SDL: Port de GPF para NDS
- devkitARM: r23b (2008)

---

## Licencia

OpenTyrian DS es software libre bajo GPL v2.
Ver archivo `COPYING` para más detalles.
