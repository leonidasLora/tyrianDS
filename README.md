# OpenTyrian DS

Puerto de Tyrian (shooter vertical) para Nintendo DS.

**Versión:** 0.3  
**Autor:** Dopefish (VespeneGas)  
**Licencia:** GPL v2

---

## Estado

✅ **COMPILADO Y TESTEADO** - Funciona en Nintendo DS real

---

## Compilación

El proyecto incluye **todas las librerías necesarias** - no necesitas descargar nada extra.

### Windows

1. Abre CMD en la carpeta del proyecto
2. Ejecuta:
   ```cmd
   compilar.bat
   ```

### Linux/Mac

```bash
export DEVKITARM=$(pwd)/devkit/devkitARM
export LIBNDS=$DEVKITARM/libnds
export PATH=$DEVKITARM/bin:$PATH
make
```

### Salida

- `tyrianDS.nds` - ROM de Nintendo DS (531 KB)

---

## Instalación en Nintendo DS

1. **DLDI Patch** - Parchea `tyrianDS.nds` para tu tarjeta de homebrew
2. Copia `tyrianDS.nds` a tu tarjeta SD
3. **Datos del juego** - Copia la carpeta `tyrian/` con los archivos de datos a la raíz de la SD

### Datos del Juego

El código fuente **NO incluye** los datos de Tyrian. Necesitas:
- Tyrian 2.1 (dominio público)
- O OpenTyrian PC: https://github.com/opentyrian/opentyrian

Copia los archivos de datos a `tyrian/`.

---

## Controles

| Botón | Acción |
|-------|--------|
| D-Pad | Mover nave / Navegar |
| A | Confirmar |
| B | Arma trasera / Volver |
| Y | Disparar |
| L/R | Sidekicks |
| Select | Menú en juego |
| Touch | Mover cursor/nave |

---

## Entorno de Desarrollo Incluido

| Componente | Versión |
|------------|---------|
| devkitARM | r23b (2008) |
| libnds | 20071023 |
| dswifi | 0.3.3 |
| libfat | 20070127 |
| SDL | GPF port para NDS |

---

## Fix Aplicado

- `arm9/source/vga256d.c` - Añadido `SDL_NDS_ALLOW_HWSURFACE` (constante faltante del port de SDL original)

---

## Recursos

- Página original: https://vespenegas.com/tyrian.html
- GameBrew: https://www.gamebrew.org/wiki/OpenTyrian_DS
- OpenTyrian PC: https://github.com/opentyrian/opentyrian
