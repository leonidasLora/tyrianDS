# OpenTyrian DS

Tyrian (vertical shooter) port for Nintendo DS.

**Version:** 0.3  
**Author:** Dopefish (VespeneGas)  
**License:** GPL v2

---

## Status

✅ **COMPILED AND TESTED** - Works on real Nintendo DS hardware

---

## Build

This project includes **all required libraries** - no extra downloads needed.

### Windows

1. Open CMD in the project folder
2. Run:
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

### Output

- `tyrianDS.nds` - Nintendo DS ROM (531 KB)

---

## Installation on Nintendo DS

1. **DLDI Patch** - Patch `tyrianDS.nds` for your homebrew card
2. Copy `tyrianDS.nds` to your SD card
3. **Game Data** - Copy the `tyrian/` folder with data files to the SD root

### Game Data

The source code **does NOT include** Tyrian game data. You need:
- Tyrian 2.1 (public domain)
- Or OpenTyrian PC: https://github.com/opentyrian/opentyrian

Copy the data files to `tyrian/`.

---

## Controls

| Button | Action |
|--------|--------|
| D-Pad | Move ship / Navigate |
| A | Confirm |
| B | Rear weapon / Back |
| Y | Fire |
| L/R | Sidekicks |
| Select | In-game menu |
| Touch | Move cursor/ship |

---

## Included Development Environment

| Component | Version |
|-----------|---------|
| devkitARM | r23b (2008) |
| libnds | 20071023 |
| dswifi | 0.3.3 |
| libfat | 20070127 |
| SDL | GPF port for NDS |

---

## Fix Applied

- `arm9/source/vga256d.c` - Added `SDL_NDS_ALLOW_HWSURFACE` (missing constant from original SDL port)

---

## Resources

- Original page: https://vespenegas.com/tyrian.html
- GameBrew: https://www.gamebrew.org/wiki/OpenTyrian_DS
- OpenTyrian PC: https://github.com/opentyrian/opentyrian
