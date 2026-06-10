# cw-cf-0x08 — Carátula ambiental BME680

Carátula nativa que muestra la hora junto con las lecturas del sensor ambiental
**Bosch BME680** vía I2C:

| Fila | Dato | Unidad | Color |
|------|------|--------|-------|
| T | Temperatura | °C | naranja |
| H | Humedad relativa | % | cyan |
| P | Presión barométrica | hPa | verde |
| G | Resistencia de gas (crudo) | kΩ | amarillo |

> La resistencia de gas es una métrica **relativa** de calidad de aire (valores
> menores ⇒ más compuestos orgánicos volátiles). No es un IAQ calibrado; para eso
> se requeriría la librería BSEC2 de Bosch.

## Hardware

Conexión I2C (ver `docs/ESP32-S3-MIGRATION.md` para el esquemático completo):

| Señal BME680 | ESP32-S3 | ESP32 original |
|--------------|----------|----------------|
| SDA | GPIO 9 | GPIO 21 |
| SCL | GPIO 10 | GPIO 22 |
| VCC | 3V3 | 3V3 |
| GND | GND | GND |

La dirección I2C se auto-detecta (0x76 / 0x77). Si el sensor no responde, la
carátula muestra `BME680 no det.` y reintenta cada 5 s.

## Selección

Es una carátula nativa; se selecciona con el nombre interno `_bme680`
(igual que `_mario` y `_pacman`).
