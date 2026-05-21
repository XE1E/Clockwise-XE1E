# Plan de Desarrollo: Sistema de Fuentes y Carátulas

## Objetivo

Permitir que usuarios finales puedan:
1. Usar carátulas de una galería (sin compilar)
2. Crear/editar fuentes y carátulas personalizadas
3. Flashear firmware via web (sin instalar herramientas)

---

## Modelo de Independencia

**Principio**: El proyecto original NUNCA se modifica por personalizaciones de usuarios.

| Tipo de Usuario | Qué usa | Modifica repo original |
|-----------------|---------|------------------------|
| Básico | Firmware oficial + carátulas de galería | NO |
| Creativo (carátulas) | Firmware oficial + carátulas propias | NO |
| Creativo (fuentes) | Fork propio + fuentes custom | NO (usa su fork) |

**Flujo para fuentes custom**:
```
Usuario hace FORK → Modifica su fork → Su GitHub Actions compila → Su .bin personal
```

Esto garantiza:
- Proyecto original limpio y estable
- Usuarios avanzados con control total
- GitHub Actions gratis para cada fork público

---

## Fases del Proyecto

### Fase 1: Sincronización de Fuentes (Editor → Firmware)

**Objetivo**: Automatizar la conversión de fuentes entre pixel-fonts.js y firmware/*.h

**Tareas**:

- [x] **1.1** Crear función `generateFontHeader(fontName)` en editor-fuentes.html
  - Input: nombre de fuente en PixelFonts
  - Output: código C++ formato Adafruit GFX
  - Incluir: bitmaps, glyphs, struct GFXfont
  - **COMPLETADO**: También `exportCurrentFontToHeader()` para fuente editada

- [x] **1.2** Agregar botón "Exportar .h" en panel de exportación
  - Genera y guarda archivo .h en carpeta seleccionada
  - Nombrado: `{fontName}.h`
  - **COMPLETADO**: Botón en panel derecho, selector de carpeta con memoria

- [x] **1.3** Agregar botón "Sincronizar con firmware" en galería
  - Genera todos los .h desde pixel-fonts.js
  - Incluye template de Clockface.h con includes
  - Incluye template de setFont() para Clockface.cpp
  - **COMPLETADO**: Genera `_INCLUDES_TEMPLATE.txt` y `_SETFONT_TEMPLATE.txt`

- [x] **1.4** Documentar proceso manual de sincronización
  - Instrucciones paso a paso
  - Dónde colocar archivos
  - Cómo actualizar Clockface.h y Clockface.cpp
  - **COMPLETADO**: Documentado en FLUJO-FUENTES.md secciones 6 y 7

**Entregables**:
- ✅ Función de conversión JS → C++
- ✅ UI para exportar fuentes individuales y en batch
- ✅ Documentación de sincronización

**Estado**: COMPLETADO

---

### Fase 2: GitHub Actions para Build Automático

**Objetivo**: Compilar firmware automáticamente cuando hay cambios

**Tareas**:

- [x] **2.1** Crear workflow `.github/workflows/build-firmware.yml`
  - Trigger: push a main (paths: firmware/**, clockface-editor/js/pixel-fonts.js)
  - Trigger manual: workflow_dispatch con nombre de release opcional
  - Steps: checkout, cache, setup-python, install platformio, build, upload artifact, create release
  - **COMPLETADO**: `.github/workflows/build-firmware.yml`

- [x] **2.2** Configurar PlatformIO para CI
  - platformio.ini ya tiene configuración correcta para esp32dev
  - No requiere script de pre-build adicional
  - **COMPLETADO**: Configuración existente funciona

- [x] **2.3** Publicar .bin como Release
  - Release automático con tag de versión (vYYYYMMDD.HHMM)
  - Adjunta firmware-{date}-{sha}.bin
  - Genera changelog con mensaje del commit
  - **COMPLETADO**: Integrado en workflow

- [x] **2.4** Agregar badge de build status al README
  - **COMPLETADO**: Badge agregado en README.md

- [x] **2.5** Documentar flujo de fork para usuarios avanzados
  - Guía paso a paso: cómo hacer fork
  - Cómo habilitar GitHub Actions
  - Cómo agregar fuentes personalizadas
  - Cómo esperar el build y descargar .bin
  - Cómo sincronizar con actualizaciones del repo original
  - **COMPLETADO**: `docs/GUIA-FORK-FUENTES-CUSTOM.md`

**Entregables**:
- ✅ Workflow de GitHub Actions funcional
- ✅ Releases automáticos con .bin descargable
- ✅ Badge de estado en README
- ✅ Guía de fork para usuarios avanzados

**Estado**: COMPLETADO

---

### Fase 3: Web Flasher

**Objetivo**: Permitir flashear firmware desde el navegador

**Tareas**:

- [x] **3.1** Crear página de flasher (`web-flasher/index.html`)
  - Integrar ESP Web Tools
  - Selector de placa (ESP32 / ESP32-S3)
  - Instrucciones de conexión USB
  - **COMPLETADO**: Página con selector de placa funcional

- [x] **3.2** Configurar manifest.json para ESP Web Tools
  - manifest.json para ESP32 (`cw-cf-0x07/manifest.json`)
  - manifest.json para ESP32-S3 (`esp32s3/manifest.json`)
  - **COMPLETADO**: Ambos manifests configurados

- [x] **3.3** Actualizar workflow para generar releases de ambas placas
  - Build paralelo ESP32 y ESP32-S3
  - Releases incluyen ambos firmwares
  - **COMPLETADO**: `.github/workflows/build-firmware.yml` actualizado

- [x] **3.4** Publicar en GitHub Pages
  - Workflow de deploy automático
  - Incluye web-flasher y clockface-editor
  - **COMPLETADO**: `.github/workflows/deploy-pages.yml`

**Entregables**:
- ✅ Página web de flasher con selector de placa
- ✅ Soporte para ESP32 y ESP32-S3
- ✅ Workflow de GitHub Pages

**Estado**: COMPLETADO

**Nota**: Después del push, habilitar GitHub Pages en Settings → Pages → Source: GitHub Actions

---

### Fase 4: Galería de Carátulas

**Objetivo**: Repositorio de carátulas listas para usar

**Tareas**:

- [ ] **4.1** Crear estructura de galería
  ```
  docs/galeria/
  ├── index.html           Página principal
  ├── clockfaces/          JSONs de carátulas
  │   ├── digital-1.json
  │   ├── analog-1.json
  │   └── ...
  └── previews/            Imágenes de preview
      ├── digital-1.png
      └── ...
  ```

- [ ] **4.2** Crear página de galería con previews
  - Grid de carátulas con imagen
  - Botón descargar JSON
  - Filtros por categoría (opcional)

- [ ] **4.3** Agregar instrucciones de instalación
  - Cómo subir JSON al reloj via WiFi
  - Troubleshooting común

- [ ] **4.4** Integrar con editor de carátulas
  - Botón "Publicar en galería" (opcional, requiere PR)
  - O instrucciones para contribuir

**Entregables**:
- Galería web con carátulas pre-hechas
- Sistema de previews
- Documentación de uso

**Estimación**: 2-3 días

---

### Fase 5: Script de Sincronización Automática

**Objetivo**: Sincronizar pixel-fonts.js → firmware automáticamente

**Tareas**:

- [ ] **5.1** Crear script Node.js `scripts/sync-fonts.js`
  - Lee pixel-fonts.js
  - Genera todos los .h
  - Actualiza Clockface.h (includes)
  - Actualiza Clockface.cpp (setFont)

- [ ] **5.2** Agregar al workflow de GitHub Actions
  - Ejecutar sync antes de compilar
  - Commit automático si hay cambios (opcional)

- [ ] **5.3** Agregar comando npm para ejecutar local
  - `npm run sync-fonts`
  - Para desarrolladores que quieren probar local

- [ ] **5.4** Validación de fuentes
  - Verificar formato correcto antes de generar
  - Reportar errores claros

**Entregables**:
- Script de sincronización funcional
- Integración con CI/CD
- Comando local para desarrolladores

**Estimación**: 2-3 días

---

### Fase 6: Mejoras de UX (Opcional)

**Objetivo**: Mejorar experiencia de usuario

**Tareas**:

- [ ] **6.1** Preview en vivo en editor de carátulas
  - Simular reloj en canvas
  - Actualización en tiempo real

- [ ] **6.2** Validación de carátulas
  - Verificar fuentes usadas existen
  - Advertir si usa fuente no disponible en firmware

- [ ] **6.3** Importar carátula desde galería al editor
  - Cargar JSON directamente
  - Modificar y re-exportar

- [ ] **6.4** Sistema de temas/plantillas
  - Carátulas base para modificar
  - Estilos predefinidos

**Entregables**:
- Mejoras de UX implementadas
- Validaciones adicionales

**Estimación**: 3-5 días

---

## Cronograma Sugerido

```
Semana 1: Fase 1 (Sincronización manual)
Semana 2: Fase 2 (GitHub Actions) + Fase 3 (Web Flasher)
Semana 3: Fase 4 (Galería) + Fase 5 (Sync automático)
Semana 4: Fase 6 (Mejoras UX) + Testing + Documentación final
```

---

## Dependencias Entre Fases

```
Fase 1 ──► Fase 5 (sync automático necesita conversión)
    │
    └──► Fase 2 ──► Fase 3 (flasher necesita .bin de CI)
              │
              └──► Fase 4 (galería puede ser paralela)
```

**Orden mínimo viable**:
1. Fase 1 (exportar .h manual)
2. Fase 2 (CI genera .bin)
3. Fase 3 (flasher web)

Con esto ya se puede usar el sistema completo de forma manual.

---

## Criterios de Éxito

### MVP (Mínimo Viable)
- [x] Usuario puede exportar fuente a .h desde editor
- [x] GitHub Actions compila firmware automáticamente
- [x] Usuario puede flashear .bin desde navegador
- [x] Documentación clara del proceso

### Versión Completa
- [ ] Sincronización automática de fuentes
- [ ] Galería de carátulas funcional
- [ ] Web flasher con selector de versiones
- [ ] Proceso end-to-end sin fricciones

---

## Recursos Necesarios

### Herramientas
- GitHub Actions (gratis para repos públicos)
- GitHub Pages (gratis)
- ESP Web Tools (open source)
- PlatformIO (open source)

### Conocimientos
- JavaScript (editores web)
- C++ básico (formato de fuentes)
- YAML (GitHub Actions)
- HTML/CSS (páginas web)

---

## Próximos Pasos

1. **Revisar y aprobar** este plan
2. **Priorizar** qué fases son más urgentes
3. **Comenzar** con Fase 1 (fundamento para todo lo demás)

---

## Notas

- El plan es flexible y puede ajustarse según necesidades
- Cada fase puede subdividirse si es muy grande
- Se recomienda hacer releases incrementales (no esperar a tener todo)
