# Clockwise XE1E

> Reloj de pared inteligente con matriz LED 64x64

Fork personalizado de [Clockwise](https://github.com/jnthas/clockwise) por [@jnthas](https://github.com/jnthas).

![Logo](https://github.com/jnthas/clockwise/blob/gh-pages/static/images/clockwise_logo.png)

---

## Funcionalidades XE1E

Este fork incluye las siguientes mejoras:

| Funcionalidad | Descripcion |
|---|---|
| Modo Nocturno | Cambia automaticamente a una caratula minimalista con brillo reducido durante la noche |
| Rotacion de Caratulas | Cambia entre caratulas seleccionadas en intervalos configurables |
| Soporte Español | Nombres de dias y meses en español |
| Paleta de Colores | Selector visual de colores para modo nocturno |
| Reloj Nocturno | Nueva caratula minimalista con digitos grandes |

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
| Reloj Nocturno | Minimalista para dormir |

---

## Herramientas Web

Inicia un servidor local para usar las herramientas:
```bash
cd clockface-editor
python -m http.server 8000
```

| Herramienta | URL |
|---|---|
| Editor de Caratulas | http://localhost:8000/index.html |
| Digit Designer | http://localhost:8000/digit-designer.html |
| Generador Thumbnails | http://localhost:8000/generate-thumbs.html |

---

## Pagina de Configuracion

La pagina de configuracion incluye:

- **Brillo:** Manual o automatico con sensor LDR
- **Formato de hora:** 12h o 24h
- **Idioma:** Español o Ingles
- **Zona horaria:** Configuracion IANA
- **Modo Nocturno:** Horario, brillo, color y caratula
- **Rotacion de Caratulas:** Intervalo y seleccion de caratulas
- **Canvas:** Servidor y archivo de descripcion

---

## Creditos

- **Proyecto original:** [Clockwise](https://github.com/jnthas/clockwise) por [@jnthas](https://github.com/jnthas)
- **Librerias:** ESP32-HUB75-MatrixPanel-DMA, Adafruit GFX, ezTime, WiFiManager

---

## Licencia

Este proyecto mantiene la misma licencia del proyecto original. Ver [LICENSE](LICENSE).
