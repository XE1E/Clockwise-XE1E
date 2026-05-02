# Manual de Configuracion - Clockwise XE1E

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
| Generador Thumbnails | http://localhost:8000/generate-thumbs.html | Generar miniaturas de caratulas |
| Pagina Config | http://localhost:8000/config-page.html | Preview de pagina configuracion |

### Digit Designer - Uso

1. Selecciona el digito (0-9) en las pestañas
2. Haz clic en los pixels para encender/apagar
3. Arrastra para dibujar multiples pixels
4. Usa "Guardar Digito" para guardar cambios
5. Usa "Exportar Todos (JSON)" para obtener el codigo
6. Los digitos son de 20x28 pixels

---

## Acceso a la Configuracion

1. Conecta el reloj a tu red WiFi
2. Abre un navegador y accede a la IP del reloj
3. La pagina de configuracion se mostrara automaticamente

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

### Activar Rotacion
1. Marca la casilla "Activar" en la tarjeta "Rotacion de Caratulas"
2. Guarda la configuracion

### Intervalo de Rotacion
- Tiempo en **minutos** entre cada cambio de caratula
- **Ejemplos:**
  - 60 = cada hora
  - 1440 = cada 24 horas (1 dia)
  - 10080 = cada semana

### Seleccionar Caratulas
1. Marca las casillas de las caratulas que quieres incluir en la rotacion
2. El reloj rotara solo entre las caratulas seleccionadas
3. Guarda la configuracion

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
