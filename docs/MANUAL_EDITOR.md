# Manual del Editor de Clockfaces

Editor visual web para diseñar clockfaces de Clockwise sin escribir JSON manualmente.

---

## Tabla de Contenidos

1. [Ubicacion y Estructura](#ubicacion-y-estructura)
2. [Funcionalidades](#funcionalidades)
3. [Como Iniciar](#como-iniciar)
4. [Interfaz del Editor](#interfaz-del-editor)
5. [Crear un Clockface](#crear-un-clockface)
6. [Herramientas](#herramientas)
7. [Propiedades de Elementos](#propiedades-de-elementos)
8. [Seccion Pantalla](#seccion-pantalla)
9. [Generador de Thumbnails](#generador-de-thumbnails)
10. [Atajos de Teclado](#atajos-de-teclado)
11. [Importar/Exportar](#importar-exportar)
12. [Imagen de Referencia](#imagen-de-referencia)
13. [Adaptar Imagenes](#adaptar-imagenes)
14. [Tips y Mejores Practicas](#tips-y-mejores-practicas)
15. [Ejemplo Completo](#ejemplo-completo)

---

## Ubicacion y Estructura

```
clockface-editor/
├── index.html              # Aplicacion principal
├── css/
│   └── style.css           # Estilos (dark theme)
└── js/
    ├── color-utils.js      # Conversion RGB <-> RGB565
    ├── elements.js         # Clases de elementos (DateTime, Text, etc.)
    ├── canvas-renderer.js  # Motor de renderizado
    └── editor.js           # Logica principal del editor
```

---

## Funcionalidades

| Caracteristica | Descripcion |
|----------------|-------------|
| Canvas 64x64 | Preview pixelado con zoom ajustable (4x-16x) |
| Herramientas | Select, DateTime, Text, Image, Sprite, Rect, FillRect, Circle, FillCircle, Line |
| Colores | Color picker con conversion automatica a RGB565 |
| Capas | Lista de elementos ordenable, separacion setup/loop |
| Import/Export | Compatible con formato JSON de Canvas Clockface |
| Tiempo real | Elementos DateTime se actualizan cada segundo |
| Sprites | Animaciones con multiples frames |
| Atajos de teclado | Flechas para mover, Delete para eliminar, Esc para deseleccionar |
| Grid | Overlay de cuadricula opcional para alineacion |
| Pixels redondeados | Efecto visual que simula LEDs reales |
| Imagen de referencia | Cargar imagen para calcar o usar como fondo |
| Preparar imagen | Recortar, ajustar y convertir imagenes a 64x64 |

---

## Como Iniciar

### Opcion A - Servidor local (Recomendado)

```bash
cd clockface-editor
npx serve .
# Abrir http://localhost:3000
```

### Opcion B - GitHub Pages

1. Ir a Settings > Pages en tu repositorio
2. Seleccionar rama `main` y carpeta `/clockface-editor`
3. URL: `https://xe1e.github.io/Clockwise/clockface-editor/`

### Opcion C - Abrir directamente

- Doble clic en `clockface-editor/index.html`
- Nota: Algunas funciones pueden no funcionar por restricciones CORS

---

## Interfaz del Editor

```
┌──────────────────────────────────────────────────────────────────────┐
│ [Logo]   [Nuevo] [Importar] [Adaptar] [Thumbnail] [Batch] [Exportar]│
├─────────┬─────────────────────────────────────┬──────────────────────┤
│ TOOLS   │                                     │ TEMA                 │
│ ┌─────┐ │                                     │ - Nombre             │
│ │Sel│Tm│ │        CANVAS 64x64                │ - Version            │
│ │Txt│Im│ │        (preview)                   │ - Autor              │
│ │Spr│Rc│ │                                     │ - Color fondo        │
│ │FRc│Cr│ │                                     │ - Delay              │
│ │FCr│Ln│ │                                     ├──────────────────────┤
│ └─────┘ │                                     │ ELEMENTO             │
│         │                                     │ - Posicion X/Y       │
│ ACCIONES│                                     │ - Fuente [preview]   │
│ [Eliminar]                                    │ - Colores            │
│ [Duplicar]                                    ├──────────────────────┤
│ [Subir/Bajar]─────────────────────────────────┤ CAPAS                │
│         │ PANTALLA                            │ - datetime: H:i      │
│ PANTALLA│ Zoom: [-] 8x [+]                    │ - text: Hola         │
│ Zoom 8x │ [x] Grid  [x] Pixels Redondeados    │ - rect               │
│ Grid    │ [Imagen Referencia...]              │                      │
│ Rounded │                                     │                      │
└─────────┴─────────────────────────────────────┴──────────────────────┘
```

### Paneles

| Panel | Ubicacion | Contenido |
|-------|-----------|-----------|
| Toolbar | Izquierda | Herramientas en grid 2 columnas, acciones, seccion Pantalla |
| Canvas | Centro | Vista previa 64x64 con zoom ajustable (4x-16x) |
| Propiedades | Derecha | Tema, elemento seleccionado con preview de fuente |
| Capas | Derecha inferior | Lista de elementos con orden de dibujo |

---

## Crear un Clockface

### Paso 1: Configurar el tema

- Escribir nombre del clockface
- Escribir version (numero entero, ej: 1, 2, 3)
- Escribir nombre del autor
- Seleccionar color de fondo (click en el cuadro de color)
- Ajustar delay (velocidad de animacion en ms)

### Paso 2: Agregar elementos

1. Click en una herramienta (ej: DateTime)
2. Click en el canvas donde quieras colocar el elemento
3. El elemento se crea y queda seleccionado
4. Ajustar propiedades en el panel derecho

### Paso 3: Editar elementos

- Click en un elemento para seleccionarlo
- Arrastrar para mover
- Usar flechas del teclado para ajuste fino (1px)
- Modificar propiedades en el panel derecho

### Paso 4: Organizar capas

- Los elementos se dibujan de abajo hacia arriba
- Usar botones UP/DOWN para reordenar
- Marcar "En Loop" para elementos que se actualizan cada ciclo

### Paso 5: Exportar

1. Click en "Exportar JSON"
2. Copiar el JSON o descargarlo
3. Subir a Clock Club o servidor propio

---

## Herramientas

| Herramienta | Icono | Uso |
|-------------|-------|-----|
| **Select** | Cursor | Seleccionar y mover elementos |
| **DateTime** | Reloj | Mostrar hora/fecha (se actualiza automaticamente) |
| **Text** | T | Texto estatico |
| **Image** | Imagen | Cargar PNG (se convierte a base64) |
| **Sprite** | Pelicula | Animacion con multiples frames |
| **Rect** | Cuadrado vacio | Rectangulo con borde |
| **FillRect** | Cuadrado lleno | Rectangulo relleno |
| **Circle** | Circulo vacio | Circulo con borde |
| **FillCircle** | Circulo lleno | Circulo relleno |
| **Line** | Linea diagonal | Linea de punto a punto |

### Acciones

| Accion | Descripcion |
|--------|-------------|
| **Delete** | Eliminar elemento seleccionado |
| **Duplicate** | Duplicar elemento seleccionado |
| **Move Up** | Subir elemento en la pila de capas |
| **Move Down** | Bajar elemento en la pila de capas |

---

## Propiedades de Elementos

### DateTime y Text

| Propiedad | Descripcion |
|-----------|-------------|
| X, Y | Posicion en pixeles (0-63) |
| Content | Texto o formato de fecha |
| Font | picopixel, tomthumb, square, medium, big, bold |
| fgColor | Color del texto (RGB565) |
| bgColor | Color de fondo del texto (RGB565) |
| En Loop | Si debe redibujarse cada ciclo |

**Preview de fuente:** Al seleccionar una fuente, se muestra un preview con "12:45" para ver como se ve cada tipografia antes de aplicarla.

#### Fuentes disponibles

| Fuente | Altura | Descripcion |
|--------|--------|-------------|
| picopixel | 5px | Muy pequeña, ideal para textos secundarios |
| tomthumb | 4px | La mas pequeña, para espacios reducidos |
| square | 11px | Solo numeros, estilo cuadrado |
| medium | 10px | Tamaño medio, buena legibilidad |
| big | 14px | Grande, para hora principal |
| bold | 13px | Gruesa, alto contraste |

#### Formatos de DateTime (ezTime)

| Codigo | Resultado | Ejemplo |
|--------|-----------|---------|
| H | Hora 24h | 14 |
| h | Hora 12h | 02 |
| i | Minutos | 30 |
| s | Segundos | 45 |
| A | AM/PM | PM |
| d | Dia | 29 |
| m | Mes | 04 |
| Y | Año | 2026 |

**Ejemplos:**
- `H:i` = 14:30
- `H:i:s` = 14:30:45
- `h:i A` = 02:30 PM
- `d/m` = 29/04

### Rect y FillRect

| Propiedad | Descripcion |
|-----------|-------------|
| X, Y | Posicion de esquina superior izquierda |
| Width | Ancho en pixeles |
| Height | Alto en pixeles |
| Color | Color del borde o relleno (RGB565) |

### Line

| Propiedad | Descripcion |
|-----------|-------------|
| X, Y | Punto inicial |
| X1, Y1 | Punto final |
| Color | Color de la linea (RGB565) |

### Circle y FillCircle

| Propiedad | Descripcion |
|-----------|-------------|
| X, Y | Centro del circulo |
| Radius | Radio en pixeles (1-32) |
| Color | Color del borde o relleno (RGB565) |

### Image

| Propiedad | Descripcion |
|-----------|-------------|
| X, Y | Posicion de esquina superior izquierda |
| Image File | Archivo PNG (se convierte a base64) |

### Sprite

| Propiedad | Descripcion |
|-----------|-------------|
| X, Y | Posicion de esquina superior izquierda |
| Sprite | Indice del sprite a usar |
| Frame Delay | Tiempo entre frames (ms) |
| Loop Delay | Pausa al terminar animacion (ms) |
| Move X/Y | Movimiento por frame (px) |

#### Editor de Sprites

1. Click en "Editar" junto al selector de sprite
2. Crear nuevo sprite con "+" 
3. Agregar frames cargando imagenes PNG
4. Preview la animacion con "Play"
5. Eliminar frames con "X" en cada frame

---

## Seccion Pantalla

La seccion "Pantalla" en la barra lateral izquierda agrupa los controles de visualizacion del canvas.

### Controles disponibles

| Control | Descripcion |
|---------|-------------|
| **Zoom** | Ajusta el tamaño del preview (4x a 16x) |
| **Grid** | Muestra cuadricula de pixeles para alineacion |
| **Pixels Redondeados** | Efecto visual que simula LEDs reales |
| **Imagen Referencia** | Abre dialogo para cargar imagen de fondo |

### Pixels Redondeados

El checkbox "Pixels Redondeados" activa un efecto visual que simula como se ven los LEDs reales del display.

- **Desactivado**: Pixeles cuadrados perfectos
- **Activado**: Pixeles con efecto de LED redondo (gradiente radial)

Este efecto se sincroniza con el nivel de zoom para mantener la proporcion correcta. Es solo visual en el editor y no afecta el archivo JSON exportado.

### Grid

La cuadricula ayuda a alinear elementos pixel por pixel. Se ajusta automaticamente al nivel de zoom para coincidir con los pixels del canvas.

---

## Generador de Thumbnails

El editor incluye herramientas para generar imagenes PNG de 64x64 de los clockfaces.

### Thumbnail Individual

1. Click en boton **"Thumbnail"** en la barra superior
2. Se descarga automaticamente un PNG con el nombre del clockface

### Thumbnails en Lote (Batch)

1. Click en boton **"Batch Thumbs"** en la barra superior
2. Se abre un dialogo con opciones:

| Opcion | Descripcion |
|--------|-------------|
| **Seleccionar JSONs** | Elegir archivos .json individuales |
| **Seleccionar Carpeta** | Elegir carpeta con multiples .json (Chrome/Edge) |

3. Los thumbnails se generan y muestran en preview
4. Opciones de guardado:

| Boton | Accion |
|-------|--------|
| **PNG** (en cada thumb) | Descargar thumbnail individual |
| **Descargar Todos** | Descarga cada PNG uno por uno |
| **Guardar en Carpeta** | Elegir carpeta destino (Chrome/Edge) |

### Requisitos del navegador

- **Seleccionar Carpeta** y **Guardar en Carpeta** usan File System Access API
- Solo disponible en Chrome, Edge y navegadores basados en Chromium
- Firefox/Safari: usar seleccion individual de archivos

### Herramienta Standalone

Tambien existe `generate-thumbs.html` que genera thumbnails de todos los clockfaces en la carpeta `clockfaces/`:

```bash
cd clockface-editor
python -m http.server 8000
# Abrir http://localhost:8000/generate-thumbs.html
```

---

## Atajos de Teclado

| Tecla | Accion |
|-------|--------|
| Flechas | Mover elemento seleccionado 1px |
| Delete / Backspace | Eliminar elemento seleccionado |
| Escape | Deseleccionar elemento |

---

## Importar/Exportar

### Exportar

1. Click en "Exportar JSON"
2. El JSON aparece en un modal
3. Click "Copiar" para copiar al portapapeles
4. Click "Descargar" para guardar como archivo .json

### Importar

1. Click en "Importar"
2. Pegar el JSON del clockface en el textarea
3. Click en "Importar"
4. Los elementos apareceran en el canvas
5. Editar y exportar nuevamente

---

## Imagen de Referencia

El editor permite cargar una imagen como guia para calcar o diseñar encima.

### Cargar imagen de referencia

1. Click en "Imagen Referencia"
2. Seleccionar cualquier imagen (foto, dibujo, captura, etc.)
3. La imagen aparece sobre/debajo del canvas

### Controles disponibles

| Control | Descripcion |
|---------|-------------|
| **Opacidad** | Ajustar transparencia de 10% a 90% |
| **Detras** | Marcar para poner la imagen debajo del diseño (ideal para calcar) |
| **X** | Quitar la imagen de referencia |
| **Usar como Fondo** | Convertir la imagen a 64x64 pixeles y agregarla como elemento |

### Convertir a elemento

1. Cargar la imagen de referencia
2. Click en "Usar como Fondo"
3. La imagen se escala automaticamente a 64x64
4. Se agrega como primer elemento (fondo)
5. Puedes ajustar su posicion despues

### Casos de uso

- Calcar un logo o dibujo pixel art
- Usar una foto como base para pixelar
- Importar un diseño de Photoshop/Figma
- Referenciar otra clockface mientras diseñas

---

## Adaptar Imagenes

El boton "Adaptar Imagen" abre una herramienta completa para convertir cualquier imagen a 64x64.

### Caracteristicas de imagen optimas para el display

| Aspecto | Valor Optimo |
|---------|--------------|
| Resolucion final | 64x64 pixeles |
| Formato | PNG |
| Estilo ideal | Pixel art, iconos planos |
| Colores | RGB565 (65,536 max) |
| Tamaño archivo | < 10KB |
| Contraste | Alto |

### Limitacion RGB565

El display usa color de 16 bits, no 24 bits como monitores normales:

```
RGB888 (normal):  16.7 millones de colores
RGB565 (display): 65,536 colores

Rojo/Azul: 32 niveles cada uno
Verde: 64 niveles
```

**Consecuencias:**
- Gradientes suaves se ven "escalonados"
- Tonos similares se fusionan
- Colores saturados funcionan mejor

### Flujo de la herramienta

```
┌─────────────────────────────────────────────────────────────┐
│  1. CARGAR IMAGEN                                           │
│     [Cargar Imagen] - Cualquier foto/logo/dibujo           │
├─────────────────────────────────────────────────────────────┤
│  2. RECORTAR (opcional)                                     │
│     ┌─────────────────────┐                                │
│     │   Arrastra para     │  Seleccion: 200x150 px         │
│     │   seleccionar area  │  [Reset Seleccion]             │
│     └─────────────────────┘                                │
├─────────────────────────────────────────────────────────────┤
│  3. AJUSTAR                                                 │
│     Escala: [Ajustar ▼]   Colores: [16 ▼]                  │
│     Contraste:  [────●────] 120%                           │
│     Brillo:     [──●──────] 100%                           │
│     Saturacion: [────────●] 150%                           │
├─────────────────────────────────────────────────────────────┤
│  4. PREVIEW                    ┌────────┐                   │
│     [Actualizar Preview]       │ 64x64  │                   │
│                                │preview │                   │
│                                └────────┘                   │
├─────────────────────────────────────────────────────────────┤
│  5. AGREGAR                                                 │
│     [Agregar al Canvas] - Inserta como elemento de imagen  │
└─────────────────────────────────────────────────────────────┘
```

### Opciones de escala

| Modo | Descripcion | Mejor para |
|------|-------------|------------|
| Ajustar | Mantiene proporcion, bordes negros | Logos, iconos |
| Recortar | Llena 64x64, corta exceso | Fotos, fondos |
| Estirar | Deforma para llenar | Casi nunca |

### Opciones de color

| Modo | Colores | Uso |
|------|---------|-----|
| Full | 65,536 | Fotos, gradientes |
| 16 | 16 | Estilo retro, logos |
| 8 | 8 | Muy retro |
| 4 | 4 | Estilo Game Boy |

### Recomendaciones para mejores resultados

1. **Recortar primero** - Selecciona solo la parte importante de la imagen
2. **Aumentar contraste** a 120-150% (LEDs necesitan mas contraste)
3. **Aumentar saturacion** a 130-150% (colores mas vivos en LEDs)
4. **Reducir colores** si la imagen tiene muchos tonos similares
5. **Usar "Recortar"** (cover) para fotos de caras/objetos centrados
6. **Probar varias combinaciones** hasta encontrar el mejor resultado

### Que funciona bien vs mal

| Funciona Bien | Funciona Mal |
|---------------|--------------|
| Pixel art 8-bit/16-bit | Fotos realistas detalladas |
| Iconos planos | Gradientes suaves |
| Colores saturados | Tonos pastel |
| Alto contraste | Sombras sutiles |
| Sprites de videojuegos | Texto pequeño (<3px) |
| Logos simples | Fotografias complejas |

---

## Tips y Mejores Practicas

### Rendimiento

- Usar formas geometricas en lugar de imagenes cuando sea posible
- Mantener imagenes pequenas (< 32x32 px ideal)
- Minimizar elementos en el loop
- Todo se guarda en RAM del ESP32 (limitada)

### Organizacion

- **Setup**: Elementos estaticos (fondo, marcos, texto fijo)
- **Loop**: Elementos dinamicos (hora, animaciones)
- Nombrar elementos descriptivamente en el contenido

### Colores

- El editor convierte automaticamente a RGB565
- Algunos colores pueden verse ligeramente diferentes en el display real
- Probar con colores puros primero (rojo, verde, azul, blanco)

### Colores comunes RGB565

| Color | RGB565 |
|-------|--------|
| Negro | 0 |
| Blanco | 65535 |
| Rojo | 63488 |
| Verde | 2016 |
| Azul | 31 |
| Amarillo | 65504 |
| Cyan | 2047 |
| Magenta | 63519 |

---

## Ejemplo Completo

### Crear un Reloj Simple

1. **Fondo:** Dejar negro (0) o elegir color

2. **Agregar texto "HORA":**
   - Tool: Text
   - Click en posicion (20, 5)
   - Content: "HORA"
   - Font: picopixel
   - fgColor: verde (2016)

3. **Agregar marco:**
   - Tool: Rect
   - Click en (5, 15)
   - Width: 54, Height: 30
   - Color: azul (31)

4. **Agregar hora:**
   - Tool: DateTime
   - Click en (15, 25)
   - Content: "H:i:s"
   - Font: big
   - fgColor: blanco (65535)
   - Marcar "En Loop"

5. **Exportar** y usar en Clockwise

### JSON Resultante

```json
{
  "name": "reloj-simple",
  "version": 1,
  "author": "XE1E",
  "bgColor": 0,
  "delay": 250,
  "setup": [
    {
      "type": "text",
      "x": 20,
      "y": 5,
      "content": "HORA",
      "font": "picopixel",
      "fgColor": 2016,
      "bgColor": 0
    },
    {
      "type": "rect",
      "x": 5,
      "y": 15,
      "width": 54,
      "height": 30,
      "color": 31
    }
  ],
  "sprites": [],
  "loop": [
    {
      "type": "datetime",
      "x": 15,
      "y": 25,
      "content": "H:i:s",
      "font": "big",
      "fgColor": 65535,
      "bgColor": 0
    }
  ]
}
```

---

## Firmware - Compilacion y Configuracion

### Requisitos

1. **VS Code**: Descargar de https://code.visualstudio.com/
2. **Extension PlatformIO**:
   - Abrir VS Code
   - Ir a Extensions (Ctrl+Shift+X)
   - Buscar "PlatformIO IDE"
   - Click en Install
   - Reiniciar VS Code

### Compilar y Subir Firmware

#### Desde Terminal (bash)

```bash
# Navegar a la carpeta del firmware
cd firmware

# Solo compilar (verificar errores)
pio run

# Compilar y subir al ESP32 (conectar USB primero)
pio run -t upload

# Ver mensajes de debug en tiempo real
pio device monitor

# Compilar, subir y monitorear en un solo comando
pio run -t upload && pio device monitor
```

#### Desde VS Code (GUI)

| Icono | Accion | Ubicacion |
|-------|--------|-----------|
| ✓ (checkmark) | Build - Compilar | Barra inferior |
| → (flecha) | Upload - Subir | Barra inferior |
| 🔌 (plug) | Monitor - Serial | Barra inferior |
| 🏠 (casa) | PIO Home | Barra lateral |

### Configuracion de Idioma

El firmware soporta **Español** e **Ingles** para nombres de dias y meses.

#### Como cambiar el idioma

1. Conectar el reloj a tu red WiFi
2. Abrir navegador y visitar la IP del reloj
3. En la pagina de Settings, buscar "Spanish"
4. Activar o desactivar segun preferencia
5. El cambio se guarda automaticamente

#### Ejemplos de traduccion

| Ingles | Español |
|--------|---------|
| Monday | Lunes |
| Tuesday | Martes |
| Wednesday | Miercoles |
| January | Enero |
| February | Febrero |
| December | Diciembre |

#### Uso en clockfaces

En el formato de fecha/hora, los nombres se traducen automaticamente:

```
Formato: "l, d F"
Ingles:  "Monday, 30 April"
Español: "Lunes, 30 Abril"
```

### Configuracion via Web

Despues de flashear, el reloj crea una red WiFi temporal o se conecta a la configurada. Visitar su IP para acceder a:

| Opcion | Descripcion |
|--------|-------------|
| Timezone | Zona horaria (America/Mexico_City, Europe/Madrid, etc.) |
| NTP Server | Servidor de tiempo (time.google.com por defecto) |
| Display Bright | Brillo del display (0-255) |
| 24h Format | Formato de hora 24h o 12h |
| Spanish | Idioma español para fechas |
| Swap Blue/Green | Para paneles con orden RBG |
| Display Rotation | Rotar display 0, 90, 180, 270 grados |
| LDR Pin | Pin del sensor de luz (35 por defecto) |
| Auto Bright | Brillo automatico con LDR |

### Estructura del Proyecto

```
firmware/
├── src/
│   └── main.cpp              # Punto de entrada
├── lib/
│   ├── cw-commons/           # Librerias comunes
│   │   ├── CWDateTime.cpp    # Manejo de fecha/hora
│   │   ├── CWPreferences.h   # Configuracion persistente
│   │   ├── CWWebServer.h     # Servidor web de config
│   │   └── WiFiController.h  # Control de WiFi
│   ├── cw-gfx-engine/        # Motor grafico
│   └── [clockface]/          # Clockface seleccionado
├── clockfaces/               # Coleccion de clockfaces
│   ├── cw-cf-0x01/           # Mario Bros
│   ├── cw-cf-0x02/           # Time in Words
│   └── cw-cf-0x07/           # Canvas (JSON)
└── platformio.ini            # Configuracion PlatformIO
```

### Solucionar Problemas Comunes

| Problema | Solucion |
|----------|----------|
| "No se encuentra el puerto" | Instalar driver CP2102 o CH340 |
| "Upload failed" | Presionar BOOT en el ESP32 durante upload |
| "No WiFi" | Solo funciona con redes 2.4GHz |
| "Hora incorrecta" | Verificar timezone y conexion a internet |
| "Colores invertidos" | Activar "Swap Blue/Green" |

---

## Soporte

- **Repositorio:** https://github.com/XE1E/Clockwise
- **Editor:** /clockface-editor/
- **Documentacion general:** ANALISIS.md
