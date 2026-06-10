#pragma once

const char WEB_UI_MINIMAL_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html><head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Clockwise</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:sans-serif;font-size:14px;padding:10px;max-width:600px;margin:0 auto}
h1{font-size:18px;margin-bottom:10px}
h2{font-size:15px;margin:15px 0 8px;padding-top:10px;border-top:1px solid #ccc}
.tabs{display:flex;gap:5px;margin-bottom:15px;flex-wrap:wrap}
.tab{padding:8px 12px;border:1px solid #999;background:#eee;cursor:pointer}
.tab.active{background:#333;color:#fff}
.panel{display:none}
.panel.active{display:block}
label{display:block;margin:8px 0 3px;font-weight:bold}
input,select{width:100%;padding:6px;border:1px solid #ccc;margin-bottom:5px}
input[type=checkbox],input[type=radio]{width:auto}
input[type=range]{padding:0}
button{padding:8px 16px;margin:5px 0;cursor:pointer}
.btn-save{background:#1a73e8;color:#fff;border:none}
.btn-danger{background:#ea4335;color:#fff;border:none}
.row{display:flex;gap:10px}
.row>*{flex:1}
.info{background:#f5f5f5;padding:8px;margin:5px 0;font-size:12px}
.cf-grid{display:grid;grid-template-columns:repeat(auto-fill,minmax(80px,1fr));gap:8px;margin:10px 0}
.cf-item{border:2px solid #ccc;padding:5px;text-align:center;cursor:pointer;font-size:11px}
.cf-item.selected{border-color:#34a853;background:#e6f4ea}
.cf-item canvas{width:64px;height:64px;background:#000;display:block;margin:0 auto 4px}
.toast{position:fixed;bottom:20px;left:50%;transform:translateX(-50%);background:#333;color:#fff;padding:10px 20px;border-radius:4px;display:none}
#status{font-size:12px;color:#666;margin-bottom:10px}
</style>
</head>
<body>
<h1>Clockwise XE1E <small id="ver">v1.0</small></h1>
<div id="status">Cargando...</div>

<div class="tabs">
<button class="tab active" data-t="wifi">WiFi</button>
<button class="tab" data-t="display">Pantalla</button>
<button class="tab" data-t="time">Hora</button>
<button class="tab" data-t="canvas">Canvas</button>
<button class="tab" data-t="system">Sistema</button>
</div>

<!-- WiFi -->
<div class="panel active" id="p-wifi">
<h2>Red WiFi 1 (Principal)</h2>
<label>SSID</label><input id="wifiSsid">
<label>Password</label><input type="password" id="wifiPwd" placeholder="(sin cambios)">
<h2>Red WiFi 2</h2>
<label>SSID</label><input id="wifiSsid2">
<label>Password</label><input type="password" id="wifiPwd2" placeholder="(sin cambios)">
<h2>Red WiFi 3</h2>
<label>SSID</label><input id="wifiSsid3">
<label>Password</label><input type="password" id="wifiPwd3" placeholder="(sin cambios)">
<button class="btn-save" onclick="saveWifi()">Guardar WiFi</button>
</div>

<!-- Display -->
<div class="panel" id="p-display">
<label>Brillo: <span id="brightVal">32</span></label>
<input type="range" id="displayBright" min="1" max="255" value="32" oninput="$('brightVal').innerText=this.value;sendBright(this.value)">
<label>Rotacion</label>
<select id="displayRotation">
<option value="0">0°</option><option value="1">90°</option><option value="2">180°</option><option value="3">270°</option>
</select>
<label><input type="checkbox" id="swapBlueGreen"> Intercambiar Blue/Green (requiere reinicio)</label>
<h2>Brillo Automatico (LDR)</h2>
<div class="row">
<div><label>Min</label><input type="number" id="autoBrightMin" min="0" max="4095"></div>
<div><label>Max</label><input type="number" id="autoBrightMax" min="0" max="4095"></div>
</div>
<label>GPIO Pin LDR</label>
<div class="row">
<input type="number" id="ldrPin" value="35">
<button onclick="readLdr()">Leer</button>
</div>
<div class="info">Valor LDR: <span id="ldrVal">--</span></div>
<button class="btn-save" onclick="saveDisplay()">Guardar Pantalla</button>
</div>

<!-- Time -->
<div class="panel" id="p-time">
<label>Zona Horaria</label>
<select id="timeZone">
<option value="CST6">Mexico Centro (UTC-6)</option>
<option value="MST7">Mexico Pacifico (UTC-7)</option>
<option value="EST5">Este USA (UTC-5)</option>
<option value="GMT0">UTC/GMT</option>
<option value="CET-1">Europa Central (UTC+1)</option>
</select>
<label>Servidor NTP</label>
<select id="ntpServer">
<option value="time.cloudflare.com">Cloudflare</option>
<option value="time.google.com">Google</option>
<option value="pool.ntp.org">NTP Pool</option>
</select>
<label><input type="checkbox" id="use24hFormat"> Formato 24 horas</label>
<label><input type="checkbox" id="useSpanish"> Usar español</label>
<label>Posix manual (opcional)</label>
<input id="manualPosix" placeholder="CST6CDT,M3.2.0,M11.1.0">
<button class="btn-save" onclick="saveTime()">Guardar Hora</button>
</div>

<!-- Canvas -->
<div class="panel" id="p-canvas">
<div class="info" id="storageInfo">Almacenamiento: cargando...</div>
<label><input type="checkbox" id="rotationEnabled" onchange="toggleRot()"> Rotacion automatica cada <input type="number" id="rotationInterval" value="60" style="width:60px"> min</label>
<h2>Caratulas en el Reloj</h2>
<div class="cf-grid" id="cfList">Cargando...</div>
<input type="hidden" id="canvasFile">
<input type="hidden" id="rotationList">

<h2>Subir Caratula</h2>
<input type="file" id="cfFile" accept=".json">
<button class="btn-save" onclick="uploadCf()">Subir</button>

<h2>Descargar del Repositorio</h2>
<select id="repoCf"></select>
<button class="btn-save" onclick="downloadCf()">Descargar</button>

<h2>Modo Nocturno</h2>
<label><input type="checkbox" id="nightEnabled" onchange="toggleNight()"> Activar</label>
<div id="nightCfg" style="display:none">
<div class="row">
<div><label>Inicio</label><input type="time" id="nightStart" value="22:00"></div>
<div><label>Fin</label><input type="time" id="nightEnd" value="07:00"></div>
</div>
<label>Brillo: <span id="nightBrightVal">8</span></label>
<input type="range" id="nightBright" min="1" max="32" value="8" oninput="$('nightBrightVal').innerText=this.value">
<label>Caratula nocturna</label>
<select id="nightClockSelect" onchange="$('nightClock').value=this.value"><option value="_builtin">Reloj integrado</option></select>
<input type="hidden" id="nightClock" value="_builtin">
<label>Color</label>
<input type="color" id="nightColor" value="#ff0000">
</div>
<button class="btn-save" onclick="saveCanvas()">Aplicar Cambios</button>
</div>

<!-- System -->
<div class="panel" id="p-system">
<div class="info">
<b>Firmware:</b> <span id="sysName">ClockWise-XE1E</span><br>
<b>Version:</b> <span id="sysVer">1.0.0</span><br>
<b>MAC:</b> <span id="sysMac">--</span><br>
<b>RAM:</b> <span id="sysRam">--</span><br>
<b>Uptime:</b> <span id="uptime">--</span>
</div>
<button class="btn-save" onclick="restart()">Reiniciar</button>
<button class="btn-danger" onclick="if(confirm('Borrar config WiFi?'))resetWifi()">Borrar WiFi</button>
<button class="btn-danger" onclick="if(confirm('RESET FABRICA?'))resetAll()">Reset Fabrica</button>
<h2>OTA</h2>
<input type="file" id="otaFile" accept=".bin">
<button onclick="alert('No habilitado')">Subir Firmware</button>
<h2>Enlaces</h2>
<a href="https://xe1e.github.io/Clockwise-XE1E/clockface-editor/" target="_blank">Editor de Caratulas</a>
</div>

<div class="toast" id="toast">OK</div>

<script>
const $=id=>document.getElementById(id);
let cfg={},cfs=[],thumbs={};

document.querySelectorAll('.tab').forEach(t=>t.onclick=()=>{
document.querySelectorAll('.tab').forEach(x=>x.classList.remove('active'));
document.querySelectorAll('.panel').forEach(x=>x.classList.remove('active'));
t.classList.add('active');
$('p-'+t.dataset.t).classList.add('active');
});

function toast(m){const t=$('toast');t.innerText=m;t.style.display='block';setTimeout(()=>t.style.display='none',2000);}
async function api(a,p={}){const q=Object.entries(p).map(([k,v])=>k+'='+encodeURIComponent(v)).join('&');return fetch('/api/'+a+(q?'?'+q:''),{method:'POST'});}
function restart(){api('restart');toast('Reiniciando...');}
function resetWifi(){api('resetwifi');toast('WiFi borrado');}
function resetAll(){api('reset');toast('Reseteando...');}

function rgb565ToHex(v){const r=((v>>11)&0x1F)*255/31,g=((v>>5)&0x3F)*255/63,b=(v&0x1F)*255/31;return '#'+[r,g,b].map(x=>Math.round(x).toString(16).padStart(2,'0')).join('');}
function hexToRgb565(h){const r=parseInt(h.slice(1,3),16),g=parseInt(h.slice(3,5),16),b=parseInt(h.slice(5,7),16);return ((r>>3)<<11)|((g>>2)<<5)|(b>>3);}

let brightT;
function sendBright(v){clearTimeout(brightT);brightT=setTimeout(()=>fetch('/api/set?displayBright='+v,{method:'POST'}),150);}

async function saveField(k,v){await fetch('/api/set?'+k+'='+encodeURIComponent(v),{method:'POST'});}

async function saveWifi(){
await saveField('wifiSsid',$('wifiSsid').value);
if($('wifiPwd').value)await saveField('wifiPwd',$('wifiPwd').value);
await saveField('wifiSsid2',$('wifiSsid2').value);
if($('wifiPwd2').value)await saveField('wifiPwd2',$('wifiPwd2').value);
await saveField('wifiSsid3',$('wifiSsid3').value);
if($('wifiPwd3').value)await saveField('wifiPwd3',$('wifiPwd3').value);
toast('WiFi guardado');
}

async function saveDisplay(){
await saveField('displayBright',$('displayBright').value);
await saveField('displayRotation',$('displayRotation').value);
await saveField('swapBlueGreen',$('swapBlueGreen').checked?'1':'0');
await saveField('autoBrightMin',$('autoBrightMin').value);
await saveField('autoBrightMax',$('autoBrightMax').value);
await saveField('ldrPin',$('ldrPin').value);
toast('Pantalla guardada');
}

async function readLdr(){
try{const r=await fetch('/api/pin?pin='+$('ldrPin').value);const d=await r.json();$('ldrVal').innerText=d.value;}catch(e){$('ldrVal').innerText='Error';}
}

async function saveTime(){
await saveField('timeZone',$('timeZone').value);
await saveField('ntpServer',$('ntpServer').value);
await saveField('use24hFormat',$('use24hFormat').checked?'1':'0');
await saveField('useSpanish',$('useSpanish').checked?'1':'0');
await saveField('manualPosix',$('manualPosix').value);
toast('Hora guardada');
}

function toggleRot(){renderCfGrid();}
function toggleNight(){$('nightCfg').style.display=$('nightEnabled').checked?'block':'none';}

async function loadStorage(){
try{const r=await fetch('/api/storage');const d=await r.json();
$('storageInfo').innerText='Almacenamiento: '+Math.round(d.used/1024)+'KB / '+Math.round(d.total/1024)+'KB';}catch(e){}
}

async function loadCfs(){
try{const r=await fetch('/api/clockfaces/list');cfs=await r.json();renderCfGrid();buildNightSel();}catch(e){$('cfList').innerHTML='Error';}
}

function renderCfGrid(){
const sel=($('rotationList').value||'').split(',').filter(s=>s);
const validSel=sel.filter(n=>cfs.some(c=>c.name===n));
if(validSel.length!==sel.length)$('rotationList').value=validSel.join(',');
if(!cfs.length){$('cfList').innerHTML='No hay caratulas';return;}
$('cfList').innerHTML=cfs.map(c=>'<div class="cf-item'+(validSel.includes(c.name)?' selected':'')+'" data-n="'+c.name+'" onclick="toggleCf(\''+c.name+'\')"><canvas width="64" height="64"></canvas>'+c.name+'</div>').join('');
cfs.forEach(c=>{if(thumbs[c.name])drawThumb(c.name);});
const pending=cfs.filter(c=>!thumbs[c.name]).map(c=>c.name);
if(pending.length)loadThumbsSeq(pending);
}

async function loadThumbsSeq(names){
for(let i=0;i<names.length;i++){await loadThumb(names[i]);}
}

async function loadThumb(n){
try{const r=await fetch('/api/clockfaces/thumb?name='+encodeURIComponent(n));if(r.ok){const d=await r.json();thumbs[n]=d;drawThumb(n);}}catch(e){}
}

function drawThumb(n){
const el=document.querySelector('.cf-item[data-n="'+n+'"] canvas');if(!el)return;
const ctx=el.getContext('2d');ctx.fillStyle=rgb565ToHex(thumbs[n].bg||0);ctx.fillRect(0,0,64,64);
if(thumbs[n].img){const img=new Image();img.onload=()=>{ctx.imageSmoothingEnabled=false;ctx.drawImage(img,0,0,64,64);};img.src='data:image/png;base64,'+thumbs[n].img;}
}

function toggleCf(n){
const sel=($('rotationList').value||'').split(',').filter(s=>s);
const i=sel.indexOf(n);if(i>=0)sel.splice(i,1);else sel.push(n);
$('rotationList').value=sel.join(',');
if(sel.length)$('canvasFile').value=sel[0];
renderCfGrid();
}

function buildNightSel(){
const s=$('nightClockSelect'),cur=$('nightClock').value||'_builtin';
s.innerHTML='<option value="_builtin">Reloj integrado</option>'+cfs.map(c=>'<option value="'+c.name+'"'+(c.name===cur?' selected':'')+'>'+c.name+'</option>').join('');
}

async function uploadCf(){
const f=$('cfFile').files[0];if(!f){toast('Selecciona archivo');return;}
const n=f.name.replace('.json','');
try{const t=await f.text();JSON.parse(t);
const r=await fetch('/api/clockfaces/upload?name='+encodeURIComponent(n),{method:'POST',headers:{'Content-Type':'application/json'},body:t});
if(r.ok){toast('Subido');$('cfFile').value='';loadStorage();loadCfs();}else toast('Error');}catch(e){toast('JSON invalido');}
}

const cfRepo=['pac-man','nyan-cat','night-clock','donkey-kong','star-wars','goomba_move','clock-club','retro-computer','snoopy3'];
$('repoCf').innerHTML=cfRepo.map(c=>'<option>'+c+'</option>').join('');

async function downloadCf(){
const n=$('repoCf').value;if(!n)return;
const url='https://xe1e.github.io/Clockwise-XE1E/clockface-editor/clockfaces/'+n+'.json';
try{const r=await fetch(url);if(!r.ok)throw new Error();const j=await r.text();
const u=await fetch('/api/clockfaces/upload?name='+encodeURIComponent(n),{method:'POST',headers:{'Content-Type':'application/json'},body:j});
if(u.ok){toast('Descargado');loadStorage();loadCfs();}else toast('Error');}catch(e){toast('Error: '+e);}
}

async function saveCanvas(){
const rot=$('rotationEnabled').checked;
const list=$('rotationList').value;
if(rot&&!list){toast('Selecciona caratula');return;}
if(!rot&&!$('canvasFile').value){toast('Selecciona caratula');return;}
await saveField('rotationEnabled',rot?'1':'0');
await saveField('rotationInterval',$('rotationInterval').value);
await saveField('canvasFile',rot?list.split(',')[0]:$('canvasFile').value);
await saveField('rotationList',rot?list:'');
await saveField('nightEnabled',$('nightEnabled').checked?'1':'0');
await saveField('nightStart',$('nightStart').value);
await saveField('nightEnd',$('nightEnd').value);
await saveField('nightBright',$('nightBright').value);
await saveField('nightColor',hexToRgb565($('nightColor').value));
await saveField('nightClock',$('nightClock').value);
await fetch('/api/reload',{method:'POST'});
toast('Aplicado');
}

let uptimeStart=0;
async function loadSys(){
try{const r=await fetch('/api/system');const d=await r.json();
$('sysRam').innerText=Math.round((d.totalHeap-d.freeHeap)/1024)+'/'+Math.round(d.totalHeap/1024)+'KB';
if(d.mac)$('sysMac').innerText=d.mac;
uptimeStart=Date.now()-d.uptimeMs;}catch(e){}
}

function updUptime(){
const s=Math.floor((Date.now()-uptimeStart)/1000);
const h=Math.floor(s/3600),m=Math.floor((s%3600)/60);
$('uptime').innerText=(h?h+'h ':'')+(m?m+'m ':'')+s%60+'s';
}
setInterval(updUptime,1000);

async function load(){
try{
const r=await fetch('/api/settings');cfg=await r.json();
$('status').innerText='WiFi: '+(cfg.wifiConnected||'-')+' | IP: '+(cfg.wifiIp||location.hostname);
$('ver').innerText='v'+(cfg.version||'1.0');
$('wifiSsid').value=cfg.wifiSsid||'';
$('wifiSsid2').value=cfg.wifiSsid2||'';
$('wifiSsid3').value=cfg.wifiSsid3||'';
$('displayBright').value=cfg.displayBright||32;
$('brightVal').innerText=cfg.displayBright||32;
$('displayRotation').value=cfg.displayRotation||0;
$('swapBlueGreen').checked=cfg.swapBlueGreen==1;
$('autoBrightMin').value=cfg.autoBrightMin||0;
$('autoBrightMax').value=cfg.autoBrightMax||0;
$('ldrPin').value=cfg.ldrPin||35;
$('timeZone').value=cfg.timeZone||'CST6';
$('ntpServer').value=cfg.ntpServer||'time.cloudflare.com';
$('use24hFormat').checked=cfg.use24hFormat==1;
$('useSpanish').checked=cfg.useSpanish==1;
$('manualPosix').value=cfg.manualPosix||'';
$('canvasFile').value=cfg.canvasFile||'';
$('rotationEnabled').checked=cfg.rotationEnabled==1;
$('rotationInterval').value=cfg.rotationInterval||60;
$('rotationList').value=cfg.rotationList||'';
$('nightEnabled').checked=cfg.nightEnabled==1;
$('nightStart').value=cfg.nightStart||'22:00';
$('nightEnd').value=cfg.nightEnd||'07:00';
$('nightBright').value=cfg.nightBright||8;
$('nightBrightVal').innerText=cfg.nightBright||8;
$('nightColor').value=rgb565ToHex(cfg.nightColor||63488);
$('nightClock').value=cfg.nightClock||'_builtin';
toggleNight();
$('sysName').innerText=cfg.name||'ClockWise-XE1E';
$('sysVer').innerText=cfg.version||'1.0.0';
loadStorage();loadCfs();loadSys();
}catch(e){$('status').innerText='Error cargando';}
}
load();
</script>
</body></html>
)rawliteral";
