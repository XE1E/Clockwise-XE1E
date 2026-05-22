# Plan: Integrar Clockfaces Nativos (Mario Bros y Pac-Man)

## Resumen Ejecutivo

Integrar las carátulas Mario Bros y Pac-Man como clockfaces nativos embebidos en el firmware, similares a cw-cf-0x07 (nocturna). Aparecerían siempre en el listado de rotación, solo se activan/desactivan.

---

## Estado Actual del Firmware

| Recurso | Usado | Disponible | Total |
|---------|-------|------------|-------|
| Flash ESP32 | 83.4% (1.09MB) | 217KB | 1.31MB |
| Flash ESP32-S3 | 30.7% (1.03MB) | 2.31MB | 3.34MB |
| RAM | 31.1% (102KB) | 226KB | 328KB |

---

## Análisis por Carátula

### 1. Pac-Man (cw-cf-0x05)

**Repositorio:** https://github.com/jnthas/cw-cf-0x05

#### Archivos necesarios
| Archivo | Tamaño | Descripción |
|---------|--------|-------------|
| Clockface.cpp/h | ~8KB | Lógica principal, mapa, pathfinding BFS |
| pacman.cpp/h | ~3KB | Sprite Pac-Man, movimiento, rotación |
| assets.h | ~1KB | Sprites (solo Pac-Man, 5x5 pixels) |
| hour_font.h | ~2KB | Fuente para la hora |
| **Total estimado** | **~14KB** | |

#### Animaciones y comportamiento
1. **Pac-Man se mueve automáticamente** por el laberinto (BFS pathfinding)
2. **Come puntos** - cuando pasa sobre ellos desaparecen
3. **Super-puntos** - al comerlos se vuelve "invencible" (colores rainbow por 7 seg)
4. **Segundos parpadean** - dos puntos entre hora:minuto
5. **Mapa se reinicia** - cuando come todos los puntos
6. **Fecha visible** - mes, día, día de semana

#### Estructuras de datos en RAM
- `_MAP[12][12]` - mapa del laberinto (~144 bytes)
- `visited[12][12]`, `parent[12][12]` - BFS (~576 bytes)
- `queue[144]` - cola BFS (~1.1KB)
- **RAM adicional estimada: ~2KB**

#### Complejidad de integración: MEDIA
- Requiere adaptar `Locator` (service locator) o eliminarlo
- BFS pathfinding es autocontenido
- Sin dependencias externas

---

### 2. Mario Bros (mariobros-clock)

**Repositorio:** https://github.com/jnthas/mariobros-clock

#### Archivos necesarios
| Archivo | Tamaño | Descripción |
|---------|--------|-------------|
| Clockface.cpp/h | ~3KB | Escena, actualización hora |
| mario.cpp/h | ~3KB | Mario: salto, estados, animación |
| block.cpp/h | ~3KB | Bloques hora/minuto con colisión |
| Sprite.cpp/h | ~2KB | Clase base sprites |
| EventBus.cpp/h | ~2KB | Sistema pub/sub para colisiones |
| Locator.cpp/h | ~1KB | Service locator |
| assets.h | ~17KB | Sprites: Mario, bloques, nubes, suelo, etc. |
| Super_Mario_Bros__24pt7b.h | ~3KB | Fuente estilo NES |
| **Total estimado** | **~34KB** | |

#### Animaciones y comportamiento
1. **Mario salta** cuando cambia el minuto (cada 60 seg)
2. **Bloques suben/bajan** cuando Mario los golpea desde abajo
3. **Física de salto** - gravedad, altura máxima
4. **Sistema de colisiones** - detecta cuando Mario toca bloques
5. **Escenario estático** - nubes, arbustos, colina, suelo (se dibuja una vez)

#### Estructuras de datos en RAM
- Objetos: Mario, 2 Blocks, tiles, objects (~500 bytes)
- EventBus subscribers (~100 bytes)
- **RAM adicional estimada: ~1KB**

#### Complejidad de integración: MEDIA-ALTA
- Sistema EventBus completo (pub/sub)
- Service Locator (Locator)
- Jerarquía de clases (Sprite base)
- Múltiples archivos interdependientes

---

## Impacto en Flash

| Componente | ESP32 | ESP32-S3 |
|------------|-------|----------|
| Firmware actual | 1.09MB (83.4%) | 1.03MB (30.7%) |
| + Pac-Man (~14KB) | 1.10MB (84.5%) | 1.04MB (31.1%) |
| + Mario (~34KB) | 1.14MB (87.0%) | 1.07MB (32.0%) |
| **Total con ambos** | **1.14MB (87%)** | **1.07MB (32%)** |

### Veredicto Flash
- **ESP32:** Viable pero ajustado (queda 13% = ~170KB)
- **ESP32-S3:** Sin problemas (queda 68% = 2.27MB)

---

## Pros y Contras

### PROS
1. **Siempre disponibles** - no dependen de SPIFFS ni descarga
2. **Experiencia completa** - animaciones que JSON no soporta
3. **Diferenciador** - carátulas únicas vs clones genéricos
4. **Código del mismo autor** - arquitectura compatible
5. **Nostálgico** - atractivo para usuarios retro/gaming
6. **ESP32-S3** - espacio de sobra

### CONTRAS
1. **Flash limitado en ESP32** - solo 13% libre después
2. **Complejidad de código** - más archivos que mantener
3. **No personalizables** - colores/posiciones fijos en código
4. **RAM adicional** - ~3KB combinado (pero hay margen)
5. **Conflictos potenciales** - EventBus/Locator vs arquitectura actual
6. **Compilación más lenta** - más archivos C++

---

## Modificabilidad

### Pac-Man - QUÉ SE PUEDE CAMBIAR
| Aspecto | Dificultad | Cómo |
|---------|------------|------|
| Colores (paredes, comida, Pac-Man) | Fácil | Constantes en código |
| Velocidad de Pac-Man | Fácil | Cambiar intervalo (75ms actual) |
| Tamaño del mapa | Media | Rediseñar `_MAP` y constantes |
| Diseño del laberinto | Media | Modificar `_MAP_CONST` |
| Posición del reloj | Media | Ajustar coordenadas en `updateClock()` |
| Agregar fantasmas | Difícil | Nuevo sistema de IA |

### Mario Bros - QUÉ SE PUEDE CAMBIAR
| Aspecto | Dificultad | Cómo |
|---------|------------|------|
| Color del cielo | Fácil | Constante `SKY_COLOR` |
| Altura del salto | Fácil | Constante `MARIO_JUMP_HEIGHT` |
| Velocidad del salto | Fácil | Intervalo en `update()` |
| Posición elementos | Media | Coordenadas en `setup()` |
| Sprites (colores) | Media | Modificar arrays en `assets.h` |
| Agregar enemigos | Difícil | Nuevo sistema de spawn + colisiones |
| Segundo jugador | Muy difícil | Duplicar lógica Mario |

---

## Plan de Implementación

### Fase 1: Preparación (2-3 horas)
1. Crear estructura `firmware/clockfaces/cw-cf-0x05/` y `cw-cf-0x08/`
2. Adaptar `Locator` como utilidad compartida o eliminar
3. Definir interfaz común con `cw-cf-0x07`

### Fase 2: Pac-Man (3-4 horas)
1. Copiar y adaptar archivos
2. Integrar con sistema de clockfaces existente
3. Agregar a rotación y UI web
4. Pruebas de funcionamiento

### Fase 3: Mario Bros (4-6 horas)
1. Copiar archivos base (Sprite, EventBus, Locator)
2. Adaptar Clockface, Mario, Block
3. Integrar assets y fuente
4. Pruebas de colisiones y animaciones

### Fase 4: UI y Configuración (2-3 horas)
1. Agregar toggles en interfaz web
2. Guardar preferencias en SPIFFS
3. Thumbnails para selector
4. Documentación

**Tiempo total estimado: 11-16 horas**

---

## Alternativas Consideradas

### A. Solo Pac-Man (recomendado para ESP32)
- Menor impacto en flash (~14KB vs ~48KB)
- Más simple de mantener
- Dejar Mario para ESP32-S3 only

### B. Clockfaces opcionales en compilación
```ini
build_flags = 
    -D INCLUDE_PACMAN=1
    -D INCLUDE_MARIO=0
```
- Usuario elige qué incluir al compilar
- Complejidad: requiere #ifdef en todo el código

### C. Cargar desde SPIFFS como .bin
- Clockfaces como "plugins" binarios
- Complejidad: muy alta, requiere loader dinámico
- No recomendado

---

## Decisión Requerida

| Opción | Flash ESP32 | Complejidad | Recomendación |
|--------|-------------|-------------|---------------|
| Solo Pac-Man | 84.5% | Baja | Para ESP32 clásico |
| Solo Mario | 87.0% | Media | Si prefieres Mario |
| Ambos | 87.0% | Alta | Solo si hay margen |
| Condicional | Variable | Media | Flexibilidad máxima |

**Recomendación:** Empezar con Pac-Man (más ligero, autocontenido), evaluar espacio restante, luego decidir Mario.

---

## Siguiente Paso

Confirmar:
1. Proceder con ambos o solo uno
2. Prioridad (Pac-Man primero vs Mario primero)
3. Soporte condicional por build flags (sí/no)
