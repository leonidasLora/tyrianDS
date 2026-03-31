# Checklist - OpenTyrian DS

## ✅ Proyecto Compilado

### Para compilar de nuevo:
1. Abre CMD en `C:\tyrianDS`
2. Ejecuta: `compilar.bat`
3. Verifica que `tyrianDS.nds` se genere

---

## ⚠️ Antes de Ejecutar en DS

### 1. Obtener Datos del Juego
- [ ] Descargar Tyrian u OpenTyrian PC
- [ ] Copiar archivos a `C:\tyrianDS\tyrian\`

### 2. DLDI Patch
- [ ] Parchar `tyrianDS.nds` con herramienta DLDI
- [ ] Compatible con tu tarjeta de homebrew

### 3. Copiar a SD
- [ ] `tyrianDS.nds` → tarjeta SD
- [ ] `tyrian/` → raiz de SD

---

## 📁 Estructura Final en SD

```
/tarjeta SD/
├── tyrianDS.nds
└── tyrian/
    ├── *.tus
    ├── *.fth
    ├── *.hsd
    └── song_*.wav  (opcional)
```

---

## 🔧 Si Falla la Compilacion

1. Verificar que `DEVKITARM` apunte a `C:\tyrianDS\devkit\devkitARM`
2. Verificar que `LIBNDS` apunte a `%DEVKITARM%\libnds`
3. Limpiar con `make clean` antes de compilar

---

## 📚 Documentacion Disponible

| Archivo | Descripcion |
|---------|-------------|
| `Test/GUIA_COMPLETA.md` | Guia detallada |
| `Test/RESUMEN.md` | Resumen rapido |
| `Test/COMPILACION.md` | Documentacion original |
| `compilar.bat` | Script de compilacion |

---

## 🐛 Errores Conocidos

### "nds.h not found"
→ Verificar variable LIBNDS

### "SDL_NDS_ALLOW_HWSURFACE undeclared"
→ Ya solucionado en vga256d.c

### Crashea al iniciar
→ Falta la carpeta `tyrian/` con datos del juego
