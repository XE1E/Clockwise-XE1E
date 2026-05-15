# Plan de Trabajo - ClockWise-XE1E v1.0

## Resumen del Proyecto
Transformar Clockwise en un proyecto personal optimizado con nueva arquitectura, 
interfaz web moderna y soporte multi-WiFi.

**Fecha inicio:** 2026-05-13  
**Versión objetivo:** 1.0

---

## Fase 1: Preparación y Renombrado
**Estado:** ✅ Completado

- [x] Renombrar proyecto a ClockWise-XE1E
- [x] Actualizar version a 1.0 en firmware
- [x] Mostrar "XE1E" debajo del logo en todas las pantallas
- [ ] Actualizar README con nueva identidad
- [ ] Limpiar referencias a proyecto original donde aplique

---

## Fase 2: Arquitectura de Red
**Estado:** ✅ Completado  
**Prioridad:** Alta

### 2.1 WiFi Multi-Red
- [x] Estructura para 3 redes WiFi (SSID, password)
- [x] Escaneo de redes al inicio
- [x] Conexión automática a mejor señal (RSSI)
- [x] Fallback a siguiente red si falla
- [x] AP mode propio si todas fallan (sin WiFiManager)
- [x] Reconexión automática si se pierde conexión (cada 30s)
- [x] Mostrar IP en pantalla al conectar (3 segundos)

### 2.2 ESPAsyncWebServer
- [x] Migrar de WiFiServer a ESPAsyncWebServer
- [x] Endpoints async no bloqueantes
- [x] Eliminar dependencia de WiFiManager
- [ ] WebSocket para actualizaciones en tiempo real (opcional)

### 2.3 FreeRTOS Tasks (Diferido)
> Nota: No implementado porque ESPAsyncWebServer ya maneja requests en segundo plano.
> El beneficio sería marginal. Se puede reconsiderar si hay problemas de bloqueo.

---

## Fase 3: Interfaz Web Nueva
**Estado:** ✅ Completado (Base)  
**Prioridad:** Alta  
**Referencia:** svitrix-firmware-XE1E/web/

### 3.1 Diseño Base
- [x] Header fijo con logo ClockWise XE1E
- [x] Navegación por pestañas (7 tabs)
- [x] Tema claro/oscuro (toggle)
- [x] Diseño responsive (mobile-friendly)
- [x] Variables CSS para theming

### 3.2 Pestañas Implementadas
1. **Inicio** ✅
   - Estado del reloj (IP, RSSI, hora, clockface actual)
   - Acciones rápidas (reiniciar, refrescar)

2. **WiFi** ✅
   - 3 redes configurables
   - Guardado independiente

3. **Pantalla** ✅
   - Brillo manual con valor visible
   - Rotación de pantalla
   - Swap Blue/Green
   - Configuración LDR (brillo automático)

4. **Hora** ✅
   - Zona horaria (POSIX, sin DST para México)
   - Formato 24h/12h
   - Servidor NTP (Cloudflare default)
   - Idioma (español/inglés)

5. **Nocturno** ✅
   - Horario inicio/fin
   - Brillo nocturno con valor visible
   - Carátula nocturna

6. **Carátula** ✅
   - Servidor y archivo
   - Rotación de carátulas (enable, intervalo, lista)

7. **Sistema** ✅
   - Info firmware (nombre, versión)
   - Reiniciar
   - Reset de fábrica

### 3.3 Pendiente
- [ ] Integrar editores de carátulas (Fase 5)
- [ ] Escaneo de redes WiFi disponibles
- [ ] Preview de carátulas

---

## Fase 4: Optimizaciones Firmware
**Estado:** En progreso  
**Prioridad:** Media

### 4.1 Ya Implementado
- [x] Quitar load() innecesarios en web server
- [x] Quitar save() en rotación de carátulas
- [x] Aumentar timeout WiFi (20s)
- [x] Reintentos de conexión WiFi (2x por red)
- [x] Mostrar IP al conectar
- [x] Aplanar submodulos a repo único
- [x] Detección automática POSIX vs Olson timezone
- [x] Carátulas guardadas en SPIFFS (funciona offline)
- [x] GitHub Pages como fuente de carátulas (SSL estable)
- [x] 26+ fuentes BDF integradas
- [x] Firmware size: 95% flash (1.2MB) - revisar fuentes para optimizar

### 4.2 Pendiente
- [ ] Cache de valores JSON en sprites (evitar parsing repetido)
- [ ] Optimizar renderElements (pre-calcular valores)
- [ ] Guardar solo preferencias modificadas (no todas)

---

## Fase 5: Clockfaces y Editores
**Estado:** Pendiente  
**Prioridad:** Media

### 5.1 Editores existentes (clockface-editor/)
- [ ] Revisar y actualizar index.html (editor principal)
- [ ] Revisar digit-designer.html
- [ ] Revisar char-designer.html
- [ ] Revisar font-converter.html
- [ ] Integrar en nueva interfaz web
- [ ] Documentar uso de cada editor

### 5.2 Clockfaces
- [ ] Verificar funcionamiento de carátulas built-in
- [ ] Verificar carátulas JSON
- [ ] Generar thumbnails faltantes
- [ ] Documentar formato JSON de carátulas

---

## Fase 6: Documentación
**Estado:** Pendiente  
**Prioridad:** Baja

- [ ] README actualizado
- [ ] Guía de instalación
- [ ] Guía de uso de editores
- [ ] Guía de desarrollo de carátulas
- [ ] Changelog v1.0

---

## Fase 7: Testing y Release
**Estado:** En progreso

- [x] Test de conexión WiFi (señal débil, múltiples redes)
- [x] Test de interfaz web (parámetros básicos)
- [ ] Test de rotación de carátulas
- [ ] Test de modo nocturno
- [x] Test en hardware real
- [ ] Compilar firmware release
- [ ] Tag v1.0 en git

---

## Archivos Modificados

### Firmware (actualizados 2026-05-13)
- `firmware/src/main.cpp` - Loop con checkReconnect()
- `firmware/lib/cw-commons/WiFiController.h` - WiFi multi-red, AP mode, auto-reconnect
- `firmware/lib/cw-commons/CWWebServer.h` - ESPAsyncWebServer, API completa
- `firmware/lib/cw-commons/WebUI.h` - Nueva interfaz web con tabs
- `firmware/lib/cw-commons/CWPreferences.h` - 3 redes WiFi, rotación
- `firmware/lib/cw-commons/StatusController.h` - XE1E branding, reconexión
- `firmware/lib/cw-commons/CWDateTime.cpp` - POSIX timezone detection
- `firmware/platformio.ini` - ESPAsyncWebServer, sin WiFiManager

### Editores (pendientes de integrar)
- `clockface-editor/index.html`
- `clockface-editor/digit-designer.html`
- `clockface-editor/char-designer.html`

---

## Notas Técnicas

### Dependencias
- ESPAsyncWebServer (reemplaza WiFiServer)
- AsyncTCP
- Sin WiFiManager (implementación propia de AP mode)

### Memoria
- Firmware: 1.2MB (95% de 1.3MB) - 26 fuentes BDF agregadas
- RAM: 103KB (31.7% de 320KB)
- SPIFFS: ~1.5MB disponible para carátulas
- Interfaz web embebida: ~15KB

### Compatibilidad
- ESP32 (original Clockwise hardware)
- Matriz LED 64x64 HUB75

---

## Historial de Cambios

### 2026-05-15
- Carátulas guardadas en SPIFFS (funciona offline)
- GitHub Pages XE1E como fuente de carátulas
- 26+ fuentes BDF convertidas e integradas
- Hora en palabras (Hw/iw) con fix de superposición
- API endpoints para gestión de carátulas locales
- Interfaz web para subir/eliminar carátulas
- Indicador de espacio de almacenamiento
- Manuales actualizados

### 2026-05-13
- Creación del plan de trabajo
- Submodulos aplanados a repo único
- WiFi multi-red con RSSI
- ESPAsyncWebServer (sin WiFiManager)
- Nueva interfaz web con 7 pestañas
- Tema claro/oscuro
- Reconexión automática WiFi
- XE1E branding en pantallas
- POSIX timezone (México sin DST)
- Reset de fábrica

