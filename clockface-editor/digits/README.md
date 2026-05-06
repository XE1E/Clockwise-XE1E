# Digit Designs

Esta carpeta contiene los diseños de dígitos para el Night Clock.

## Archivos

- `digit-X.json` - Dígito individual (0-9)
- `all-digits.json` - Todos los dígitos en un archivo

## Flujo de trabajo para sincronizar entre PCs

### Guardar cambios (PC actual):
1. Abre `digit-designer.html` con servidor local (`python -m http.server 8000`)
2. Edita los dígitos que necesites
3. Usa "Exportar Dígito" o "Exportar Todos" para descargar JSON
4. Guarda los archivos en esta carpeta (`clockface-editor/digits/`)
5. Commit y push a GitHub

### Cargar cambios (otra PC):
1. Pull del repositorio
2. Abre `digit-designer.html` con servidor local
3. Usa "Cargar Dígito del Repo" o "Cargar Todos del Repo"

## Formato de archivo

### Dígito individual (`digit-X.json`):
```json
{
  "version": 1,
  "digit": 0,
  "width": 20,
  "height": 28,
  "data": [[0,0,0,...], ...]
}
```

### Todos los dígitos (`all-digits.json`):
```json
{
  "version": 1,
  "width": 20,
  "height": 28,
  "digits": {
    "0": [[...], ...],
    "1": [[...], ...],
    ...
  }
}
```
