# Análisis del Proyecto Clockwise

**Repositorio:** https://github.com/yuan910715/clockwise  
**Fork de:** https://github.com/jnthas/clockwise  
**Sitio web:** https://clockwise.page  
**Fecha de análisis:** 2026-04-29

---

## Descripción General

Clockwise es un **reloj de pared inteligente DIY** (hazlo tú mismo) basado en matrices LED RGB. Es una plataforma open-source que permite crear "Clockfaces" (carátulas de reloj) personalizadas con animaciones y efectos visuales.

La idea surgió mientras el autor trabajaba con matrices LED 64x64. Estas pantallas tienen el tamaño aproximado de un reloj de pared, y combinadas con un ESP32, se obtiene conectividad WiFi, Bluetooth, botones táctiles y soporte para sensores.

---

## Información del Repositorio

| Campo | Valor |
|-------|-------|
| **Nombre** | clockwise |
| **Creado** | 2024-04-25 |
| **Última actualización** | 2024-06-25 |
| **Lenguaje principal** | C++ (73,935 bytes) |
| **Otros lenguajes** | CMake (611 bytes) |
| **Licencia** | MIT |
| **Estrellas** | 3 |
| **Forks** | 0 |
| **Rama principal** | main |

---

## Hardware Requerido

### Componentes Principales
1. **Matriz LED HUB75/HUB75E** - 64x64 píxeles
2. **ESP32** - Microcontrolador con WiFi/Bluetooth
3. **Fuente de alimentación** - Mínimo 3A

### Placas Compatibles Recomendadas
- **ESP32 Trinity** (Brian Lough) - Plug & play
- **Mario Clock PCB** (@Alexvanheu) - PCB diseñado específicamente
- **ESP32 D1 Mini RGB Matrix Shield** (@hallard)

### Conexionado
El proyecto usa la biblioteca ESP32-HUB75-MatrixPanel-I2S-DMA con el siguiente cableado por defecto:
- Documentación de cableado en el README del repositorio
- Compatible con cualquier biblioteca Adafruit GFX

---

## Estructura del Proyecto

```
clockwise/
├── .github/
│   └── workflows/
│       ├── clockwise-ci.yml      # CI/CD principal
│       └── esp-idf.yml           # Build con esp-idf
├── firmware/
│   ├── lib/
│   │   ├── cw-commons/           # Utilidades comunes
│   │   │   ├── CWDateTime.cpp/h  # Manejo de fecha/hora
│   │   │   ├── CWHttpClient.h    # Cliente HTTP
│   │   │   ├── CWPreferences.h   # Almacenamiento de preferencias
│   │   │   ├── CWWebServer.h     # Servidor web embebido
│   │   │   ├── IClockface.h      # Interfaz para clockfaces
│   │   │   ├── Icons.h           # Iconos del sistema
│   │   │   ├── SettingsWebPage.h # Página de configuración
│   │   │   ├── StatusController.h# Control de estado
│   │   │   ├── WiFiController.h  # Control de WiFi
│   │   │   └── picopixel.h       # Fuente de texto pequeña
│   │   └── cw-gfx-engine/        # Motor gráfico
│   │       ├── ColorUtil.h       # Utilidades de color
│   │       ├── EventBus.cpp/h    # Sistema de eventos
│   │       ├── EventTask.h       # Tareas basadas en eventos
│   │       ├── Game.h            # Base para animaciones
│   │       ├── ImageUtils.h      # Utilidades de imagen
│   │       ├── Locator.cpp/h     # Service locator pattern
│   │       ├── Macros.h          # Macros útiles
│   │       ├── Object.h          # Objeto base
│   │       ├── Sprite.cpp/h      # Sistema de sprites
│   │       └── Tile.h            # Sistema de tiles
│   ├── src/
│   │   └── main.cpp              # Punto de entrada
│   ├── test/
│   │   ├── test_embedded/        # Tests para ESP32
│   │   └── test_native/          # Tests nativos
│   └── platformio.ini            # Configuración PlatformIO
├── clockfaces/                   # Carpeta de clockfaces (submodules)
├── CMakeLists.txt                # Build con esp-idf
├── CHANGELOG.md                  # Historial de cambios
├── LICENSE                       # Licencia MIT
└── README.md                     # Documentación principal
```

---

## Clockfaces Disponibles

| Nombre | Repositorio | Descripción |
|--------|-------------|-------------|
| **Mario Bros. Clock** | cw-cf-0x01 | Reloj temático de Mario Bros |
| **Time in Words** | cw-cf-0x02 | Hora escrita en palabras |
| **World Map Clock** | cw-cf-0x03 | Mapa mundial con zonas horarias |
| **Castlevania Clock Tower** | cw-cf-0x04 | Temática de Castlevania |
| **Pacman** | cw-cf-0x05 | Temática de Pacman |
| **Pokedex** | cw-cf-0x06 | Temática de Pokémon |
| **Canvas** | cw-cf-0x07 | Renderiza temas desde JSON (especial) |

### Canvas Clockface
Canvas es un tipo especial de clockface que puede renderizar diferentes temas descritos en un archivo JSON, permitiendo crear nuevas carátulas sin programar.

---

## Características del Sistema

### Conectividad
- **WiFi 2.4GHz** - Requerido (no soporta 5GHz)
- **Configuración Improv** - Setup WiFi vía web
- **Servidor web embebido** - Página de configuración

### Sincronización de Tiempo
- **NTP** - Sincronización automática de hora
- **Servidor por defecto:** time.google.com (configurable)
- **Zonas horarias** - Formato IANA (America/New_York, Europe/Paris, etc.)
- **Posix Timezone String** - Opcional para evitar lookups remotos

### Configuración Disponible
- Zona horaria
- Servidor NTP
- Intercambio de pines Blue/Green (para displays RBG)
- Brillo del display
- Formato 12h/24h
- Brillo automático (con LDR)
- Pin GPIO del LDR
- Rotación del display

### Sensor de Luz (LDR)
- Pin por defecto: GPIO 35
- Permite ajuste automático de brillo según luz ambiental

---

## Métodos de Instalación

### 1. Web Flashing (Recomendado)
1. Ir a https://clockwise.page
2. Seleccionar clockface deseado
3. Conectar ESP32 por USB
4. Click en "Flash"
5. Seleccionar puerto USB
6. Seguir asistente de instalación
7. Configurar WiFi con Improv

### 2. PlatformIO
```bash
# Clonar repositorio
git clone https://github.com/yuan910715/clockwise.git
cd clockwise

# Clonar submodules de clockfaces
git submodule update --init firmware/clockfaces

# Crear enlace simbólico al clockface deseado
cd firmware/lib
ln -s ../clockfaces/cw-cf-0x02/ timeinwords

# Compilar y subir con PlatformIO
```

### 3. esp-idf
```bash
# Clonar con submodules
git clone --recurse-submodules https://github.com/yuan910715/clockwise.git

# Configurar
idf.py reconfigure
idf.py menuconfig  # Seleccionar clockface

# Compilar y flashear
idf.py flash
idf.py monitor
```

---

## Arquitectura de Software

### Patrón Service Locator
El proyecto usa el patrón Service Locator (`Locator.cpp/h`) para gestionar dependencias y servicios globales.

### Sistema de Eventos
`EventBus` proporciona un sistema de publicación/suscripción para comunicación entre componentes.

### Interfaz IClockface
Todas las clockfaces implementan la interfaz `IClockface`, permitiendo intercambiarlas fácilmente.

### Motor Gráfico (cw-gfx-engine)
- **Sprites** - Imágenes animadas
- **Tiles** - Mapas de baldosas
- **ColorUtil** - Conversiones y manipulación de colores
- **ImageUtils** - Carga y procesamiento de imágenes

---

## Crear Nuevos Clockfaces

### Opción 1: Código C++
Partir del template: https://github.com/jnthas/cw-cf-0x00

### Opción 2: Canvas (JSON) - Sin Programar
Usar el sistema Canvas para crear clockfaces describiendo elementos en JSON.

---

## Sistema Canvas - Guía Completa

El Canvas Clockface (0x07) es un sistema basado en JSON que permite crear carátulas personalizadas **sin necesidad de programar en C++**.

### Cómo Funciona

1. El ESP32 descarga un archivo JSON desde un servidor
2. Interpreta las instrucciones del JSON
3. Renderiza los elementos en la matriz LED

### Configuración en Clockwise

| Parámetro | Valor por defecto | Descripción |
|-----------|-------------------|-------------|
| **Server Address** | `raw.githubusercontent.com` | Servidor donde están los JSON |
| **Description File** | nombre del tema | Archivo JSON (sin extensión) |

### Estructura del JSON

```json
{
  "name": "mi-tema",
  "version": 1,
  "author": "tu-nombre",
  "bgColor": 0,
  "delay": 250,
  "setup": [],
  "sprites": [],
  "loop": []
}
```

| Campo | Tipo | Descripción |
|-------|------|-------------|
| `name` | string | Identificador del tema |
| `version` | number | Versión del tema |
| `author` | string | Nombre del autor |
| `bgColor` | number | Color de fondo (formato decimal RGB565) |
| `delay` | number | Intervalo del loop en milisegundos |
| `setup` | array | Elementos que se dibujan una vez |
| `sprites` | array | Secuencias de imágenes para animaciones |
| `loop` | array | Elementos que se redibujan cada ciclo |

### Primitivas de Dibujo

#### 1. DateTime - Mostrar Hora/Fecha
```json
{
  "type": "datetime",
  "x": 6,
  "y": 11,
  "content": "H:i:s",
  "font": "square",
  "fgColor": 65535,
  "bgColor": 0
}
```

**Formatos de fecha/hora (ezTime):**
| Código | Resultado | Ejemplo |
|--------|-----------|---------|
| `H` | Hora 24h | 14 |
| `h` | Hora 12h | 02 |
| `i` | Minutos | 30 |
| `s` | Segundos | 45 |
| `A` | AM/PM | PM |
| `d` | Día | 29 |
| `m` | Mes | 04 |
| `Y` | Año | 2026 |

#### 2. Text - Texto Estático
```json
{
  "type": "text",
  "x": 2,
  "y": 56,
  "content": "Hola Mundo",
  "font": "picopixel",
  "fgColor": 65535,
  "bgColor": 0
}
```

**Fuentes disponibles:**
- `picopixel` - Muy pequeña (3px)
- `square` - Cuadrada, buena para hora
- `medium` - Tamaño medio
- `big` - Grande

#### 3. Image - Imagen PNG en Base64
```json
{
  "type": "image",
  "x": 0,
  "y": 0,
  "image": "iVBORw0KGgoAAAANSUhEUgAAAAE..."
}
```

#### 4. Line - Línea
```json
{
  "type": "line",
  "x": 0,
  "y": 0,
  "x1": 63,
  "y1": 63,
  "color": 65535
}
```

#### 5. Rect - Rectángulo (contorno)
```json
{
  "type": "rect",
  "x": 10,
  "y": 10,
  "width": 20,
  "height": 15,
  "color": 65535
}
```

#### 6. Fillrect - Rectángulo Relleno
```json
{
  "type": "fillrect",
  "x": 10,
  "y": 10,
  "width": 20,
  "height": 15,
  "color": 65535
}
```

### Sistema de Sprites (Animaciones)

Los sprites son secuencias de imágenes que se muestran en el loop:

```json
"sprites": [
  [
    {"image": "base64_frame1..."},
    {"image": "base64_frame2..."},
    {"image": "base64_frame3..."}
  ]
]
```

Uso en el loop:
```json
"loop": [
  {
    "type": "sprite",
    "x": 17,
    "y": 27,
    "sprite": 0
  }
]
```

### Colores - Formato RGB565

Los colores usan formato **RGB565 en decimal**. Conversión:

```
RGB565 = (R >> 3) << 11 | (G >> 2) << 5 | (B >> 3)
```

| Color | RGB | RGB565 Decimal |
|-------|-----|----------------|
| Negro | 0,0,0 | 0 |
| Blanco | 255,255,255 | 65535 |
| Rojo | 255,0,0 | 63488 |
| Verde | 0,255,0 | 2016 |
| Azul | 0,0,255 | 31 |
| Amarillo | 255,255,0 | 65504 |
| Cyan | 0,255,255 | 2047 |
| Magenta | 255,0,255 | 63519 |

### Coordenadas

- Display: **64x64 píxeles**
- Origen (0,0): esquina superior izquierda
- X: 0-63 (izquierda a derecha)
- Y: 0-63 (arriba a abajo)

### Limitaciones Importantes

- Todo se almacena en **RAM del ESP32** (limitada)
- Evitar imágenes grandes (64x64 o mayores)
- Preferir formas geométricas sobre imágenes
- Eliminar metadatos de PNG al exportar
- Optimizar tamaño de base64

### Ejemplo Completo: Reloj Simple

```json
{
  "name": "reloj-simple",
  "version": 1,
  "author": "mi-nombre",
  "bgColor": 0,
  "delay": 1000,
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
      "height": 25,
      "color": 31
    }
  ],
  "sprites": [],
  "loop": [
    {
      "type": "datetime",
      "x": 10,
      "y": 22,
      "content": "H:i:s",
      "font": "big",
      "fgColor": 65535,
      "bgColor": 0
    }
  ]
}
```

### Desarrollo Local

Para probar temas sin subir a GitHub:

1. Clonar clock-club: `git clone https://github.com/jnthas/clock-club`
2. Ir a carpeta local: `cd clock-club/local`
3. Ejecutar servidor: `python local-server.py`
4. En Clockwise Settings:
   - Server Address: `tu-ip-local` (ej: 192.168.1.100)
   - Description file: `nombre-archivo` (sin .json)
5. Reiniciar Clockwise

### Temas Disponibles en Clock Club

| Tema | Descripción |
|------|-------------|
| `donkey-kong` | Temática Donkey Kong |
| `nyan-cat` | Nyan Cat animado |
| `pac-man` | Temática Pacman |
| `star-wars` | Star Wars |
| `snoopy3` | Snoopy |
| `retro-computer` | Computadora retro |
| `clock-club` | Logo del club |
| `goomba-move` | Goomba animado |

---

## Propuestas para Facilitar Diseño de Carátulas

### Problema Actual
Crear carátulas requiere:
1. Escribir JSON manualmente
2. Convertir imágenes a base64
3. Calcular colores RGB565
4. Prueba y error sin preview

### Soluciones Propuestas

#### 1. Editor Visual Web (Recomendado)
Crear una aplicación web con:
- Canvas interactivo 64x64
- Drag & drop de elementos
- Paleta de colores con conversión automática
- Import de imágenes con conversión a base64
- Preview en tiempo real
- Exportar JSON listo para usar

**Tecnología sugerida:** React/Vue + HTML Canvas

#### 2. Plugin para Figma/Photoshop
- Diseñar en herramienta conocida
- Exportar a formato Canvas JSON

#### 3. Herramientas CLI
- Convertidor de imágenes a base64 optimizado
- Validador de JSON
- Calculadora RGB565

#### 4. Simulador de Display
- Aplicación que renderiza el JSON
- Sin necesidad de hardware físico
- Hot-reload al guardar cambios

---

## Propuestas de Nuevas Funcionalidades

### 1. Integraciones y Notificaciones

| Idea | Descripción | Complejidad |
|------|-------------|-------------|
| **Clima en tiempo real** | Mostrar temperatura, humedad, iconos del clima (API OpenWeather) | Media |
| **Calendario/Eventos** | Próximos eventos de Google Calendar | Alta |
| **Notificaciones push** | Alertas de teléfono vía Bluetooth | Alta |
| **Home Assistant/MQTT** | Integración con domótica | Media |

### 2. Funcionalidades de Productividad

| Idea | Descripción | Complejidad |
|------|-------------|-------------|
| **Pomodoro Timer** | Temporizador visual 25/5 minutos con indicador de progreso | Baja |
| **Alarma visual** | Animaciones de despertar, colores progresivos | Baja |
| **Countdown** | Cuenta regresiva para eventos importantes | Baja |

### 3. Entretenimiento

| Idea | Descripción | Complejidad |
|------|-------------|-------------|
| **Visualizador de audio** | Reacción a música con micrófono (FFT) | Media |
| **Mini-juegos** | Snake, Tetris, Pong controlados por web/botones | Media |
| **Modo ambiente** | Animaciones relajantes (fuego, agua, aurora boreal) | Baja |

### 4. Información en Vivo

| Idea | Descripción | Complejidad |
|------|-------------|-------------|
| **Crypto/Stocks ticker** | Precios en tiempo real de criptomonedas o acciones | Media |
| **Resultados deportivos** | Marcadores en vivo de equipos favoritos | Media |
| **Mensajes personalizados** | API REST para mostrar texto desde otras apps | Baja |

### 5. Sensores Adicionales

| Idea | Descripción | Complejidad |
|------|-------------|-------------|
| **Sensor temperatura/humedad** | DHT22 o BME280 integrado en display | Baja |
| **Detector de presencia** | PIR para encender/apagar automáticamente | Baja |
| **Sensor de sonido** | Activar display con palmadas o voz | Baja |

### 6. Mejoras del Sistema

| Idea | Descripción | Complejidad |
|------|-------------|-------------|
| **Rotación automática de clockfaces** | Cambiar carátula cada X minutos | Baja |
| **OTA Updates** | Actualizaciones de firmware sin cable USB | Media |
| **App móvil** | Configuración desde el teléfono (Flutter/React Native) | Alta |
| **Modo nocturno programado** | Apagar o atenuar display por horario | Baja |

### Recomendaciones para Empezar

Las funcionalidades de **complejidad baja** que agregan más valor son:

1. **Pomodoro Timer** - Muy útil para productividad, fácil de implementar
2. **Sensor de temperatura** - Hardware barato (~$2), código simple
3. **Rotación de clockfaces** - Solo requiere lógica, sin hardware adicional
4. **Modo nocturno programado** - Funcionalidad muy solicitada

---

## Web Flashing - Documentacion Tecnica

El sistema de flasheo web permite actualizar el firmware del ESP32 directamente desde el navegador.

### Tecnologia: ESP Web Tools

- **Biblioteca:** [ESP Web Tools](https://esphome.github.io/esp-web-tools/) de ESPHome
- **API:** WebSerial (Chrome 89+, Edge 89+, Opera 75+)
- **Requisito:** HTTPS obligatorio

### Estructura de Firmware

```
static/firmware/
├── base_firmware/              # Archivos comunes a todos los clockfaces
│   ├── bootloader_dio_40m.bin  # Bootloader ESP32
│   ├── partitions.bin          # Tabla de particiones
│   └── boot_app0.bin           # OTA data
└── cw-cf-0x07/                 # Clockface especifico
    ├── manifest.json           # Descriptor para ESP Web Tools
    └── firmware.bin            # Aplicacion compilada
```

### Mapa de Memoria ESP32

| Archivo | Offset | Hex | Descripcion |
|---------|--------|-----|-------------|
| bootloader.bin | 4096 | 0x1000 | Bootloader |
| partitions.bin | 32768 | 0x8000 | Tabla de particiones |
| boot_app0.bin | 57344 | 0xE000 | OTA data inicial |
| firmware.bin | 65536 | 0x10000 | Aplicacion principal |

### Formato manifest.json

```json
{
    "name": "Clockwise Canvas",
    "version": "1.4.2",
    "builds": [
        {
            "chipFamily": "ESP32",
            "improv": true,
            "parts": [
                {"path": "../base_firmware/bootloader_dio_40m.bin", "offset": 4096},
                {"path": "../base_firmware/partitions.bin", "offset": 32768},
                {"path": "../base_firmware/boot_app0.bin", "offset": 57344},
                {"path": "firmware.bin", "offset": 65536}
            ]
        }
    ]
}
```

### Componente HTML

```html
<script type="module" 
  src="https://unpkg.com/esp-web-tools@10/dist/web/install-button.js?module">
</script>

<esp-web-install-button manifest="static/firmware/cw-cf-0x07/manifest.json">
    <button slot="activate">Flashear</button>
    <span slot="unsupported">Navegador no compatible</span>
    <span slot="not-allowed">Requiere HTTPS</span>
</esp-web-install-button>
```

### Improv WiFi

ESP Web Tools soporta Improv para configurar WiFi post-flasheo:
- Se activa con `"improv": true` en el manifest
- El firmware debe incluir la biblioteca Improv-WiFi-Library
- Permite configurar WiFi sin necesidad de AP mode

### Despliegue

**GitHub Pages (Recomendado):**
- HTTPS automatico
- Gratis para repositorios publicos/privados
- Rama `gh-pages` o carpeta `/docs`

**Servidor propio:**
- Requiere certificado SSL
- Para desarrollo local: usar mkcert

### Archivos Creados

Se ha creado la carpeta `web-flasher/` con:
- `index.html` - Pagina de flasheo personalizada
- `static/firmware/` - Estructura para binarios
- `README.md` - Documentacion de uso

---

## Editor Visual de Clockfaces

Se ha desarrollado un editor web interactivo para diseñar clockfaces sin escribir JSON manualmente.

### Ubicacion

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

### Funcionalidades

| Caracteristica | Descripcion |
|----------------|-------------|
| Canvas 64x64 | Preview pixelado con zoom ajustable (4x-16x) |
| Herramientas | Select, DateTime, Text, Image, Rect, FillRect, Line |
| Colores | Color picker con conversion automatica a RGB565 |
| Capas | Lista de elementos ordenable, separacion setup/loop |
| Import/Export | Compatible con formato JSON de Canvas Clockface |
| Tiempo real | Elementos DateTime se actualizan cada segundo |
| Atajos de teclado | Flechas para mover, Delete para eliminar, Esc para deseleccionar |
| Grid | Overlay de cuadricula opcional para alineacion |

### Como Usar el Editor

#### 1. Iniciar el Editor

**Opcion A - Servidor local:**
```bash
cd clockface-editor
npx serve .
# Abrir http://localhost:3000
```

**Opcion B - GitHub Pages:**
- Configurar en Settings > Pages
- URL: https://xe1e.github.io/Clockwise/clockface-editor/

**Opcion C - Abrir directamente:**
- Doble clic en `clockface-editor/index.html`
- (Algunas funciones pueden no funcionar por restricciones CORS)

#### 2. Interfaz del Editor

```
┌─────────────────────────────────────────────────────────────────┐
│  [Logo]                          [Nuevo] [Importar] [Exportar]  │
├────────┬────────────────────────────────────┬───────────────────┤
│        │                                    │ TEMA              │
│ TOOLS  │                                    │ - Nombre          │
│        │         CANVAS 64x64               │ - Autor           │
│ Select │         (preview)                  │ - Color fondo     │
│ Time   │                                    │ - Delay           │
│ Text   │                                    ├───────────────────┤
│ Image  │                                    │ ELEMENTO          │
│ Rect   │                                    │ - Posicion X/Y    │
│ FillR  │                                    │ - Propiedades     │
│ Line   ├────────────────────────────────────┤ - Colores         │
│        │  [-] [8x] [+]  [x] Grid            ├───────────────────┤
│ DELETE │                                    │ CAPAS             │
│ DUPLIC │                                    │ - datetime: H:i   │
│ UP/DOWN│                                    │ - text: Hola      │
└────────┴────────────────────────────────────┴───────────────────┘
```

#### 3. Crear un Clockface Paso a Paso

**Paso 1: Configurar el tema**
- Escribir nombre del clockface
- Escribir nombre del autor
- Seleccionar color de fondo (click en el cuadro de color)
- Ajustar delay (velocidad de animacion en ms)

**Paso 2: Agregar elementos**
1. Click en una herramienta (ej: DateTime)
2. Click en el canvas donde quieras colocar el elemento
3. El elemento se crea y queda seleccionado
4. Ajustar propiedades en el panel derecho

**Paso 3: Editar elementos**
- Click en un elemento para seleccionarlo
- Arrastrar para mover
- Usar flechas del teclado para ajuste fino
- Modificar propiedades en el panel derecho

**Paso 4: Organizar capas**
- Los elementos se dibujan de abajo hacia arriba
- Usar botones UP/DOWN para reordenar
- Marcar "En Loop" para elementos que se actualizan

**Paso 5: Exportar**
1. Click en "Exportar JSON"
2. Copiar el JSON o descargarlo
3. Subir a Clock Club o servidor propio

#### 4. Herramientas Disponibles

| Herramienta | Icono | Uso |
|-------------|-------|-----|
| **Select** | Cursor | Seleccionar y mover elementos |
| **DateTime** | Reloj | Mostrar hora/fecha (se actualiza) |
| **Text** | T | Texto estatico |
| **Image** | Imagen | Cargar PNG (se convierte a base64) |
| **Rect** | Cuadrado vacio | Rectangulo con borde |
| **FillRect** | Cuadrado lleno | Rectangulo relleno |
| **Line** | Linea diagonal | Linea de punto a punto |

#### 5. Propiedades por Tipo de Elemento

**DateTime y Text:**
- Posicion X, Y
- Contenido (texto o formato de fecha)
- Fuente (picopixel, square, medium, big)
- Color de texto (fgColor)
- Color de fondo (bgColor)

**Rect y FillRect:**
- Posicion X, Y
- Ancho (width)
- Alto (height)
- Color

**Line:**
- Punto inicial X, Y
- Punto final X1, Y1
- Color

**Image:**
- Posicion X, Y
- Archivo de imagen (se convierte a base64)

#### 6. Atajos de Teclado

| Tecla | Accion |
|-------|--------|
| Flechas | Mover elemento seleccionado 1px |
| Delete/Backspace | Eliminar elemento seleccionado |
| Escape | Deseleccionar |

#### 7. Importar un Clockface Existente

1. Click en "Importar"
2. Pegar el JSON del clockface
3. Click en "Importar"
4. Los elementos apareceran en el canvas
5. Editar y exportar nuevamente

#### 8. Tips y Mejores Practicas

**Rendimiento:**
- Usar formas geometricas en lugar de imagenes cuando sea posible
- Mantener imagenes pequenas (< 32x32 px ideal)
- Minimizar elementos en el loop

**Organizacion:**
- Elementos estaticos (fondo, marcos) en Setup
- Elementos dinamicos (hora, animaciones) en Loop
- Nombrar elementos descriptivamente en el contenido

**Colores:**
- El editor convierte automaticamente a RGB565
- Algunos colores pueden verse ligeramente diferentes en el display real
- Probar con colores puros primero (rojo, verde, azul, blanco)

### Ejemplo: Crear un Reloj Simple

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

## Estructura del Proyecto Local

```
C:\Documents\GitHub\Clockwise\
├── ANALISIS.md                 # Este documento
├── clockface-editor/           # Editor visual de clockfaces
│   ├── index.html
│   ├── css/style.css
│   └── js/*.js
├── web-flasher/                # Herramienta de flasheo web
│   ├── index.html
│   ├── README.md
│   └── static/firmware/
├── firmware/                   # Codigo fuente del reloj
│   ├── lib/
│   │   ├── cw-commons/
│   │   └── cw-gfx-engine/
│   ├── src/main.cpp
│   └── platformio.ini
├── components/                 # Dependencias ESP32
│   ├── arduino/
│   ├── Adafruit-GFX-Library/
│   └── ESP32-HUB75-MatrixPanel-I2S-DMA/
└── README.md
```

---

## Referencias

### Proyecto Original
- **Repositorio principal:** https://github.com/jnthas/clockwise
- **Fork base:** https://github.com/yuan910715/clockwise
- **Sitio web oficial:** https://clockwise.page
- **Wiki:** https://github.com/jnthas/clockwise/wiki
- **Clock Club (temas):** https://github.com/jnthas/clock-club

### Nuestro Repositorio
- **GitHub:** https://github.com/XE1E/Clockwise
- **Editor de Clockfaces:** /clockface-editor/
- **Web Flasher:** /web-flasher/

### Bibliotecas y Herramientas
- **ESP32-HUB75-MatrixPanel-I2S-DMA:** https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-I2S-DMA
- **ESP Web Tools:** https://esphome.github.io/esp-web-tools/
- **Adafruit GFX:** https://github.com/adafruit/Adafruit-GFX-Library
- **ezTime:** https://github.com/ropg/ezTime

### Hardware Recomendado
- **ESP32 Trinity:** https://github.com/witnessmenow/ESP32-Trinity
- **Mario Clock PCB:** https://github.com/Alexvanheu/Mario-Clock-PCB-ESP32
