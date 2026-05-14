# Herramientas de Conversion

Scripts para convertir fuentes a formato Clockwise.

## convert_ttf.py

Convierte fuentes TTF (TrueType) a formato Clockwise.

### Requisitos

```bash
pip install pillow fonttools
```

### Uso

**Convertir a JSON** (para importar en char-designer):
```bash
python convert_ttf.py "MiFuente.ttf" --size 12
python convert_ttf.py "MiFuente.ttf" --size 12 --output ../fonts/
```

**Convertir a JS** (para agregar a pixel-fonts.js):
```bash
python convert_ttf.py "MiFuente.ttf" --size 12 --js
python convert_ttf.py "MiFuente.ttf" --size 12 --js --name miFuente
```

### Parametros

| Parametro | Descripcion | Default |
|-----------|-------------|---------|
| `ttf` | Archivo TTF a convertir | (requerido) |
| `--size` | Tamaño en pixels | 12 |
| `--name` | Nombre de la fuente | nombre del archivo |
| `--output` | Carpeta de salida | carpeta actual |
| `--js` | Generar codigo JS | false (genera JSON) |

### Tamaños recomendados

| Display | Tamaño | Uso |
|---------|--------|-----|
| 64x64 | 8-12px | Hora grande |
| 64x64 | 5-8px | Texto secundario |
| 64x64 | 12-16px | Solo digitos |

### Ejemplo

```bash
# Descargar fuente LED de dafont.com
# Convertir a tamaño 12px
python convert_ttf.py "NEW LED DISPLAY ST.ttf" --size 12 --js --name ledDisplay

# Copiar el codigo generado a js/pixel-fonts.js
```

## Donde encontrar fuentes

- [dafont.com](https://www.dafont.com) - Seccion "Bitmap" o "LED"
- [FontStruct](https://fontstruct.com) - Fuentes pixel personalizadas
- [Google Fonts](https://fonts.google.com) - Algunas fuentes pixel
- Paquetes de sistema (Windows Fonts, etc.)

## Notas

- Las fuentes TTF son vectoriales, se renderizan a bitmap
- Tamaños pequeños (8-14px) funcionan mejor en display 64x64
- Fuentes "pixel" o "bitmap" dan mejores resultados
- Las fuentes con serif pueden verse mal a tamaños pequeños
