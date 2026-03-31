# Quick Reference - OpenTyrian DS Build

## Prerequisites

1. **devkitPro + devkitARM** from https://devkitpro.org/
2. Set environment: `export DEVKITARM=/path/to/devkitpro/devkitARM`

## Build Commands

| Command | Description |
|---------|-------------|
| `make` | Full build (creates .nds file) |
| `make clean` | Clean build artifacts |
| `make test` | Run in desmume emulator |

## Output Files

- `opentyrian.arm7` - ARM7 binary
- `opentyrian.arm9` - ARM9 binary  
- `opentyrian.nds` - Final ROM

## Installation

1. Patch with DLDI: `dldi-patch opentyrian.nds`
2. Copy `opentyrian.nds` to SD card
3. Copy `tyrian/` folder to card root

## Project Structure

```
tyrianDS/
├── Makefile      # Root makefile
├── arm7/         # ARM7 (audio, input) - arm7tdmi
├── arm9/         # ARM9 (main CPU) - arm946e-s
├── tyrian/       # Game data files
└── Test/         # This documentation
```

## Key Libraries

- SDL (custom DS port)
- libnds9 / libnds7
- libfat (FAT filesystem)
- libndswifi7

## Notes

- Game data in `tyrian/` folder is REQUIRED
- Music files (`song_*.wav`) are optional
- Save games compatible with OpenTyrian PC version
