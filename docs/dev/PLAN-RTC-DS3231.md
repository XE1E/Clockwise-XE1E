# Plan: RTC externo DS3231 (respaldo de hora sin red) — ESP32 y ESP32-S3

> Estado: **planificado** · Fecha: 2026-06-12 · Aplica a **ambas builds** (ESP32 clásico y ESP32-S3)

## Resumen

Integrar un **RTC DS3231** (módulo ZS-042, I²C) como **respaldo de la hora**. Hoy Clockwise
obtiene la hora **solo por NTP/WiFi** (`CWDateTime` sobre `ezTime`): si arranca sin red, no tiene
hora hasta sincronizar. El DS3231 resuelve eso:

- **Al arrancar sin WiFi/NTP** → la hora se siembra desde el DS3231 (el reloj funciona offline).
- **Cuando NTP sincroniza** → se escribe la hora (UTC) de vuelta al DS3231 para mantenerlo a punto.

NTP sigue siendo la **fuente de verdad**; el DS3231 es el respaldo. Mismo código para las dos
placas (sin `#ifdef` por placa): es I²C estándar, igual que el BME280.

---

## Decisiones tomadas

| Tema | Decisión | Motivo |
|------|----------|--------|
| **Modelo** | **DS3231** (módulo ZS-042) | TCXO compensado por temperatura (±2 ppm, ~1 min/año). El estándar para relojes. |
| **Placas** | ESP32 clásico **y** S3, código idéntico | I²C estándar; ambas tienen `Wire` |
| **Bus I²C** | **Compartido con el BME280** (mismos SDA/SCL) | El RTC (`0x68`) no colisiona con el BME280 (`0x76/0x77`) |
| **Fuente de verdad** | **NTP**; DS3231 = respaldo | Evita derivas; el RTC solo cubre el offline |
| **Zona horaria** | El **DS3231 guarda UTC**; ezTime aplica el TZ | El TZ ya lo gestiona `ezTime`; no duplicar lógica |
| **Librería** | `adafruit/RTClib` | Estándar, ligera, soporta DS3231 |
| **EEPROM AT24C32** (`0x57`) | **Ignorada por ahora** | Viene en el módulo pero no se usa; documentar que existe |
| **Wire.begin()** | **Owner único compartido** (helper `cwI2CEnsureBegun`) | Hoy lo hace `CWSensor`; el RTC necesita el bus aunque el sensor esté desactivado |

---

## 1. Hardware

El DS3231 es I²C y se **conecta en paralelo** al BME280 sobre el mismo bus (4 cables a SDA/SCL/3V3/GND).
No consume pines nuevos.

| Señal | ESP32 clásico (default) | ESP32-S3 (default) | Nota |
|-------|-------------------------|--------------------|------|
| SDA | **GPIO 21** | **GPIO 10** | Mismo pin que el BME280 (bus compartido) |
| SCL | **GPIO 22** | **GPIO 9** | Mismo pin que el BME280 (bus compartido) |
| VCC | 3V3 | 3V3 | El ZS-042 funciona a 3V3 |
| GND | GND | GND | |
| Dirección I²C | `0x68` | `0x68` | Fija en el DS3231 (no colisiona con BME280) |

> El módulo trae además una EEPROM **AT24C32** en `0x57` (o `0x50–0x57`). No la usamos; solo
> tenerla presente al escanear el bus. La pila **CR2032** es la que conserva la hora sin alimentación.
>
> ⚠️ **3V3, no 5V.** El ZS-042 tiene una resistencia de carga de la pila pensada para LIR2032
> (recargable). Con una **CR2032 (no recargable)** conviene cortar/quitar esa resistencia de carga
> (diodo+resistencia hacia VCC) para no intentar "cargar" una pila no recargable. Documentarlo en el manual.

---

## 2. Firmware

Un solo módulo, igual para ambas placas. Puntos de integración (líneas reales de hoy):

| Aspecto | Archivo | Patrón a seguir |
|---------|---------|-----------------|
| Init del bus I²C | `CWSensor.cpp:22` (`Wire.begin`) | Extraer a helper compartido (ver Tarea 1) |
| Init en setup | `main.cpp:287-293` (sensor begin) | Mismo bloque/orden, **antes** de `cwDateTime.begin` |
| Sembrar/sincronizar hora | `main.cpp:327-330` (`wifi.begin` → `cwDateTime.begin`) | Leer RTC antes de NTP; escribir RTC tras sync |
| Acople con ezTime | `CWDateTime.cpp:21-44` (`begin`) | Usar `ezt::setTime(epochUTC)` para sembrar offline |
| Config / NVS | `CWPreferences.h` | Campos del sensor (reusa pines I²C) |
| API web | `CWWebServer.h` (`/api/sensor`, `/api/settings`, `/api/set`) | Espejo de los endpoints del sensor |
| Fuente de datos | nuevo `CWRtc.h/.cpp` | Espejo de `CWSensor` (singleton) |

### Tareas

1. **Bus I²C compartido (refactor mínimo).** Crear `cwI2CEnsureBegun(sda, scl)` (helper con
   `static bool` que llama `Wire.begin()` una sola vez). Hacer que `CWSensor::begin()` lo use en
   lugar de `Wire.begin()` directo (`CWSensor.cpp:22`). Así el RTC puede inicializar el bus aunque
   el BME280 esté desactivado, y nunca se hace `Wire.begin()` dos veces.

2. **`platformio.ini`**: añadir `adafruit/RTClib` al `lib_deps` **común** (ambos entornos). No hace
   falta define de pines: el RTC reusa los pines I²C del sensor (`CW_SENSOR_SDA/SCL_DEFAULT`).

3. **`CWRtc.h/.cpp`** (nuevo, singleton — espejo de `CWSensor`):
   - `bool begin(uint8_t sda, uint8_t scl)` → `cwI2CEnsureBegun`; `_rtc.begin(&Wire)`; setea `_available`.
   - `bool available()`.
   - `uint32_t getEpochUTC()` → `_rtc.now().unixtime()` (0 si no disponible o `lostPower()`).
   - `void setEpochUTC(uint32_t t)` → `_rtc.adjust(DateTime(t))`.
   - `bool lostPower()` → `_rtc.lostPower()` (true si la pila murió → la hora del RTC no es fiable).
   - **Sin compilación condicional por placa.**

4. **`CWDateTime`**: añadir punto de entrada para sembrar la hora desde el RTC. Opción simple:
   método `void setFromEpochUTC(uint32_t utc)` que llame `ezt::setTime(utc)` (ezTime acepta epoch UTC
   y luego `myTZ` aplica el TZ). Y un getter `uint32_t getEpochUTC()` (= `UTC.now()`) para escribir
   al RTC tras el sync.

5. **`main.cpp` — orquestación** (el corazón del plan):
   - **En `setup()`, junto al sensor (`~287`):** `CWRtc::begin(sda, scl)` si `rtcEnabled`.
   - **Antes de NTP (`~327`):** si hay RTC y `!lostPower()`, `cwDateTime.setFromEpochUTC(rtc.getEpochUTC())`
     → el reloj ya tiene hora aunque no haya WiFi.
   - **Después de `cwDateTime.begin()` (NTP, `~330`):** si la sync fue OK (`timeStatus()==timeSet`),
     escribir `rtc.setEpochUTC(cwDateTime.getEpochUTC())` → mantiene el RTC a punto.
   - **Periódico (opcional):** reescribir el RTC desde NTP cada N horas (igual patrón `millis()` que el sensor).

6. **`CWPreferences.h`**: un solo campo nuevo `rtcEnabled` (bool). **Reusa** `sensorSdaPin`/`sensorSclPin`
   como pines del bus I²C compartido (documentarlo). Si en el futuro se quiere desacoplar, se renombran
   a `i2cSdaPin/i2cSclPin`; por ahora no hace falta.

7. **`CWWebServer.h`**: 
   - `GET /api/rtc` → `{"present":true,"lostPower":false,"epoch":...,"iso":"..."}`.
   - `rtcEnabled` en `/api/settings` (lectura) y `/api/set` (guardado).
   - (Opcional) `POST /api/rtc/sync` → fuerza escribir la hora actual al RTC desde la Web UI.

---

## 3. Web UI + documentación

8. **`WebUI.h`**: en la pestaña Pantalla (junto a la tarjeta "Sensor Ambiental"), tarjeta **"Reloj de
   respaldo (RTC DS3231)"**: checkbox habilitar, estado en vivo (presente / pila OK o agotada / hora
   leída) consumiendo `/api/rtc`, y botón "Sincronizar ahora" (escribe NTP→RTC). Guardar `rtcEnabled`
   en `saveDisplay()` y cargarlo en settings.
   **Recordar:** tras editar `WebUI.h`, regenerar `WebUI_gz.h` (`tools/gzip_webui.py`) o el cambio no surte efecto.

9. **`MANUAL_USUARIO.md`**: subsección "Reloj de respaldo (RTC DS3231)" (para qué sirve, opcional) y
   añadir el RTC a la tabla de **pinout/periféricos** (§ Conexiones de Hardware): bus I²C compartido
   con el BME280, dirección `0x68`, 3V3, nota de la CR2032 vs LIR2032.

---

## 4. Notas de diseño y casos borde

- **Bus compartido sin conflicto:** RTC `0x68`, BME280 `0x76/0x77`, EEPROM `0x57`. Conviven.
- **`lostPower()`:** si la pila está agotada, la hora del RTC es basura → **no** sembrar desde él;
  esperar a NTP. En cuanto NTP sincronice, se reescribe el RTC y queda válido.
- **UTC siempre:** el RTC guarda UTC; el cambio de horario de verano y el TZ los sigue gestionando
  `ezTime`. Nunca guardar hora local en el RTC.
- **Orden en `setup()`:** RTC `begin` debe ir **antes** de sembrar la hora y antes de `cwDateTime.begin`.
  El sensor y el RTC comparten bus, así que da igual cuál llame primero a `cwI2CEnsureBegun`.
- **DS3231 con sensor de temperatura interno:** el DS3231 mide su propia temperatura (para el TCXO).
  Se puede exponer como dato curioso, pero **no** sustituye al BME280 (mide el chip, no el ambiente).
  Fuera de alcance por ahora.

---

## Orden de ejecución sugerido

1. **Fase 1 (firmware base).** Helper `cwI2CEnsureBegun` + refactor de `CWSensor`; `RTClib` en
   `platformio.ini`; `CWRtc.h/.cpp` (singleton, `begin/available/getEpochUTC/setEpochUTC/lostPower`);
   `pref rtcEnabled`. Compilar en ambos entornos. (Sin hardware aún, igual que el sensor.)
2. **Fase 2 (orquestación de hora).** `CWDateTime::setFromEpochUTC/getEpochUTC`; en `main.cpp` sembrar
   desde RTC antes de NTP y reescribir RTC tras sync (+ resync periódico opcional). Compilar.
3. **Fase 3 (API + Web UI).** `/api/rtc`, `rtcEnabled` en settings/set, tarjeta RTC en `WebUI.h`,
   regenerar `WebUI_gz.h`.
4. **Fase 4 (docs).** Subsección en el manual + RTC en la tabla de pinout.
5. **Prueba en hardware** cuando llegue el módulo: arrancar sin WiFi (debe dar hora del RTC), luego
   con WiFi (debe sincronizar y escribir RTC), y cortar alimentación para verificar que conserva hora.
