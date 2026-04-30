# Clockwise Web Flasher

Herramienta web para flashear firmware de Clockwise directamente desde el navegador usando ESP Web Tools.

## Requisitos

- Navegador con soporte WebSerial (Chrome 89+, Edge 89+, Opera 75+)
- Servidor HTTPS (WebSerial no funciona en HTTP)
- Archivos de firmware compilados

## Estructura de Archivos

```
web-flasher/
├── index.html                          # Pagina principal
├── static/
│   └── firmware/
│       ├── base_firmware/              # Archivos comunes
│       │   ├── bootloader_dio_40m.bin  # Bootloader ESP32
│       │   ├── partitions.bin          # Tabla de particiones
│       │   └── boot_app0.bin           # Boot app
│       ├── cw-cf-0x01/                 # Mario Bros
│       │   ├── manifest.json
│       │   └── firmware.bin
│       ├── cw-cf-0x02/                 # Time in Words
│       │   ├── manifest.json
│       │   └── firmware.bin
│       └── ...                         # Otras clockfaces
```

## Formato del manifest.json

```json
{
    "name": "Nombre del Clockface",
    "version": "1.4.2",
    "builds": [
        {
            "chipFamily": "ESP32",
            "improv": true,
            "parts": [
                {
                    "path": "../base_firmware/bootloader_dio_40m.bin",
                    "offset": 4096
                },
                {
                    "path": "../base_firmware/partitions.bin",
                    "offset": 32768
                },
                {
                    "path": "../base_firmware/boot_app0.bin",
                    "offset": 57344
                },
                {
                    "path": "firmware.bin",
                    "offset": 65536
                }
            ]
        }
    ]
}
```

### Offsets de memoria

| Archivo | Offset | Descripcion |
|---------|--------|-------------|
| bootloader_dio_40m.bin | 0x1000 (4096) | Bootloader |
| partitions.bin | 0x8000 (32768) | Tabla de particiones |
| boot_app0.bin | 0xE000 (57344) | OTA data |
| firmware.bin | 0x10000 (65536) | Aplicacion principal |

## Compilar Firmware

### Con PlatformIO

```bash
cd firmware

# Crear enlace al clockface deseado
cd lib
ln -s ../clockfaces/cw-cf-0x07/ current_clockface
cd ..

# Compilar
pio run

# Los archivos estaran en:
# .pio/build/esp32dev/firmware.bin
# .pio/build/esp32dev/bootloader.bin
# .pio/build/esp32dev/partitions.bin
```

### Con esp-idf

```bash
# Configurar clockface
idf.py menuconfig

# Compilar
idf.py build

# Los archivos estaran en:
# build/clockwise.bin
# build/bootloader/bootloader.bin
# build/partition_table/partition-table.bin
```

## Despliegue

### GitHub Pages (Recomendado)

1. Crea una rama `gh-pages`:
```bash
git checkout -b gh-pages
```

2. Copia los archivos del web-flasher a la raiz

3. Agrega los binarios de firmware compilados

4. Push y activa GitHub Pages en Settings

### Servidor Local (Desarrollo)

Para pruebas locales necesitas HTTPS. Usa mkcert:

```bash
# Instalar mkcert
choco install mkcert  # Windows
brew install mkcert   # macOS

# Crear certificados locales
mkcert -install
mkcert localhost

# Servir con Python
python -m http.server 8443 --bind localhost
# (necesitas configurar SSL manualmente)

# O usar serve con https
npx serve -l 8443 --ssl-cert localhost.pem --ssl-key localhost-key.pem
```

## ESP Web Tools

Este proyecto usa [ESP Web Tools](https://esphome.github.io/esp-web-tools/) de ESPHome.

### Caracteristicas:
- Flasheo directo desde el navegador
- No requiere drivers especiales
- Soporte para Improv WiFi (configuracion WiFi post-flash)
- Compatible con ESP32, ESP32-S2, ESP32-C3, ESP8266

### Documentacion:
- https://esphome.github.io/esp-web-tools/
- https://github.com/esphome/esp-web-tools

## Troubleshooting

### "Tu navegador no soporta WebSerial"
- Usa Chrome, Edge u Opera en escritorio
- Safari y Firefox no soportan WebSerial
- Moviles no son compatibles

### "WebSerial requiere HTTPS"
- Despliega en GitHub Pages (HTTPS automatico)
- O configura HTTPS local con mkcert

### "No se detecta el puerto COM"
- Instala drivers CP210x o CH340 segun tu ESP32
- Verifica que el cable USB soporte datos (no solo carga)
- Prueba otro puerto USB

### "Error de conexion"
- Cierra otros programas que usen el puerto serial
- Reinicia el ESP32 (boton EN/RST)
- Mantén presionado BOOT mientras conectas si es necesario
