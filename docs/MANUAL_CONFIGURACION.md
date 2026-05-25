# Manual de Configuracion Tecnica - Clockwise XE1E

Manual tecnico para desarrollo, compilacion y uso de herramientas.

---

## Tabla de Contenidos

1. [Requisitos Previos](#requisitos-previos)
2. [Clonar el Repositorio](#clonar-el-repositorio)
3. [Compilar y Subir Firmware](#compilar-y-subir-firmware)
4. [Herramientas Web](#herramientas-web)
5. [Editor de Caratulas](#editor-de-caratulas)
6. [Otras Herramientas](#otras-herramientas)
7. [Hardware y Conexiones](#hardware-y-conexiones)
8. [Estructura del Proyecto](#estructura-del-proyecto)
9. [Solucion de Problemas](#solucion-de-problemas)

---

## Requisitos Previos

### Software Necesario

| Software | Proposito | Instalacion |
|----------|-----------|-------------|
| **Git** | Control de versiones | `winget install Git.Git` |
| **Python 3.x** | PlatformIO y servidor local | `winget install Python.Python.3` |
| **PlatformIO** | Compilar firmware ESP32 | `pip install platformio` |
| **VS Code** (opcional) | IDE con extension PlatformIO | `winget install Microsoft.VisualStudioCode` |
| **Chrome/Edge** | Herramientas web (File System API) | Ya instalado |

### Verificar Instalacion

```powershell
git --version
python --version
python -m platformio --version
```

Si `pio` no funciona directamente, usar `python -m platformio` en su lugar.

---

## Clonar el Repositorio

### Primera vez

```powershell
cd D:\Documents\GitHub
git clone https://github.com/XE1E/Clockwise-XE1E.git
cd Clockwise-XE1E
```

### Actualizar repositorio existente

```powershell
cd D:\Documents\GitHub\Clockwise-XE1E
git pull origin main
```

---

## Compilar y Subir Firmware

### Compilar

```powershell
cd D:\Documents\GitHub\Clockwise-XE1E\firmware
python -m platformio run
```

Primera compilacion toma varios minutos (descarga dependencias).

### Subir al ESP32

1. Conectar ESP32 por USB
2. Ejecutar:

```powershell
python -m platformio run --target upload
```

### Subir a puerto especifico

```powershell
# Ver puertos disponibles
python -m platformio device list

# Subir a puerto especifico
python -m platformio run --target upload --upload-port COM5
```

### Monitor Serial (ver logs)

```powershell
python -m platformio device monitor --baud 115200
```

Presiona `Ctrl+C` para salir.

### Compilar y subir en un comando

```powershell
python -m platformio run --target upload; python -m platformio device monitor --baud 115200
```

### Limpiar y recompilar

```powershell
python -m platformio run --target clean
python -m platformio run
```

### Regenerar WebUI comprimido

Si modificas `lib/cw-commons/WebUI.h`, debes regenerar el archivo comprimido:

```powershell
python tools/gzip_webui.py
```

Esto genera `WebUI_gz.h` con el HTML comprimido (reduce ~70% el tamaño de transferencia).

### Problemas al Flashear

| Problema | Solucion |
|----------|----------|
| ESP32 no detectado | Verificar cable USB (debe ser de datos). Instalar drivers CP2102 o CH340 |
| "Failed to connect" | Modo boot manual: mantener BOOT, presionar EN, soltar BOOT, ejecutar upload |
| "Permission denied" | Cerrar Serial Monitor u otros programas usando el puerto |
| Se queda en "Connecting..." | Desconectar panel LED temporalmente, usar modo boot manual |

**Drivers:**
- CP2102: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
- CH340: https://www.wch.cn/download/CH341SER_EXE.html

---

## Herramientas Web

Las herramientas requieren servidor local para acceso completo a archivos.

### Iniciar Servidor Local

```powershell
cd D:\Documents\GitHub\Clockwise-XE1E\clockface-editor
python -m http.server 8000
```

Dejar terminal abierta. Acceder a `http://localhost:8000/`

### URLs de Herramientas

**Servidor Local (desarrollo):**

| Herramienta | URL Local |
|-------------|-----------|
| Editor de Caratulas | http://localhost:8000/index.html |
| Generador Thumbnails | http://localhost:8000/generate-thumbs.html |
| Editor de Fuentes | http://localhost:8000/editor-fuentes.html |
| Font Converter | http://localhost:8000/font-converter.html |

**GitHub Pages (sin servidor local):**

| Herramienta | URL GitHub |
|-------------|------------|
| Editor de Caratulas | https://xe1e.github.io/Clockwise-XE1E/clockface-editor/ |
| Generador Thumbnails | https://xe1e.github.io/Clockwise-XE1E/clockface-editor/generate-thumbs.html |
| Editor de Fuentes | https://xe1e.github.io/Clockwise-XE1E/clockface-editor/editor-fuentes.html |

**Nota:** Desde GitHub Pages algunas funciones no estan disponibles (guardar en carpetas locales).

---

## Editor de Caratulas

### Acceso
- **Local:** http://localhost:8000/index.html
- **GitHub:** https://xe1e.github.io/Clockwise-XE1E/clockface-editor/

### Interfaz Principal

```
┌─────────────────────────────────────────────────────────────────────────┐
│ [Nuevo] [Importar] [Adaptar] [Thumbnail] [Batch] [Exportar] [Probar]   │
├──────────┬────────────────────────────────┬─────────────────────────────┤
│ TOOLS    │                                │ TEMA                        │
│ ┌──────┐ │                                │ - Nombre                    │
│ │Sel Tm│ │      CANVAS 64x64              │ - Version                   │
│ │Txt Im│ │      (preview)                 │ - Autor                     │
│ │Spr Rc│ │                                │ - Color fondo               │
│ │FRc Cr│ │                                │ - Delay                     │
│ │FCr Ln│ │                                ├─────────────────────────────┤
│ └──────┘ │                                │ ELEMENTO                    │
│          │                                │ - Propiedades segun tipo    │
│ ACCIONES │                                ├─────────────────────────────┤
│ [Delete] │                                │ CAPAS                       │
│ [Duplic] │                                │ - Lista de elementos        │
│ [Up/Down]│                                │ - Orden de dibujo           │
│          │                                │ - Checkbox "En Loop"        │
│ PANTALLA │                                │                             │
│ Zoom/Grid│                                │                             │
└──────────┴────────────────────────────────┴─────────────────────────────┘
```

### Elementos Disponibles

| Elemento | Descripcion | Propiedades |
|----------|-------------|-------------|
| **DateTime** | Hora/fecha dinamica | x, y, content, font, fgColor, bgColor |
| **Text** | Texto estatico | x, y, content, font, fgColor, bgColor |
| **Image** | Imagen PNG base64 | x, y, image (base64) |
| **Sprite** | Animacion multiframe | x, y, sprite, frameDelay, loopDelay, moveX, moveY |
| **Rect** | Rectangulo contorno | x, y, width, height, color |
| **FillRect** | Rectangulo relleno | x, y, width, height, color |
| **Circle** | Circulo contorno | x, y, radius, color |
| **FillCircle** | Circulo relleno | x, y, radius, color |
| **Line** | Linea | x, y, x1, y1, color |

### Formatos DateTime

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
| l | Dia semana | Lunes |
| D | Dia corto | Lun |
| F | Mes nombre | Abril |
| M | Mes corto | Abr |
| Hw | Hora en palabras | DIEZ |
| iw | Minutos en palabras | y media |

### Crear Sprite (Animacion)

1. En barra lateral, click en icono de Sprites
2. Click "+ Nuevo" para crear sprite
3. Agregar frames:
   - **Importar:** Cargar imagenes PNG
   - **Dibujar:** Editor de pixeles integrado
4. Click "Play" para previsualizar
5. Cerrar modal
6. Seleccionar herramienta Sprite, click en canvas
7. Configurar propiedades:
   - **frameDelay:** ms entre frames (100 = rapido)
   - **loopDelay:** pausa al terminar ciclo
   - **moveX/moveY:** desplazamiento por frame

### Exportar Caratula

1. Click "Exportar"
2. Opciones:
   - **Copiar:** Al portapapeles
   - **Descargar:** Archivo .json a Descargas
   - **Guardar:** A carpeta seleccionada (recuerda ultima)
3. Checkbox "Incluir thumbnail" genera preview embebido

### Probar en Reloj

1. Click boton verde "Probar en Reloj"
2. Ingresar IP del reloj (ej: 192.168.1.50)
3. Click "Enviar al Reloj"
4. La caratula aparece inmediatamente (temporal, se pierde al reiniciar)

### Atajos de Teclado

| Tecla | Accion |
|-------|--------|
| Flechas | Mover elemento 1px |
| Delete | Eliminar elemento |
| Escape | Deseleccionar |

---

## Otras Herramientas

### Generador de Thumbnails

**URL:** http://localhost:8000/generate-thumbs.html

Genera thumbnails PNG y los inyecta en archivos JSON.

**Pestana "Generar desde carpeta":**
1. Click "Seleccionar Carpeta"
2. Elegir carpeta con archivos .json
3. Se generan todos los thumbnails
4. "Descargar JSONs con Thumbnail" para obtener actualizados

**Pestana "Inyectar en JSON":**
1. Click "Seleccionar JSON"
2. Elegir archivo individual
3. "JSON + Thumb" para descargar actualizado

**Importante:** El thumbnail debe estar al inicio del JSON (antes de `setup`) para que el ESP32 lo encuentre (buffer de 12KB).

### Editor de Fuentes

**URL:** http://localhost:8000/editor-fuentes.html

Editor unificado de fuentes pixel para crear o modificar tipografias.

**Grupos de caracteres:**
- 0-9: Digitos
- A-Z: Mayusculas
- a-z: Minusculas
- !@#: Simbolos
- Acentos: Caracteres especiales en espanol (aeiou con tildes, ene, signos)

**Flujo:**
1. Cargar fuente base:
   - Fuentes predefinidas (pixel-fonts.js)
   - Archivo JSON exportado
   - Archivo BDF externo
   - Galeria (carpeta fonts/)
2. Seleccionar grupo de caracteres
3. Editar pixel por pixel (click/arrastrar)
4. Guardar caracter
5. Exportar:
   - "Guardar en fonts/" → Galeria personalizada
   - "Agregar a predefinidas" → pixel-fonts.js
   - "Descargar" → Archivo JSON backup
   - "Exportar PNGs" → Imagenes individuales

**Importar fuentes BDF:**
- Font Library: https://fontlibrary.org
- Bitmap Fonts: https://github.com/Tecate/bitmap-fonts
- Unifont: https://unifoundry.com/unifont/

**Tamaño:** Configurable 3-32px (ancho y alto independientes)

### Font Converter

**URL:** http://localhost:8000/font-converter.html

Convierte fuentes BDF a formato JavaScript para pixel-fonts.js.

**Uso avanzado:** Preferir Editor de Fuentes para flujo mas simple.

---

## Hardware y Conexiones

### Componentes

| Componente | Descripcion |
|------------|-------------|
| ESP32 DevKit v1 | Microcontrolador (o ESP32-S3 N16R8) |
| Panel LED HUB75 | Matriz 64x64 pixels |
| Fuente 5V 4A | Alimentacion panel LED |
| LDR + 10K (opcional) | Sensor brillo automatico |

### Pines HUB75 → ESP32

| Pin Panel | GPIO | Notas |
|-----------|------|-------|
| R1 | 25 | |
| G1 | 26 | (27 si swap) |
| B1 | 27 | (26 si swap) |
| R2 | 14 | |
| G2 | 12 | (13 si swap) |
| B2 | 13 | (12 si swap) |
| A | 23 | |
| B | 19 | |
| C | 5 | |
| D | 17 | |
| E | 18 | |
| CLK | 16 | |
| LAT | 4 | |
| OE | 15 | |
| GND | GND | |

### Conexion LDR

```
3.3V ──── LDR ────┬──── GPIO35
                  │
                 10K
                  │
                 GND
```

---

## Estructura del Proyecto

```
Clockwise-XE1E/
├── firmware/                    # Codigo ESP32
│   ├── src/main.cpp             # Punto de entrada
│   ├── lib/
│   │   ├── cw-commons/          # WiFi, WebServer, Preferences
│   │   └── cw-cf-0x07/          # Clockface Canvas (JSON)
│   ├── clockfaces/              # Otros clockfaces compilados
│   ├── tools/
│   │   └── gzip_webui.py        # Comprime WebUI.h para transferencia rapida
│   └── platformio.ini           # Configuracion PlatformIO
│
├── clockface-editor/            # Herramientas web
│   ├── index.html               # Editor principal
│   ├── generate-thumbs.html     # Generador thumbnails
│   ├── editor-fuentes.html      # Editor de fuentes pixel
│   ├── js/
│   │   ├── editor.js            # Logica editor
│   │   ├── elements.js          # Clases elementos
│   │   ├── canvas-renderer.js   # Motor renderizado
│   │   ├── pixel-fonts.js       # Fuentes predefinidas
│   │   └── color-utils.js       # RGB <-> RGB565
│   └── fonts/                   # Fuentes personalizadas
│
├── clockfaces/                  # JSONs de caratulas
│   ├── pac-man.json
│   ├── nyan-cat.json
│   └── ...
│
└── docs/                        # Documentacion
    ├── MANUAL_USUARIO.md
    ├── MANUAL_CONFIGURACION.md
    └── MANUAL_EDITOR.md
```

---

## Solucion de Problemas

### Compilacion

| Error | Solucion |
|-------|----------|
| "pio not found" | Usar `python -m platformio` |
| Dependencias no descargan | `python -m platformio lib update` |
| Error de memoria | Limpiar y recompilar |

### Herramientas Web

| Problema | Solucion |
|----------|----------|
| "Seleccionar Carpeta" no aparece | Usar Chrome/Edge (File System API) |
| Fuentes no cargan | Iniciar servidor local |
| Thumbnails no aparecen en reloj | Regenerar con generate-thumbs.html y subir |

### Hardware

| Problema | Solucion |
|----------|----------|
| Colores invertidos | Activar "Intercambiar Azul/Verde" |
| Panel no enciende | Verificar fuente 5V 4A minimo |
| Brillo automatico no funciona | Calibrar min/max del LDR |

---

## Referencias

- **Repositorio:** https://github.com/XE1E/Clockwise-XE1E
- **Editor Online:** https://xe1e.github.io/Clockwise-XE1E/clockface-editor/
- **Libreria HUB75:** https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-DMA
- **PlatformIO:** https://platformio.org/
