# TODO - ClockWise-XE1E

> Ver [docs/dev/PLAN-DE-TRABAJO.md](docs/dev/PLAN-DE-TRABAJO.md) para el plan completo.

## En Progreso

### Fase 2: Arquitectura FreeRTOS
- [x] WiFi multi-red (3 redes, mejor señal)
- [x] Migrar a ESPAsyncWebServer
- [ ] Tasks separados (Network / Display)

### Fase 3: Interfaz Web Nueva
- [x] Diseño con pestañas
- [x] Tema claro/oscuro
- [x] Integrar editores de clockface

### Sensor Ambiental BME280
> Plan: [docs/dev/PLAN-SENSOR-AMBIENTAL.md](docs/dev/PLAN-SENSOR-AMBIENTAL.md)
- [x] Firmware: CWSensor.cpp/h, endpoints API, integración main.cpp
- [x] Editor: SensorElement, panel de propiedades, valores de prueba
- [x] Web UI: Tarjeta sensor con config de pines
- [x] Render carátula: type:"sensor" con source temp/hum/pres
- [ ] Fuentes: Glifo ° en casos especiales (picopixel, tomthumb, square, atari, 8x13, haxor-12)
- [ ] Prueba en hardware real con módulo BME280

### RTC DS3231 (Reloj de respaldo)
> Plan: [docs/dev/PLAN-RTC-DS3231.md](docs/dev/PLAN-RTC-DS3231.md)
- [ ] Firmware: Helper cwI2CEnsureBegun (bus I²C compartido)
- [ ] Firmware: CWRtc.cpp/h (begin, available, getEpochUTC, setEpochUTC, lostPower)
- [ ] Firmware: RTClib en platformio.ini
- [ ] Firmware: Orquestación en main.cpp (sembrar hora desde RTC, sync a RTC tras NTP)
- [ ] Firmware: CWDateTime setFromEpochUTC/getEpochUTC
- [ ] Firmware: pref rtcEnabled en CWPreferences.h
- [ ] Firmware: Endpoints /api/rtc, rtcEnabled en /api/settings y /api/set
- [ ] Web UI: Tarjeta RTC en WebUI.h + regenerar WebUI_gz.h
- [ ] Docs: Subsección RTC en MANUAL_USUARIO.md

### Pendientes
- [ ] Actualizacion OTA (subir firmware desde web)
- [ ] Subir carátulas al repo GitHub desde el editor (requiere token)
- [ ] Agregar capturas de pantalla a MANUAL_CONFIGURACION.md (crear docs/images/)
- [ ] Integrar Mario Bros clock como clockface nativo (cw-cf-0x08) - https://github.com/jnthas/mariobros-clock
- [ ] Integrar cw-cf-0x05 como clockface nativo - https://github.com/jnthas/cw-cf-0x05

## Próximo

### Fase 4: Optimizaciones
- [x] Cache JSON en sprites
- [ ] Guardar solo prefs modificadas
- [ ] Buffer HTTP completo

## Completado

### 2026-05-19
- [x] Fix: página web no cargaba (ArduinoJson en /api/settings evita fragmentación heap)
- [x] Fix: thumbnails cargando en paralelo agotaban memoria (carga secuencial)
- [x] Revisar fuentes firmware: eliminadas no funcionales, solo 8x13, 8x13B, haxor-12
- [x] WebUI_minimal.h creada como alternativa ligera para testing

### 2026-05-15
- [x] Simplificación: carátulas SIEMPRE desde SPIFFS local
- [x] Repos (GitHub, local) solo para descargar y guardar nuevas carátulas
- [x] Flash reducido de 95% a 83.8% (eliminado código de descarga directa)
- [x] Interfaz web simplificada: guardadas + descarga de repo + selección
- [x] Fuente "nocturno" agregada al editor y firmware (dígitos grandes 20x29)
- [x] Recuperar archivos mi-fuente (dígitos personalizados)
- [x] Mario clockface convertido a JSON (parcial)
- [x] Eliminar workflow esp-idf.yml (proyecto usa PlatformIO)
- [x] Carátulas guardadas en reloj (SPIFFS) - funciona offline
- [x] GitHub Pages XE1E como fuente de carátulas (estable SSL)
- [x] 26+ fuentes BDF convertidas e integradas (editor y firmware)
- [x] Hora en palabras (formato Hw/iw) con fix de superposición
- [x] Conversión de fuentes JSON al formato correcto (bitmaps/glyphs)
- [x] Endpoints API: /api/storage, /api/clockfaces/list, upload, delete, get
- [x] Interfaz web para gestionar carátulas locales (subir, listar, eliminar, espacio)

### 2026-05-14
- [x] Servidor local: interfaz para probar carátulas sin subir a GitHub
- [x] Editor: botón "Probar en Reloj" para envío directo de carátulas
- [x] Simplificación: sección de carátulas unificada (fuente + selección + rotación)
- [x] Brillo: aplicación inmediata desde web (sin reiniciar)
- [x] Rotación: inicia con primera caratula de la lista
- [x] Modo nocturno: color dinámico configurable (fgColor: -1)
- [x] Modo nocturno: datetime en setup y loop arrays
- [x] API: añadir nightColor a GET/POST settings
- [x] CDN XE1E: servidor alternativo para clockfaces (SSL compatible)
- [x] Selector de fuente: CDN vs GitHub
- [x] Fallback clock cuando falla descarga

### 2026-05-13
- [x] Aplanar submodulos a repo único
- [x] Fix: quitar load() innecesarios en web server
- [x] Fix: quitar save() en rotación
- [x] WiFi: timeout aumentado a 20s
- [x] WiFi: 3 reintentos de conexión
- [x] WiFi: mostrar IP y RSSI en pantalla
- [x] Crear plan de trabajo
- [x] Renombrar proyecto a ClockWise-XE1E v1.0.0
- [x] WiFi multi-red: soporte para 3 redes
- [x] WiFi multi-red: escaneo y conexión a mejor señal
- [x] WiFi multi-red: fallback automático
- [x] Actualizar CWPreferences con redes 2 y 3
- [x] Actualizar CWWebServer con endpoints para redes 2 y 3
- [x] StatusController: pantalla de escaneo WiFi

### Anteriores
- [x] Fork de clockfaces (0x01, 0x02, 0x03, 0x07)
- [x] Modo nocturno con horario configurable
- [x] Rotación de carátulas
- [x] Soporte idioma español
- [x] Manuales de usuario
