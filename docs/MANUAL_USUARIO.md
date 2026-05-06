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

### 2. Conectar a WiFi

1. Enciende el reloj (conecta la fuente de 5V)
2. El reloj creara una red WiFi llamada **"Clockwise"**
3. Conectate a esa red desde tu celular o computadora
4. Se abrira automaticamente una pagina de configuracion
5. Selecciona tu red WiFi e ingresa la contrasena
6. El reloj se reiniciara y se conectara a tu red

### 3. Acceder a la Configuracion

1. Asegurate de estar en la misma red WiFi que el reloj
2. Abre un navegador y escribe: **http://clockwise.local**
3. La pagina de configuracion se mostrara

**Nota:** Si `clockwise.local` no funciona (comun en Windows sin Bonjour), busca la IP del reloj:
   - Revisa tu router para ver dispositivos conectados
   - O usa una app como "Fing" para escanear la red
   - Luego escribe la IP directamente (ej: `192.168.1.50`)

---

## Configuracion del Reloj

### Brillo
- Mueve el control deslizante para ajustar el brillo
- 0 = apagado, 255 = maximo brillo
- **Recomendado:** 30-60 para uso normal

### Formato de Hora
- Activa para mostrar hora en formato 24 horas (14:00)
- Desactiva para formato 12 horas (2:00 PM)

### Idioma
- Activa "Espanol" para ver dias y meses en espanol
- Lun, Mar, Mie... Ene, Feb, Mar...

### Zona Horaria
Ingresa tu zona horaria. Ejemplos:
| Pais | Zona Horaria |
|---|---|
| Mexico (Centro) | America/Mexico_City |
| Mexico (Pacifico) | America/Tijuana |
| Colombia | America/Bogota |
| Argentina | America/Argentina/Buenos_Aires |
| Espana | Europe/Madrid |
| Chile | America/Santiago |
| Peru | America/Lima |
| Venezuela | America/Caracas |

---

## Brillo Automatico

Si tu reloj tiene sensor de luz (LDR), puedes activar el brillo automatico:

1. Busca la tarjeta "Automatic Bright"
2. Calibra los valores:
   - Haz clic en "Read Pin" con luz normal y anota el valor
   - Oscurece la habitacion, haz clic en "Read Pin" y anota el valor
   - Ingresa el valor bajo en "Min" y el alto en "Max"
3. Guarda la configuracion

**Para desactivar:** Pon ambos valores en 0.

---

## Modo Nocturno

El modo nocturno reduce el brillo y cambia la caratula durante la noche para no molestar al dormir.

### Configurar
1. Activa "Modo Nocturno"
2. Configura el horario (ej: inicio 22:00, fin 07:00)
3. Ajusta el brillo nocturno (recomendado: 4-8)
4. Selecciona el color de los digitos (rojo recomendado)
5. Elige la caratula nocturna
6. Guarda y reinicia

### Colores Recomendados
- **Rojo:** No afecta la vision nocturna
- **Naranja:** Suave y calido
- **Verde oscuro:** Bajo impacto visual

---

## Rotacion de Caratulas

Cambia automaticamente entre diferentes caratulas.

### Configurar
1. Activa "Rotacion de Caratulas"
2. Configura el intervalo en minutos:
   - 60 = cada hora
   - 1440 = cada dia
3. Marca las caratulas que quieres incluir
4. Guarda y reinicia

**Nota:** La rotacion se pausa durante el modo nocturno.

---

## Caratulas Disponibles

| Caratula | Descripcion |
|---|---|
| Nyan Cat | El gato arcoiris animado |
| Pac-Man | Clasico de arcade |
| Goomba | Enemigo de Mario Bros |
| Snoopy | El perro de Peanuts |
| Snoopy Navidad | Version navidena |
| Clock Club | Estilo moderno |
| Donkey Kong | Arcade clasico |
| Pepsi | Logo retro |
| Retro Computer | Estilo computadora antigua |
| Star Wars | Tematica espacial |
| Reloj Nocturno | Minimalista, ideal para dormir |

---

## Cambiar Caratula Manualmente

1. Accede a la pagina de configuracion
2. En "Description file" escribe el nombre de la caratula
3. Guarda y reinicia

Nombres disponibles: `nyan-cat`, `pac-man`, `goomba_move`, `snoopy3`, `christmassnoopy`, `clock-club`, `donkey-kong`, `pepsi-final-2`, `retro-computer`, `star-wars`, `night-clock`

---

## Reiniciar el Reloj

Haz clic en el boton "Restart" en la parte superior de la pagina de configuracion.

Algunos cambios requieren reinicio para aplicarse:
- Zona horaria
- Modo nocturno
- Rotacion de caratulas

---

## Solucionar Problemas

### No puedo conectarme a la red "Clockwise"
- Espera 30 segundos despues de encender
- Acercate mas al reloj
- Reinicia el reloj desconectando y conectando la fuente

### Olvide la IP del reloj
- Intenta acceder a **http://clockwise.local** (mDNS)
- Si no funciona, revisa la lista de dispositivos en tu router
- Usa una app como "Fing" para escanear la red
- Busca un dispositivo llamado "ESP32" o "Clockwise"

### La hora esta incorrecta
- Verifica que la zona horaria sea correcta
- Asegurate de que el reloj tenga conexion a internet
- Reinicia el reloj

### El brillo no cambia
- Si usas brillo automatico, verifica los valores min/max
- Intenta poner ambos en 0 para desactivar y usar brillo manual

### El modo nocturno no se activa
- Verifica que este activado
- Confirma que el horario sea correcto
- Revisa que la hora del reloj sea correcta

### Los colores se ven mal (azul y verde invertidos)
- Activa la opcion "Swap Blue/Green pins" en configuracion avanzada
- Reinicia el reloj

### El reloj no se conecta a mi WiFi
- Verifica que la contrasena sea correcta
- Asegurate de que tu red sea de 2.4 GHz (no 5 GHz)
- Acerca el reloj al router

---

## Resetear Configuracion WiFi

Si necesitas conectar el reloj a otra red WiFi:
1. El reloj debe estar encendido
2. Espera a que intente conectar (aprox 30 segundos)
3. Si no puede conectar, automaticamente creara la red "Clockwise"
4. Conectate y configura la nueva red

---

## Especificaciones

| Caracteristica | Valor |
|---|---|
| Display | Panel LED 64x64 pixels |
| Conexion | WiFi 2.4 GHz |
| Alimentacion | 5V / 4A minimo |
| Sincronizacion | NTP (automatica por internet) |

---

## Soporte

Si tienes problemas o sugerencias, visita:
- GitHub: [Enlace al repositorio]
- Email: [Correo de soporte]
