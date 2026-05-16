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
      <input type="range" id="displayBright" min="1" max="255" value="32" oninput="$('brightVal').textContent=this.value;sendBright(this.value)">
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
        <option value="nigth-clock-1">Reloj Nocturno</option>
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
          <label>Colores brillantes</label>
          <div style="display:flex;gap:4px;margin-top:4px">
            <button type="button" onclick="setNightColor('#ff0000')" style="width:32px;height:32px;background:#ff0000;padding:0"></button>
            <button type="button" onclick="setNightColor('#ff6600')" style="width:32px;height:32px;background:#ff6600;padding:0"></button>
            <button type="button" onclick="setNightColor('#00ff00')" style="width:32px;height:32px;background:#00ff00;padding:0"></button>
            <button type="button" onclick="setNightColor('#0088ff')" style="width:32px;height:32px;background:#0088ff;padding:0"></button>
          </div>
          <label style="margin-top:8px">Colores tenues (para dormir)</label>
          <div style="display:flex;gap:4px;margin-top:4px">
            <button type="button" onclick="setNightColor('#200000')" style="width:32px;height:32px;background:#200000;border:1px solid #444;padding:0" title="Rojo tenue"></button>
            <button type="button" onclick="setNightColor('#201000')" style="width:32px;height:32px;background:#201000;border:1px solid #444;padding:0" title="Ambar tenue"></button>
            <button type="button" onclick="setNightColor('#002000')" style="width:32px;height:32px;background:#002000;border:1px solid #444;padding:0" title="Verde tenue"></button>
            <button type="button" onclick="setNightColor('#000820')" style="width:32px;height:32px;background:#000820;border:1px solid #444;padding:0" title="Azul tenue"></button>
          </div>
        </div>
      </div>
    </div>
    <button class="btn-primary" onclick="saveNight()">Guardar Nocturno</button>
  </div>

  <!-- CLOCKFACE -->
  <div id="clock" class="page">
    <div class="card">
      <h2>Caratulas Guardadas</h2>
      <div style="background:var(--input);padding:10px;border-radius:var(--r);margin-bottom:12px">
        <div style="display:flex;justify-content:space-between;align-items:center;margin-bottom:8px">
          <span style="font-size:13px">Almacenamiento:</span>
          <span id="storageInfo" style="font-size:12px;color:var(--dim)">Cargando...</span>
        </div>
        <div style="height:6px;background:#333;border-radius:3px;overflow:hidden">
          <div id="storageBar" style="height:100%;background:var(--accent);width:0%;transition:width 0.3s"></div>
        </div>
      </div>
      <div id="storedList" style="background:var(--input);padding:8px;border-radius:var(--r);max-height:180px;overflow-y:auto;font-size:12px;margin-bottom:12px">
        <span style="color:var(--dim)">Cargando...</span>
      </div>
      <h3>Subir archivo JSON</h3>
      <input type="file" id="clockfaceFile" accept=".json" style="margin-bottom:8px">
      <button onclick="uploadClockface()" style="width:100%">Subir al reloj</button>
    </div>
    <div class="card">
      <h2>Descargar del Repositorio</h2>
      <p style="color:var(--dim);font-size:12px;margin-bottom:12px">Descarga caratulas y guardalas en el reloj</p>
      <label>Fuente</label>
      <select id="repoSource" style="margin-bottom:8px">
        <option value="github">GitHub XE1E</option>
        <option value="local">Servidor Local</option>
      </select>
      <div id="localConfig" style="display:none;margin-bottom:8px">
        <label>IP del servidor</label>
        <input type="text" id="localServerHost" placeholder="192.168.1.100" style="margin-bottom:4px">
        <label>Puerto</label>
        <input type="number" id="localServerPort" min="1" max="65535" value="8080">
      </div>
      <label>Caratula</label>
      <div style="display:flex;gap:8px;margin-bottom:8px">
        <select id="repoClockface" style="flex:1"></select>
        <input type="text" id="customClockface" placeholder="nombre" style="flex:1;display:none">
      </div>
      <button onclick="downloadFromRepo()" style="width:100%">Descargar y Guardar</button>
    </div>
    <div class="card">
      <h2>Seleccion Activa</h2>
      <div class="check-row" style="margin-bottom:12px">
        <input type="checkbox" id="rotationEnabled" onchange="onRotationToggle()">
        <label>Rotar entre varias</label>
      </div>
      <div id="rotationConfig" style="display:none;margin-bottom:12px">
        <label>Intervalo (minutos)</label>
        <input type="number" id="rotationInterval" min="1" max="1440" value="60">
      </div>
      <div id="singleSelect">
        <label>Caratula a mostrar</label>
        <select id="canvasSelect" onchange="onClockSelect()"></select>
      </div>
      <div id="multiSelect" style="display:none">
        <label>Caratulas <small style="color:var(--dim)">(arrastra para ordenar)</small></label>
        <div id="clockfaceList" style="background:var(--input);padding:8px;border-radius:var(--r);max-height:250px;overflow-y:auto;margin-bottom:10px"></div>
      </div>
      <input type="hidden" id="canvasFile">
      <input type="hidden" id="rotationList">
    </div>
    <button class="btn-primary" onclick="saveClock()">Aplicar Seleccion</button>
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

const cfGHPages=['pac-man','nyan-cat','night-clock','night-clock-xe1e','donkey-kong','star-wars','goomba_move','clock-club','retro-computer','snoopy3','christmassnoopy','eletrogate','pepsi-final-2','world-clock'];
const clockfaces=cfGHPages;

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

let storedClockfaces=[];

async function loadStorageInfo(){
  try{
    const r=await fetch('/api/storage');
    const d=await r.json();
    const pct=Math.round(d.used/d.total*100);
    const freeKB=Math.round(d.free/1024);
    $('storageInfo').textContent=freeKB+'KB libres ('+pct+'% usado)';
    $('storageBar').style.width=pct+'%';
    $('storageBar').style.background=pct>90?'#f44':pct>70?'#fa0':'var(--accent)';
  }catch(e){$('storageInfo').textContent='Error';}
}

async function loadStoredClockfaces(){
  try{
    const r=await fetch('/api/clockfaces/list');
    storedClockfaces=await r.json();
    renderStoredList();
    buildStoredSelect();
  }catch(e){$('storedList').innerHTML='<span style="color:#f44">Error al cargar</span>';}
}

function renderStoredList(){
  if(storedClockfaces.length===0){
    $('storedList').innerHTML='<span style="color:var(--dim)">No hay caratulas guardadas</span>';
    return;
  }
  $('storedList').innerHTML=storedClockfaces.map(c=>
    '<div style="display:flex;justify-content:space-between;align-items:center;padding:4px 0;border-bottom:1px solid #333">'+
    '<span>'+c.name+' <small style="color:var(--dim)">('+Math.round(c.size/1024)+'KB)</small></span>'+
    '<button onclick="deleteClockface(\''+c.name+'\')" style="background:#f44;color:#fff;border:none;padding:2px 8px;border-radius:4px;cursor:pointer;font-size:11px">X</button>'+
    '</div>'
  ).join('');
}

function buildStoredSelect(){
  const rot=$('rotationEnabled').checked;
  if(storedClockfaces.length===0){
    $('canvasSelect').innerHTML='<option value="">-- Sin caratulas --</option>';
    $('clockfaceCheckboxes').innerHTML='<span style="color:var(--dim)">Descarga o sube caratulas primero</span>';
    return;
  }
  if(rot){
    $('multiSelect').style.display='block';
    $('singleSelect').style.display='none';
    const selected=($('rotationList').value||'').split(',').map(s=>s.trim()).filter(s=>s);
    // Sort: selected first (in order), then unselected - keep full objects for bg color
    const getItem=n=>storedClockfaces.find(c=>c.name===n)||{name:n,bg:0};
    const sorted=[...selected.filter(n=>storedClockfaces.some(c=>c.name===n)).map(getItem),...storedClockfaces.filter(c=>!selected.includes(c.name))];
    renderClockfaceList(sorted,selected);
  }else{
    $('multiSelect').style.display='none';
    $('singleSelect').style.display='block';
    const current=$('canvasFile').value||'';
    $('canvasSelect').innerHTML=storedClockfaces.map(c=>
      '<option value="'+c.name+'"'+(c.name===current?' selected':'')+'>'+c.name+'</option>'
    ).join('');
  }
}

function buildRepoSelect(){
  const src=$('repoSource').value;
  const isLocal=src==='local';
  $('localConfig').style.display=isLocal?'block':'none';
  $('repoClockface').style.display=isLocal?'none':'block';
  $('customClockface').style.display=isLocal?'block':'none';
  if(!isLocal){
    $('repoClockface').innerHTML=cfGHPages.map(c=>'<option value="'+c+'">'+c+'</option>').join('');
  }
}

async function downloadFromRepo(){
  const src=$('repoSource').value;
  const isLocal=src==='local';
  const name=isLocal?$('customClockface').value.trim():$('repoClockface').value;
  if(!name){toast('Ingresa nombre de caratula');return;}
  let url;
  if(isLocal){
    const host=$('localServerHost').value||'192.168.1.100';
    const port=$('localServerPort').value||8080;
    url='http://'+host+':'+port+'/'+name+'.json';
  }else{
    url='https://xe1e.github.io/Clockwise-XE1E/clockface-editor/clockfaces/'+name+'.json';
  }
  toast('Descargando...');
  try{
    const r=await fetch(url);
    if(!r.ok)throw new Error('HTTP '+r.status);
    const json=await r.text();
    JSON.parse(json);
    const up=await fetch('/api/clockfaces/upload?name='+encodeURIComponent(name),{method:'POST',headers:{'Content-Type':'application/json'},body:json});
    if(up.ok){toast('Guardado: '+name);loadStorageInfo();loadStoredClockfaces();}
    else toast('Error al guardar');
  }catch(e){toast('Error: '+e.message);}
}

async function uploadClockface(){
  const fileInput=$('clockfaceFile');
  if(!fileInput.files.length){toast('Selecciona archivo');return;}
  const file=fileInput.files[0];
  const name=file.name.replace('.json','');
  try{
    const text=await file.text();
    JSON.parse(text);
    const r=await fetch('/api/clockfaces/upload?name='+encodeURIComponent(name),{method:'POST',headers:{'Content-Type':'application/json'},body:text});
    if(r.ok){toast('Subido: '+name);fileInput.value='';loadStorageInfo();loadStoredClockfaces();}
    else toast('Error al subir');
  }catch(e){toast('JSON invalido');}
}

async function deleteClockface(name){
  if(!confirm('Eliminar '+name+'?'))return;
  try{
    await fetch('/api/clockfaces/delete?name='+encodeURIComponent(name),{method:'POST'});
    toast('Eliminado');loadStorageInfo();loadStoredClockfaces();
  }catch(e){toast('Error');}
}

function onRotationToggle(){
  const rot=$('rotationEnabled').checked;
  $('rotationConfig').style.display=rot?'block':'none';
  buildStoredSelect();
}

let dragItem=null;
let thumbCache={};
function rgb565ToHex(c){
  const r=((c>>11)&0x1F)*255/31|0;
  const g=((c>>5)&0x3F)*255/63|0;
  const b=(c&0x1F)*255/31|0;
  return '#'+((1<<24)|(r<<16)|(g<<8)|b).toString(16).slice(1);
}
async function loadThumb(name,canvas){
  if(thumbCache[name]){drawThumb(canvas,thumbCache[name]);return;}
  try{
    const r=await fetch('/api/clockfaces/get?name='+encodeURIComponent(name));
    const cf=await r.json();
    const bg=cf.bgColor||0;
    let img=null;
    if(cf.setup)for(const el of cf.setup){if(el.image){img=el.image;break;}}
    if(!img&&cf.loop)for(const el of cf.loop){if(el.image){img=el.image;break;}}
    thumbCache[name]={bg,img};
    drawThumb(canvas,thumbCache[name]);
  }catch(e){canvas.style.background='#333';}
}
function drawThumb(canvas,data){
  const ctx=canvas.getContext('2d');
  ctx.fillStyle=rgb565ToHex(data.bg);
  ctx.fillRect(0,0,48,48);
  if(data.img){
    const img=new Image();
    img.onload=()=>{ctx.imageSmoothingEnabled=false;ctx.drawImage(img,0,0,48,48);};
    img.src='data:image/png;base64,'+data.img;
  }
}
function renderClockfaceList(allItems,selected){
  const cont=$('clockfaceList');
  cont.innerHTML=allItems.map(item=>{
    const n=typeof item==='string'?item:item.name;
    const checked=selected.includes(n);
    const idx=checked?selected.indexOf(n)+1:0;
    return '<div draggable="true" data-name="'+n+'" style="padding:6px 8px;margin:3px 0;background:var(--bg);border-radius:6px;cursor:grab;font-size:13px;display:flex;align-items:center;gap:10px">'
      +'<input type="checkbox" class="cf-cb" '+(checked?'checked':'')+' onchange="updateFromList()">'
      +'<canvas width="48" height="48" style="width:48px;height:48px;border-radius:4px;border:1px solid #444;image-rendering:pixelated;flex-shrink:0"></canvas>'
      +'<span style="color:var(--dim);min-width:20px;font-weight:bold">'+(idx?idx+'.':'')+'</span>'
      +'<span style="flex:1;overflow:hidden;text-overflow:ellipsis">'+n+'</span></div>';
  }).join('');
  cont.querySelectorAll('[draggable]').forEach(el=>{
    const name=el.dataset.name;
    const canvas=el.querySelector('canvas');
    loadThumb(name,canvas);
    el.ondragstart=e=>{dragItem=el;el.style.opacity='0.5';};
    el.ondragend=e=>{el.style.opacity='1';dragItem=null;};
    el.ondragover=e=>{e.preventDefault();};
    el.ondrop=e=>{
      e.preventDefault();
      if(!dragItem||dragItem===el)return;
      const list=Array.from(cont.children);
      const fromIdx=list.indexOf(dragItem),toIdx=list.indexOf(el);
      if(fromIdx<toIdx)el.after(dragItem);else el.before(dragItem);
      updateFromList();
    };
  });
}

function updateFromList(){
  const items=Array.from($('clockfaceList').querySelectorAll('[data-name]'));
  const checked=items.filter(el=>el.querySelector('.cf-cb').checked).map(el=>el.dataset.name);
  $('rotationList').value=checked.join(',');
  if(checked.length>0)$('canvasFile').value=checked[0];
  // Just update numbers without full re-render
  items.forEach(el=>{
    const n=el.dataset.name;
    const idx=checked.includes(n)?checked.indexOf(n)+1:0;
    const numSpan=el.querySelectorAll('span')[0];
    if(numSpan)numSpan.textContent=idx?idx+'.':'';
  });
}

function onClockSelect(){
  const v=$('canvasSelect').value;
  if(v)$('canvasFile').value=v;
}

function onNightClockSelect(){
  const v=$('nightClockSelect').value;
  $('customNightClock').style.display=v==='_custom'?'block':'none';
  if(v&&v!=='_custom')$('nightClock').value=v;
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
    $('wifiPwd').placeholder=settings.wifiHasPwd?'(guardado)':'';
    $('wifiSsid2').value=settings.wifiSsid2||'';
    $('wifiPwd2').placeholder=settings.wifiHasPwd2?'(guardado)':'';
    $('wifiSsid3').value=settings.wifiSsid3||'';
    $('wifiPwd3').placeholder=settings.wifiHasPwd3?'(guardado)':'';

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
    $('nightClock').value=settings.nightClock||'nigth-clock-1';
    $('nightColor').value=rgb565ToHex(settings.nightColor||63488);
    const nc=settings.nightClock||'nigth-clock-1';
    if(nc==='nigth-clock-1'){
      $('nightClockSelect').value=nc;
    }else{
      $('nightClockSelect').value='_custom';
      $('customNightClock').style.display='block';
    }

    // Clock
    $('canvasFile').value=settings.canvasFile||'';
    $('rotationEnabled').checked=settings.rotationEnabled==1;
    $('rotationInterval').value=settings.rotationInterval||60;
    $('rotationList').value=settings.rotationList||'';
    $('rotationConfig').style.display=settings.rotationEnabled==1?'block':'none';
    $('localServerHost').value=settings.localServerHost||'192.168.1.100';
    $('localServerPort').value=settings.localServerPort||8080;
    loadStorageInfo();
    loadStoredClockfaces();
    buildRepoSelect();
    $('repoSource').onchange=buildRepoSelect;

    // System
    $('sys-name').value=settings.name||'ClockWise-XE1E';
    $('sys-version').value=settings.version||'1.0.0';
    $('ver').textContent=settings.version||'1.0.0';
  }catch(e){console.error(e);}
}

let brightTimeout=null;
function sendBright(val){
  clearTimeout(brightTimeout);
  brightTimeout=setTimeout(()=>{
    fetch('/api/set?displayBright='+val,{method:'POST'});
  },100);
}

async function saveField(key,val){
  await fetch('/api/set?'+key+'='+encodeURIComponent(val),{method:'POST'});
}

async function saveWifi(){
  await saveField('wifiSsid',$('wifiSsid').value);
  if($('wifiPwd').value)await saveField('wifiPwd',$('wifiPwd').value);
  await saveField('wifiSsid2',$('wifiSsid2').value);
  if($('wifiPwd2').value)await saveField('wifiPwd2',$('wifiPwd2').value);
  await saveField('wifiSsid3',$('wifiSsid3').value);
  if($('wifiPwd3').value)await saveField('wifiPwd3',$('wifiPwd3').value);
  toast('WiFi guardado. Reinicia para conectar.');
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
  const rot=$('rotationEnabled').checked;

  await saveField('clockfaceSource','stored');
  await saveField('rotationEnabled',rot?'1':'0');
  await saveField('rotationInterval',$('rotationInterval').value);

  if(rot){
    const list=$('rotationList').value;
    if(!list){toast('Selecciona al menos una caratula');return;}
    await saveField('canvasFile',list.split(',')[0]);
    await saveField('rotationList',list);
  }else{
    const cf=$('canvasFile').value;
    if(!cf){toast('Selecciona una caratula');return;}
    await saveField('canvasFile',cf);
    await saveField('rotationList','');
  }
  await fetch('/api/reload',{method:'POST'});
  toast('Caratula aplicada');
}

load();
setInterval(()=>{$('st-time').value=new Date().toLocaleTimeString();},1000);
</script>
</body>
</html>
)rawliteral";
