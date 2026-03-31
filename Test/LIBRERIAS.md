# Librerías Exactas para Compilar OpenTyrian DS

## Sistema de Compilación

**devkitPro Pacman** - Gestor de paquetes de devkitPro

### 1. Instalación de devkitPro (Windows)

```bash
# Usar MSYS2 o el instalador oficial desde:
# https://github.com/devkitPro/installer/releases
```

### 2. Paquetes Requeridos para Nintendo DS

```bash
# Paquetes base para Nintendo DS:
dkp-pacman -S devkitarm
dkp-pacman -S libnds
dkp-pacman -S libfat-nds
dkp-pacman -S dswifi

# NOTA: SDL para NDS ya no está en devkitPro (fue descontinuado)
# El proyecto usa un port especial de SDL que está incluido en el código
# o debe compilarse por separado.
```

### 3. Paquetes Completos Recomendados

```bash
# Instalar todos los paquetes de NDS de una vez:
dkp-pacman -S nds-dev
```

Esto instala:
- `devkitARM` - Toolchain ARM
- `libnds` - Librería base NDS
- `libfat-nds` - Soporte FAT
- `dswifi` - WiFi
- `nds-pkg-config` - pkg-config wrapper
- `nds-examples` - Ejemplos

## Sobre SDL

### El Problema

El proyecto OpenTyrian DS fue creado en **2008** y usa un port antiguo de SDL para Nintendo DS. Este port de SDL **ya no está disponible** en devkitPro.

### Soluciones Posibles

#### Opción 1: Buscar el port histórico de SDL
El código menciona que usa "GPF's port of SDL to DS". Este port histórico podría estar en:
- El tarball `sdl.tar.bz2` mencionado en la página original
- Archivos espejo de devkitPro antiguos

#### Opción 2: Usar un port moderno
- **BlocksDS SDL** - https://github.com/blocksds/sdk
- Proyecto activo con SDL 1.2/2.0 para NDS

#### Opción 3: Compilar SDL manualmente
```bash
# Descargar el source de SDL para NDS desde la página original
# https://vespenegas.com/tyrian.html
```

## SDL Histórico - Descarga Obligatoria

El sitio web original de VespeneGas proporciona los archivos SDL necesarios:

**URL:** https://vespenegas.com/tyrian.html

**Archivos a descargar:**
- `sdl.tar.bz2` - Librerías pre-compiladas de SDL para NDS
- `sdl-source.tar.bz2` - Código fuente de SDL (por si necesitas recompilar)

```bash
# Descargar desde la página original
curl -L -o sdl.tar.bz2 "https://vespenegas.com/sdl.tar.bz2"
tar -xjf sdl.tar.bz2
```

**Ubicación esperada de los includes:**
```
${DEVKITARM}/include/SDL/
${DEVKITARM}/lib/
```

**Librerías a linkear:**
- `-lSDL` - SDL principal
- `-lfat` - Soporte FAT filesystem
- `-lnds9` - libnds ARM9
- `-lnds7` - libnds ARM7
- `-ldswifi` - WiFi
- `-lm` - Matemáticas

## Estructura de Directorios Esperada

```
devkitpro/
├── devkitARM/
│   ├── bin/          # arm-none-eabi-* compilers
│   ├── arm-none-eabi/
│   └── ...
├── libnds/
│   └── ...
├── libfat/
│   └── ...
└── ...
```

## Variables de Entorno

```bash
export DEVKITARM=/path/to/devkitARM
export LIBNDS=${DEVKITARM}/libnds
export PATH=$DEVKITARM/bin:$PATH
```

## Errores Comunes y Soluciones

### Error: "SDL not found"
SDL para NDS fue descontinuado. Necesitas:
1. Obtener el port antiguo de SDL
2. O adaptar el código a BlocksDS SDK

### Error: "libnds not found"
```bash
dkp-pacman -S libnds
```

### Error: "Cannot find -lfat"
```bash
dkp-pacman -S libfat-nds
```

### Error: "undefined reference to SDL_*"
El proyecto espera SDL en una ubicación específica. Verifica que los includes estén en:
```
${DEVKITARM}/libnds/include/SDL/
```

## Recursos Adicionales

- DevkitPro Wiki: https://devkitpro.org/wiki/
- GitHub devkitPro: https://github.com/devkitPro
- BlocksDS SDK (alternativa moderna): https://github.com/blocksds/sdk
- GameBrew OpenTyrian DS: https://www.gamebrew.org/wiki/OpenTyrian_DS

## Resumen de Comandos de Instalación

```bash
# 1. Instalar devkitPro (usar el instalador oficial o MSYS2)
# https://github.com/devkitPro/installer/releases

# 2. Actualizar paquetes
dkp-pacman -Syu

# 3. Instalar herramientas de NDS
dkp-pacman -S nds-dev

# 4. Instalar herramientas opcionales
dkp-pacman -S dkp-toolchain-vars devkit-env

# 5. Configurar entorno
source /etc/profile.d/devkit-env.sh

# 6. Verificar instalación
arm-none-eabi-gcc --version
```

## Alternativa Moderna: BlocksDS SDK

BlocksDS es un fork moderno de libnds que incluye SDL 1.2/2.0 para Nintendo DS.

**Sitio web:** https://blocksds.skylyrac.net/
**GitHub:** https://github.com/blocksds/sdk

### Instalación de BlocksDS:

```bash
# En MSYS2 o Linux
git clone https://github.com/blocksds/sdk.git
cd sdk
sudo ./install.sh /opt/blocksds
```

### Paquetes de BlocksDS:
- `blocksds` - Paquete principal
- `blocksds-toolchain` - Toolchain GCC
- `blocksds-libs` - Librerías (incluye SDL)

---

## Nota sobre la Versión del Proyecto

Este proyecto es de **2008** (v0.3). Las librerías han cambiado significativamente desde entonces. 

**Opciones disponibles:**

1. **Usar el port histórico de SDL** - Descargar `sdl.tar.bz2` desde https://vespenegas.com/tyrian.html
2. **Adaptar el código a BlocksDS SDK** - Requiere cambios en el código
3. **Usar libnds moderna** - Eliminar dependencias de SDL y usar APIs nativas

El método más sencillo es obtener el tarball de SDL antiguo del sitio web original.
