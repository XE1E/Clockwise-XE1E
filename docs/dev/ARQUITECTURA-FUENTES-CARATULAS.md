# Arquitectura de Fuentes y Carátulas - Clockwise

## Resumen Ejecutivo

Este documento describe la arquitectura actual del sistema de fuentes y carátulas de Clockwise, y el plan para permitir que usuarios finales puedan personalizar el reloj sin necesidad de compilar firmware localmente.

---

## Arquitectura Actual

### Componentes del Sistema

```
┌─────────────────────────────────────────────────────────────────┐
│                        EDITORES WEB                              │
│  clockface-editor/                                               │
│  ├── index.html          Editor de carátulas                    │
│  ├── editor-fuentes.html Editor de fuentes                      │
│  └── js/pixel-fonts.js   Fuentes para preview en editores       │
└─────────────────────────────────────────────────────────────────┘
                              │
                              │ Exportar JSON
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                         FIRMWARE                                 │
│  firmware/clockfaces/cw-cf-0x07/                                │
│  ├── Clockface.cpp       Motor de renderizado                   │
│  ├── Clockface.h         Includes de fuentes (.h)               │
│  └── fonts/*.h           Fuentes compiladas (Adafruit GFX)      │
└─────────────────────────────────────────────────────────────────┘
                              │
                              │ Compilar + Flash
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                          RELOJ                                   │
│  SPIFFS/                                                         │
│  └── clockfaces/*.json   Carátulas (datos)                      │
│                                                                  │
│  Flash (firmware)                                                │
│  └── Fuentes compiladas  (código)                               │
└─────────────────────────────────────────────────────────────────┘
```

### Estado de Cada Componente

| Componente | Almacenamiento | Modificable sin recompilar |
|------------|----------------|---------------------------|
| Carátulas | JSON en SPIFFS | ✅ SÍ |
| Fuentes | Compiladas en firmware (.h) | ❌ NO |
| Imágenes/Sprites | PNG en SPIFFS | ✅ SÍ |

### Flujo Actual de Carátulas

```
Editor Web → JSON → Subir a SPIFFS → Reloj lee JSON → Renderiza
```

Las carátulas YA son data-driven:
- `deserializeDefinition()` carga JSON desde SPIFFS
- `renderElements()` interpreta y dibuja elementos
- Soporta: texto, rectángulos, imágenes, sprites animados

### Flujo Actual de Fuentes

```
pixel-fonts.js → [MANUAL] → fonts/*.h → Compilar → Flash
```

Las fuentes están hardcodeadas en `setFont()`:
```cpp
void Clockface::setFont(const char *fontName) {
    if (strcmp(fontName, "picopixel") == 0)
        Locator::getDisplay()->setFont(&Picopixel);
    else if (strcmp(fontName, "led-display") == 0)
        Locator::getDisplay()->setFont(&ledDisplay);
    // ... más fuentes
}
```

---

## Formatos de Fuente

### pixel-fonts.js (Editor Web)

```javascript
PixelFonts.miFuente = {
    bitmaps: [0xE8, 0xB4, 0x57, ...],
    glyphs: [
        [offset, width, height, xAdvance, xOffset, yOffset],
        // ...
    ],
    first: 0x20,
    last: 0x7E,
    yAdvance: 7
};
```

### Adafruit GFX .h (Firmware)

```cpp
const uint8_t miFuenteBitmaps[] PROGMEM = {0xE8, 0xB4, 0x57, ...};

const GFXglyph miFuenteGlyphs[] PROGMEM = {
    {offset, width, height, xAdvance, xOffset, yOffset},
    // ...
};

const GFXfont miFuente PROGMEM = {
    (uint8_t *)miFuenteBitmaps,
    (GFXglyph *)miFuenteGlyphs,
    0x20,  // first
    0x7E,  // last
    7      // yAdvance
};
```

**Los formatos son prácticamente idénticos**, solo cambia la sintaxis (JS vs C++).

---

## Arquitectura Propuesta

### Objetivo

Permitir dos tipos de usuarios:

1. **Usuario Básico**: Usa galería de carátulas pre-hechas
2. **Usuario Creativo**: Diseña carátulas y fuentes personalizadas

Ambos sin necesidad de compilar firmware localmente.

### Modelo de Independencia: Repo Original + Forks

**Principio fundamental**: El proyecto original NUNCA se modifica por personalizaciones de usuarios.

```
┌─────────────────────────────────────────────────────────────────┐
│                 REPO ORIGINAL (mantenedor)                       │
│                                                                  │
│  - Firmware "oficial" con 15-20 fuentes comunes                 │
│  - Releases estables y probados                                 │
│  - Editores web (fuentes, carátulas)                            │
│  - Galería de carátulas oficiales                               │
│  - Documentación                                                │
│                                                                  │
│  ✓ 90% de usuarios usarán esto directamente                     │
│  ✓ NO se modifica por usuarios individuales                     │
└─────────────────────────────────────────────────────────────────┘
                              │
                              │ Usuario quiere fuentes custom
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                    FORK DEL USUARIO                              │
│                                                                  │
│  - Copia completa e independiente del proyecto                  │
│  - Usuario agrega sus fuentes personalizadas                    │
│  - Su propio GitHub Actions compila automáticamente             │
│  - Genera su propio firmware .bin                               │
│  - Puede recibir actualizaciones del repo original (git merge)  │
│                                                                  │
│  ✓ 100% independiente del proyecto original                     │
│  ✓ GitHub Actions gratis para repos públicos                    │
│  ✓ Control total sobre su versión                               │
└─────────────────────────────────────────────────────────────────┘
```

### Flujos por Tipo de Usuario

| Usuario | Necesita | Usa |
|---------|----------|-----|
| Básico | Solo carátulas | Repo original + firmware oficial |
| Creativo (carátulas) | Diseñar carátulas con fuentes existentes | Repo original + firmware oficial |
| Creativo (fuentes) | Fuentes personalizadas | Fork propio + su firmware |

### Solución Técnica: Firmware Universal + Forks

```
┌─────────────────────────────────────────────────────────────────┐
│                     EDITORES WEB                                 │
│                                                                  │
│  Editor Fuentes ──────► pixel-fonts.js (todas las fuentes)      │
│        │                                                         │
│        └──────────────► Exportar a firmware/*.h                 │
│                                                                  │
│  Editor Carátulas ────► JSON (exportar/subir directo)           │
└─────────────────────────────────────────────────────────────────┘
                              │
            ┌─────────────────┴─────────────────┐
            │                                   │
            ▼                                   ▼
┌───────────────────────┐          ┌───────────────────────┐
│   GITHUB ACTIONS      │          │   SUBIDA DIRECTA      │
│   (Build Service)     │          │   (Carátulas JSON)    │
│                       │          │                       │
│ - Detecta cambios     │          │ - Via WiFi/Web UI     │
│ - Compila firmware    │          │ - A SPIFFS del reloj  │
│ - Genera .bin         │          │                       │
│ - Publica release     │          │                       │
└───────────┬───────────┘          └───────────────────────┘
            │
            ▼
┌───────────────────────────────────────────────────────────────┐
│                      WEB FLASHER                               │
│                                                                │
│  - ESP Web Tools (flash desde navegador)                      │
│  - Usuario selecciona .bin y flashea                          │
│  - Sin instalación de software                                │
└───────────────────────────────────────────────────────────────┘
```

### Flujo Usuario Básico

```
1. Visita galería web
2. Selecciona carátula
3. Descarga/sube JSON al reloj via WiFi
4. Listo
```

### Flujo Usuario Creativo (Carátulas)

```
1. Diseña carátula en editor web
2. Usa fuentes disponibles en firmware
3. Exporta JSON
4. Sube al reloj via WiFi
5. Listo
```

### Flujo Usuario Creativo (Fuentes Nuevas)

```
1. Hacer FORK del repo original en GitHub
2. Clonar su fork localmente (o editar en GitHub)
3. Crear/editar fuente en editor web
4. Agregar a pixel-fonts.js (en su fork)
5. Exportar a firmware/*.h (en su fork)
6. Commit + Push a su fork
7. GitHub Actions de su fork compila nuevo .bin
6. Descarga .bin y flashea con Web Flasher
```

---

## Fuentes Incluidas en Firmware Universal

El firmware incluirá TODAS las fuentes comunes para cubrir la mayoría de casos:

| Nombre | Tamaño | Uso típico |
|--------|--------|------------|
| picopixel | 3x5 | Texto muy pequeño |
| tomthumb | 4x6 | Texto pequeño |
| tiny5 | 5x5 | Texto compacto |
| square | 7x11 | Digital/retro |
| medium | 7x9 | Propósito general |
| big | 8x12 | Hora principal |
| bold | 9pt | Títulos |
| led-display | 8x14 | Estilo LED/7-seg |
| nocturno | 12x | Modo nocturno |
| 8x13 | 8x13 | Monospace |
| 8x13B | 8x13 | Monospace bold |
| haxor-12 | 12px | Estilo hacker |

---

## Sincronización pixel-fonts.js ↔ firmware/*.h

### Problema

Actualmente hay dos copias de cada fuente:
- `clockface-editor/js/pixel-fonts.js` (para editores web)
- `firmware/clockfaces/cw-cf-0x07/fonts/*.h` (para firmware)

Deben mantenerse sincronizadas manualmente.

### Solución

Crear herramienta de sincronización:

```
pixel-fonts.js ──[Convertidor]──► fonts/*.h
                     │
                     ├── Genera archivos .h
                     ├── Actualiza includes en Clockface.h
                     └── Actualiza setFont() en Clockface.cpp
```

---

## Componentes a Desarrollar

### 1. Convertidor pixel-fonts.js → .h

**Ubicación**: `clockface-editor/js/font-converter.js` o función en editor

**Funcionalidad**:
- Lee fuente de PixelFonts
- Genera código C++ en formato Adafruit GFX
- Opción: guardar archivo .h directamente

### 2. Sincronizador de Fuentes

**Ubicación**: `scripts/sync-fonts.js` (Node.js) o en editor web

**Funcionalidad**:
- Lee todas las fuentes de pixel-fonts.js
- Genera/actualiza todos los .h en firmware/fonts/
- Actualiza Clockface.h (includes)
- Actualiza Clockface.cpp (setFont switch)

### 3. GitHub Actions Workflow

**Ubicación**: `.github/workflows/build-firmware.yml`

**Funcionalidad**:
- Trigger: push a main con cambios en firmware/ o pixel-fonts.js
- Compila firmware con PlatformIO
- Genera .bin
- Crea/actualiza GitHub Release
- Publica en página de descargas

### 4. Página Web de Releases

**Ubicación**: GitHub Pages o subdirectorio del proyecto

**Funcionalidad**:
- Lista versiones de firmware disponibles
- Changelog de cada versión
- Botón para flashear con ESP Web Tools
- Instrucciones para usuarios

### 5. Galería de Carátulas

**Ubicación**: Web (GitHub Pages o similar)

**Funcionalidad**:
- Muestra carátulas disponibles con preview
- Permite descargar JSON
- Instrucciones para subir al reloj

---

## Diagrama de Componentes Final

```
┌─────────────────────────────────────────────────────────────────────┐
│                           GITHUB REPO                                │
│                                                                      │
│  clockface-editor/                                                  │
│  ├── js/pixel-fonts.js ◄──────────────────┐                        │
│  ├── editor-fuentes.html                   │                        │
│  └── index.html                            │                        │
│                                            │ Sincronizar            │
│  firmware/                                 │                        │
│  └── clockfaces/cw-cf-0x07/               │                        │
│      ├── fonts/*.h ◄──────────────────────┘                        │
│      ├── Clockface.cpp (setFont actualizado)                       │
│      └── Clockface.h (includes actualizados)                       │
│                                                                      │
│  .github/workflows/                                                 │
│  └── build-firmware.yml ──────► Compila ──────► Release .bin       │
│                                                                      │
│  docs/                                                              │
│  ├── galeria/          Galería de carátulas                        │
│  └── releases/         Página de descargas                         │
└─────────────────────────────────────────────────────────────────────┘
                              │
                              │ Usuario descarga
                              ▼
┌─────────────────────────────────────────────────────────────────────┐
│                         WEB FLASHER                                  │
│                    (ESP Web Tools)                                  │
│                                                                      │
│  [Seleccionar .bin] ──► [Conectar USB] ──► [Flashear] ──► ✓        │
└─────────────────────────────────────────────────────────────────────┘
```

---

## Consideraciones Técnicas

### Tamaño del Firmware

- ESP32 tiene ~4MB de flash
- Firmware actual: ~1.5MB aprox
- Cada fuente: 5-20KB
- Con 15-20 fuentes: +300KB máx
- **Hay espacio suficiente**

### Memoria RAM

- Las fuentes están en PROGMEM (flash, no RAM)
- Solo se carga el puntero, no los datos
- **No hay impacto significativo**

### Compatibilidad

- Adafruit GFX es el estándar
- Mismo formato que librerías existentes
- Fácil agregar fuentes de otras fuentes (BDF, TTF convertidas)

---

## Riesgos y Mitigaciones

| Riesgo | Mitigación |
|--------|------------|
| Fuentes desincronizadas | Script de sincronización automático |
| Build falla en CI | Tests de compilación, notificaciones |
| Usuario flashea mal | Instrucciones claras, recovery mode |
| Fuente corrupta rompe firmware | Validación antes de compilar |

---

## Referencias

- [Adafruit GFX Font Format](https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts)
- [ESP Web Tools](https://esphome.github.io/esp-web-tools/)
- [PlatformIO CI](https://docs.platformio.org/en/latest/integration/ci/github-actions.html)
