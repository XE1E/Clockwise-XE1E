# Plan de Trabajo - ClockWise-XE1E v1.0

## Resumen del Proyecto
Transformar Clockwise en un proyecto personal optimizado con nueva arquitectura, 
interfaz web moderna y soporte multi-WiFi.

**Fecha inicio:** 2026-05-13  
**Versión objetivo:** 1.0

---

## Fase 1: Preparación y Renombrado
**Estado:** Pendiente

- [ ] Renombrar proyecto a ClockWise-XE1E
- [ ] Actualizar version a 1.0 en firmware
- [ ] Actualizar README con nueva identidad
- [ ] Limpiar referencias a proyecto original donde aplique

---

## Fase 2: Arquitectura FreeRTOS
**Estado:** Pendiente  
**Prioridad:** Alta

### 2.1 Estructura de Tasks
```
Core 0: taskNetwork
  - WiFi Manager (3 redes, auto-connect mejor señal)
  - ESPAsyncWebServer
  - NTP sync
  - mDNS

Core 1: taskDisplay (loop principal)
  - Rendering clockface
  - Animaciones
  - Modo nocturno
```

### 2.2 WiFi Multi-Red
- [ ] Estructura para 3 redes WiFi (SSID, password, enabled)
- [ ] Escaneo de redes al inicio
- [ ] Conexión automática a mejor señal (RSSI)
- [ ] Fallback a siguiente red si falla
- [ ] AP mode si todas fallan
- [ ] Reconexión automática si se pierde conexión
- [ ] Mostrar IP y RSSI en pantalla al conectar (3 segundos)

### 2.3 Comunicación entre Tasks
- [ ] Queue para comandos (reload clockface, cambiar brillo, etc.)
- [ ] Mutex para acceso a preferencias
- [ ] Event groups para sincronización

### 2.4 ESPAsyncWebServer
- [ ] Migrar de WiFiServer a ESPAsyncWebServer
- [ ] Endpoints async no bloqueantes
- [ ] WebSocket para actualizaciones en tiempo real (opcional)

---

## Fase 3: Interfaz Web Nueva
**Estado:** Pendiente  
**Prioridad:** Alta  
**Referencia:** svitrix-firmware-XE1E/web/

### 3.1 Diseño Base
- [ ] Header fijo con logo ClockWise
- [ ] Navegación por pestañas
- [ ] Tema claro/oscuro (toggle)
- [ ] Diseño responsive (mobile-friendly)
- [ ] Variables CSS para theming

### 3.2 Pestañas/Secciones
1. **Inicio**
   - Estado del reloj (IP, RSSI, hora, clockface actual)
   - Acciones rápidas (reiniciar, cambiar clockface)

2. **Carátulas**
   - Grid de thumbnails
   - Preview de carátula
   - Selector de carátula activa
   - Lista de rotación

3. **WiFi**
   - 3 redes configurables
   - Escaneo de redes disponibles
   - Indicador de señal
   - Test de conexión

4. **Pantalla**
   - Brillo manual/automático
   - Rotación de pantalla
   - Swap Blue/Green

5. **Modo Nocturno**
   - Horario inicio/fin
   - Brillo nocturno
   - Color nocturno
   - Carátula nocturna

6. **Hora**
   - Zona horaria
   - Formato 24h/12h
   - Servidor NTP
   - Idioma (español/inglés)

7. **Editores** (integrados de clockface-editor)
   - Editor de carátulas (index.html)
   - Diseñador de dígitos (digit-designer.html)
   - Diseñador de caracteres (char-designer.html)
   - Conversor de fuentes (font-converter.html)

8. **Sistema**
   - Info firmware (versión, memoria libre)
   - Actualización OTA
   - Backup/Restore configuración
   - Reset a defaults

### 3.3 Estilos (basado en svitrix)
```css
:root {
  --bg: #0f1117;
  --bg-card: #1a1d2e;
  --bg-input: #252940;
  --border: #363b5c;
  --text: #e0e0e8;
  --text-dim: #9090a8;
  --accent: #f0b800;  /* Color Clockwise */
  --radius: 8px;
}
```

---

## Fase 4: Optimizaciones Firmware
**Estado:** En progreso  
**Prioridad:** Media

### 4.1 Ya Implementado (2026-05-13)
- [x] Quitar load() innecesarios en web server
- [x] Quitar save() en rotación de carátulas
- [x] Aumentar timeout WiFi (20s)
- [x] Reintentos de conexión WiFi (3x)
- [x] Mostrar IP y RSSI al conectar
- [x] Aplanar submodulos a repo único

### 4.2 Pendiente
- [ ] Cache de valores JSON en sprites (evitar parsing repetido)
- [ ] Optimizar renderElements (pre-calcular valores)
- [ ] Buffer de HTTP request completo (no char por char)
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
- [ ] Verificar funcionamiento de 7 carátulas built-in
- [ ] Verificar 11 carátulas JSON
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
**Estado:** Pendiente

- [ ] Test de conexión WiFi (señal débil, múltiples redes)
- [ ] Test de interfaz web (todos los parámetros)
- [ ] Test de rotación de carátulas
- [ ] Test de modo nocturno
- [ ] Test en hardware real
- [ ] Compilar firmware release
- [ ] Tag v1.0 en git

---

## Archivos Clave a Modificar

### Firmware
- `firmware/src/main.cpp` - Loop principal, tasks
- `firmware/lib/cw-commons/WiFiController.h` - WiFi multi-red
- `firmware/lib/cw-commons/CWWebServer.h` - Migrar a async
- `firmware/lib/cw-commons/CWPreferences.h` - 3 redes WiFi
- `firmware/lib/cw-commons/StatusController.h` - Display de estado

### Web (nuevo)
- `web/index.html` - SPA principal
- `web/css/styles.css` - Estilos globales
- `web/js/app.js` - Lógica principal
- `web/js/api.js` - Comunicación con ESP32

### Editores (existentes)
- `clockface-editor/index.html`
- `clockface-editor/digit-designer.html`
- `clockface-editor/char-designer.html`

---

## Notas Técnicas

### Dependencias nuevas
- ESPAsyncWebServer
- AsyncTCP

### Memoria estimada
- Interfaz web embebida: ~50KB gzip
- Firmware: ~1.2MB
- Espacio libre SPIFFS: ~1.5MB

### Compatibilidad
- ESP32 (original Clockwise hardware)
- Matriz LED 64x64 HUB75

---

## Historial de Cambios

### 2026-05-13
- Creación del plan de trabajo
- Submodulos aplanados a repo único
- Fixes iniciales de WiFi y web server
- Propuesta de arquitectura FreeRTOS

