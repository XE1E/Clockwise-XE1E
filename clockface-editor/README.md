# Clockface Editor - Herramientas

Suite de herramientas web y scripts para crear, editar y gestionar caratulas para Clockwise.

## Herramientas Web

### Editor de Caratulas (`index.html`)
Editor visual principal para crear y modificar caratulas JSON.

**Funciones:**
- Crear/editar elementos: imágenes, texto, datetime, líneas, rectángulos, sprites
- Vista previa en tiempo real con simulación de hora
- Importar/exportar JSON
- **Auto-generación de thumbnail** al exportar (para preview en interfaz web)
- Enviar directamente al reloj via WiFi
- Paleta de colores RGB565
- Múltiples fuentes pixel disponibles

**Uso:**
1. Abrir en navegador (local o desde GitHub Pages)
2. Agregar elementos con los botones del panel izquierdo
3. Configurar propiedades en el panel derecho
4. Exportar JSON (incluye thumbnail automáticamente) o enviar al reloj

**Opciones de exportación:**
- Checkbox "Incluir thumbnail" - activo por defecto
- Muestra tamaño del JSON resultante

### Diseñador de Dígitos (`digit-designer.html`)
Crea sprites personalizados para dígitos 0-9 y separador.

**Funciones:**
- Editor pixel a pixel
- Preview de todos los dígitos
- Exporta array de sprites para usar en caratulas
- Configurable tamaño y colores

### Diseñador de Caracteres (`char-designer.html`)
Diseña caracteres individuales o sets de caracteres.

**Funciones:**
- Editor de bitmap para caracteres
- Soporte para múltiples tamaños
- Exporta en formato compatible con el editor

### Convertidor de Fuentes (`font-converter.html`)
Convierte fuentes a formato JSON para el editor.

**Funciones:**
- Importar fuentes en varios formatos
- Ajustar parámetros de renderizado
- Exportar como JSON compatible

### Generador de Thumbnails (`generate-thumbs.html`)
Genera y embebe thumbnails en las caratulas para mostrar previews en la interfaz web del reloj.

**Tab 1 - Generar desde carpeta (dinámico):**
1. Abrir `generate-thumbs.html` en Chrome o Edge
2. Click en "Seleccionar Carpeta"
3. Elegir cualquier carpeta con archivos .json
4. Se detectan y procesan automáticamente todos los JSONs
5. Opciones:
   - **Descargar PNGs**: Imágenes sueltas para uso externo
   - **Descargar JSONs**: Carátulas con thumbnail embebido (listas para subir al reloj)

**Tab 2 - Inyectar en JSON individual:**
1. Click en "Seleccionar JSON"
2. El thumbnail se genera automáticamente
3. Descargar el JSON actualizado con thumbnail embebido

**Nota:** El thumbnail se guarda en el campo `thumbnail` del JSON y la interfaz web del reloj lo usa para mostrar la preview de cada carátula. No requiere servidor local.

### Página de Configuración (`config-page.html`)
Interfaz alternativa de configuración del reloj con previews visuales.

**Funciones:**
- Configuración WiFi
- Selección de caratulas con thumbnails
- Ajustes de pantalla y modo nocturno
- Gestión de rotación de caratulas

### Guía de Instalación (`install.html`)
Documentación paso a paso para instalar el firmware.

---

## Scripts Python (`tools/`)

### convert_gfx_font.py
Convierte archivos de fuente GFX (.h) a formato JSON para el editor.

```bash
python convert_gfx_font.py <fuente.h> [salida.json]
```

**Ejemplo:**
```bash
python convert_gfx_font.py font5x7.h 5x7.json
```

### convert_all_fonts.py
Convierte todas las fuentes del firmware a JSON en batch.

```bash
python convert_all_fonts.py
```

Busca fuentes en `../../firmware/clockfaces/cw-cf-0x07/fonts/` y las guarda en `../fonts/`.

### convert_ttf.py
Convierte fuentes TrueType (.ttf) a formato GFX o JSON.

```bash
python convert_ttf.py <fuente.ttf> <tamaño> [salida]
```

### convert_bdf_to_json.py
Convierte fuentes BDF (Bitmap Distribution Format) a JSON.

```bash
python convert_bdf_to_json.py <fuente.bdf> [salida.json]
```

### convert_bdf_to_gfx.py
Convierte fuentes BDF a formato GFX header (.h).

```bash
python convert_bdf_to_gfx.py <fuente.bdf> [salida.h]
```

### convert_json_to_gfx.py
Convierte fuentes JSON de vuelta a formato GFX header.

```bash
python convert_json_to_gfx.py <fuente.json> [salida.h]
```

### convert_old_json.py
Migra caratulas del formato JSON antiguo al actual.

```bash
python convert_old_json.py <caratula_vieja.json> [salida.json]
```

### generate_worldmap_frames.py
Genera los frames de animación para la caratula del mapa mundial.

```bash
python generate_worldmap_frames.py
```

Genera frames con la línea del terminador (día/noche) para cada hora.

---

## Estructura de Archivos

```
clockface-editor/
├── index.html              # Editor principal
├── digit-designer.html     # Diseñador de dígitos
├── char-designer.html      # Diseñador de caracteres
├── font-converter.html     # Convertidor de fuentes
├── generate-thumbs.html    # Generador de thumbnails
├── config-page.html        # Página de configuración
├── install.html            # Guía de instalación
├── js/
│   ├── editor.js           # Lógica del editor
│   ├── elements.js         # Definición de elementos
│   ├── pixel-fonts.js      # Renderizado de fuentes
│   └── color-utils.js      # Utilidades de color RGB565
├── fonts/                  # Fuentes JSON para el editor
├── clockfaces/             # Caratulas de ejemplo
│   └── thumbs/             # Thumbnails de caratulas
├── digits/                 # Sets de dígitos personalizados
└── tools/                  # Scripts Python
    ├── convert_gfx_font.py
    ├── convert_all_fonts.py
    ├── convert_ttf.py
    ├── convert_bdf_to_json.py
    ├── convert_bdf_to_gfx.py
    ├── convert_json_to_gfx.py
    ├── convert_old_json.py
    └── generate_worldmap_frames.py
```

---

## Formato JSON de Caratula

```json
{
  "name": "mi-caratula",
  "version": 1,
  "author": "Tu Nombre",
  "bgColor": 0,
  "delay": 100,
  "thumbnail": "iVBORw0KGgoAAAANS...",
  "setup": [
    { "type": "image", "x": 0, "y": 0, "image": "base64..." }
  ],
  "sprites": [
    [{ "image": "base64..." }, { "image": "base64..." }]
  ],
  "loop": [
    { "type": "datetime", "x": 0, "y": 50, "content": "H:i", "font": "5x7", "fgColor": 65535 },
    { "type": "sprite", "x": 10, "y": 10, "sprite": 0, "frameDelay": 100 }
  ]
}
```

### Campo `thumbnail`
PNG 64x64 codificado en base64. Se genera automáticamente al exportar desde el editor.
- La interfaz web del reloj lo usa para mostrar previews
- Opcional pero recomendado (~1-4 KB adicionales)
- Si no existe, la interfaz intenta extraer la primera imagen de `setup`, `loop` o `sprites`
```

### Tipos de Elementos

| Tipo | Descripción | Propiedades |
|------|-------------|-------------|
| `image` | Imagen PNG base64 | x, y, image |
| `datetime` | Texto con fecha/hora | x, y, content, font, fgColor, bgColor |
| `text` | Texto estático | x, y, text, font, fgColor, bgColor |
| `line` | Línea | x, y, x2, y2, color |
| `rect` | Rectángulo | x, y, w, h, color, fill |
| `sprite` | Sprite animado | x, y, sprite, frameDelay |

### Formatos de Fecha/Hora

| Código | Descripción | Ejemplo |
|--------|-------------|---------|
| `H` | Hora 24h | 14 |
| `h` | Hora 12h | 02 |
| `i` | Minutos | 30 |
| `s` | Segundos | 45 |
| `A` | AM/PM | PM |
| `d` | Día del mes | 15 |
| `D` | Día abreviado | Dom |
| `l` | Día completo | Domingo |
| `M` | Mes abreviado | Ene |
| `F` | Mes completo | Enero |
| `m` | Mes numérico | 01 |
| `y` | Año 2 dígitos | 26 |
| `Y` | Año 4 dígitos | 2026 |
| `Hw` | Hora en palabras | OCHO |
| `iw` | Minutos en palabras | y media |

**Hora en palabras (español):**
- `Hw`: UNA, DOS, TRES... DOCE (mayúsculas)
- `iw`: en punto, y cuarto, y media, treinta\ny uno, etc.

---

## Colores RGB565

El display usa formato RGB565 (16 bits):
- Rojo: 5 bits (0-31)
- Verde: 6 bits (0-63)
- Azul: 5 bits (0-31)

**Colores comunes:**
| Color | RGB565 | Hex |
|-------|--------|-----|
| Negro | 0 | #000000 |
| Blanco | 65535 | #FFFFFF |
| Rojo | 63488 | #FF0000 |
| Verde | 2016 | #00FF00 |
| Azul | 31 | #0000FF |
| Amarillo | 65504 | #FFFF00 |
| Cyan | 2047 | #00FFFF |
| Magenta | 63519 | #FF00FF |

---

## Servidor Local para Desarrollo

Para probar el editor localmente:

```bash
cd clockface-editor
python -m http.server 8000
```

Abrir `http://localhost:8000` en el navegador.

---

## Enviar Caratula al Reloj

1. Conectar a la misma red WiFi que el reloj
2. En el editor, configurar IP del reloj en "Configuración"
3. Click en "Enviar al Reloj"

O usar el servidor local:
1. Ejecutar `python -m http.server 8080`
2. En el reloj, configurar servidor local con tu IP y puerto 8080
3. Descargar desde el web config del reloj
