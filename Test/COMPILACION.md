# OpenTyrian DS - Documentacion de Compilacion

## Informacion General

- **Proyecto:** OpenTyrian DS v0.3
- **Autor:** Dopefish (VespeneGas)
- **Licencia:** GPL v2
- **Plataforma:** Nintendo DS
- **Basado en:** OpenTyrian Classic (SVN r734)

---

## Requisitos para Compilar

### 1. devkitARM

Descarga e instala devkitARM, el kit de desarrollo para Nintendo DS:

```
https://devkitpro.org/
```

**Variables de entorno necesarias:**
```bash
export DEVKITARM=/path/to/devkitARM
```

### 2. libnds

Libreria base para desarrollo en Nintendo DS (incluida con devkitARM).

### 3. SDL para NDS

El proyecto usa SDL para video, timing e input. Necesitaras las librerias SDL portadas para Nintendo DS.

**Librerias requeridas:**
- `-lSDL` - SDL principal
- `-lfat` - Soporte FAT para tarjetas SD
- `-lm` - Libreria matematica
- `-lnds9` - libnds para ARM9
- `-lnds7` - libnds para ARM7
- `-ldswifi7` - Soporte WiFi (ARM7)

---

## Estructura del Proyecto

```
tyrianDS/
├── Makefile           # Makefile principal
├── arm7/
│   ├── Makefile       # Compila el procesador ARM7
│   └── source/
│       └── main.c     # Codigo ARM7 (audio, entrada)
├── arm9/
│   ├── Makefile       # Compila el procesador ARM9
│   └── source/        # ~85 archivos .c/.h
│       ├── mainint.c  # Inicializacion
│       ├── tyrian2.c  # Logica principal del juego
│       ├── opentyr.c  # Menu OpenTyrian
│       ├── sound.c    # Audio
│       └── ...
├── tyrian/            # Datos del juego (sprites, niveles, etc.)
├── COPYING            # Licencia GPL v2
├── README             # Documentacion
└── TODO               # Tareas pendientes
```

---

## Compilacion

### Comando basico:

```bash
make
```

Esto genera:
1. `opentyrian.arm7` - Binario para ARM7
2. `opentyrian.arm9` - Binario para ARM9
3. `opentyrian.nds`  - Archivo NDS final (combina ambos)

### Compilar componentes por separado:

```bash
# Solo ARM7
make opentyrian.arm7

# Solo ARM9
make opentyrian.arm9
```

### Limpiar archivos compilados:

```bash
make clean
```

### Generar imagen de prueba (para emulator):

```bash
make images     # Genera logo.s desde logo.png
make cflash.img # Crea imagen de disco
make test       # Ejecuta con desmume
```

---

## Instalacion en la Nintendo DS

1. **DLDI Patch:** Aplica el parche DLDI correspondiente a tu tarjeta:
   ```bash
   make install
   ```
   O manualmente con `dldi-scsd-moon opentyrian.nds`

2. **Copiar archivos:**
   - `opentyrian.nds` -> tarjeta SD
   - Carpeta `tyrian/` -> raiz de la tarjeta SD

3. **Archivos de musica (opcional):**
   Los archivos `song_*.wav` pueden omitirse si no quieres musica.

---

## Compilacion Cruzada (Cross-Compiling)

Este proyecto esta disenado para compilarse en una PC (Linux/Windows con MSYS) targeting Nintendo DS. No puede compilarse nativamente en la DS.

### En Windows:
1. Instala MSYS2 o Cygwin
2. Instala devkitPro con devkitARM
3. Ejecuta `make` desde MSYS

---

## Dependencias del Codigo

### ARM9 (procesador principal):
- SDL (video, input, timing)
- libnds9
- libfat
- Librerias matematicas

### ARM7 (procesador secundario):
- libnds7
- libndswifi7

### Librerias propias incluidas:
- `fmopl.c` - Emulacion de OPL2 (Adlib FM)
- `loudness.c` - Sistema de sonido Loudness
- `lds_play.c` - Reproduccion de archivos LDS
- `adpcm_decoder.c` - Decodificador ADPCM para audio

---

## Flags de Compilacion

### ARM9:
```
-mthumb -mthumb-interwork
-march=armv5te -mtune=arm946e-s
-ffast-math
-DTARGET_NDS -DNDS_VERSION
```

### ARM7:
```
-mthumb-interwork
-mcpu=arm7tdmi -mtune=arm7tdmi
-ffast-math
```

---

## Problemas Comunes

### "Please set DEVKITARM in your environment"
```bash
export DEVKITARM=/opt/devkitpro/devkitARM
```

### Errores de SDL no encontrada
Asegurate de tener SDL para NDS instalado. El proyecto usa un port especial de SDL para Nintendo DS.

### Errores de memoria
El juego requiere que la carpeta `tyrian/` con los datos este presente. Sin estos archivos, el juego crashea.

---

## Recursos

- Repositorio original: http://code.google.com/p/opentyrian/
- DevkitPro: https://devkitpro.org/
- DLDI: http://dldi.drunkencoders.com/

---

## Changelog del Proyecto

### v0.3 (Abril 2008)
- Mejor mapeo de controles para B, L, R
- Estabilidad y rendimiento mejorados
- Nueva pantalla inferior rediseñada
- Mejor loop en canciones

### v0.2
- Modo sin escalado horizontal (texto mas legible)
- Opcion para modo Destruct/Super Arcade
- Arreglado bug de save games corruptos
- Gamma mejorado

### v0.1
- Primera version publica
