#pragma once

const char WEB_UI_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ClockWise XE1E</title>
  <style>
*{margin:0;padding:0;box-sizing:border-box}
:root{--bg:#0f1117;--card:#1a1d2e;--input:#252940;--border:#363b5c;--text:#e0e0e8;--dim:#9090a8;--accent:#f0b800;--accent-h:#ffd040;--danger:#e04040;--success:#40c040;--r:8px}
[data-theme="light"]{--bg:#e8eaef;--card:#f5f5f8;--input:#eaedf2;--border:#c0c5d0;--text:#2a2a3a;--dim:#5a6070;--accent:#c89500;--accent-h:#a67c00}
html,body{font-family:system-ui,sans-serif;font-size:14px;background:var(--bg);color:var(--text);min-height:100vh}
nav{display:flex;gap:4px;padding:8px 12px;background:var(--card);border-bottom:1px solid var(--border);flex-wrap:wrap;align-items:center;position:sticky;top:0;z-index:100;box-shadow:0 2px 8px rgba(0,0,0,.3)}
.logo{color:var(--accent);font-weight:700;margin-right:12px;font-size:15px}
.tab{padding:6px 12px;border-radius:var(--r);background:transparent;color:var(--dim);font-size:13px;cursor:pointer;border:none}
.tab:hover{color:var(--text)}
.tab.active{background:var(--input);color:var(--accent)}
.theme-btn{margin-left:auto;background:none;border:none;font-size:18px;color:var(--dim);cursor:pointer;padding:4px 8px}
.theme-btn:hover{color:var(--accent)}
main{padding:16px;max-width:700px;margin:0 auto}
.page{display:none}
.page.active{display:block}
.card{background:var(--card);border:1px solid var(--border);border-radius:var(--r);padding:16px;margin-bottom:12px}
.card h2{font-size:14px;color:var(--accent);margin-bottom:12px;font-weight:600}
.card h3{font-size:13px;color:var(--dim);margin:12px 0 8px;font-weight:500}
label{display:block;color:var(--dim);font-size:12px;margin-bottom:4px}
input,select{width:100%;padding:8px 12px;background:var(--input);border:1px solid var(--border);border-radius:var(--r);color:var(--text);font:inherit;margin-bottom:10px}
input:focus,select:focus{outline:none;border-color:var(--accent)}
input[type="checkbox"]{width:auto;accent-color:var(--accent)}
input[type="range"]{padding:4px 0;accent-color:var(--accent)}
.row{display:grid;grid-template-columns:1fr 1fr;gap:12px}
.row3{display:grid;grid-template-columns:1fr 1fr 1fr;gap:8px}
.check-row{display:flex;align-items:center;gap:8px;margin-bottom:10px}
.check-row label{margin:0;color:var(--text)}
button{cursor:pointer;font:inherit;border:1px solid var(--border);border-radius:var(--r);padding:10px 20px;background:var(--input);color:var(--text);transition:all .15s}
button:hover{border-color:var(--accent)}
.btn-primary{background:var(--accent);color:#000;border-color:var(--accent);font-weight:600;width:100%}
.btn-primary:hover{background:var(--accent-h)}
.btn-danger{border-color:var(--danger);color:var(--danger)}
.btn-danger:hover{background:var(--danger);color:#fff}
.status-bar{display:flex;gap:16px;flex-wrap:wrap;padding:10px;background:var(--input);border-radius:var(--r);margin-bottom:12px;font-size:13px}
.status-item{display:flex;align-items:center;gap:6px}
.status-item span{color:var(--accent)}
.version{text-align:center;color:var(--dim);font-size:12px;margin-top:16px}
.toast{position:fixed;bottom:20px;left:50%;transform:translateX(-50%);background:var(--accent);color:#000;padding:10px 24px;border-radius:var(--r);font-weight:600;z-index:1000;display:none}
@media(max-width:500px){.row,.row3{grid-template-columns:1fr}.tab{padding:4px 8px;font-size:12px}}
  </style>
</head>
<body>
<nav>
  <span class="logo">ClockWise XE1E</span>
  <button class="tab active" data-page="home">Inicio</button>
  <button class="tab" data-page="wifi">WiFi</button>
  <button class="tab" data-page="display">Pantalla</button>
  <button class="tab" data-page="time">Hora</button>
  <button class="tab" data-page="night">Nocturno</button>
  <button class="tab" data-page="clock">Caratula</button>
  <button class="tab" data-page="editors">Editores</button>
  <button class="tab" data-page="system">Sistema</button>
  <button class="theme-btn" onclick="toggleTheme()" title="Cambiar tema">&#9728;</button>
</nav>

<main>
  <!-- HOME -->
  <div id="home" class="page active">
    <div class="status-bar">
      <div class="status-item">WiFi: <span id="st-ssid">-</span></div>
      <div class="status-item">IP: <span id="st-ip">-</span></div>
      <div class="status-item">RSSI: <span id="st-rssi">-</span> dBm</div>
    </div>
    <div class="card">
      <h2>Estado</h2>
      <div class="row">
        <div><label>Caratula actual</label><input id="st-clock" readonly></div>
        <div><label>Brillo</label><input id="st-bright" readonly></div>
      </div>
      <div class="row">
        <div><label>Modo nocturno</label><input id="st-night" readonly></div>
        <div><label>Hora</label><input id="st-time" readonly></div>
      </div>
    </div>
    <div class="card">
      <h2>Acciones rapidas</h2>
      <div class="row">
        <button onclick="api('restart')">Reiniciar</button>
        <button onclick="location.reload()">Refrescar</button>
      </div>
    </div>
  </div>

  <!-- WIFI -->
  <div id="wifi" class="page">
    <div class="card">
      <h2>Red Principal</h2>
      <div class="row">
        <div><label>SSID</label><input type="text" id="wifiSsid"></div>
        <div><label>Password</label><input type="password" id="wifiPwd"></div>
      </div>
    </div>
    <div class="card">
      <h2>Red 2 (Respaldo)</h2>
      <div class="row">
        <div><label>SSID</label><input type="text" id="wifiSsid2"></div>
        <div><label>Password</label><input type="password" id="wifiPwd2"></div>
      </div>
    </div>
    <div class="card">
      <h2>Red 3 (Respaldo)</h2>
      <div class="row">
        <div><label>SSID</label><input type="text" id="wifiSsid3"></div>
        <div><label>Password</label><input type="password" id="wifiPwd3"></div>
      </div>
    </div>
    <button class="btn-primary" onclick="saveWifi()">Guardar WiFi</button>
  </div>

  <!-- DISPLAY -->
  <div id="display" class="page">
    <div class="card">
      <h2>Pantalla</h2>
      <label>Brillo: <span id="brightVal">32</span></label>
      <input type="range" id="displayBright" min="1" max="255" value="32" oninput="$('brightVal').textContent=this.value">
      <label>Rotacion</label>
      <select id="displayRotation">
        <option value="0">0° (Normal)</option>
        <option value="1">90°</option>
        <option value="2">180°</option>
        <option value="3">270°</option>
      </select>
      <div class="check-row">
        <input type="checkbox" id="swapBlueGreen">
        <label>Intercambiar Azul/Verde (paneles RBG)</label>
      </div>
    </div>
    <div class="card">
      <h2>Brillo automatico (LDR)</h2>
      <div class="row">
        <div><label>Min LDR</label><input type="number" id="autoBrightMin" min="0" max="4095"></div>
        <div><label>Max LDR</label><input type="number" id="autoBrightMax" min="0" max="4095"></div>
      </div>
      <p style="color:var(--dim);font-size:12px;margin-top:8px">Dejar Max en 0 para desactivar</p>
    </div>
    <button class="btn-primary" onclick="saveDisplay()">Guardar Pantalla</button>
  </div>

  <!-- TIME -->
  <div id="time" class="page">
    <div class="card">
      <h2>Zona Horaria</h2>
      <label>Zona</label>
      <select id="timeZone">
        <option value="CST6">Mexico Centro (UTC-6)</option>
        <option value="MST7">Mexico Pacifico (UTC-7)</option>
        <option value="PST8">Mexico Noroeste (UTC-8)</option>
        <option value="EST5">Este USA (UTC-5)</option>
        <option value="CST6CDT,M3.2.0,M11.1.0">Centro USA (UTC-6 DST)</option>
        <option value="MST7MDT,M3.2.0,M11.1.0">Montana USA (UTC-7 DST)</option>
        <option value="PST8PDT,M3.2.0,M11.1.0">Pacifico USA (UTC-8 DST)</option>
        <option value="GMT0">UTC/GMT</option>
        <option value="CET-1">Europa Central (UTC+1)</option>
        <option value="CST-8">China (UTC+8)</option>
      </select>
      <label>Servidor NTP</label>
      <select id="ntpServer">
        <option value="time.cloudflare.com">Cloudflare (recomendado)</option>
        <option value="time.google.com">Google</option>
        <option value="pool.ntp.org">NTP Pool</option>
        <option value="time.nist.gov">NIST</option>
      </select>
    </div>
    <div class="card">
      <h2>Formato</h2>
      <div class="row">
        <div class="check-row">
          <input type="checkbox" id="use24hFormat">
          <label>Formato 24 horas</label>
        </div>
        <div class="check-row">
          <input type="checkbox" id="useSpanish">
          <label>Espanol</label>
        </div>
      </div>
    </div>
    <button class="btn-primary" onclick="saveTime()">Guardar Hora</button>
  </div>

  <!-- NIGHT MODE -->
  <div id="night" class="page">
    <div class="card">
      <h2>Modo Nocturno</h2>
      <div class="check-row">
        <input type="checkbox" id="nightEnabled">
        <label>Activar modo nocturno</label>
      </div>
      <div class="row">
        <div><label>Hora inicio</label><input type="time" id="nightStart" value="22:00"></div>
        <div><label>Hora fin</label><input type="time" id="nightEnd" value="07:00"></div>
      </div>
      <label>Brillo nocturno: <span id="nightBrightVal">8</span></label>
      <input type="range" id="nightBright" min="1" max="32" value="8" oninput="$('nightBrightVal').textContent=this.value">
    </div>
    <div class="card">
      <h2>Caratula Nocturna</h2>
      <label>Seleccionar caratula</label>
      <select id="nightClockSelect" onchange="onNightClockSelect()">
        <option value="night-clock">Night Clock (Rojo)</option>
        <option value="night-clock-xe1e">Night Clock XE1E</option>
        <option value="_custom">-- Personalizado --</option>
      </select>
      <div id="customNightClock" style="display:none">
        <label>Nombre caratula</label>
        <input type="text" id="nightClock" placeholder="night-clock">
      </div>
      <div class="row" style="margin-top:12px">
        <div>
          <label>Color de digitos</label>
          <input type="color" id="nightColor" value="#ff0000" style="width:100%;height:40px">
        </div>
        <div>
          <label>Colores rapidos</label>
          <div style="display:flex;gap:4px;margin-top:4px">
            <button type="button" onclick="setNightColor('#ff0000')" style="width:32px;height:32px;background:#ff0000;padding:0"></button>
            <button type="button" onclick="setNightColor('#ff6600')" style="width:32px;height:32px;background:#ff6600;padding:0"></button>
            <button type="button" onclick="setNightColor('#00ff00')" style="width:32px;height:32px;background:#00ff00;padding:0"></button>
            <button type="button" onclick="setNightColor('#0088ff')" style="width:32px;height:32px;background:#0088ff;padding:0"></button>
          </div>
        </div>
      </div>
    </div>
    <button class="btn-primary" onclick="saveNight()">Guardar Nocturno</button>
  </div>

  <!-- CLOCKFACE -->
  <div id="clock" class="page">
    <div class="card">
      <h2>Caratula</h2>
      <label>Seleccionar caratula</label>
      <select id="canvasSelect" onchange="onClockSelect()">
        <option value="">-- Seleccionar --</option>
        <option value="pac-man">Pac-Man</option>
        <option value="nyan-cat">Nyan Cat</option>
        <option value="donkey-kong">Donkey Kong</option>
        <option value="star-wars">Star Wars</option>
        <option value="goomba_move">Goomba</option>
        <option value="clock-club">Clock Club</option>
        <option value="retro-computer">Retro Computer</option>
        <option value="snoopy3">Snoopy</option>
        <option value="christmassnoopy">Christmas Snoopy</option>
        <option value="eletrogate">Eletrogate</option>
        <option value="pepsi-final-2">Pepsi</option>
        <option value="night-clock">Night Clock</option>
        <option value="night-clock-xe1e">Night Clock XE1E</option>
        <option value="_custom">-- Personalizado --</option>
      </select>
      <div id="customClock" style="display:none">
        <label>Servidor</label>
        <input type="text" id="canvasServer" placeholder="raw.githubusercontent.com">
        <label>Archivo</label>
        <input type="text" id="canvasFile" placeholder="nombre-caratula">
      </div>
    </div>
    <div class="card">
      <h2>Servidor de caratulas</h2>
      <label>Fuente</label>
      <select id="clockfaceSource">
        <option value="cdn">CDN XE1E (Recomendado)</option>
        <option value="github">GitHub (puede fallar)</option>
      </select>
      <p style="color:var(--dim);font-size:12px;margin-top:4px">GitHub puede no funcionar en algunos ESP32 por incompatibilidad SSL</p>
    </div>
    <div class="card">
      <h2>Rotacion de caratulas</h2>
      <div class="check-row">
        <input type="checkbox" id="rotationEnabled">
        <label>Activar rotacion</label>
      </div>
      <label>Intervalo (minutos)</label>
      <input type="number" id="rotationInterval" min="1" max="1440" value="60">
      <label>Lista de caratulas</label>
      <div id="rotationCheckboxes" style="display:grid;grid-template-columns:1fr 1fr;gap:4px;margin-bottom:10px;max-height:150px;overflow-y:auto;background:var(--input);padding:8px;border-radius:var(--r)"></div>
      <input type="hidden" id="rotationList">
    </div>
    <button class="btn-primary" onclick="saveClock()">Guardar Caratula</button>
  </div>

  <!-- EDITORS -->
  <div id="editors" class="page">
    <div class="card">
      <h2>Editores de Caratulas</h2>
      <p style="color:var(--dim);font-size:13px;margin-bottom:16px">
        Herramientas web para crear y editar caratulas. Se abren en nueva ventana.
      </p>
      <div class="row" style="margin-bottom:12px">
        <a href="https://xe1e.github.io/Clockwise-XE1E/clockface-editor/" target="_blank" style="text-decoration:none">
          <button style="width:100%">Editor de Caratulas</button>
        </a>
        <a href="https://xe1e.github.io/Clockwise-XE1E/clockface-editor/digit-designer.html" target="_blank" style="text-decoration:none">
          <button style="width:100%">Disenador de Digitos</button>
        </a>
      </div>
      <div class="row" style="margin-bottom:12px">
        <a href="https://xe1e.github.io/Clockwise-XE1E/clockface-editor/char-designer.html" target="_blank" style="text-decoration:none">
          <button style="width:100%">Disenador de Caracteres</button>
        </a>
        <a href="https://xe1e.github.io/Clockwise-XE1E/clockface-editor/font-converter.html" target="_blank" style="text-decoration:none">
          <button style="width:100%">Conversor de Fuentes</button>
        </a>
      </div>
      <div class="row" style="margin-bottom:12px">
        <a href="https://xe1e.github.io/Clockwise-XE1E/clockface-editor/generate-thumbs.html" target="_blank" style="text-decoration:none">
          <button style="width:100%">Generar Thumbnails</button>
        </a>
        <a href="https://xe1e.github.io/Clockwise-XE1E/clockface-editor/config-page.html" target="_blank" style="text-decoration:none">
          <button style="width:100%">Configuracion</button>
        </a>
      </div>
    </div>
    <div class="card">
      <h2>Recursos</h2>
      <div class="row" style="margin-bottom:12px">
        <a href="https://xe1e.github.io/Clockwise-XE1E/clockface-editor/install.html" target="_blank" style="text-decoration:none">
          <button style="width:100%">Guia de Instalacion</button>
        </a>
        <a href="https://github.com/XE1E/Clockwise-XE1E/tree/main/clockface-editor/clockfaces" target="_blank" style="text-decoration:none">
          <button style="width:100%">Caratulas de ejemplo</button>
        </a>
      </div>
      <div class="row">
        <a href="https://github.com/XE1E/Clockwise-XE1E/tree/main/clockface-editor/fonts" target="_blank" style="text-decoration:none">
          <button style="width:100%">Fuentes disponibles</button>
        </a>
        <a href="https://github.com/XE1E/Clockwise-XE1E" target="_blank" style="text-decoration:none">
          <button style="width:100%">Repositorio GitHub</button>
        </a>
      </div>
    </div>
  </div>

  <!-- SYSTEM -->
  <div id="system" class="page">
    <div class="card">
      <h2>Informacion</h2>
      <div class="row">
        <div><label>Firmware</label><input id="sys-name" readonly></div>
        <div><label>Version</label><input id="sys-version" readonly></div>
      </div>
    </div>
    <div class="card">
      <h2>Acciones</h2>
      <div class="row" style="margin-bottom:12px">
        <button onclick="api('restart')">Reiniciar</button>
        <button class="btn-danger" onclick="if(confirm('Restaurar valores de fabrica?'))api('reset')">Reset fabrica</button>
      </div>
    </div>
  </div>

  <div class="version">ClockWise XE1E v<span id="ver">1.0.0</span></div>
</main>

<div class="toast" id="toast">Guardado</div>

<script>
const $=id=>document.getElementById(id);
let settings={};

function toggleTheme(){
  const t=document.documentElement.getAttribute('data-theme')==='light'?'dark':'light';
  document.documentElement.setAttribute('data-theme',t);
  localStorage.setItem('theme',t);
  document.querySelector('.theme-btn').innerHTML=t==='dark'?'&#9728;':'&#9790;';
}
(function(){
  const t=localStorage.getItem('theme')||'dark';
  document.documentElement.setAttribute('data-theme',t);
  document.querySelector('.theme-btn').innerHTML=t==='dark'?'&#9728;':'&#9790;';
})();

document.querySelectorAll('.tab').forEach(tab=>{
  tab.onclick=()=>{
    document.querySelectorAll('.tab').forEach(t=>t.classList.remove('active'));
    document.querySelectorAll('.page').forEach(p=>p.classList.remove('active'));
    tab.classList.add('active');
    $(tab.dataset.page).classList.add('active');
  };
});

function toast(msg){
  const t=$('toast');
  t.textContent=msg||'Guardado';
  t.style.display='block';
  setTimeout(()=>t.style.display='none',2000);
}

async function api(action,params={}){
  try{
    const q=Object.entries(params).map(([k,v])=>k+'='+encodeURIComponent(v)).join('&');
    await fetch('/api/'+action+(q?'?'+q:''),{method:'POST'});
    toast(action==='restart'?'Reiniciando...':'OK');
  }catch(e){toast('Error');}
}

const clockfaces=['pac-man','nyan-cat','donkey-kong','star-wars','goomba_move','clock-club','retro-computer','snoopy3','christmassnoopy','eletrogate','pepsi-final-2','night-clock','night-clock-xe1e'];

function rgb565ToHex(v){
  const r=((v>>11)&0x1F)*255/31;
  const g=((v>>5)&0x3F)*255/63;
  const b=(v&0x1F)*255/31;
  return '#'+[r,g,b].map(x=>Math.round(x).toString(16).padStart(2,'0')).join('');
}
function hexToRgb565(h){
  const r=parseInt(h.slice(1,3),16);
  const g=parseInt(h.slice(3,5),16);
  const b=parseInt(h.slice(5,7),16);
  return ((r>>3)<<11)|((g>>2)<<5)|(b>>3);
}
function setNightColor(c){$('nightColor').value=c;}
async function onClockSelect(){
  const v=$('canvasSelect').value;
  $('customClock').style.display=v==='_custom'?'block':'none';
  if(v&&v!=='_custom'){
    $('canvasFile').value=v;
    $('canvasServer').value='raw.githubusercontent.com';
    await saveField('canvasServer','raw.githubusercontent.com');
    await saveField('canvasFile',v);
    await fetch('/api/reload',{method:'POST'});
    toast('Cambiando a '+v+'...');
  }
}
function onNightClockSelect(){
  const v=$('nightClockSelect').value;
  $('customNightClock').style.display=v==='_custom'?'block':'none';
  if(v&&v!=='_custom')$('nightClock').value=v;
}
function buildRotationCheckboxes(){
  const cont=$('rotationCheckboxes');
  const sel=($('rotationList').value||'').split(',').map(s=>s.trim()).filter(s=>s);
  cont.innerHTML=clockfaces.map(c=>'<label style="font-size:12px"><input type="checkbox" class="rot-cb" value="'+c+'"'+(sel.includes(c)?' checked':'')+' onchange="updateRotationList()">'+c+'</label>').join('');
}
function updateRotationList(){
  const cbs=document.querySelectorAll('.rot-cb:checked');
  $('rotationList').value=Array.from(cbs).map(c=>c.value).join(',');
}

async function load(){
  try{
    const r=await fetch('/api/settings');
    settings=await r.json();

    // Status
    $('st-ssid').textContent=settings.wifiConnected||'-';
    $('st-ip').textContent=location.hostname;
    $('st-rssi').textContent=settings.wifiRssi||'-';
    $('st-clock').value=settings.canvasFile||'-';
    $('st-bright').value=settings.displayBright||'-';
    $('st-night').value=settings.nightEnabled==1?'Activo':'Inactivo';
    $('st-time').value=new Date().toLocaleTimeString();

    // WiFi
    $('wifiSsid').value=settings.wifiSsid||'';
    $('wifiSsid2').value=settings.wifiSsid2||'';
    $('wifiSsid3').value=settings.wifiSsid3||'';

    // Display
    $('displayBright').value=settings.displayBright||32;
    $('brightVal').textContent=settings.displayBright||32;
    $('displayRotation').value=settings.displayRotation||0;
    $('swapBlueGreen').checked=settings.swapBlueGreen==1;
    $('autoBrightMin').value=settings.autoBrightMin||0;
    $('autoBrightMax').value=settings.autoBrightMax||0;

    // Time
    $('timeZone').value=settings.timeZone||'CST6';
    $('ntpServer').value=settings.ntpServer||'time.cloudflare.com';
    $('use24hFormat').checked=settings.use24hFormat==1;
    $('useSpanish').checked=settings.useSpanish==1;

    // Night
    $('nightEnabled').checked=settings.nightEnabled==1;
    $('nightStart').value=settings.nightStart||'22:00';
    $('nightEnd').value=settings.nightEnd||'07:00';
    $('nightBright').value=settings.nightBright||8;
    $('nightBrightVal').textContent=settings.nightBright||8;
    $('nightClock').value=settings.nightClock||'night-clock';
    $('nightColor').value=rgb565ToHex(settings.nightColor||63488);
    const nc=settings.nightClock||'night-clock';
    if(clockfaces.includes(nc)||nc==='night-clock'||nc==='night-clock-xe1e'){
      $('nightClockSelect').value=nc;
    }else{
      $('nightClockSelect').value='_custom';
      $('customNightClock').style.display='block';
    }

    // Clock
    $('canvasServer').value=settings.canvasServer||'raw.githubusercontent.com';
    $('canvasFile').value=settings.canvasFile||'';
    $('clockfaceSource').value=settings.clockfaceSource||'cdn';
    $('rotationEnabled').checked=settings.rotationEnabled==1;
    $('rotationInterval').value=settings.rotationInterval||60;
    $('rotationList').value=settings.rotationList||'';
    const cf=settings.canvasFile||'';
    if(clockfaces.includes(cf)){
      $('canvasSelect').value=cf;
    }else if(cf){
      $('canvasSelect').value='_custom';
      $('customClock').style.display='block';
    }
    buildRotationCheckboxes();

    // System
    $('sys-name').value=settings.name||'ClockWise-XE1E';
    $('sys-version').value=settings.version||'1.0.0';
    $('ver').textContent=settings.version||'1.0.0';
  }catch(e){console.error(e);}
}

async function saveField(key,val){
  await fetch('/api/set?'+key+'='+encodeURIComponent(val),{method:'POST'});
}

async function saveWifi(){
  await saveField('wifiSsid',$('wifiSsid').value);
  await saveField('wifiPwd',$('wifiPwd').value);
  await saveField('wifiSsid2',$('wifiSsid2').value);
  await saveField('wifiPwd2',$('wifiPwd2').value);
  await saveField('wifiSsid3',$('wifiSsid3').value);
  await saveField('wifiPwd3',$('wifiPwd3').value);
  toast('WiFi guardado');
}

async function saveDisplay(){
  await saveField('displayBright',$('displayBright').value);
  await saveField('displayRotation',$('displayRotation').value);
  await saveField('swapBlueGreen',$('swapBlueGreen').checked?'1':'0');
  await saveField('autoBrightMin',$('autoBrightMin').value);
  await saveField('autoBrightMax',$('autoBrightMax').value);
  toast('Pantalla guardada');
}

async function saveTime(){
  await saveField('timeZone',$('timeZone').value);
  await saveField('ntpServer',$('ntpServer').value);
  await saveField('use24hFormat',$('use24hFormat').checked?'1':'0');
  await saveField('useSpanish',$('useSpanish').checked?'1':'0');
  toast('Hora guardada');
}

async function saveNight(){
  await saveField('nightEnabled',$('nightEnabled').checked?'1':'0');
  await saveField('nightStart',$('nightStart').value);
  await saveField('nightEnd',$('nightEnd').value);
  await saveField('nightBright',$('nightBright').value);
  await saveField('nightColor',hexToRgb565($('nightColor').value));
  await saveField('nightClock',$('nightClock').value);
  toast('Modo nocturno guardado');
}

async function saveClock(){
  await saveField('canvasServer',$('canvasServer').value);
  await saveField('canvasFile',$('canvasFile').value);
  await saveField('clockfaceSource',$('clockfaceSource').value);
  await saveField('rotationEnabled',$('rotationEnabled').checked?'1':'0');
  await saveField('rotationInterval',$('rotationInterval').value);
  await saveField('rotationList',$('rotationList').value);
  await fetch('/api/reload',{method:'POST'});
  toast('Caratula aplicada');
}

load();
setInterval(()=>{$('st-time').value=new Date().toLocaleTimeString();},1000);
</script>
</body>
</html>
)rawliteral";
