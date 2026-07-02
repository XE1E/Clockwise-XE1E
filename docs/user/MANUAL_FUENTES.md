# Manual de Creacion de Fuentes - Clockwise XE1E

Guia para disenar fuentes pixel personalizadas usando el Editor de Fuentes.

---

## Tabla de Contenidos

1. [Acceso al Editor](#acceso-al-editor)
2. [Metodos de Creacion](#metodos-de-creacion)
3. [Flujo de Trabajo](#flujo-de-trabajo)
4. [Importar Fuentes BDF](#importar-fuentes-bdf)
5. [Guardar y Exportar](#guardar-y-exportar)
6. [Tips y Recomendaciones](#tips-y-recomendaciones)
7. [Recursos de Fuentes BDF](#recursos-de-fuentes-bdf)

---

## Acceso al Editor

| Entorno | URL |
|---------|-----|
| **Servidor local** | http://localhost:8000/editor-fuentes.html |
| **GitHub Pages** | https://xe1e.github.io/Clockwise-XE1E/clockface-editor/editor-fuentes.html |

**Nota:** Algunas funciones (guardar en carpetas) requieren servidor local y Chrome/Edge.

---

## Metodos de Creacion

### Opcion 1: Partir de una fuente existente (recomendado)

Ideal para modificar o extender fuentes ya disponibles.

1. Abre el Editor de Fuentes
2. En **"Fuentes predefinidas"**, selecciona una base:
   - `picopixel` (5px) - muy pequena
   - `tomthumb` (6px) - pequena
   - `medium` (12px) - uso general
   - `big` (15px) - titulos
   - `bold` (22px) - grande
3. Click **"Cargar fuente"**
4. Edita los caracteres que quieras modificar
5. Agrega los caracteres espanoles en el grupo **"acentos"**

### Opcion 2: Importar fuente BDF externa

Para convertir fuentes bitmap de otras fuentes.

1. Descarga un archivo `.bdf` (ver seccion Recursos)
2. Click **"Importar archivo .bdf"**
3. Selecciona el archivo
4. Confirma la importacion
5. Edita/ajusta los caracteres segun necesites

### Opcion 3: Disenar desde cero

Para crear una fuente completamente original.

1. Configura el tamano:
   - Pequena: 4x6 o 5x8
   - Mediana: 8x12 o 10x14
   - Grande: 12x16 o 16x20
2. Click **"Aplicar tamano"**
3. Disena cada caracter grupo por grupo

**Orden recomendado:**
1. Digitos 0-9 (los mas usados en reloj)
2. Mayusculas A-Z
3. Minusculas a-z
4. Simbolos basicos (espacio, puntos, comas, dos puntos)
5. Acentos si necesitas espanol

---

## Flujo de Trabajo

### Interfaz del Editor

```
┌─────────────────────────────────────────────────────────────────────┐
│                        Editor de Fuentes                            │
├──────────────┬─────────────────────────┬────────────────────────────┤
│ CARGAR       │  [0-9] [A-Z] [a-z]      │ GUARDAR / EXPORTAR         │
│              │  [!@#] [acentos]        │                            │
│ - Predefinidas│                        │ - Guardar JSON             │
│ - Archivo JSON│  ┌─────────────────┐   │ - Guardar en fonts/        │
│ - Archivo BDF │  │                 │   │ - Agregar a predefinidas   │
│ - Galeria     │  │   Grid pixels   │   │ - Exportar PNGs            │
│              │  │                 │   │                            │
│ Configuracion │  └─────────────────┘   │                            │
│ Ancho: [8]   │  [Limpiar] [Guardar]   │                            │
│ Alto:  [12]  │                        │                            │
└──────────────┴─────────────────────────┴────────────────────────────┘
```

### Grupos de Caracteres

| Grupo | Contenido |
|-------|-----------|
| **0-9** | Digitos: 0123456789 |
| **A-Z** | Mayusculas: ABCDEFGHIJKLMNOPQRSTUVWXYZ |
| **a-z** | Minusculas: abcdefghijklmnopqrstuvwxyz |
| **!@#** | Simbolos: espacio, puntuacion, operadores |
| **acentos** | Espanol: AEIOUaeiou con tildes, N, signos |

### Editar un Caracter

1. Selecciona el grupo (ej: 0-9)
2. Click en el caracter en las pestanas
3. Dibuja en el grid:
   - **Click** = activar/desactivar pixel
   - **Arrastrar** = dibujar continuo
4. Click **"Guardar"** para persistir en navegador
5. El caracter aparece con borde verde (tiene datos)

### Indicadores Visuales

- **Pestana activa (verde):** Caracter seleccionado
- **Borde verde:** Caracter con datos guardados
- **Vista previa 1x:** Tamano real
- **Vista previa 4x:** Ampliada para detalle
- **Vista "todos":** Muestra caracteres con datos

---

## Importar Fuentes BDF

### Que es BDF

BDF (Bitmap Distribution Format) es un formato estandar para fuentes bitmap. Contiene la definicion pixel por pixel de cada caracter.

### Proceso de Importacion

#### Paso 1: Obtener archivo BDF

Ejemplo con fuente 4x6 del repositorio:

```
URL: https://raw.githubusercontent.com/XE1E/bdf-fonts/main/4x6.bdf
```

1. Abre la URL en el navegador
2. Click derecho → "Guardar como..." → `4x6.bdf`

#### Paso 2: Importar en Editor

1. Abre el Editor de Fuentes
2. Click **"Importar archivo .bdf"**
3. Selecciona el archivo descargado
4. Aparece dialogo de confirmacion:
   ```
   Fuente: 4x6
   Tamano: 4x6
   Caracteres: 919
   
   ¿Cargar esta fuente?
   ```
5. Click **Aceptar**

#### Paso 3: Verificar importacion

1. Revisa cada grupo de caracteres
2. Los caracteres importados tendran borde verde
3. La vista previa mostrara todos los disponibles

#### Paso 4: Ajustar si es necesario

Algunos caracteres pueden necesitar ajustes:
- Alineacion vertical
- Espaciado
- Caracteres faltantes

### Fuentes BDF Recomendadas

| Fuente | Tamano | Uso |
|--------|--------|-----|
| 4x6 | 4x6 | Texto muy pequeno, info secundaria |
| 5x7 | 5x7 | Texto compacto |
| 6x10 | 6x10 | Texto legible pequeno |
| 8x13 | 8x13 | Uso general |
| 10x20 | 10x20 | Titulos, hora principal |

---

## Guardar y Exportar

### Opciones de Guardado

| Boton | Destino | Uso |
|-------|---------|-----|
| **Guardar en carpeta...** | Carpeta seleccionada | Backup JSON en ubicacion especifica |
| **Descargar** | Carpeta Descargas | Backup JSON rapido |
| **Guardar en fonts/** | clockface-editor/fonts/ | Para usar en caratulas JSON |
| **Agregar a pixel-fonts.js** | js/pixel-fonts.js | Fuente integrada en editor |
| **Guardar PNGs en carpeta...** | Carpeta seleccionada | Imagenes individuales |
| **Descargar PNGs** | Carpeta Descargas | Imagenes PNG de cada caracter |

### Guardar en Galeria (fonts/)

Para que la fuente este disponible en el editor de caratulas:

1. Escribe un nombre (ej: `mi-fuente-4x6`)
2. Click **"Guardar en fonts/"**
3. Selecciona la carpeta `clockface-editor/fonts/`
4. Confirma

La fuente queda guardada como JSON y se actualiza el indice automaticamente.

### Agregar a Predefinidas

Para que aparezca en el selector de fuentes del editor:

1. Escribe un nombre valido (letras, numeros, guion bajo)
2. Click **"Agregar a pixel-fonts.js"**
3. Selecciona la carpeta `clockface-editor/js/`
4. Confirma
5. **Recarga la pagina** para ver la fuente en el selector

### Formato JSON de Fuente

```json
{
  "version": 1,
  "name": "mi-fuente",
  "width": 8,
  "height": 12,
  "chars": {
    "48": { "char": "0", "data": [[0,0,1,1,1,0,0,0], ...] },
    "49": { "char": "1", "data": [[0,0,0,1,0,0,0,0], ...] },
    ...
  }
}
```

---

## Tips y Recomendaciones

### Tamanos Recomendados

| Uso | Tamano | Notas |
|-----|--------|-------|
| Hora principal | 12x16 o mayor | Debe ser legible a distancia |
| Fecha, info | 6x10 a 8x12 | Texto secundario |
| Texto compacto | 4x6 o 5x7 | Mucha informacion en poco espacio |

### Diseno de Caracteres

1. **Consistencia:** Mantener el mismo grosor de trazo
2. **Baseline:** Alinear todos los caracteres a la misma linea base
3. **Espaciado:** Dejar al menos 1 pixel de margen derecho
4. **Legibilidad:** Probar a tamano real (1x) frecuentemente

### Caracteres Prioritarios para Reloj

Si solo necesitas lo basico para mostrar hora:

```
Minimos: 0123456789:
Basicos: 0123456789: APMapm
Fecha:   0123456789:/ LunMarMieJueVieSabDom
Completo: Todo lo anterior + meses + acentos
```

### Fuentes Proporcionales vs Monoespaciadas

- **Monoespaciada:** Todos los caracteres tienen el mismo ancho
- **Proporcional:** Cada caracter tiene su propio ancho (ej: "i" mas angosta que "m")

El editor asume monoespaciada. Para proporcional, deja espacio vacio a la derecha de caracteres angostos.

---

## Recursos de Fuentes BDF

### Repositorios

| Recurso | URL | Descripcion |
|---------|-----|-------------|
| XE1E bdf-fonts | https://github.com/XE1E/bdf-fonts | Coleccion curada |
| Tecate bitmap-fonts | https://github.com/Tecate/bitmap-fonts | Gran variedad |
| Font Library | https://fontlibrary.org | Buscar "bitmap" |
| Unifont | https://unifoundry.com/unifont/ | Unicode completo |

### Fuentes Populares

| Nombre | Tamano | Estilo |
|--------|--------|--------|
| Terminus | varios | Programacion |
| Dina | 8x16 | Legible |
| Proggy | varios | Programacion |
| Gohufont | 11, 14 | Minimalista |
| Cozette | 13 | Retro |

### Descargar desde GitHub

Para cualquier archivo BDF en GitHub:

1. Abre el archivo en GitHub
2. Click en **"Raw"**
3. Click derecho → "Guardar como..."

O construye la URL raw:
```
https://raw.githubusercontent.com/[usuario]/[repo]/[rama]/[archivo].bdf
```

---

## Solucion de Problemas

### El archivo BDF no importa

- Verificar que el archivo sea BDF valido (texto, no binario)
- Algunos BDF muy antiguos pueden tener formato incompatible
- Probar con otra fuente BDF conocida

### Los caracteres se ven desalineados

- El BDF puede tener offsets negativos
- Editar manualmente los caracteres afectados
- Ajustar la altura del grid si es necesario

### No aparece en el editor de caratulas

- Verificar que se guardo en fonts/ correctamente
- Recargar la pagina del editor de caratulas
- Verificar que el JSON sea valido

### Los acentos no se importaron

- Verificar que el BDF incluya caracteres Unicode extendidos
- Agregar manualmente los caracteres faltantes
- Usar como base otra fuente que los tenga

---

## Ejemplo Completo

### Crear fuente "reloj-digital" desde BDF

1. **Descargar fuente base**
   ```
   https://raw.githubusercontent.com/XE1E/bdf-fonts/main/4x6.bdf
   ```

2. **Importar en editor**
   - Abrir editor-fuentes.html
   - Click "Importar archivo .bdf"
   - Seleccionar 4x6.bdf
   - Confirmar

3. **Personalizar**
   - Modificar el estilo de los digitos 0-9
   - Ajustar los dos puntos (:) para que se vean centrados
   - Agregar caracteres faltantes si es necesario

4. **Guardar**
   - Nombre: `reloj-digital`
   - Click "Guardar en fonts/"
   - Seleccionar carpeta fonts/

5. **Usar en caratula**
   - Abrir editor de caratulas (index.html)
   - Crear elemento DateTime
   - Seleccionar fuente "reloj-digital" de la galeria

---

**Autor:** XE1E  
**Repositorio:** https://github.com/XE1E/Clockwise-XE1E
