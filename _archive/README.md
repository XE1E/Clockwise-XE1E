# Archivado

Archivos del proyecto original (jnthas/clockwise) que ya no se usan activamente en ClockWise-XE1E.

## Contenido

- `components/` - Librerías ESP-IDF (ahora se usan via PlatformIO en `.pio/libdeps/`)
- `main/` - Estructura ESP-IDF (proyecto migrado a PlatformIO)
- `tools/` - Scripts antiguos de conversión
- `CMakeLists.txt` - Configuración ESP-IDF
- `sdkconfig.defaults` - Configuración ESP-IDF
- `CHANGELOG.md` - Historial del proyecto original
- `CHECKLIST.md` - Checklist de release original
- `get-platformio.py` - Script de instalación

## Nota

Estos archivos se conservan por referencia histórica. El proyecto actual usa:
- PlatformIO para compilación (`firmware/platformio.ini`)
- `clockface-editor/tools/` para scripts de conversión
