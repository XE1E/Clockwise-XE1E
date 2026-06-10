# Migración a ESP32-S3-DevKitC-1

Este documento describe los cambios necesarios para soportar la tarjeta ESP32-S3-DevKitC-1 en el proyecto Clockwise.

## Resumen

| Aspecto | ESP32 actual | ESP32-S3-DevKitC-1 |
|---------|--------------|---------------------|
| Board PlatformIO | `esp32doit-devkit-v1` | `esp32-s3-devkitc-1` |
| Chip | ESP32 (2016) | ESP32-S3 (2021) |
| CPU | Dual-core 240MHz | Dual-core 240MHz + AI acceleration |
| RAM | 520KB SRAM | 512KB SRAM + 8MB PSRAM (opcional) |
| Flash | 4MB | 8MB (típico) |
| USB | Via CP2102/CH340 | USB nativo (OTG) |
| LED integrado | GPIO 2 (simple) | GPIO 48 (RGB WS2812) |

## Impacto en desarrollo actual

**NO hay impacto** si se implementa como nuevo environment:
- El `default_envs = esp32dev` permanece sin cambios
- Compilación normal: `pio run` → usa ESP32 original
- Compilación para S3: `pio run -e esp32s3` → usa ESP32-S3
- Ambos environments pueden coexistir

## Cambios requeridos

### 1. platformio.ini

Agregar nuevo environment (no reemplazar el existente):

```ini
[env:esp32s3]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino
test_ignore = test_native
monitor_speed = 115200
board_build.arduino.memory_type = qio_opi
build_flags =
    -D CW_FW_VERSION="\"1.0.0\""
    -D CW_FW_NAME="\"ClockWise-XE1E-S3\""
    -D CLOCKFACE_NAME="\"cw-cf-0x07\""
    -D BOARD_HAS_PSRAM
    -DARDUINO_USB_CDC_ON_BOOT=1
lib_deps =
    https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-I2S-DMA.git
    adafruit/Adafruit GFX Library@^1.10.1
    adafruit/Adafruit BusIO@^1.5.0
    SPI
    Wire
    ropg/ezTime@^0.8.3
    https://github.com/jnthas/Improv-WiFi-Library
    bblanchon/ArduinoJson@^6.21.2
    bitbank2/PNGdec@^1.0.1
    https://github.com/me-no-dev/ESPAsyncWebServer.git
    https://github.com/me-no-dev/AsyncTCP.git
lib_extra_dirs =
    clockfaces
lib_ignore =
    cw-cf-0x01
    cw-cf-0x02
    cw-cf-0x03
    cw-cf-0x04
    cw-cf-0x05
    cw-cf-0x06
build_src_filter = +<*> -<.git/> -<.svn/> -<example/> -<examples/> -<test/> -<tests/>
```

### 2. main.cpp - LED integrado

El ESP32-S3-DevKitC-1 usa un LED RGB WS2812 en GPIO 48, no un LED simple.

**Opción A - Desactivar LED** (más simple):
```cpp
#ifdef CONFIG_IDF_TARGET_ESP32S3
  #define ESP32_LED_BUILTIN -1  // No usar LED en S3
#else
  #define ESP32_LED_BUILTIN 2
#endif
```

**Opción B - Usar LED RGB** (requiere librería adicional):
```cpp
#ifdef CONFIG_IDF_TARGET_ESP32S3
  #include <Adafruit_NeoPixel.h>
  #define ESP32_LED_BUILTIN 48
  Adafruit_NeoPixel rgbLed(1, ESP32_LED_BUILTIN, NEO_GRB + NEO_KHZ800);
#else
  #define ESP32_LED_BUILTIN 2
#endif
```

### 3. main.cpp - Configuración de pines HUB75

La biblioteca detecta automáticamente el ESP32-S3 y usa pines diferentes. Modificar `displaySetup()`:

```cpp
void displaySetup(bool swapBlueGreen, uint8_t displayBright, uint8_t displayRotation)
{
  HUB75_I2S_CFG mxconfig(64, 64, 1);

#ifdef CONFIG_IDF_TARGET_ESP32S3
  // ESP32-S3: usa pines por defecto de la biblioteca
  // Solo necesitamos configurar el pin E para paneles 64x64
  mxconfig.gpio.e = 38;  // Elegir GPIO disponible
  
  // Si tu panel tiene colores invertidos en S3, descomentar:
  // mxconfig.gpio.b1 = 5;
  // mxconfig.gpio.b2 = 15;
  // mxconfig.gpio.g1 = 6;
  // mxconfig.gpio.g2 = 16;
  
#else
  // ESP32 original: configuración actual
  if (swapBlueGreen)
  {
    mxconfig.gpio.b1 = 26;
    mxconfig.gpio.b2 = 12;
    mxconfig.gpio.g1 = 27;
    mxconfig.gpio.g2 = 13;
  }
  mxconfig.gpio.e = 18;
#endif

  mxconfig.clkphase = false;

  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(displayBright);
  dma_display->clearScreen();
  dma_display->setRotation(displayRotation);
}
```

## Cableado físico

### Pinout ESP32-S3-DevKitC-1 para HUB75

| Señal HUB75 | GPIO ESP32-S3 | Notas |
|-------------|---------------|-------|
| R1 | 4 | Rojo fila superior |
| G1 | 5 | Verde fila superior |
| B1 | 6 | Azul fila superior |
| R2 | 7 | Rojo fila inferior |
| G2 | 15 | Verde fila inferior |
| B2 | 16 | Azul fila inferior |
| A | 18 | Dirección línea bit 0 |
| B | 8 | Dirección línea bit 1 |
| C | 3 | Dirección línea bit 2 |
| D | 42 | Dirección línea bit 3 |
| E | 38 | Dirección línea bit 4 (solo 64x64) |
| CLK | 41 | Reloj |
| LAT | 40 | Latch |
| OE | 2 | Output Enable |
| GND | GND | Tierra |

![ESP32-S3-DevKitC-1 Pinout](ESP32-S3-DevKit-C.png)

![ESP32-S3-DevKitC-1 IPEX N16R8](ESP32-S3-DevKit-C-Ipex.png)

### Diagrama de conexión

<table>
<tr>
<td>

```
ESP32-S3-DevKitC-1          Panel HUB75 64x64
┌─────────────────┐         ┌─────────────────┐
│                 │         │                 │
│  GPIO 4  ───────┼─────────┼─► R1            │
│  GPIO 5  ───────┼─────────┼─► G1            │
│  GPIO 6  ───────┼─────────┼─► B1            │
│  GPIO 7  ───────┼─────────┼─► R2            │
│  GPIO 15 ───────┼─────────┼─► G2            │
│  GPIO 16 ───────┼─────────┼─► B2            │
│                 │         │                 │
│  GPIO 18 ───────┼─────────┼─► A             │
│  GPIO 8  ───────┼─────────┼─► B             │
│  GPIO 3  ───────┼─────────┼─► C             │
│  GPIO 42 ───────┼─────────┼─► D             │
│  GPIO 38 ───────┼─────────┼─► E             │
│                 │         │                 │
│  GPIO 41 ───────┼─────────┼─► CLK           │
│  GPIO 40 ───────┼─────────┼─► LAT           │
│  GPIO 2  ───────┼─────────┼─► OE            │
│                 │         │                 │
│  GND ───────────┼─────────┼─► GND           │
│                 │         │                 │
└─────────────────┘         └─────────────────┘
```

</td>
<td>

![Conector HUB75](HUB75.png)

</td>
</tr>
</table>

### Pines a evitar en ESP32-S3

Los siguientes pines están reservados y NO deben usarse:

| GPIOs | Razón |
|-------|-------|
| 26, 27, 28, 29, 30, 31, 32 | Reservados para flash/PSRAM (QSPI) |
| 19, 20 | USB D-/D+ |
| 43, 44 | UART0 (consola) |
| 0 | Boot mode |

### LDR (sensor de luz)

El pin por defecto del LDR es **GPIO 35** (ver `CWPreferences.h`). En el ESP32-S3 con
PSRAM octal (`board_build.arduino.memory_type = qio_opi`, módulos N16R8) los **GPIO 35,
36 y 37 están ocupados por la PSRAM** y NO pueden usarse. Por eso en el S3 el LDR debe
moverse a otro pin ADC1 libre.

- **Recomendado en S3:** `GPIO 1` (ADC1_CH0). Configurar el pin desde la web UI o
  `ldrPin` en preferencias.
- GPIOs válidos para ADC en S3:
  - GPIO 1, 9, 10 (ADC1, libres tras el cableado HUB75)
  - GPIO 11, 12, 13, 14 (ADC2 — no usar con WiFi activo)
- Pines ADC ya ocupados por HUB75 (no usar): 2, 3, 4, 5, 6, 7, 8, 15, 16, 18.

#### Esquemático de conexión LDR

Divisor de tensión: la lectura ADC sube con más luz (LDR arriba, resistencia fija a GND).

```
            3V3
             │
            ┌┴┐
            │ │   LDR  (fotorresistor / GL5516, etc.)
            └┬┘
             │
             ├───────────────────────►  GPIO 1  (ADC1_CH0)
             │
            ┌┴┐
            │ │   R1 = 10 kΩ   (fija)
            └┬┘
             │
            ─┴─  GND

   Opcional: condensador 100 nF entre el nodo ADC y GND
   para estabilizar la lectura (reduce ruido).
```

| Componente | Conexión |
|------------|----------|
| LDR pata A | 3V3 |
| LDR pata B | nodo → GPIO 1 (ADC) |
| R1 (10 kΩ) pata A | nodo → GPIO 1 (ADC) |
| R1 (10 kΩ) pata B | GND |
| C1 (100 nF, opcional) | entre nodo ADC y GND |

> Calibración: usar el botón **Leer** del LDR en la web UI para ver el valor crudo
> (0–4095) y ajustar `autoBrightMin` / `autoBrightMax`.

### BME680 (sensor ambiental: temperatura, humedad, presión, gas/IAQ)

Sensor I2C de Bosch para mostrar datos ambientales en una nueva carátula. Se alimenta a
3.3 V y comparte el bus I2C (no usado actualmente por el proyecto).

- **SDA → GPIO 9**
- **SCL → GPIO 10**
- Dirección I2C: `0x76` (pin SDO a GND) o `0x77` (pin SDO a 3V3). Por defecto la mayoría
  de breakouts GY-BME680 vienen en `0x77`.
- La mayoría de los módulos breakout ya incluyen resistencias pull-up y regulador; en ese
  caso **omitir** las pull-ups externas y conectar VCC a 3V3.

#### Esquemático de conexión BME680

```
   ESP32-S3-DevKitC-1                   Módulo BME680 (GY-BME680 / Adafruit)
   ┌───────────────┐                    ┌─────────────────────────┐
   │           3V3 ├────────────────────┤ VCC / VIN               │
   │           GND ├──────────┬─────────┤ GND                     │
   │                          │         │                         │
   │        GPIO 9 ├────┬─────┼─────────┤ SDA / SDI               │
   │       GPIO 10 ├─┬──┼─────┼─────────┤ SCL / SCK               │
   │               │ │  │     │         │                         │
   └───────────────┘ │  │     │         │ SDO ──► GND  = addr 0x76 │
                     │  │     │         │ SDO ──► 3V3  = addr 0x77 │
            pull-ups │  │     │         │ CS  ──► 3V3  = modo I2C  │
         (solo si el R  R     │         └─────────────────────────┘
          módulo NO  │  │     │
          las trae)  └──┴─────┘  R = 4.7 kΩ a 3V3
                     3V3 3V3
```

| Señal BME680 | GPIO ESP32-S3 | Notas |
|--------------|---------------|-------|
| VCC / VIN | 3V3 | NO usar 5V salvo que el módulo tenga regulador |
| GND | GND | Tierra común |
| SDA / SDI | GPIO 9 | Datos I2C |
| SCL / SCK | GPIO 10 | Reloj I2C |
| SDO | GND o 3V3 | Selecciona dirección 0x76 / 0x77 |
| CS | 3V3 | Mantener alto para forzar modo I2C |

> **Pull-ups:** si el bus no responde, agregar 4.7 kΩ de SDA→3V3 y SCL→3V3. Los breakouts
> tipo GY-BME680 / Adafruit ya las incluyen; los chips "pelones" no.

> **Firmware:** inicializar con `Wire.begin(9, 10);` y **auto-detectar la dirección**
> probando `0x76` y luego `0x77` (`if (!bme.begin(0x76)) bme.begin(0x77);`). Para mostrar
> Gas/IAQ se usa la librería **BSEC2** de Bosch (mayor consumo de Flash/RAM); para T/H/P
> directos basta `Adafruit_BME680`.

## Consideraciones adicionales

### PSRAM

El ESP32-S3-DevKitC-1 puede tener PSRAM de 8MB. Para habilitarlo:

```ini
build_flags =
    -D BOARD_HAS_PSRAM
    -D CONFIG_SPIRAM_USE
```

Beneficios:
- Buffers DMA más grandes
- Mejor rendimiento con animaciones complejas
- Posibilidad de doble buffer

### USB nativo

El S3 tiene USB nativo. Para usar la consola serial por USB:

```ini
build_flags =
    -DARDUINO_USB_CDC_ON_BOOT=1
```

### Velocidad de compilación

El ESP32-S3 puede tardar más en compilar la primera vez debido a las bibliotecas adicionales.

## Verificación post-migración

1. [ ] Compilación exitosa: `pio run -e esp32s3`
2. [ ] Subida de firmware: `pio run -e esp32s3 -t upload`
3. [ ] Consola serial funciona
4. [ ] Panel LED muestra imagen
5. [ ] Colores correctos (no invertidos)
6. [ ] WiFi conecta correctamente
7. [ ] Interfaz web accesible
8. [ ] LDR funciona (brillo automático) — recordar mover `ldrPin` a GPIO 1 en S3
9. [ ] BME680 detectado en el bus I2C (`bme.begin()` retorna true en 0x76/0x77)
10. [ ] Lecturas de temperatura/humedad/presión coherentes

## Rollback

Si algo falla, volver al ESP32 original es simple:
```bash
pio run -e esp32dev -t upload
```

El código soporta ambas placas simultáneamente.

---

## Cambios aplicados (2026-05-21)

### Archivos modificados

| Archivo | Cambio |
|---------|--------|
| `firmware/platformio.ini` | Agregado environment `[env:esp32s3]` |
| `firmware/src/main.cpp` | LED condicional, pines HUB75 para S3, pinMode protegido |
| `firmware/lib/cw-commons/StatusController.h` | LED condicional, blink_led() con validación |

### Detalle de cambios en main.cpp

```cpp
// LED condicional (línea 16-20)
#ifdef CONFIG_IDF_TARGET_ESP32S3
  #define ESP32_LED_BUILTIN -1  // ESP32-S3 DevKit uses RGB LED (WS2812), not simple GPIO
#else
  #define ESP32_LED_BUILTIN 2
#endif

// pinMode protegido en setup()
if (ESP32_LED_BUILTIN >= 0) {
  pinMode(ESP32_LED_BUILTIN, OUTPUT);
}

// displaySetup() con pines condicionales para S3
#ifdef CONFIG_IDF_TARGET_ESP32S3
  mxconfig.gpio.e = 38;
  if (swapBlueGreen) {
    mxconfig.gpio.b1 = 5;
    mxconfig.gpio.b2 = 15;
    mxconfig.gpio.g1 = 6;
    mxconfig.gpio.g2 = 16;
  }
#else
  // ESP32 original (sin cambios)
#endif
```

### Detalle de cambios en StatusController.h

```cpp
// LED condicional (igual que main.cpp)
#ifdef CONFIG_IDF_TARGET_ESP32S3
  #define ESP32_LED_BUILTIN -1
#else
  #define ESP32_LED_BUILTIN 2
#endif

// blink_led() con validación
void blink_led(int d, int times) {
  if (ESP32_LED_BUILTIN < 0) return;  // Skip if no simple LED available
  // ... resto sin cambios
}
```

### Resultado de compilación

| Environment | Estado | RAM | Flash |
|-------------|--------|-----|-------|
| esp32dev | ✅ SUCCESS | 31.1% (102KB/328KB) | 83.4% (1.09MB/1.31MB) |
| esp32s3 | ✅ SUCCESS | 31.2% (102KB/328KB) | 30.7% (1.03MB/3.34MB) |

### Comandos de uso

```bash
# Compilar para ESP32 original (default)
pio run
# o explícitamente:
pio run -e esp32dev

# Compilar para ESP32-S3
pio run -e esp32s3

# Subir firmware a ESP32-S3
pio run -e esp32s3 -t upload

# Monitor serial ESP32-S3
pio device monitor -e esp32s3
```
