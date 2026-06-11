# Plan: Sensor ambiental BME280 (Temp/Humedad/Presión) — ESP32 y ESP32-S3

> Estado: **planificado** · Fecha: 2026-06-11 · Aplica a **ambas builds** (ESP32 clásico y ESP32-S3)

## Resumen

Integrar un sensor ambiental **BME280** (I²C) para mostrar **temperatura, humedad y presión**
en las carátulas. Funciona en las **dos versiones de hardware con el mismo código** (sin `#ifdef`
por placa): lo único que cambia es el pin I²C por defecto, y eso es configurable por Web UI.

Se descarta IAQ por ahora. Camino de upgrade futuro: cambiar a **BME680 + BSEC2** reusando la
misma abstracción `CWSensor` y el mismo `type:"sensor"` (solo se añadiría `source:"iaq"`).

---

## Decisiones tomadas

| Tema | Decisión | Motivo |
|------|----------|--------|
| **Sensor** | **BME280** (T/H/P, sin gas/IAQ) | Mismo código para ambas placas; sin la complejidad de BSEC |
| **Placas** | **ESP32 clásico Y ESP32-S3**, código idéntico | El BME280 es I²C estándar; ambas placas tienen `Wire` |
| **IAQ** | Descartado por ahora | Requiere BME680 + BSEC (warm-up, calibración persistente, peso) |
| **Formato en carátula** | Nuevo `type:"sensor"` | Limpio; evita colisión de tokens con ezTime |
| **Símbolo de grado `°`** | Diseñar glifo y extender el rango de la fuente a `0xB0` | Correcto y estándar; herramientas ya existen |
| **Dirección I²C y pines** | **Configurables por Web UI** (default por placa; addr autodetect 0x76/0x77) | El módulo físico aún no está a la mano |

---

## 1. Hardware

El BME280 es I²C. Misma dirección y conexión que el BME680. Pines I²C libres por placa
(elegidos para **no chocar con el display ni el LDR** — ver §8 pinout completo):

| Señal | ESP32 clásico (default) | ESP32-S3 (default) | Nota |
|-------|-------------------------|--------------------|------|
| SDA | **GPIO 21** | **GPIO 10** | Configurable por Web UI |
| SCL | **GPIO 22** | **GPIO 9** | Configurable por Web UI |
| VCC | 3V3 | 3V3 | |
| GND | GND | GND | |
| Dirección I²C | `0x76` / `0x77` | `0x76` / `0x77` | Autodetectar; override configurable |

`Adafruit BusIO` ya está vendorizada en `platformio.ini`. El LDR no choca (ESP32: GPIO 35; S3: GPIO 1).

> ⚠️ **Cuidado en S3:** el GPIO **8** lo usa el display (línea de dirección **B** del HUB75),
> por eso el I²C de la S3 NO puede usar 8. Se usa **SDA=10 / SCL=9** (ambos libres).
> En el ESP32 clásico, 21/22 (I²C clásico) sí quedan libres.

---

## 2. Firmware

Un solo módulo, igual para ambas placas. Puntos de integración:

| Aspecto | Archivo | Patrón a seguir |
|---------|---------|-----------------|
| Lectura periódica | `firmware/src/main.cpp` (~247-271) | `automaticBrightControl()` (polling `millis()`) |
| Config / NVS | `firmware/lib/cw-commons/CWPreferences.h` | campos LDR (pin/min/max) |
| API web | `firmware/lib/cw-commons/CWWebServer.h` (~208-217) | endpoint `/api/pin` |
| Render carátula | `firmware/clockfaces/cw-cf-0x07/JsonClockface.cpp` (~294-326) | `refreshDateTime()` |
| Fuente de datos | nuevo `firmware/lib/cw-commons/CWSensor.h/.cpp` | espejo de `CWDateTime` |

### Tareas
1. **`platformio.ini`**: añadir `adafruit/Adafruit BME280 Library` al `lib_deps` **común** (ambos entornos); define de pines I²C default **por placa** (21/22 clásico, 8/9 S3) — solo el default, no lógica.
2. **`CWSensor.h/.cpp`** (nuevo): `begin()`, `update()`, `available()`, `getTemperature()`, `getHumidity()`, `getPressure()`. (Se deja `getIaq()` declarado/stub para el futuro upgrade a BME680, pero sin usar.) **Sin compilación condicional por placa.**
3. **`main.cpp`**: instanciar `CWSensor`, `begin()` en setup, `update()` por `millis()` (~30 s) en `loop()`.
4. **`CWPreferences.h`**: `sensorEnable`, `sensorSdaPin`, `sensorSclPin`, `sensorAddr` (0=auto), `tempUnit` (C/F).
5. **`CWWebServer.h`**: `GET /api/sensor` → `{"temp":..,"hum":..,"pres":..,"ok":true}`; endpoints para guardar config de pines/unidad.

> Autodetección de dirección: `begin()` prueba `0x76` y luego `0x77` (o usa `sensorAddr` si != 0).
> Así no hace falta tener el módulo a la mano para escribir el código.

---

## 3. Render en carátula — nuevo `type:"sensor"`

`type:"datetime"` pasa `content` directo a ezTime; meter tokens de sensor ahí arriesga colisión.
Por eso, elemento aparte.

### Esquema propuesto
```json
{
  "type": "sensor",
  "source": "temp",        // "temp" | "hum" | "pres"   (futuro: "iaq")
  "decimals": 1,
  "unit": true,             // mostrar unidad (°C / % / hPa)
  "font": "medium",
  "fgColor": 65535,
  "bgColor": 0,
  "x": 10,
  "y": 30
}
```

### Tarea
6. **`JsonClockface.cpp`**: parsear `type:"sensor"`; nuevo `refreshSensors()` que toma el valor de
   `CWSensor`, formatea (incluido el byte `0xB0` para `°` cuando `unit` y `source=="temp"`) y reusa
   `renderText()`. Si el sensor no está disponible, no dibuja (o muestra `--`).

---

## 4. Editor de carátulas

| Tarea | Archivo |
|-------|---------|
| 7. Clase `SensorElement` + `getDisplayText()` | `clockface-editor/js/elements.js` (espejo de `DateTimeElement`, ~112-159) |
| 8. Botón "Sensor", panel de props (source/decimales/unidad) y valores de prueba `window.testSensor = {temp,hum,pres}` | `clockface-editor/index.html` |
| 9. `renderSensor()` | `clockface-editor/js/canvas-renderer.js` (~71-74) |

Notas:
- Campos nuevos son **opcionales** → no rompen carátulas existentes.
- `clockface-editor/clockfaces/` y `web-flasher/gallery/clockfaces/` **no se sincronizan** automáticamente.

---

## 5. Fuentes — símbolos `°` y `%`

Estado actual (13 fuentes firmware, formato Adafruit GFX, rango `0x20–0x7E`):

- **`%` (0x25):** presente en casi todas. Vacío/ausente en `nocturno` (solo dígitos) y `ledDisplay` → dibujar si se usan con humedad.
- **`°` (grado):** **ausente en todas** (está en 0xB0, fuera de ASCII).

**Complicación:** `°` en UTF-8 son 2 bytes (0xC2 0xB0) y Adafruit GFX indexa por byte.
**Solución:** el firmware **construye la cadena** y emite un **byte único `0xB0`**; la fuente debe tener el glifo en `0xB0`.

### Tarea
10. Diseñar glifo `°` en `clockface-editor/editor-fuentes.html`; extender el rango a `0xB0` en las
    fuentes que se usen para temperatura; rellenar `%` en `nocturno`/`ledDisplay` si aplica.
    Regenerar `.h` con `clockface-editor/tools/convert_json_to_gfx.py` o `scripts/sync-fonts.js`.
    Replicar el glifo en `clockface-editor/js/pixel-fonts.js` para el preview del editor.

> Solo en las fuentes que realmente se usen para temperatura, no en las 13.

---

## 6. Web UI + documentación

11. Mostrar la lectura del sensor en la Web UI de configuración (consume `/api/sensor`) y exponer
    config de pines/dirección/unidad. Añadir sección al manual de usuario.
    Recordar: tras editar `WebUI.h`, regenerar `WebUI_gz.h` (`tools/gzip_webui.py`).
12. **Documentar el pinout completo por versión en el manual de usuario** (ver §8). Modelo:
    el usuario solo elige su firmware en el flasher (ESP32 o ESP32-S3), lo graba, cablea según
    la tabla de su versión y queda funcionando. El firmware ya trae el pinout correcto compilado.

---

## 7. Upgrade futuro — BME680 + IAQ (opcional)

Si más adelante se quiere calidad de aire: cambiar a **BME680 + BSEC2 de Bosch**, poblar
`CWSensor::getIaq()` y habilitar `source:"iaq"` en carátula/editor. La abstracción `CWSensor` y el
`type:"sensor"` ya quedan preparados. Ojo entonces con flash/RAM y, en S3, NO usar
`memory_type=qio_opi` (boot loop).

---

## 8. Pinout completo por versión (para el manual de usuario)

> **Modelo de usuario:** dos firmwares en el flasher (ESP32 clásico y ESP32-S3). El usuario elige
> el de su placa, lo graba, cablea según SU tabla y listo. El firmware ya trae el pinout compilado;
> no hay que configurar nada del display. Solo el sensor BME280 (opcional) y el LDR son editables
> por Web UI.

### Pantalla 64x64 HUB75 (conector estándar de 16 pines)

| Señal HUB75 | ESP32 clásico (GPIO) | ESP32-S3 (GPIO) | Origen |
|-------------|----------------------|-----------------|--------|
| R1 | 25 | 4 | default librería |
| G1 | 26 | 5 | default librería |
| B1 | 27 | 6 | default librería |
| R2 | 14 | 7 | default librería |
| G2 | 12 | 15 | default librería |
| B2 | 13 | 16 | default librería |
| A | 23 | 18 | default librería |
| B | 19 | **8** | default librería |
| C | 5 | 3 | default librería |
| D | 17 | 42 | default librería |
| **E** | **18** | **38** | **override firmware** (necesario para 64x64) |
| LAT (STB) | 4 | 40 | default librería |
| OE | 15 | 2 | default librería |
| CLK | 16 | 41 | default librería |
| GND | GND | GND | — |

> **Swap Blue/Green:** opción de software en la Web UI para paneles con orden de color RBG.
> Al activarla, el firmware intercambia: clásico → B1=26,B2=12,G1=27,G2=13; S3 → B1=5,B2=15,G1=6,G2=16.
> No requiere recablear; es solo el orden de color del panel.

### Periféricos

| Periférico | Señal | ESP32 clásico | ESP32-S3 | Configurable Web UI |
|------------|-------|---------------|----------|---------------------|
| LDR (sensor de luz) | ADC | GPIO 35 | GPIO 1 | Sí (pin + min/max) |
| BME280 (ambiental) | SDA | GPIO 21 | GPIO 10 | Sí |
| BME280 (ambiental) | SCL | GPIO 22 | GPIO 9 | Sí |
| BME280 (ambiental) | VCC / GND | 3V3 / GND | 3V3 / GND | — |

> **Conflictos a evitar (S3):** GPIO 8 está ocupado por el display (línea B), por eso el I²C usa
> 10/9. En S3 con PSRAM octal, evitar también GPIO 35–37 (reservados para PSRAM).
> En el clásico, GPIO 34/36/39 son solo-entrada (no sirven para I²C).

---

## Orden de ejecución sugerido

1. Fase 1 (firmware: lib + `CWSensor` + polling + prefs + `/api/sensor`).
2. Fase 2 (render `type:"sensor"` en `JsonClockface`).
3. Fase 3 (editor: elemento + preview + valores de prueba).
4. Fase 4 (fuentes: glifo `°`).
5. Fase 5 (Web UI + docs, incl. tabla de pinout §8 en el manual de usuario).
