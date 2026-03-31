# Cómo Contribuir

## Requisitos

- Windows, Linux o Mac
- Compilador `arm-eabi-gcc` (incluido en `devkit/devkitARM/bin/`)

## Compilación

```bash
# Windows
compilar.bat

# Linux/Mac
export DEVKITARM=$(pwd)/devkit/devkitARM
export LIBNDS=$DEVKITARM/libnds
make
```

## Estructura del Código

```
arm7/source/main.c     - ARM7: audio, entrada, WiFi
arm9/source/          - ARM9: gráficos, lógica del juego
```

## Reglas

1. Mantener compatibilidad con devkitARM r23b
2. No modificar las librerías en `devkit/`
3. Probar en hardware real si es posible
4. Documentar cambios significativos

## Problemas Conocidos

- El juego requiere datos de Tyrian en `tyrian/` (no incluido)
- Sin soporte de red WiFi

## Enlaces

- DevkitPro: https://devkitpro.org/
- Mirror librerías: https://wii.leseratte10.de/devkitPro/
