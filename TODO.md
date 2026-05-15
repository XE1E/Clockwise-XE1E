# TODO - ClockWise-XE1E

> Ver [PLAN-DE-TRABAJO.md](PLAN-DE-TRABAJO.md) para el plan completo.

## En Progreso

### Fase 2: Arquitectura FreeRTOS
- [x] WiFi multi-red (3 redes, mejor señal)
- [x] Migrar a ESPAsyncWebServer
- [ ] Tasks separados (Network / Display)

### Fase 3: Interfaz Web Nueva
- [x] Diseño con pestañas
- [ ] Tema claro/oscuro
- [ ] Integrar editores de clockface

### Pendientes
- [ ] Convertir clockfaces 0x01-0x06 a JSON
- [ ] Listar solo clockfaces disponibles en CDN/GitHub
- [ ] Sistema de fuentes dinámicas (cargar desde SPIFFS sin recompilar firmware)
  - Actualmente las fuentes deben compilarse en el firmware
  - Opciones: SPIFFS, sprites por dígito, o mantener fuentes predefinidas

## Próximo

### Fase 4: Optimizaciones
- [x] Cache JSON en sprites
- [ ] Guardar solo prefs modificadas
- [ ] Buffer HTTP completo

### Fase 5: Editores
- [ ] Revisar digit-designer
- [ ] Integrar en web principal

## Completado

### 2026-05-15
- [x] Fuente "nocturno" agregada al editor y firmware (dígitos grandes 20x29)
- [x] Recuperar archivos mi-fuente (dígitos personalizados)
- [x] Mario clockface convertido a JSON (parcial)
- [x] Eliminar workflow esp-idf.yml (proyecto usa PlatformIO)

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
