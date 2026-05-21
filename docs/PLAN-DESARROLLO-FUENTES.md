# Plan de Desarrollo: Sistema de Fuentes y Carátulas

## Objetivo

Permitir que usuarios finales puedan:
1. Usar carátulas de una galería (sin compilar)
2. Crear/editar fuentes y carátulas personalizadas
3. Flashear firmware via web (sin instalar herramientas)

---

## Fases del Proyecto

### Fase 1: Sincronización de Fuentes (Editor → Firmware)

**Objetivo**: Automatizar la conversión de fuentes entre pixel-fonts.js y firmware/*.h

**Tareas**:

- [ ] **1.1** Crear función `generateFontHeader(fontName)` en editor-fuentes.html
  - Input: nombre de fuente en PixelFonts
  - Output: código C++ formato Adafruit GFX
  - Incluir: bitmaps, glyphs, struct GFXfont

- [ ] **1.2** Agregar botón "Exportar .h" en galería de fuentes
  - Genera y descarga archivo .h individual
  - Nombrado: `{fontName}.h`

- [ ] **1.3** Agregar botón "Exportar todas a firmware"
  - Genera ZIP con todos los .h
  - Incluye template de Clockface.h con includes
  - Incluye template de setFont() para Clockface.cpp

- [ ] **1.4** Documentar proceso manual de sincronización
  - Instrucciones paso a paso
  - Dónde colocar archivos
  - Cómo actualizar Clockface.h y Clockface.cpp

**Entregables**:
- Función de conversión JS → C++
- UI para exportar fuentes individuales y en batch
- Documentación de sincronización

**Estimación**: 2-3 días

---

### Fase 2: GitHub Actions para Build Automático

**Objetivo**: Compilar firmware automáticamente cuando hay cambios

**Tareas**:

- [ ] **2.1** Crear workflow `.github/workflows/build-firmware.yml`
  ```yaml
  - Trigger: push a main (paths: firmware/**, clockface-editor/js/pixel-fonts.js)
  - Steps: checkout, setup-python, install platformio, build, upload artifact
  ```

- [ ] **2.2** Configurar PlatformIO para CI
  - Verificar platformio.ini tiene configuración correcta
  - Agregar script de pre-build si es necesario

- [ ] **2.3** Publicar .bin como Release
  - Crear release automático con tag de versión
  - Adjuntar firmware.bin
  - Generar changelog básico

- [ ] **2.4** Agregar badge de build status al README

**Entregables**:
- Workflow de GitHub Actions funcional
- Releases automáticos con .bin descargable
- Badge de estado en README

**Estimación**: 1-2 días

---

### Fase 3: Web Flasher

**Objetivo**: Permitir flashear firmware desde el navegador

**Tareas**:

- [ ] **3.1** Crear página de flasher (`docs/flasher/index.html`)
  - Integrar ESP Web Tools
  - Selector de versión de firmware
  - Instrucciones de conexión USB

- [ ] **3.2** Configurar manifest.json para ESP Web Tools
  - Apuntar a .bin de releases
  - Configurar particiones si es necesario

- [ ] **3.3** Agregar página de releases/descargas
  - Lista de versiones disponibles
  - Changelog de cada versión
  - Links directos a .bin

- [ ] **3.4** Publicar en GitHub Pages
  - Configurar GitHub Pages para /docs
  - Dominio personalizado (opcional)

**Entregables**:
- Página web de flasher funcional
- Página de releases con historial
- Sitio publicado en GitHub Pages

**Estimación**: 2-3 días

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
- [ ] Usuario puede exportar fuente a .h desde editor
- [ ] GitHub Actions compila firmware automáticamente
- [ ] Usuario puede flashear .bin desde navegador
- [ ] Documentación clara del proceso

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
