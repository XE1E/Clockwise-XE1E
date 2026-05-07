# Manual de Configuracion - Clockwise XE1E

---

## Clonar el Repositorio

Para clonar el proyecto en una computadora nueva:
```bash
git clone --recursive https://github.com/XE1E/Clockwise-XE1E.git
```

El flag `--recursive` descarga todos los submodulos (clockfaces, librerias).

Si ya clonaste sin `--recursive`:
```bash
git submodule update --init --recursive
```

---

## Compilacion del Firmware

### Requisitos
- Python 3.x
- PlatformIO CLI
- Cable USB para conectar el ESP32

### Instalar Python (si no lo tienes)
```bash
winget install Python.Python.3
```
Reinicia la terminal despues de instalar.

### Instalar PlatformIO
```bash
pip install platformio
```

### Compilar el Firmware
```bash
cd C:\Documents\GitHub\Clockwise-XE1E\firmware
python -m platformio run
```

**Nota:** Si `pio run` no funciona, usa `python -m platformio run` en su lugar.

### Subir al ESP32
1. Conecta el ESP32 por USB
2. Ejecuta:
```bash
python -m platformio run --target upload
```

### Subir a un Puerto Especifico
```bash
python -m platformio run --target upload --upload-port COM3
```
Cambia `COM3` por el puerto donde esta tu ESP32.

### Ver Puertos Disponibles
```bash
python -m platformio device list
```

### Monitor Serial (ver logs)
```bash
python -m platformio device monitor --baud 115200
```

### Limpiar y Recompilar
```bash
python -m platformio run --target clean
python -m platformio run
```

### Solucion de Problemas al Flashear

#### El ESP32 no es detectado
1. Verifica que el cable USB sea de datos (no solo de carga)
2. Prueba otro puerto USB
3. Instala los drivers si es necesario:
   - **CP2102:** https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
   - **CH340:** https://www.wch.cn/download/CH341SER_EXE.html
4. En Windows, revisa el Administrador de Dispositivos para ver si aparece el puerto COM

#### Modo Boot Manual
Si el upload falla con "Failed to connect", pon el ESP32 en modo boot:
1. Manten presionado el boton **BOOT** (o IO0)
2. Presiona y suelta el boton **EN** (reset)
3. Suelta el boton **BOOT**
4. Ejecuta el comando de upload inmediatamente

#### Error "A fatal error occurred: Failed to connect"
- El ESP32 no entro en modo boot automaticamente
- Usa el metodo manual descrito arriba
- Algunos modelos requieren un capacitor en el pin EN

#### Error "Permission denied" o "Access denied"
- Cierra cualquier programa que use el puerto serial (monitor, IDE)
- En Windows: cierra el Serial Monitor de Arduino IDE si esta abierto
- En Linux: agrega tu usuario al grupo `dialout`:
```bash
sudo usermod -a -G dialout $USER
```
Reinicia sesion despues de ejecutar.

#### El upload se queda en "Connecting..."
- El ESP32 no responde, prueba el modo boot manual
- Desconecta cualquier componente del pin GPIO0
- Algunos paneles LED interfieren - desconecta el panel temporalmente

#### Verificar que el firmware se subio correctamente
Despues de flashear, abre el monitor serial:
```bash
python -m platformio device monitor --baud 115200
```
Deberias ver mensajes de inicio del Clockwise.

---

## Herramientas Web (Editor, Designer, Thumbnails)

Las herramientas web requieren un servidor local para funcionar.

### Iniciar Servidor Local
```bash
cd C:\Documents\GitHub\Clockwise-XE1E\clockface-editor
python -m http.server 8000
```

Deja la terminal abierta mientras usas las herramientas.

### Herramientas Disponibles

| Herramienta | URL | Descripcion |
|---|---|---|
| Editor de Clockfaces | http://localhost:8000/index.html | Crear y editar caratulas |
| Digit Designer | http://localhost:8000/digit-designer.html | Diseñar digitos para reloj nocturno |
| Generador Thumbnails | http://localhost:8000/generate-thumbs.html | Generar miniaturas de todas las caratulas |
| Pagina Config | http://localhost:8000/config-page.html | Preview de pagina configuracion |
| **Character Designer** | http://localhost:8000/char-designer.html | Editor de fuentes (importar, editar, guardar en galeria) |
| **Font Converter** | http://localhost:8000/font-converter.html | Convertir BDF a JS (avanzado) |
| Digit Designer | http://localhost:8000/digit-designer.html | Editor de digitos para Night Clock |

### Generador de Thumbnails

Hay dos formas de generar thumbnails:

1. **Desde el Editor** (recomendado):
   - Boton "Thumbnail": genera PNG del clockface actual
   - Boton "Batch Thumbs": genera multiples thumbnails seleccionando JSONs o carpeta

2. **Pagina standalone** (generate-thumbs.html):
   - Genera thumbnails de todos los clockfaces en carpeta `clockfaces/`
   - Requiere servidor local

### Digit Designer - Uso

1. Selecciona el digito (0-9) en las pestañas
2. Haz clic en los pixels para encender/apagar
3. Arrastra para dibujar multiples pixels
4. Los digitos guardados muestran borde verde en las pestañas

**Tamaño configurable:**
- Cambia Ancho y Alto (8-64 px) y haz clic en "Aplicar Tamaño"
- Tamaño por defecto: 20x28 pixels
- Los patrones prediseñados solo estan disponibles para 20x28

**Guardado local (localStorage):**
- **Guardar Local:** Guarda el digito actual en el navegador
- Los datos persisten aunque cierres el navegador

**Archivos (Export/Import):**
- **Exportar Digito:** Descarga el digito actual como JSON
- **Importar Digito:** Carga un digito desde archivo JSON
- **Exportar Todos:** Descarga todos los digitos como un solo JSON
- **Importar Todos:** Carga todos los digitos desde archivo JSON

**GitHub (sincronizacion entre PCs):**
- **Cargar Digito del Repo:** Carga desde `clockface-editor/digits/digit-X.json`
- **Cargar Todos del Repo:** Carga desde `clockface-editor/digits/all-digits.json`

### Sincronizar entre PCs via GitHub

1. **Guardar cambios (PC actual):**
   - Edita los digitos
   - Usa "Exportar Digito" o "Exportar Todos"
   - Guarda los JSON en `clockface-editor/digits/`
   - Commit y push a GitHub

2. **Cargar cambios (otra PC):**
   - Pull del repositorio
   - Abre digit-designer con servidor local
   - Usa "Cargar Digito del Repo" o "Cargar Todos del Repo"

---

## Acceso a la Configuracion

1. Conecta el reloj a tu red WiFi
2. Abre un navegador y accede a: **http://clockwise.local**
3. La pagina de configuracion se mostrara automaticamente

**Alternativa:** Si `clockwise.local` no funciona (Windows sin Bonjour instalado), usa la IP del reloj directamente. La IP se muestra en el monitor serial al iniciar:
```
[WiFi] mDNS started: http://clockwise.local
[WiFi] Connected to MiRed, IP address 192.168.1.50
```

---

## Configuracion Basica

### Brillo del Display
- **Rango:** 0 (apagado) - 255 (maximo brillo)
- **Recomendado:** 30-60 para uso normal

### Formato de Hora
- **24h:** Muestra 20:00 en lugar de 8:00 PM
- Activar si prefieres formato militar

### Idioma
- **Espanol:** Usa nombres de dias y meses en espanol (Lun, Mar, Ene, Feb...)

### Zona Horaria
- Ingresa tu zona horaria en formato IANA
- **Ejemplos:** 
  - `America/Mexico_City`
  - `America/Bogota`
  - `Europe/Madrid`
  - `America/Argentina/Buenos_Aires`

### Servidor NTP
- Servidor para sincronizar la hora
- **Por defecto:** `time.google.com`
- **Alternativas:** `pool.ntp.org`, `time.cloudflare.com`

---

## Brillo Automatico (LDR)

El sensor LDR ajusta el brillo segun la luz ambiente.

### Configuracion
1. **Pin LDR:** GPIO donde esta conectado el sensor (default: 35)
2. **Valor Minimo:** Lectura del LDR cuando esta oscuro (0-4095)
3. **Valor Maximo:** Lectura del LDR cuando hay luz (0-4095)

### Como calibrar
1. Haz clic en "Read Pin" para ver el valor actual del LDR
2. Oscurece la habitacion y anota el valor (sera tu minimo)
3. Ilumina el sensor y anota el valor (sera tu maximo)
4. Ingresa ambos valores y guarda

**Nota:** Si ambos valores estan en 0, el brillo automatico esta desactivado.

---

## Modo Nocturno

El modo nocturno cambia automaticamente a una caratula especial con brillo reducido durante las horas de la noche.

### Activar Modo Nocturno
1. Marca la casilla "Activar" en la tarjeta "Modo Nocturno"
2. Guarda la configuracion

### Horario Nocturno
- **Hora de inicio:** Cuando comienza el modo nocturno (ej: 22:00)
- **Hora de fin:** Cuando termina el modo nocturno (ej: 07:00)
- El horario puede cruzar medianoche (22:00 a 07:00 funciona correctamente)

### Brillo Nocturno
- **Rango:** 1-64
- **Recomendado:** 4-8 para no molestar al dormir
- Este brillo se usa solo durante el horario nocturno

### Color Nocturno
- Selecciona el color de los digitos en modo nocturno
- **Rojo** es recomendado porque no afecta tanto la vision nocturna
- El color se guarda en formato RGB565

### Caratula Nocturna
- Selecciona cual caratula se mostrara durante la noche
- **Recomendado:** "Reloj Nocturno" (night-clock) - diseno minimalista con digitos grandes

---

## Rotacion de Caratulas

Cambia automaticamente entre diferentes caratulas segun un intervalo de tiempo.

### Como Funciona Internamente

El reloj usa un clockface especial llamado **Canvas** (cw-cf-0x07) que puede mostrar cualquier caratula definida en JSON:

1. El firmware tiene compilado el clockface Canvas
2. Canvas descarga el archivo JSON de la caratula desde el servidor configurado
3. La rotacion simplemente cambia **cual JSON se descarga** cada cierto tiempo
4. El JSON actual se mantiene en memoria RAM

```
[Reloj] ──descarga──> [Servidor]/nyan-cat.json
                              │
        (60 min despues)      │
                              ▼
[Reloj] ──descarga──> [Servidor]/pac-man.json
```

### ¿Que pasa si se pierde el internet?

- La caratula actual **sigue funcionando** (esta en RAM)
- El reloj no puede descargar nuevas caratulas hasta que vuelva internet
- Si el reloj se **reinicia** sin internet, no podra cargar ninguna caratula JSON
- **Solucion futura:** Cache en flash para guardar la ultima caratula usada

### Activar Rotacion
1. Marca la casilla "Activar rotacion automatica"
2. Configura el intervalo de tiempo
3. Selecciona las caratulas que quieres incluir
4. Guarda la configuracion

### Intervalo de Rotacion
- Tiempo en **minutos** entre cada cambio de caratula
- El campo muestra la equivalencia en horas automaticamente
- **Ejemplos:**
  - 60 min = 1 hora
  - 120 min = 2 horas
  - 1440 min = 24 horas (1 dia)

### Seleccionar Caratulas
1. Marca las casillas de las caratulas que quieres incluir en la rotacion
2. El reloj rotara solo entre las caratulas seleccionadas
3. Necesitas al menos 2 caratulas para que la rotacion funcione

### Caratulas Disponibles
| ID | Nombre |
|---|---|
| nyan-cat | Nyan Cat |
| pac-man | Pac-Man |
| goomba_move | Goomba |
| snoopy3 | Snoopy |
| christmassnoopy | Snoopy Navidad |
| clock-club | Clock Club |
| donkey-kong | Donkey Kong |
| eletrogate | Eletrogate |
| pepsi-final-2 | Pepsi |
| retro-computer | Retro Computer |
| star-wars | Star Wars |
| night-clock | Reloj Nocturno |

**Nota:** La rotacion se pausa automaticamente durante el modo nocturno.

---

## Configuracion Canvas

El sistema Canvas permite cargar y mostrar caratulas dinamicas desde archivos JSON. Estas opciones solo aparecen si el firmware esta compilado con el clockface Canvas (cw-cf-0x07).

### Archivo de Descripcion
- Nombre del archivo JSON que define la caratula (sin extension)
- **Ejemplos:** `nyan-cat`, `pac-man`, `snoopy3`
- Los archivos se descargan automaticamente del servidor configurado

### Direccion del Servidor
- URL del servidor donde estan los archivos de caratulas
- **Por defecto:** `https://clockwise.page/canvas/`
- **Para pruebas locales:** `http://192.168.1.XXX:8000/` (cambia la IP por la de tu computadora)

### Como probar caratulas localmente
1. Inicia el servidor local:
```bash
cd C:\Documents\GitHub\Clockwise-XE1E\clockface-editor
python -m http.server 8000
```
2. En la configuracion del reloj, cambia "Server Address" a tu IP local (ej: `http://192.168.1.100:8000/`)
3. En "Description file" pon el nombre de tu caratula
4. Reinicia el reloj

---

## Configuracion Avanzada

### Rotacion del Display
- Rota la imagen del display: 0, 90, 180 o 270 grados
- Util si el reloj esta montado en diferente orientacion

### Intercambiar Azul/Verde
- Activa si los colores azul y verde estan invertidos
- Algunos paneles LED tienen pines RBG en lugar de RGB

### String Posix Manual
- Para evitar consultas remotas de zona horaria
- Ingresa el string Posix correspondiente a tu zona
- **Ejemplo Mexico:** `CST6CDT,M4.1.0,M10.5.0`
- Dejalo vacio para obtener automaticamente del servidor

---

## Fuentes Disponibles

### Fuentes Personalizadas (en clockfaces)

| Fuente | Tamaño | Ubicacion | Uso |
|---|---|---|---|
| small4pt7b | 4pt | cw-cf-0x02, cw-cf-0x03 | Texto pequeño, fechas |
| minute7pt7b | 7pt | cw-cf-0x02 | Minutos en palabras |
| hour8pt7b | 8pt | cw-cf-0x02 | Horas en palabras |
| Super_Mario_Bros__24pt7b | 24pt | cw-cf-0x01 | Estilo Mario Bros |
| FreeSansBold9pt7b | 9pt | cw-cf-0x07 | Reloj nocturno |
| TomThumb | 3pt | cw-cf-0x07 | Texto muy pequeño |

### Fuentes Adafruit GFX (libreria estandar)

Disponibles en `firmware/.pio/libdeps/esp32dev/Adafruit GFX Library/Fonts/`

| Familia | Tamaños Disponibles |
|---|---|
| FreeMono | 9, 12, 18, 24pt |
| FreeMonoBold | 9, 12, 18, 24pt |
| FreeSans | 9, 12, 18, 24pt |
| FreeSansBold | 9, 12, 18, 24pt |
| FreeSerif | 9, 12, 18, 24pt |
| FreeSerifBold | 9, 12, 18, 24pt |
| TomThumb | 3pt (muy pequeña) |

### Como usar una fuente

```cpp
#include <Fonts/FreeSansBold12pt7b.h>

// En el codigo:
display->setFont(&FreeSansBold12pt7b);
display->print("Hola");
```

### Crear fuentes personalizadas

Usa la herramienta **fontconvert** de Adafruit GFX para convertir fuentes TTF:
```bash
./fontconvert mifuente.ttf 12 > mifuente12pt7b.h
```

### Recursos de Fuentes Externas

#### Fuentes BDF (Bitmap Distribution Format)

| Sitio | URL | Descripcion |
|---|---|---|
| Font Library | https://fontlibrary.org | Muchas fuentes libres, buscar "bitmap" |
| Bitmap Fonts Collection | https://github.com/Tecate/bitmap-fonts | Gran coleccion de BDF |
| Unifont | https://unifoundry.com/unifont/ | Unicode completo, ideal para simbolos |
| Xorg Fonts | https://gitlab.freedesktop.org/xorg/font | Fuentes clasicas X11 |
| Adobe Source Code Pro | https://github.com/adobe-fonts/source-code-pro | Fuentes de Adobe en BDF |

#### Fuentes Adafruit GFX

| Recurso | URL |
|---|---|
| Repositorio oficial | https://github.com/adafruit/Adafruit-GFX-Library/tree/master/Fonts |
| GFX Font Customiser | https://tchapi.github.io/Adafruit-GFX-Font-Customiser/ |
| fontconvert (herramienta) | Incluido en Adafruit-GFX-Library |

#### Convertidores TTF a BDF

| Herramienta | Uso |
|---|---|
| FontForge | Editor de fuentes gratuito. Abre TTF, exporta como BDF |
| otf2bdf | Linea de comandos: `otf2bdf -p 12 font.ttf -o font.bdf` |
| BDF2GFX | Convierte BDF a formato Adafruit GFX (.h) |

#### Como agregar fuentes a la galeria

**Metodo simple (recomendado):**

1. Descarga un archivo `.bdf` de cualquiera de los sitios anteriores
2. Abre `char-designer.html` con servidor local
3. Click en "Importar archivo .bdf"
4. Edita los caracteres si lo deseas
5. Click en **"Guardar en fonts/"**
6. Selecciona la carpeta `clockface-editor/fonts/`
7. Recarga el editor de caratulas
8. La fuente aparece en el dropdown

**Metodo avanzado (manual):**

1. Usa `font-converter.html` para generar codigo JS
2. Copia el codigo generado
3. Pega en `js/pixel-fonts.js` dentro del objeto `PixelFonts`

#### Flujo de trabajo

```
┌─────────────────────────────────────────────────────────────────┐
│  GALERIA DE FUENTES                                             │
│  ├── js/pixel-fonts.js  (fuentes predefinidas)                 │
│  └── fonts/*.json       (fuentes personalizadas)               │
│                                                                 │
│  → Se muestran automaticamente en el editor de caratulas       │
└─────────────────────────────────────────────────────────────────┘
                           ▲
                           │
              ┌────────────┴────────────┐
              │    Character Designer    │
              │    (char-designer.html)  │
              │                          │
              │  1. Importar BDF/JSON    │
              │  2. Editar si necesario  │
              │  3. Guardar en fonts/    │
              └────────────┬────────────┘
                           │
              ┌────────────┴────────────┐
              │     Fuentes externas     │
              │  - dafont.com (.bdf)     │
              │  - fontstruct.com        │
              │  - github repos          │
              └─────────────────────────┘
```

#### Estructura de carpetas

```
clockface-editor/
├── js/
│   └── pixel-fonts.js      # Fuentes del sistema
├── fonts/
│   ├── index.json          # Indice (se actualiza automatico)
│   ├── mi-fuente.json      # Fuente personalizada
│   └── otra-fuente.json    # Otra fuente
├── char-designer.html      # Editor de fuentes
└── font-converter.html     # Convertidor BDF (avanzado)
```

---

## Solucion de Problemas

### El brillo automatico no funciona
- Verifica que los valores min/max no esten ambos en 0
- Usa "Read Pin" para confirmar que el LDR responde
- Asegurate que el pin LDR este correctamente configurado

### El modo nocturno no se activa
- Verifica que la casilla "Activar" este marcada
- Confirma que la hora del reloj sea correcta
- Revisa que el horario de inicio/fin sea correcto

### La rotacion no cambia de caratula
- Verifica que al menos 2 caratulas esten seleccionadas
- Confirma que el intervalo no sea muy largo
- La rotacion no funciona durante modo nocturno

### Los colores se ven mal
- Prueba activar "Intercambiar Azul/Verde"
- Algunos paneles tienen diferente orden de pines

---

## Reiniciar el Dispositivo

Despues de cambiar la configuracion, haz clic en el boton "Restart" para aplicar los cambios. Algunos cambios requieren reinicio:

- Zona horaria
- Servidor NTP
- Intercambio de pines
- Rotacion del display

---

## Hardware

### Componentes Necesarios
| Componente | Descripcion |
|---|---|
| ESP32 | Microcontrolador (DevKit v1 o similar) |
| Panel LED HUB75 | Matriz 64x64 pixels |
| LDR (opcional) | Sensor de luz para brillo automatico |
| Resistencia 10K (opcional) | Para divisor de voltaje del LDR |
| Fuente 5V | Minimo 4A para el panel LED |

### Conexion del LDR
```
3.3V ---- LDR ----+---- GPIO35
                  |
                 10K
                  |
                 GND
```

### Pines del Panel HUB75
| Pin Panel | GPIO ESP32 |
|---|---|
| R1 | 25 |
| G1 | 26 (o 27 si swap) |
| B1 | 27 (o 26 si swap) |
| R2 | 14 |
| G2 | 12 (o 13 si swap) |
| B2 | 13 (o 12 si swap) |
| A | 23 |
| B | 19 |
| C | 5 |
| D | 17 |
| E | 18 |
| CLK | 16 |
| LAT | 4 |
| OE | 15 |
| GND | GND |
