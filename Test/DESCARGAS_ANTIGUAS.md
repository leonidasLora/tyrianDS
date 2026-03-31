# Descarga de Librerías Antiguas para OpenTyrian DS

## Mirror con Versiones Antiguas de devkitPro

**URL:** https://wii.leseratte10.de/devkitPro/

Este mirror contiene versiones antiguas de devkitPro que son compatibles con proyectos de 2008.

---

## Descargas Necesarias

### 1. devkitARM (Toolchain)

El proyecto es de 2008, necesitas una versión antigua:

| Versión | Fecha | Archivo | Enlace |
|---------|-------|---------|--------|
| **r17** | ~2007 | devkitARM_r17-for-dslinux | https://wii.leseratte10.de/devkitPro/devkitARM/r17/ |
| r47 | 2017 | devkitARM_r47 | https://wii.leseratte10.de/devkitPro/devkitARM/r47%20%282017%29/ |
| r53 | 2019 | devkitARM-r53 | https://wii.leseratte10.de/devkitPro/devkitARM/r53%20%282019-06%29/ |

**Recomendado para tu proyecto:** `devkitARM_r17` (la versión de la época)

**Descarga directa r17 Windows:**
```
https://wii.leseratte10.de/devkitPro/devkitARM/r17/devkitARM_r17-win32.exe
```

**Descarga directa r17 Linux:**
```
https://wii.leseratte10.de/devkitPro/devkitARM/r17/devkitARM_r17-for-dslinux-built-on-debian-etch-..> 
https://wii.leseratte10.de/devkitPro/devkitARM/r17/devkitARM_r17-for-dslinux-built-on-gentoo-x86.tbz2
```

---

### 2. libnds (Librería Base NDS)

| Versión | Fecha | Archivo | Enlace |
|---------|-------|---------|--------|
| 1.3.1 | - | libnds_1.3.1 | https://wii.leseratte10.de/devkitPro/libnds/libnds_1.3.1/ |
| 1.3.2 | - | libnds_1.3.2 | https://wii.leseratte10.de/devkitPro/libnds/libnds_1.3.2/ |
| 1.4.3 | 2010 | libnds_1.4.3 | https://wii.leseratte10.de/devkitPro/libnds/libnds_1.4.3%20(2010)/ |

---

### 3. libfat (Soporte FAT)

| Versión | Fecha | Archivo | Enlace |
|---------|-------|---------|--------|
| 1.0.10 | 2011 | libfat_1.0.10 | https://wii.leseratte10.de/devkitPro/libfat/libfat_1.0.10%20(2011)/ |
| 1.0.11 | 2012 | libfat_1.0.11 | https://wii.leseratte10.de/devkitPro/libfat/libfat_1.0.11%20(2012)/ |
| 1.0.12 | 2014 | libfat_1.0.12 | https://wii.leseratte10.de/devkitPro/libfat/libfat_1.0.12%20(2014)/ |

---

### 4. dswifi (WiFi)

| Versión | Archivo | Enlace |
|---------|---------|--------|
| 0.4.0 (2017) | dswifi-0.4.0.tar.bz2 | https://wii.leseratte10.de/devkitPro/dswifi/dswifi-0.4.0%20(2017)/ |
| 0.4.0 | v0.4.0.tar.gz | https://wii.leseratte10.de/devkitPro/dswifi/dswifi-0.4.0%20(2017)/ |

---

### 5. SDL (Librería Gráficos)

| Archivo | Enlace |
|---------|--------|
| SDL-1.2.15.tar.gz | https://wii.leseratte10.de/devkitPro/other-stuff/libraries/SDL/SDL-1.2.15.tar.gz |

---

### 6. ndstool (Herramienta para crear .nds)

| Versión | Archivo | Enlace |
|---------|---------|--------|
| 1.50.3 | ndstool-1.50.3.tar.bz2 | https://wii.leseratte10.de/devkitPro/other-stuff/ndstool/ndstool-1.50.3.tar.bz2 |
| 2.0.1 | ndstool-2.0.1.tar.gz | https://wii.leseratte10.de/devkitPro/other-stuff/ndstool/ndstool-2.0.1.tar.gz |

---

### 7. Ejemplos NDS (para referencia)

| Fecha | Archivo | Enlace |
|-------|---------|--------|
| 2008-04-27 | nds-examples-20080427.tar.bz2 | https://wii.leseratte10.de/devkitPro/nds-examples/nds-examples-20080427.tar.bz2 |
| 2008-12-10 | nds-examples-20081210.tar.bz2 | https://wii.leseratte10.de/devkitPro/nds-examples/nds-examples-20081210.tar.bz2 |
| 2009-02-21 | nds-examples-20090221.tar.bz2 | https://wii.leseratte10.de/devkitPro/nds-examples/nds-examples-20090221.tar.bz2 |

---

## Paquete Completo Antiguo (Todo en Uno)

Este archivo contiene todo el entorno de desarrollo de 2007-2008:

```
https://www.libsdl.org/extras/nds/devkitPro-20070503-linux.tar.gz
```

---

## Comandos de Descarga

```bash
# Crear carpeta para las librerías
mkdir -p devkit_old
cd devkit_old

# Descargar devkitARM r17 (Windows)
curl -L -o devkitARM_r17-win32.exe "https://wii.leseratte10.de/devkitPro/devkitARM/r17/devkitARM_r17-win32.exe"

# Descargar libnds 1.4.3 (2010)
curl -L -o libnds-1.4.3.tar.bz2 "https://wii.leseratte10.de/devkitPro/libnds/libnds_1.4.3%20(2010).tar.bz2"

# Descargar libfat 1.0.12
curl -L -o libfat-1.0.12.tar.bz2 "https://wii.leseratte10.de/devkitPro/libfat/libfat_1.0.12%20(2014).tar.bz2"

# Descargar dswifi
curl -L -o dswifi-0.4.0.tar.bz2 "https://wii.leseratte10.de/devkitPro/dswifi/dswifi-0.4.0%20(2017)/dswifi-0.4.0.tar.bz2"

# Descargar SDL
curl -L -o SDL-1.2.15.tar.gz "https://wii.leseratte10.de/devkitPro/other-stuff/libraries/SDL/SDL-1.2.15.tar.gz"

# Descargar ndstool
curl -L -o ndstool-1.50.3.tar.bz2 "https://wii.leseratte10.de/devkitPro/other-stuff/ndstool/ndstool-1.50.3.tar.bz2"

# Paquete completo Linux (2007)
curl -L -o devkitPro-20070503-linux.tar.gz "https://www.libsdl.org/extras/nds/devkitPro-20070503-linux.tar.gz"
```

---

## Estructura de Instalación Esperada

Después de instalar, tu devkitARM debería tener esta estructura:

```
devkitARM/
├── bin/
│   ├── arm-none-eabi-gcc
│   ├── arm-none-eabi-g++
│   ├── arm-none-eabi-ld
│   └── ...
├── arm-none-eabi/
│   └── ...
├── lib/
│   ├── libnds9.a
│   ├── libnds7.a
│   ├── libfat.a
│   └── ...
├── include/
│   ├── nds/
│   │   └── ...
│   └── SDL/
│       └── ...
└── ...
```

---

## Alternativa: Usar BlocksDS SDK

Si las librerías antiguas no funcionan, BlocksDS SDK es una alternativa moderna que podría compilar con modificaciones:

**URL:** https://github.com/blocksds/sdk

BlocksDS incluye SDL moderno y es compatible con proyectos que usan SDL.

---

## Ayuda Adicional

Si necesitas más versiones antiguas, este mirror es mantenido por **Leseratte** en GBAtemp.net.
Contacto: GBAtemp.net/conversations/add?to=Leseratte
