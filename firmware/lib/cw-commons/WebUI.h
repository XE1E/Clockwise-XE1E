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
      <label>Caratula nocturna</label>
      <input type="text" id="nightClock" placeholder="night-clock">
    </div>
    <button class="btn-primary" onclick="saveNight()">Guardar Nocturno</button>
  </div>

  <!-- CLOCKFACE -->
  <div id="clock" class="page">
    <div class="card">
      <h2>Caratula</h2>
      <label>Servidor</label>
      <input type="text" id="canvasServer" placeholder="raw.githubusercontent.com">
      <label>Archivo</label>
      <input type="text" id="canvasFile" placeholder="nombre-caratula">
    </div>
    <div class="card">
      <h2>Rotacion de caratulas</h2>
      <div class="check-row">
        <input type="checkbox" id="rotationEnabled">
        <label>Activar rotacion</label>
      </div>
      <label>Intervalo (minutos)</label>
      <input type="number" id="rotationInterval" min="1" max="1440" value="60">
      <label>Lista (separadas por coma)</label>
      <input type="text" id="rotationList" placeholder="clock1,clock2,clock3">
    </div>
    <button class="btn-primary" onclick="saveClock()">Guardar Caratula</button>
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
    $('nightClock').value=settings.nightClock||'';

    // Clock
    $('canvasServer').value=settings.canvasServer||'';
    $('canvasFile').value=settings.canvasFile||'';
    $('rotationEnabled').checked=settings.rotationEnabled==1;
    $('rotationInterval').value=settings.rotationInterval||60;
    $('rotationList').value=settings.rotationList||'';

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
  await saveField('nightClock',$('nightClock').value);
  toast('Modo nocturno guardado');
}

async function saveClock(){
  await saveField('canvasServer',$('canvasServer').value);
  await saveField('canvasFile',$('canvasFile').value);
  await saveField('rotationEnabled',$('rotationEnabled').checked?'1':'0');
  await saveField('rotationInterval',$('rotationInterval').value);
  await saveField('rotationList',$('rotationList').value);
  toast('Caratula guardada');
}

load();
setInterval(()=>{$('st-time').value=new Date().toLocaleTimeString();},1000);
</script>
</body>
</html>
)rawliteral";
