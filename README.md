# Clockwise XE1E

[![Build Firmware](https://github.com/XE1E/Clockwise-XE1E/actions/workflows/build-firmware.yml/badge.svg)](https://github.com/XE1E/Clockwise-XE1E/actions/workflows/build-firmware.yml)

> Reloj de pared inteligente con matriz LED 64x64

Fork personalizado de [Clockwise](https://github.com/jnthas/clockwise) por [@jnthas](https://github.com/jnthas).

![Logo](https://github.com/jnthas/clockwise/blob/gh-pages/static/images/clockwise_logo.png)

---

## Funcionalidades XE1E

Este fork incluye las siguientes mejoras:

| Funcionalidad | Descripcion |
|---|---|
| Editor de Caratulas | Diseña caratulas con vista previa en tiempo real y prueba directo en el reloj |
| Editor de Caracteres | Crea y edita fuentes pixel art para usar en las caratulas |
| Caratulas Locales | Guarda caratulas en la memoria del reloj (SPIFFS) - funciona sin internet |
| Thumbnails | Preview de caratulas en la interfaz web del reloj |
| GitHub Pages | Nueva fuente de caratulas estable con SSL |
| Modo Nocturno | Cambia automaticamente a una caratula minimalista con brillo reducido durante la noche |
| Rotacion de Caratulas | Cambia entre caratulas seleccionadas en intervalos configurables |
| Hora en Palabras | Muestra la hora en texto: "OCHO y media", "TRES y cuarto" |
| 26+ Fuentes BDF | Fuentes bitmap adicionales para el editor y firmware |
| Soporte Español | Nombres de dias y meses en español |
| Paleta de Colores | Selector visual de colores para modo nocturno |
| Herramientas Dev | Scripts para conversion de fuentes, generacion de thumbnails y mas |

---

## Instalacion Rapida

### Clonar el repositorio
```bash
git clone --recursive https://github.com/XE1E/Clockwise-XE1E.git
cd Clockwise-XE1E
```

### Compilar y flashear
```bash
cd firmware
pip install platformio
python -m platformio run --target upload
```

---

## Configuracion

1. Enciende el reloj
2. Conectate a la red WiFi **"Clockwise"**
3. Configura tu red WiFi
4. Accede a la IP del reloj desde tu navegador

---

## Documentacion

| Manual | Descripcion |
|---|---|
| [Manual de Usuario](docs/MANUAL_USUARIO.md) | Guia para usuarios finales |
| [Manual de Configuracion](docs/MANUAL_CONFIGURACION.md) | Guia tecnica completa |

---

## Hardware

| Componente | Descripcion |
|---|---|
| ESP32 | DevKit v1 o similar |
| Panel LED | HUB75 64x64 pixels |
| Fuente | 5V / 4A minimo |
| LDR (opcional) | Para brillo automatico |

### Conexiones

```
ESP32          Panel HUB75
─────          ───────────
GPIO 25   ──>  R1
GPIO 26   ──>  G1
GPIO 27   ──>  B1
GPIO 14   ──>  R2
GPIO 12   ──>  G2
GPIO 13   ──>  B2
GPIO 23   ──>  A
GPIO 19   ──>  B
GPIO 5    ──>  C
GPIO 17   ──>  D
GPIO 18   ──>  E
GPIO 16   ──>  CLK
GPIO 4    ──>  LAT
GPIO 15   ──>  OE
GND       ──>  GND
```

---

## Caratulas Disponibles

| Caratula | Descripcion |
|---|---|
| Nyan Cat | Gato arcoiris animado |
| Pac-Man | Clasico de arcade |
| Goomba | Enemigo de Mario Bros |
| Snoopy | El perro de Peanuts |
| Snoopy Navidad | Version navidena |
| Clock Club | Estilo moderno |
| Donkey Kong | Arcade clasico |
| Pepsi | Logo retro |
| Retro Computer | Estilo computadora antigua |
| Star Wars | Tematica espacial |

---

## Herramientas Web

Disponibles en GitHub Pages (sin instalar nada):

| Herramienta | Descripcion | URL |
|---|---|---|
| Editor de Caratulas | Diseña caratulas con preview en tiempo real | [Abrir](https://xe1e.github.io/Clockwise-XE1E/clockface-editor/) |
| Generador Thumbnails | Agrega thumbnails a caratulas existentes | [Abrir](https://xe1e.github.io/Clockwise-XE1E/clockface-editor/generate-thumbs.html) |
| Digit Designer | Crea sets de digitos personalizados | [Abrir](https://xe1e.github.io/Clockwise-XE1E/clockface-editor/digit-designer.html) |
| Web Flasher | Instala firmware desde el navegador | [Abrir](https://xe1e.github.io/Clockwise-XE1E/web-flasher/) |

**Servidor local** (necesario para generar thumbnails desde carpeta):
```bash
cd clockface-editor
python -m http.server 8000
# Abre http://localhost:8000
```

---

## Pagina de Configuracion

Accede desde cualquier navegador a la IP del reloj.

### Tabs disponibles:

| Tab | Funciones |
|-----|-----------|
| **WiFi** | Hasta 3 redes WiFi con prioridad |
| **Pantalla** | Brillo manual/automatico (LDR), rotacion fisica, panel RGB/RBG |
| **Hora** | Zona horaria, servidor NTP, formato 12h/24h, idioma |
| **Canvas** | Gestion de caratulas (ver abajo) |
| **Sistema** | Info del firmware, reinicio, reset de fabrica |

### Tab Canvas - Gestion de Caratulas

- **Almacenamiento:** Barra de uso de memoria SPIFFS
- **Thumbnails:** Vista previa de cada caratula guardada
- **Seleccion:** Click para seleccionar caratula activa
- **Rotacion:** Activa rotacion automatica, arrastra para ordenar
- **Subir:** Carga caratulas JSON desde tu computadora
- **Descargar:** Obtiene caratulas del repositorio GitHub o servidor local
- **Modo Nocturno:** Horario, brillo, color, caratula personalizada o integrada

### Thumbnails en Caratulas

Para que aparezca la preview de una caratula en la interfaz web:

1. **Automatico:** El editor genera thumbnail al exportar
2. **Manual:** Usa `generate-thumbs.html` para agregar thumbnail a JSONs existentes

El thumbnail se guarda en el campo `thumbnail` del JSON (~1-4 KB adicionales).

---

## Creditos

- **Proyecto original:** [Clockwise](https://github.com/jnthas/clockwise) por [@jnthas](https://github.com/jnthas)
- **Librerias:** ESP32-HUB75-MatrixPanel-DMA, Adafruit GFX, ezTime, WiFiManager

---

## Licencia

Este proyecto mantiene la misma licencia del proyecto original. Ver [LICENSE](LICENSE).
