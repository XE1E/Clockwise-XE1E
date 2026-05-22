# Changelog: Editor de Carátulas y Sprites

## Resumen de Cambios (Mayo 2026)

### Editor de Sprites - Mejoras Completadas

#### Interfaz Principal
- **Ventana más grande** (900px) para evitar scroll horizontal
- **Botón "Dibujar"** en la sección de lista de sprites para crear sprites dibujando directamente
- **Selección de sprites** funciona haciendo clic en cualquier parte del item

#### Gestión de Sprites
- **Importar/Exportar sprites** como archivos JSON independientes
  - Exportar muestra diálogo para elegir nombre y ubicación
  - Permite reutilizar sprites entre carátulas
- **Arrastrar para reordenar frames** en la secuencia de animación

#### Editor de Pixels
- **Canvas no cuadrado**: ancho y alto independientes (ej: 16x32)
- **Botón "+" en lista de frames** para crear nuevos frames dibujando
- **Clic en frame** para cargarlo en el editor de pixels
- **Botón "Copiar"** para copiar un frame existente al editor
- **Botón "Agregar"** agrega frame y resetea canvas (no cierra el editor)
- **Botón "Reemplazar"** guarda cambios en frame existente (no cierra el editor)
- **Botón "Cerrar"** para salir del editor

#### Animación
- **Pausa automática** de animación de sprite mientras se arrastra para posicionarlo

### Editor de Carátulas - Mejoras Completadas

#### Imágenes
- **Redimensionar arrastrando esquinas** - Las imágenes ahora muestran handles en las 4 esquinas
- **Controles W y H** siempre visibles en panel de propiedades con botón "Aplicar Tamaño"

#### Imagen de Referencia
- **Escala Ancho y Alto separados** (sliders independientes) ✓
- **Handles en esquinas** para redimensionar arrastrando ✓

### Archivos Modificados

#### HTML
- `clockface-editor/index.html`
  - Nueva estructura de sprite editor con layout mejorado
  - Botones importar/exportar sprites
  - Botón "Dibujar" para crear sprites
  - Editor de pixels con width/height separados
  - Botón "Copiar" en editor de pixels
  - Handles para imagen de referencia

#### CSS
- `clockface-editor/css/style.css`
  - Estilos para modal más grande (900px)
  - Estilos para frames arrastrables
  - Estilos para frame-item-add (+)
  - Estilos para handles de imagen de referencia
  - Estilos para reference-container

#### JavaScript
- `clockface-editor/js/editor.js`
  - `exportSprite()` - Exporta sprite seleccionado con diálogo de guardado
  - `importSprite()` - Importa sprite desde archivo JSON
  - `reorderFrame()` - Reordena frames mediante drag & drop
  - `copyFrameToPixelEditor()` - Copia frame existente al editor
  - `loadFrameToPixelEditor()` - Carga frame para editar
  - `openPixelEditorForNewFrame()` - Abre editor para nuevo frame
  - Soporte para canvas no cuadrado (width/height independientes)
  - Pausa de animación durante arrastre de sprite
  - Handles de redimensionamiento para imágenes
  - Escalado independiente X/Y para imagen de referencia

- `clockface-editor/js/canvas-renderer.js`
  - `getHandles()` - Ahora incluye handles para elementos tipo 'image'

### Correcciones Recientes

1. **Imagen de referencia - redimensionar arrastrando**: Corregido. El problema era que la imagen no tenía `width: 100%; height: 100%` en CSS, así que no seguía el tamaño del contenedor. Ahora la imagen se redimensiona correctamente al arrastrar las esquinas.

### Cómo Usar

#### Crear Sprite Nuevo
1. Abrir Editor de Sprites (botón "Sprites" en barra lateral)
2. Clic en "Dibujar" para crear sprite y abrir editor de pixels
3. Dibujar el frame, clic en "+ Agregar"
4. Repetir para más frames
5. Clic en "Cerrar" cuando termine

#### Editar Frame Existente
1. Clic en el frame en la lista
2. Se abre el editor de pixels con el contenido del frame
3. Editar y clic en "Reemplazar"

#### Reordenar Frames
- Arrastrar y soltar los frames en la lista

#### Exportar/Importar Sprites
- **Exportar**: Seleccionar sprite, clic en "Exportar", elegir ubicación
- **Importar**: Clic en "Importar", seleccionar archivo .json

#### Redimensionar Imágenes
- Seleccionar imagen en el canvas
- Arrastrar las esquinas verdes para redimensionar
- O usar controles W/H en panel derecho + "Aplicar Tamaño"
