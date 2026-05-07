# Clockwise - Pendientes

## Por hacer

### Editor de Dígitos/Caracteres
- [ ] Hacer tamaño de caracter configurable (ancho x alto pixeles) en lugar de fijo 20x28
- [ ] Conectar editor de dígitos con editor de clockfaces para usar caracteres personalizados
- [ ] Perfeccionar dígitos 0-9 en `digit-designer.html`
- [ ] Crear fuente GFX grande (24pt+) o modificar firmware para soportar digit-sprites
- [ ] Integrar dígitos bitmap en night-clock.json

### Thumbnails
- [ ] Generar thumbnails de las 11 caratulas JSON (abrir `clockface-editor/generate-thumbs.html` con servidor local)
- [ ] Capturar screenshots de las 7 caratulas built-in del reloj fisico y guardarlas en `clockface-editor/clockfaces/thumbs/`

### Caratulas
- [ ] Recrear el reloj nocturno (numeros grandes) cuando se tenga la foto de referencia

### Firmware
- [ ] Compilar firmware universal
- [ ] Probar instalador web (`clockface-editor/install.html`)
- [ ] Diseñar caratula simple de respaldo para cuando no hay internet (offline fallback)
- [ ] Implementar cache de caratula en flash (SPIFFS/LittleFS) para no perderla al reiniciar

### GitHub Pages
- [ ] Activar GitHub Pages cuando el repo sea publico

## Completado

- [x] Editor de clockfaces con soporte para sprites
- [x] Pagina de configuracion (config-page.html)
- [x] Instalador web con esp-web-tools (install.html)
- [x] Generador de thumbnails (generate-thumbs.html)
- [x] Descargar 11 caratulas JSON de clock-club
- [x] Soporte para 7 caratulas built-in + 11 JSON en configurador
