# Guía: Fork para Fuentes Personalizadas

Esta guía explica cómo crear tu propia versión del firmware con fuentes personalizadas, sin afectar el proyecto original.

---

## ¿Por qué hacer un Fork?

El proyecto original incluye ~15 fuentes comunes que cubren la mayoría de casos. Si necesitas:

- Agregar una fuente que creaste
- Modificar una fuente existente
- Tener control total sobre las fuentes de tu reloj

Entonces necesitas un **fork** (copia personal del proyecto).

---

## Requisitos

- Cuenta de GitHub (gratis)
- Navegador web
- No necesitas instalar nada en tu computadora

---

## Paso 1: Crear el Fork

1. Ve a [github.com/XE1E/Clockwise-XE1E](https://github.com/XE1E/Clockwise-XE1E)
2. Clic en el botón **"Fork"** (esquina superior derecha)
3. Selecciona tu cuenta como destino
4. Espera a que se cree la copia

Ahora tienes tu propio repositorio: `github.com/TU_USUARIO/Clockwise-XE1E`

---

## Paso 2: Habilitar GitHub Actions

1. Ve a tu fork en GitHub
2. Clic en la pestaña **"Actions"**
3. Clic en **"I understand my workflows, go ahead and enable them"**

Esto permite que tu fork compile el firmware automáticamente.

---

## Paso 3: Crear/Editar tu Fuente

1. Ve al editor de fuentes: `clockface-editor/editor-fuentes.html`
   - Puedes usar la versión del proyecto original
   - O clonar tu fork y abrir el archivo local

2. Crea o edita tu fuente
3. Guarda como JSON (backup)

---

## Paso 4: Agregar la Fuente a tu Fork

### Opción A: Editar directamente en GitHub

1. Ve a tu fork en GitHub
2. Navega a `clockface-editor/js/pixel-fonts.js`
3. Clic en el icono de lápiz (editar)
4. Agrega tu fuente al final (antes del cierre `};`)
5. Clic en **"Commit changes"**

### Opción B: Clonar y editar localmente

```bash
git clone https://github.com/TU_USUARIO/Clockwise-XE1E.git
cd Clockwise-XE1E

# Edita pixel-fonts.js con tu editor favorito
# Luego:
git add clockface-editor/js/pixel-fonts.js
git commit -m "feat: add my custom font"
git push
```

---

## Paso 5: Agregar el Archivo .h al Firmware

1. En el editor de fuentes, exporta tu fuente como `.h`:
   - Panel derecho → "Exportar a firmware (.h)"

2. Sube el archivo a tu fork:
   - `firmware/clockfaces/cw-cf-0x07/fonts/tuFuente.h`

3. Edita `firmware/clockfaces/cw-cf-0x07/Clockface.h`:
   - Agrega: `#include "fonts/tuFuente.h"`

4. Edita `firmware/clockfaces/cw-cf-0x07/Clockface.cpp`:
   - En la función `setFont()`, agrega:
   ```cpp
   else if (strcmp(fontName, "tu-fuente") == 0) {
       Locator::getDisplay()->setFont(&tuFuente);
   }
   ```

5. Commit y push los cambios

---

## Paso 6: Esperar la Compilación

1. Ve a tu fork → pestaña **"Actions"**
2. Verás un workflow "Build Firmware" ejecutándose
3. Espera ~5 minutos a que termine
4. Si es exitoso (checkmark verde), continúa

---

## Paso 7: Descargar y Flashear

1. Ve a tu fork → pestaña **"Releases"**
2. Descarga el archivo `firmware-XXXXXX-XXXXXX.bin`
3. Usa [ESP Web Tools](https://web.esphome.io/) para flashear:
   - Conecta el reloj por USB
   - Selecciona el archivo .bin
   - Clic en "Install"

---

## Mantener tu Fork Actualizado

Cuando el proyecto original tenga actualizaciones:

1. Ve a tu fork en GitHub
2. Verás un mensaje "This branch is X commits behind"
3. Clic en **"Sync fork"** → **"Update branch"**

Esto trae las actualizaciones sin perder tus fuentes personalizadas.

---

## Solución de Problemas

### El build falla

1. Ve a Actions → clic en el workflow fallido
2. Expande los pasos para ver el error
3. Errores comunes:
   - Sintaxis incorrecta en pixel-fonts.js
   - Falta include en Clockface.h
   - Nombre de fuente incorrecto en setFont()

### La fuente no aparece en el reloj

1. Verifica que el nombre en `setFont()` coincide con el JSON de la carátula
2. Verifica que el include está correcto en Clockface.h
3. Revisa la consola del reloj (Serial 115200)

### No puedo hacer fork

- Asegúrate de tener una cuenta de GitHub
- El fork es 100% gratis para repositorios públicos

---

## Resumen de Archivos a Modificar

| Archivo | Cambio |
|---------|--------|
| `clockface-editor/js/pixel-fonts.js` | Agregar fuente (para editores web) |
| `firmware/.../fonts/tuFuente.h` | Agregar archivo .h exportado |
| `firmware/.../Clockface.h` | Agregar `#include` |
| `firmware/.../Clockface.cpp` | Agregar caso en `setFont()` |

---

## ¿Preguntas?

Abre un Issue en el repositorio original o tu fork.
