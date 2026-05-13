# TODO - ClockWise-XE1E

> Ver [PLAN-DE-TRABAJO.md](PLAN-DE-TRABAJO.md) para el plan completo.

## En Progreso

### Fase 2: Arquitectura FreeRTOS
- [x] WiFi multi-red (3 redes, mejor señal)
- [ ] Migrar a ESPAsyncWebServer
- [ ] Tasks separados (Network / Display)

### Fase 3: Interfaz Web Nueva
- [ ] Diseño con pestañas (estilo svitrix)
- [ ] Tema claro/oscuro
- [ ] Integrar editores de clockface

## Próximo

### Fase 4: Optimizaciones
- [ ] Cache JSON en sprites
- [ ] Guardar solo prefs modificadas
- [ ] Buffer HTTP completo

### Fase 5: Editores
- [ ] Revisar digit-designer
- [ ] Integrar en web principal

## Completado

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
