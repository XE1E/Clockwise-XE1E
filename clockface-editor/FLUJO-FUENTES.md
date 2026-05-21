# Flujo de Fuentes - Clockwise Editor

## Archivos Involucrados

| Archivo | Propósito |
|---------|-----------|
| `js/pixel-fonts.js` | Almacén central de fuentes predefinidas (formato PixelFonts) |
| `editor-fuentes.html` | Editor visual para crear/modificar fuentes |
| `index.html` | Editor de carátulas que consume las fuentes |

---

## Opciones de Guardar/Exportar

El editor de fuentes tiene **3 opciones** para guardar tu trabajo:

| Opción | Destino | Formato | Disponible en Editor Carátulas |
|--------|---------|---------|-------------------------------|
| **Guardar JSON (backup)** | Cualquier carpeta | JSON | NO (solo backup personal) |
| **Agregar a galería** | Carpeta `fonts/` | JSON | SÍ (se carga automáticamente) |
| **Agregar a predefinidas** | `pixel-fonts.js` | JavaScript | SÍ (siempre disponible) |

### 1. Guardar JSON (backup)

- Guarda un archivo `.json` en la carpeta que elijas
- **Solo sirve como backup** - no aparece en editor de carátulas
- Útil para: trabajo en progreso, compartir, transferir entre computadoras
- Para usarlo después: "Importar JSON" en el editor de fuentes

### 2. Agregar a galería (`fonts/`)

- Guarda un archivo `.json` en la carpeta `fonts/`
- **SÍ aparece en editor de carátulas** (se carga automáticamente)
- El editor ejecuta `PixelFonts.loadFromFolder('fonts/')` al iniciar
- Fácil de gestionar: agregar/quitar archivos de la carpeta
- Requiere que exista la carpeta `fonts/` junto al editor

### 3. Agregar a predefinidas (`pixel-fonts.js`)

- Inserta código JavaScript directamente en `pixel-fonts.js`
- **SÍ aparece en editor de carátulas** (siempre disponible)
- Las fuentes quedan embebidas en el código
- Funcionan aunque no exista la carpeta `fonts/`
- Más permanente pero menos flexible que galería

### ¿Cuál usar?

| Situación | Opción recomendada |
|-----------|-------------------|
| Guardar trabajo en progreso | **Guardar JSON** |
| Probar una fuente temporalmente | **Agregar a galería** |
| Fuente lista y definitiva | **Agregar a predefinidas** |
| Compartir fuente con alguien | **Guardar JSON** |

---

## Fuentes: Dónde se Almacenan

**IMPORTANTE:** Las fuentes originales NO están en archivos JSON. Están en código JavaScript.

El editor de carátulas carga fuentes de **dos lugares**:

```
Editor Carátulas
      │
      ├── 1. pixel-fonts.js (predefinidas - siempre disponibles)
      │       └── PixelFonts.picopixel, .square, .medium, etc.
      │
      └── 2. fonts/ carpeta (galería - cargadas dinámicamente)
              └── *.json files → se agregan a PixelFonts al iniciar
```

```javascript
// editor.js - carga fuentes de ambos lugares
const loaded = await PixelFonts.loadFromFolder('fonts/');
for (const [name, font] of Object.entries(PixelFonts)) { ... }
```

---

## Formatos de Fuente

### Formato PixelFonts (pixel-fonts.js)

Formato compacto optimizado para renderizado. Usado en el firmware y editores.

```javascript
PixelFonts.miFuente = {
    bitmaps: [0x00, 0x7C, 0x82, ...],  // Bytes empaquetados de todos los glifos
    glyphs: {
        32: [0, 0, 0, 4, 0, 0],        // espacio
        65: [0, 5, 7, 6, 0, 0],        // 'A'
        // [offset, width, height, xAdvance, xOffset, yOffset]
    },
    lineHeight: 8
};
```

**Estructura de glyphs:**
- `offset`: Posición inicial en el array bitmaps
- `width`: Ancho del glifo en píxeles
- `height`: Alto del glifo en píxeles
- `xAdvance`: Espacio horizontal hasta el siguiente carácter
- `xOffset`: Desplazamiento horizontal del glifo
- `yOffset`: Desplazamiento vertical desde la línea base

### Formato Editor (JSON exportado)

Formato expandido para edición visual. Cada carácter es una matriz 2D.

```javascript
{
    "name": "miFuente",
    "lineHeight": 8,
    "chars": {
        "65": [[0,1,1,1,0], [1,0,0,0,1], ...],  // 'A' como grid de píxeles
        "66": [[1,1,1,0,0], [1,0,0,1,0], ...],  // 'B'
    }
}
```

---

## Flujos de Trabajo

### 1. Crear Fuente Nueva desde Cero

```
Editor Fuentes → Exportar JSON → [opcional] Agregar a Predefinidas
```

1. Abrir `editor-fuentes.html`
2. Configurar dimensiones de celda (ancho × alto)
3. Dibujar cada carácter en el grid
4. **Guardar como JSON**: Archivo local para backup/edición futura
5. **Agregar a Predefinidas**: Inserta directamente en `pixel-fonts.js`

### 2. Editar Fuente Predefinida

```
pixel-fonts.js → Editor Fuentes → Modificar → pixel-fonts.js
```

1. En Editor Fuentes, usar **"Cargar Predefinida"**
2. Seleccionar la fuente del dropdown
3. La fuente se convierte de PixelFonts → formato Editor automáticamente
4. Editar los caracteres necesarios
5. Guardar:
   - **JSON**: Para backup o continuar editando después
   - **Agregar a Predefinidas**: Sobrescribe o crea nueva en `pixel-fonts.js`

### 3. Importar Fuente desde JSON

```
Archivo JSON → Editor Fuentes → Editar → Exportar
```

1. En Editor Fuentes, usar **"Importar JSON"**
2. Seleccionar archivo `.json` previamente exportado
3. El editor detecta automáticamente el formato:
   - Si es formato Editor: carga directo
   - Si es formato PixelFonts: convierte automáticamente
4. Continuar editando o exportar

### 4. Usar Fuente en Carátula

```
pixel-fonts.js → Editor Carátulas → Renderizado
```

1. La fuente debe existir en `pixel-fonts.js`
2. Abrir `index.html` (Editor de Carátulas)
3. **Recargar página** si modificaste pixel-fonts.js recientemente
4. La fuente aparece en el dropdown de fuentes
5. Seleccionar fuente y usarla en elementos de texto

### 5. Gestionar Fuentes (Galería)

```
Editor Fuentes → Ver galería → Editar/Eliminar
```

1. En Editor Fuentes, clic en **"Ver galería / Gestionar"**
2. Se abre un modal con todas las fuentes de `pixel-fonts.js`
3. Cada fuente muestra:
   - Nombre y dimensiones (ej: `picopixel 5x7`)
   - Cantidad de caracteres
   - Vista previa: `ABC abc 123`
4. Acciones disponibles:
   - **Editar**: Carga la fuente en el editor
   - **Eliminar**: Borra la fuente de `pixel-fonts.js` (pide confirmación)
5. Cerrar galería: clic en X, tecla Escape, o clic fuera del modal

---

## Conversión entre Formatos

### PixelFonts → Editor (al cargar predefinida)

```javascript
// Se ejecuta automáticamente en loadSystemFont() y convertPixelFontsToEditor()

1. Lee glyphs para obtener dimensiones de cada carácter
2. Calcula altura total: maxYEnd - minYOffset
3. Desempaqueta bitmaps bit a bit
4. Genera matriz 2D para cada carácter
5. Aplica yOffset para posicionar correctamente
```

### Editor → PixelFonts (al agregar a predefinidas)

```javascript
// Se ejecuta en addToPredefined()

1. Lee cada carácter del objeto chars
2. Calcula offset acumulado en bitmaps
3. Empaqueta píxeles en bytes (8 bits por byte)
4. Genera array glyphs con metadatos
5. Formatea como código JavaScript
6. Inserta en pixel-fonts.js
```

---

## Persistencia de Directorios

El editor recuerda las últimas carpetas usadas mediante IndexedDB:

| Clave | Uso |
|-------|-----|
| `fontJsonDir` | Carpeta para importar/exportar JSON |
| `pixelFontsDir` | Carpeta donde está pixel-fonts.js |

Al guardar/cargar, el editor:
1. Intenta recuperar el directorio guardado
2. Verifica permisos (pide si es necesario)
3. Si falla, muestra selector de carpeta
4. Guarda el nuevo directorio para futuro uso

---

## Diagrama de Flujo General

```
                    ┌─────────────────────┐
                    │   pixel-fonts.js    │
                    │  (fuentes compactas)│
                    └──────────┬──────────┘
                               │
              ┌────────────────┼────────────────┐
              │                │                │
              ▼                ▼                ▼
    ┌─────────────────┐ ┌─────────────┐ ┌─────────────────┐
    │ Editor Fuentes  │ │   Editor    │ │    Firmware     │
    │ (editar/crear)  │ │  Carátulas  │ │   (reloj real)  │
    └────────┬────────┘ │ (usar en UI)│ └─────────────────┘
             │          └─────────────┘
             │
             ▼
    ┌─────────────────┐
    │  Archivos JSON  │
    │    (backup)     │
    └─────────────────┘
```

---

## Solución de Problemas

### La fuente no aparece en Editor Carátulas
1. Verificar que se guardó correctamente en `pixel-fonts.js`
2. Recargar la página con **Ctrl+Shift+R** (forzar sin caché)
3. Abrir consola (F12) y verificar que no hay errores de sintaxis

### JSON no carga correctamente
1. Verificar formato válido JSON (usar validador online si es necesario)
2. El editor acepta ambos formatos (Editor y PixelFonts)
3. Revisar consola por errores específicos

### Caracteres se ven desplazados
1. Verificar valores de `yOffset` en los glyphs
2. El `yOffset` controla posición vertical desde línea base
3. Valores negativos mueven hacia arriba, positivos hacia abajo

### "Agregar a Predefinidas" no funciona
1. Debe seleccionar la carpeta donde está `pixel-fonts.js`
2. El navegador pedirá permisos de escritura
3. Verificar que el archivo no esté abierto en otro programa

---

## Códigos de Formato para Texto (Editor Carátulas)

| Código | Descripción | Ejemplo |
|--------|-------------|---------|
| `H` | Hora 24h | 14 |
| `h` | Hora 12h | 2 |
| `i` | Minutos | 05 |
| `s` | Segundos | 30 |
| `Hw` | Hora en palabras | CATORCE |
| `iw` | Minutos en palabras | CINCO |
| `d` | Día del mes | 15 |
| `m` | Mes numérico | 03 |
| `D` | Día abreviado | LUN |
| `l` | Día completo | LUNES |
| `M` | Mes abreviado | MAR |
| `F` | Mes completo | MARZO |
| `y` | Año corto | 24 |
| `Y` | Año completo | 2024 |
