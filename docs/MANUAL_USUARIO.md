# Manual de Usuario - Clockwise XE1E

---

## Inicio Rapido

### 1. Flashear el Firmware

#### Opcion A: Web Flasher (Recomendado)
1. Conecta el ESP32 a tu computadora por USB
2. Abre el Web Flasher en tu navegador (Chrome o Edge)
3. Haz clic en "Conectar" y selecciona el puerto del ESP32
4. Haz clic en "Flashear" y espera a que termine
5. Desconecta el ESP32

#### Opcion B: Archivo .bin manual
1. Descarga el archivo `.bin` del firmware
2. Usa una herramienta como ESP Flash Tool
3. Flashea el archivo a la direccion 0x0

### 2. Conectar a WiFi (Primera vez)

1. Enciende el reloj (conecta la fuente de 5V)
2. El reloj mostrara el logo de Clockwise y luego "Conectando WiFi..."
3. Como no hay red configurada, creara una red WiFi llamada **"ClockWise-XE1E"**
4. Conectate a esa red desde tu celular o computadora
5. Se abrira automaticamente una pagina de configuracion (Improv WiFi)
6. Selecciona tu red WiFi e ingresa la contrasena
7. El reloj se reiniciara y se conectara a tu red

### 3. Acceder a la Configuracion

Despues de conectar el reloj a tu WiFi, puedes acceder a la configuracion de dos formas:

#### Ver la IP en pantalla
- Al encender, el reloj muestra brevemente la IP asignada (ej: `192.168.1.50`)
- Escribe esa IP en tu navegador

#### Usar el nombre mDNS
- Abre un navegador y escribe: **http://clockwise-xe1e.local**
- Funciona en la mayoria de dispositivos (Mac, iOS, Android, Linux)
- En Windows puede requerir Bonjour instalado

**Si ninguna opcion funciona:**
- Revisa tu router para ver dispositivos conectados
- Busca un dispositivo llamado "ESP32" o "Clockwise"
- Usa una app como "Fing" para escanear la red

---

## Interfaz de Configuracion

La pagina de configuracion tiene 5 pestanas:

| Pestana | Contenido |
|---------|-----------|
| **WiFi** | Configurar hasta 3 redes WiFi |
| **Pantalla** | Brillo, rotacion, brillo automatico |
| **Hora y Fecha** | Zona horaria, formato 24h, idioma, servidor NTP |
| **Canvas** | Caratulas guardadas, subir/descargar, modo nocturno, rotacion |
| **Sistema** | Version de firmware, reiniciar, reset de fabrica |

### Barra Superior
- Muestra red WiFi actual, IP, memoria disponible, uso de SPIFFS y tiempo encendido
- Boton **Reiniciar** para aplicar cambios

---

## WiFi

### Multiples Redes WiFi
El reloj puede guardar hasta **3 redes WiFi**:
- **Red 1 (Principal):** La red que usaras normalmente
- **Red 2 y 3 (Respaldo):** Redes alternativas si la principal no esta disponible

El reloj intentara conectar en orden: Red 1 → Red 2 → Red 3

### Escanear Redes WiFi
1. Ve a la pestana WiFi
2. Haz clic en **"Escanear"**
3. Espera unos segundos mientras busca redes disponibles
4. Selecciona una red del menu desplegable
5. Elige en cual slot guardarla (WiFi 1, 2 o 3)
6. Haz clic en **"Usar"** para llenar los campos
7. Ingresa la contrasena si es necesario
8. Haz clic en "Guardar WiFi"

### Cambiar de Red Manualmente
1. Ve a la pestana WiFi
2. Ingresa SSID y contrasena de la nueva red
3. Haz clic en "Guardar WiFi"
4. Reinicia el reloj

**Nota:** Solo funcionan redes WiFi de **2.4 GHz** (no 5 GHz).

### Borrar Configuracion WiFi
Para que el reloj olvide las redes guardadas y vuelva a crear el punto de acceso:
1. Ve a la pestana **Sistema**
2. Haz clic en **"Borrar WiFi"**
3. El reloj se reiniciara y creara la red "ClockWise-XE1E"

**Alternativa:** Si el reloj no puede conectar a ninguna red guardada despues de varios intentos, automaticamente creara el punto de acceso. Cada 5 minutos intentara reconectar a las redes guardadas.

---

## Pantalla

### Brillo
- Mueve el control deslizante para ajustar (1-255)
- El cambio se aplica inmediatamente
- **Recomendado:** 30-60 para uso normal

### Rotacion de Pantalla
Si montaste el reloj en otra orientacion:
- 0° = Normal
- 90° = Rotado a la derecha
- 180° = De cabeza
- 270° = Rotado a la izquierda

Haz clic en **"Guardar Pantalla"** para aplicar el cambio.

### Panel RGB (Intercambiar Azul/Verde)
Algunos paneles LED tienen los colores azul y verde invertidos. Si los colores se ven mal:
1. Activa la opcion **"Intercambiar Azul/Verde"**
2. Haz clic en "Guardar Pantalla"

### Brillo Automatico (LDR)
Si tu reloj tiene sensor de luz (LDR):

1. En "Brillo Automatico", haz clic en **"Leer Pin"** con luz normal
2. Anota el valor (ej: 2500)
3. Oscurece la habitacion y haz clic en **"Leer Pin"** de nuevo
4. Anota el valor (ej: 500)
5. Ingresa el valor bajo en "Min" y el alto en "Max"
6. Guarda

**Para desactivar:** Pon ambos valores en 0.

### Sensor Ambiental (BME280)
Sensor I2C **opcional** para mostrar **temperatura, humedad y presion** en las caratulas.
Funciona igual en ambas versiones (ESP32 y ESP32-S3); solo cambian los pines I2C por defecto.

En la tarjeta **"Sensor Ambiental (BME280)"** (pestana Pantalla):
1. Activa **"Habilitar sensor"**.
2. Revisa **SDA** y **SCL** (vienen con el default de tu placa — ver [Conexiones de Hardware](#conexiones-de-hardware-pinout)).
3. **Direccion I2C:** deja **0** para autodeteccion (`0x76`/`0x77`).
4. Elige la **unidad** de temperatura (Celsius o Fahrenheit).
5. Haz clic en **"Guardar Pantalla"** y **reinicia** el reloj (los cambios del sensor se aplican al reiniciar).
6. Tras reiniciar, usa **"Leer sensor"** para verificar la lectura.

Para usarlo en una caratula, en el editor agrega un elemento **Sensor** y elige la magnitud
(temperatura / humedad / presion).

> El simbolo de grado (°) esta disponible en las fuentes **medium, big y tiny5**.

---

## Hora y Fecha

### Zona Horaria
Ingresa tu zona horaria. Ejemplos:

| Pais | Zona Horaria |
|------|--------------|
| Mexico (Centro) | America/Mexico_City |
| Mexico (Pacifico) | America/Tijuana |
| Colombia | America/Bogota |
| Argentina | America/Argentina/Buenos_Aires |
| Espana | Europe/Madrid |
| Chile | America/Santiago |
| Peru | America/Lima |
| Venezuela | America/Caracas |

### Formato de Hora
- **Activado:** Formato 24 horas (14:00)
- **Desactivado:** Formato 12 horas (2:00 PM)

### Idioma Espanol
- **Activado:** Dias y meses en espanol (Lun, Mar, Ene, Feb...)
- **Desactivado:** En ingles (Mon, Tue, Jan, Feb...)

### Servidor NTP
Por defecto usa `time.cloudflare.com` (recomendado por su velocidad y privacidad). Puedes cambiarlo si lo necesitas.

### Zona Horaria Manual (POSIX)
Si tu zona horaria no esta en la lista o tiene reglas especiales de horario de verano, puedes usar una cadena POSIX:

**Formato:** `STDoffset[DST[offset],start,end]`

**Ejemplos:**
| Zona | Cadena POSIX |
|------|--------------|
| Mexico Central | `CST6CDT,M4.1.0,M10.5.0` |
| Argentina (sin DST) | `ART3` |
| Espana | `CET-1CEST,M3.5.0,M10.5.0` |

Deja el campo vacio para usar la zona horaria normal.

---

## Canvas (Caratulas)

### Caratulas Nativas
El firmware incluye 2 caratulas nativas integradas que no ocupan espacio en memoria:
- **Pac-Man:** Animacion clasica del comecocos con fantasmas
- **Mario Bros:** Escena animada con Mario, bloques, monedas y nubes

Estas caratulas aparecen automaticamente en la galeria y no pueden eliminarse.

### Caratulas Guardadas
Las caratulas JSON se almacenan en la memoria del reloj. Puedes:
- **Ver miniaturas** de cada caratula
- **Seleccionar** cual mostrar haciendo clic
- **Eliminar** con el boton X
- **Subir nuevas** desde tu computadora
- **Descargar** del repositorio de GitHub

### Subir Caratula
1. Haz clic en "Subir Caratula"
2. Selecciona un archivo `.json` de caratula
3. La caratula se guarda en el reloj

### Descargar del Repositorio
1. Selecciona la fuente (GitHub XE1E o Servidor Local)
2. Elige la caratula del menu desplegable
3. Haz clic en "Descargar y Guardar"

### Servidor Local (Desarrollo)
Si estas creando caratulas y quieres probarlas:
1. Selecciona fuente "Servidor Local"
2. Ingresa la IP de tu computadora y puerto (ej: 8080)
3. Ejecuta en tu computadora: `python -m http.server 8080`
4. Escribe el nombre de la caratula y descarga

---

## Modo Nocturno

Reduce el brillo y cambia la caratula durante la noche.

### Configurar
1. Activa **"Modo Nocturno"**
2. Configura horario:
   - **Hora inicio:** Cuando empieza (ej: 22:00)
   - **Hora fin:** Cuando termina (ej: 07:00)
3. Ajusta el **brillo nocturno** (recomendado: 4-8)
4. Selecciona el **color de los digitos** haciendo clic en uno de los botones de color
5. Elige la **caratula nocturna**:
   - **"Reloj Nocturno"** usa un reloj minimalista integrado con el color seleccionado
   - **"Pac-Man"** o **"Mario Bros"** para usar las caratulas nativas
   - O selecciona una de las **caratulas guardadas** en memoria
6. Haz clic en "Guardar Canvas"

**Nota:** El selector de color solo aplica para el reloj nocturno integrado. Las caratulas nativas y guardadas usan sus propios colores.

### Colores Recomendados para Dormir

**Colores suaves (menor impacto visual):**
| Color | Efecto |
|-------|--------|
| **Rojo** | No afecta la vision nocturna, ideal para dormir |
| **Rojo oscuro** | Aun mas tenue que el rojo |
| **Naranja** | Suave y calido |
| **Verde oscuro** | Bajo impacto visual |

**Evitar para dormir:**
- Blanco, cyan, azul brillante (muy intensos)
- Colores claros o saturados

El color seleccionado muestra un borde blanco para identificarlo facilmente.

---

## Rotacion Automatica de Caratulas

Cambia automaticamente entre diferentes caratulas.

### Configurar
1. Activa **"Rotacion automatica"**
2. Configura el **intervalo en minutos**:
   - 60 = cada hora
   - 1440 = cada dia (24 horas)
3. Selecciona las caratulas haciendo clic en cada una (aparece numero de orden)
4. Arrastra para reordenar la secuencia
5. Haz clic en "Guardar Canvas"

**Nota:** La rotacion se pausa durante el modo nocturno.

---

## Sistema

### Version de Firmware
Muestra la version actual del firmware instalado.

### Reiniciar
Reinicia el reloj. Necesario despues de algunos cambios.

### Borrar WiFi
Borra solo las redes WiFi guardadas sin afectar otras configuraciones:
1. Haz clic en **"Borrar WiFi"**
2. El reloj se reiniciara y creara el punto de acceso "ClockWise-XE1E"
3. Conectate al punto de acceso para configurar una nueva red

### Reset de Fabrica
Borra toda la configuracion y vuelve a valores por defecto:
- WiFi: sin configurar
- Zona horaria: America/Mexico_City
- Brillo: 32
- Formato 24h: activado
- Idioma espanol: activado

**Atencion:** Tambien borra las caratulas guardadas.

### Actualizacion OTA (En desarrollo)
La actualizacion por aire (Over-The-Air) permitira actualizar el firmware sin conectar el cable USB:

1. Ve a la pestana **Sistema**
2. Haz clic en **"Actualizar Firmware"**
3. Selecciona el archivo `.bin` del nuevo firmware
4. Espera a que termine la actualizacion
5. El reloj se reiniciara automaticamente

**Nota:** Esta funcion esta en desarrollo y estara disponible en una version futura.

---

## Solucionar Problemas

### No puedo conectarme a la red "Clockwise-XXXX"
- Espera 30 segundos despues de encender
- Acercate mas al reloj
- Reinicia el reloj desconectando y conectando la fuente

### La pagina de configuracion no carga
- Verifica que estes en la misma red WiFi que el reloj
- Intenta con la IP directa en lugar de clockwise-xe1e.local
- Espera unos segundos y recarga la pagina
- Si el problema persiste, reinicia el reloj

### Olvide la IP del reloj
- La IP aparece brevemente en la pantalla al encender
- Intenta **http://clockwise-xe1e.local**
- Revisa la lista de dispositivos en tu router
- Usa una app como "Fing" para escanear la red

### La hora esta incorrecta
- Verifica que la zona horaria sea correcta
- Asegurate de que el reloj tenga conexion a internet
- Reinicia el reloj

### El brillo no cambia
- Si usas brillo automatico, verifica los valores min/max
- Pon ambos en 0 para desactivar y usar brillo manual

### El modo nocturno no se activa
- Verifica que este activado y guardado
- Confirma que el horario sea correcto
- Revisa que la hora del reloj sea correcta

### Los colores se ven mal (azul y verde invertidos)
- En Pantalla, activa "Intercambiar Azul/Verde"
- Reinicia el reloj

### El reloj no se conecta a mi WiFi
- Verifica que la contrasena sea correcta
- Asegurate de que tu red sea de **2.4 GHz** (no 5 GHz)
- Acerca el reloj al router

### Los thumbnails no aparecen
- Las caratulas necesitan tener el campo "thumbnail" en el JSON
- Usa el generador de thumbnails del editor para agregarlos
- Vuelve a subir las caratulas actualizadas

---

## Especificaciones

| Caracteristica | Valor |
|----------------|-------|
| Display | Panel LED HUB75 64x64 pixels |
| Microcontrolador | ESP32 |
| Conexion | WiFi 2.4 GHz |
| Alimentacion | 5V / 4A minimo |
| Sincronizacion | NTP (automatica por internet) |
| Almacenamiento | SPIFFS para caratulas |

---

## Conexiones de Hardware (Pinout)

Hay **dos firmwares** (ESP32 clasico y ESP32-S3). Graba el de tu placa, cablea segun **tu** tabla y
listo: el pinout del display ya viene compilado, no hay que configurar nada de la pantalla.

### Pantalla 64x64 HUB75

| Senal HUB75 | ESP32 clasico (GPIO) | ESP32-S3 (GPIO) |
|-------------|----------------------|-----------------|
| R1 / G1 / B1 | 25 / 26 / 27 | 4 / 5 / 6 |
| R2 / G2 / B2 | 14 / 12 / 13 | 7 / 15 / 16 |
| A / B / C / D | 23 / 19 / 5 / 17 | 18 / 8 / 3 / 42 |
| E (64x64) | 18 | 38 |
| LAT / OE / CLK | 4 / 15 / 16 | 40 / 2 / 41 |
| GND | GND | GND |

> Si los colores se ven mal (panel RBG), activa **"Intercambiar Azul/Verde"** en la pestana Pantalla;
> no hay que recablear.

### Perifericos (LDR y BME280)

| Periferico | Senal | ESP32 clasico | ESP32-S3 |
|------------|-------|---------------|----------|
| LDR (sensor de luz) | ADC | GPIO 35 | GPIO 1 |
| BME280 (ambiental) | SDA | GPIO 21 | GPIO 10 |
| BME280 (ambiental) | SCL | GPIO 22 | GPIO 9 |
| BME280 (ambiental) | VCC / GND | 3V3 / GND | 3V3 / GND |

> El BME280 es I2C y usa **3V3** (nunca 5V). En el ESP32-S3 el GPIO 8 NO sirve para I2C
> (lo usa el display). Los pines del sensor y del LDR son configurables desde la Web UI.

---

## Soporte

Si tienes problemas o sugerencias:
- **GitHub:** https://github.com/XE1E/Clockwise-XE1E
- **Email:** webmaster@luyoa.com
