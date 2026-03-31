# OpenTyrian DS - Resumen de Compilacion

## Estado: ✅ COMPILADO

---

## Para Compilar de Nuevo

1. **Abrir CMD** en `C:\tyrianDS`

2. **Ejecutar:**
   ```cmd
   compilar.bat
   ```

   O manualmente:
   ```cmd
   set DEVKITARM=C:\tyrianDS\devkit\devkitARM
   set LIBNDS=%DEVKITARM%\libnds
   set PATH=%DEVKITARM%\bin;%PATH%
   make
   ```

---

## Requisitos

| Componente | Estado |
|------------|--------|
| devkitARM r23b | ✅ Incluido |
| libnds (2007) | ✅ Incluido |
| dswifi (0.3.3) | ✅ Incluido |
| libfat (2007) | ✅ Incluido |
| SDL (GPF port) | ✅ Incluido |

---

## Archivos Generados

| Archivo | Tamano |
|---------|--------|
| `tyrianDS.nds` | 531 KB |
| `tyrianDS.arm7` | 26 KB |
| `tyrianDS.arm9` | 504 KB |

---

## IMPORTANTE: Datos del Juego

El codigo fuente **NO incluye** los datos del juego.

Necesitas:
1. Descargar Tyrian u OpenTyrian PC
2. Copiar los archivos a `C:\tyrianDS\tyrian\`

Sin los datos, el juego crasheara.

---

## Fixes Aplicados

- `arm9/source/vga256d.c` - Anadido `SDL_NDS_ALLOW_HWSURFACE`

---

## Documentacion

Ver `Test/GUIA_COMPLETA.md` para instrucciones detalladas.
