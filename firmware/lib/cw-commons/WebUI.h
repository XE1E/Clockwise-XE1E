#pragma once

const char WEB_UI_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Clockwise - Configuracion</title>
    <style>
        :root {
            --primary: #1a73e8;
            --primary-dark: #1557b0;
            --success: #34a853;
            --warning: #fbbc04;
            --danger: #ea4335;
            --bg: #f8f9fa;
            --card-bg: #ffffff;
            --text: #202124;
            --text-secondary: #5f6368;
            --border: #dadce0;
        }
        * { box-sizing: border-box; margin: 0; padding: 0; }
        body { font-family: 'Segoe UI', Roboto, Arial, sans-serif; background: var(--bg); color: var(--text); line-height: 1.5; }
        .sticky-top { position: sticky; top: 0; z-index: 100; }
        .header { background: linear-gradient(135deg, #1a73e8, #34a853); color: white; padding: 20px; text-align: center; }
        .header h1 { font-size: 24px; font-weight: 500; margin-bottom: 5px; }
        .header .version { font-size: 12px; opacity: 0.8; }
        .toolbar { background: #333; color: white; padding: 10px 20px; display: flex; justify-content: space-between; align-items: center; flex-wrap: wrap; gap: 10px; }
        .toolbar-info { display: flex; gap: 20px; font-size: 14px; }
        .toolbar-info span { display: flex; align-items: center; gap: 5px; }
        .btn { padding: 8px 16px; border: none; border-radius: 4px; cursor: pointer; font-size: 14px; font-weight: 500; transition: all 0.2s; }
        .btn-restart { background: var(--danger); color: white; }
        .btn-restart:hover { background: #c5221f; }
        .btn-save, .btn-primary { background: var(--primary); color: white; }
        .btn-save:hover, .btn-primary:hover { background: var(--primary-dark); }
        .btn-warning { background: #ff9800 !important; animation: pulse 1.5s infinite; }
        @keyframes pulse { 0%,100%{opacity:1} 50%{opacity:0.7} }
        .btn-secondary { background: #e8e8e8; color: var(--text); }
        .btn-secondary:hover { background: #d0d0d0; }
        .container { max-width: 1200px; margin: 0 auto; padding: 20px; }
        .tabs-container { background: var(--bg); padding-top: 10px; }
        .tabs { display: flex; gap: 5px; margin-bottom: 20px; border-bottom: 2px solid var(--border); padding-bottom: 0; flex-wrap: wrap; }
        .tab { padding: 12px 24px; background: none; border: none; cursor: pointer; font-size: 14px; font-weight: 500; color: var(--text-secondary); border-bottom: 3px solid transparent; margin-bottom: -2px; transition: all 0.2s; }
        .tab:hover { color: var(--primary); }
        .tab.active { color: var(--primary); border-bottom-color: var(--primary); }
        .tab-content { display: none; }
        .tab-content.active { display: block; }
        .section-title { font-size: 18px; font-weight: 500; margin-bottom: 15px; color: var(--text); display: flex; align-items: center; gap: 10px; }
        .section-title svg { width: 24px; height: 24px; fill: var(--primary); }
        .cards { display: grid; grid-template-columns: repeat(auto-fill, minmax(350px, 1fr)); gap: 20px; }
        .card { background: var(--card-bg); border-radius: 8px; box-shadow: 0 1px 3px rgba(0,0,0,0.1); overflow: hidden; }
        .card-header { padding: 15px 20px; background: #f1f3f4; border-bottom: 1px solid var(--border); display: flex; align-items: center; gap: 10px; }
        .card-header h3 { font-size: 16px; font-weight: 500; }
        .card-header svg { width: 20px; height: 20px; fill: var(--text-secondary); }
        .card-body { padding: 20px; }
        .card-description { font-size: 13px; color: var(--text-secondary); margin-bottom: 15px; }
        .form-group { margin-bottom: 15px; }
        .form-group:last-child { margin-bottom: 0; }
        .form-label { display: block; font-size: 13px; font-weight: 500; margin-bottom: 5px; color: var(--text); }
        .form-input { width: 100%; padding: 10px 12px; border: 1px solid var(--border); border-radius: 4px; font-size: 14px; transition: border-color 0.2s; }
        .form-input:focus { outline: none; border-color: var(--primary); }
        .form-select { width: 100%; padding: 10px 12px; border: 1px solid var(--border); border-radius: 4px; font-size: 14px; background: white; cursor: pointer; }
        .form-checkbox { display: flex; align-items: center; gap: 10px; }
        .form-checkbox input { width: 18px; height: 18px; cursor: pointer; accent-color: var(--primary); }
        .form-checkbox label { cursor: pointer; }
        .input-row { display: flex; gap: 10px; }
        .input-row .form-input, .input-row .form-group { flex: 1; }
        .range-value { font-weight: 600; color: var(--primary); }
        .wifi-network { background: #f8f9fa; border: 1px solid var(--border); border-radius: 8px; padding: 15px; margin-bottom: 15px; }
        .wifi-network-header { display: flex; justify-content: space-between; align-items: center; margin-bottom: 10px; }
        .wifi-network-title { font-weight: 500; font-size: 14px; }
        .wifi-priority { font-size: 12px; color: var(--text-secondary); background: #e8f0fe; padding: 2px 8px; border-radius: 10px; }
        .storage-bar { height: 8px; background: #e0e0e0; border-radius: 4px; overflow: hidden; margin: 10px 0; }
        .storage-bar-fill { height: 100%; background: var(--primary); transition: width 0.3s; }
        .clockface-grid { display: grid; grid-template-columns: repeat(auto-fill, minmax(100px, 1fr)); gap: 10px; margin-top: 10px; }
        .clockface-item { border: 2px solid var(--border); border-radius: 8px; padding: 8px; text-align: center; cursor: pointer; transition: all 0.2s; background: white; }
        .clockface-item:hover { border-color: var(--primary); }
        .clockface-item.selected { border-color: var(--success); background: #e6f4ea; }
        .clockface-item[draggable="true"] { cursor: grab; }
        .clockface-item.dragging { opacity: 0.5; }
        .clockface-item canvas, .clockface-item .thumb { width: 64px; height: 64px; background: #000; margin: 0 auto 6px; border-radius: 4px; display: block; image-rendering: pixelated; }
        .clockface-item .name { font-size: 11px; font-weight: 500; overflow: hidden; text-overflow: ellipsis; white-space: nowrap; }
        .clockface-item .size { font-size: 10px; color: var(--text-secondary); }
        .clockface-item .order-num { position: absolute; top: 4px; left: 4px; background: var(--success); color: white; width: 20px; height: 20px; border-radius: 50%; font-size: 11px; font-weight: bold; display: flex; align-items: center; justify-content: center; }
        .clockface-item { position: relative; }
        .delete-btn { position: absolute; top: 4px; right: 4px; background: var(--danger); color: white; border: none; width: 20px; height: 20px; border-radius: 50%; cursor: pointer; font-size: 12px; display: none; }
        .clockface-item:hover .delete-btn { display: flex; align-items: center; justify-content: center; }
        .night-preview { width: 100%; min-width: 180px; background: #000; border-radius: 8px; display: flex; align-items: center; justify-content: center; }
        .night-preview .time { font-family: 'Segoe UI', sans-serif; font-weight: 300; letter-spacing: 3px; }
        .color-grid { display: flex; gap: 6px; flex-wrap: wrap; margin-top: 8px; }
        .color-btn { width: 32px; height: 32px; border: 2px solid transparent; border-radius: 4px; cursor: pointer; padding: 0; }
        .color-btn:hover { border-color: var(--primary); }
        .color-btn.active { border-color: var(--text); }
        .toast { position: fixed; bottom: 20px; left: 50%; transform: translateX(-50%); background: var(--success); color: white; padding: 12px 24px; border-radius: 8px; font-weight: 500; z-index: 1000; display: none; box-shadow: 0 4px 12px rgba(0,0,0,0.2); }
        @media (max-width: 600px) { .cards { grid-template-columns: 1fr; } .tabs { overflow-x: auto; } .tab { padding: 10px 15px; white-space: nowrap; } }
    </style>
</head>
<body>
<div class="sticky-top">
    <div class="header">
        <h1>Clockwise XE1E</h1>
        <div class="version">Firmware v<span id="fw-version">1.0.0</span></div>
    </div>
    <div class="toolbar">
        <div class="toolbar-info">
            <span>WiFi: <strong id="current-ssid">-</strong> <small id="wifi-rssi"></small></span>
            <span>IP: <strong id="ip-address">-</strong></span>
            <span>RAM: <strong id="ram-info">-</strong></span>
            <span>SPIFFS: <strong id="spiffs-info">-</strong></span>
            <span>Uptime: <strong id="uptime">-</strong></span>
        </div>
        <div style="display: flex; gap: 10px;">
            <button class="btn btn-restart" onclick="restartDevice()">Reiniciar</button>
        </div>
    </div>
    <div class="tabs-container">
        <div class="tabs" style="max-width:1200px;margin:0 auto;padding:0 20px">
            <button class="tab active" data-tab="wifi">WiFi</button>
            <button class="tab" data-tab="display">Pantalla</button>
            <button class="tab" data-tab="time">Hora y Fecha</button>
            <button class="tab" data-tab="canvas">Canvas</button>
            <button class="tab" data-tab="system">Sistema</button>
        </div>
    </div>
</div>

<div class="container">

    <!-- WiFi Tab -->
    <div class="tab-content active" id="tab-wifi">
        <div class="section-title">
            <svg viewBox="0 0 24 24"><path d="M12 3C7.5 3 3.75 4.95 1 8l1.5 1.5C4.75 7 8.25 5.5 12 5.5s7.25 1.5 9.5 4L23 8c-2.75-3.05-6.5-5-11-5zm0 5c-3 0-5.75 1.35-7.5 3.5L6 13c1.35-1.65 3.4-2.75 6-2.75s4.65 1.1 6 2.75l1.5-1.5C17.75 9.35 15 8 12 8zm0 5c-1.85 0-3.35.85-4.5 2L12 19l4.5-4c-1.15-1.15-2.65-2-4.5-2z"/></svg>
            Configuracion WiFi
        </div>
        <div class="wifi-network">
            <div class="wifi-network-header">
                <span class="wifi-network-title">Red WiFi 1</span>
                <span class="wifi-priority">Principal</span>
            </div>
            <div class="input-row">
                <div class="form-group"><label class="form-label">SSID</label><input type="text" class="form-input" id="wifiSsid"></div>
                <div class="form-group"><label class="form-label">Password</label><input type="password" class="form-input" id="wifiPwd" placeholder="(sin cambios)"></div>
            </div>
        </div>
        <div class="wifi-network">
            <div class="wifi-network-header">
                <span class="wifi-network-title">Red WiFi 2</span>
                <span class="wifi-priority">Respaldo</span>
            </div>
            <div class="input-row">
                <div class="form-group"><label class="form-label">SSID</label><input type="text" class="form-input" id="wifiSsid2"></div>
                <div class="form-group"><label class="form-label">Password</label><input type="password" class="form-input" id="wifiPwd2" placeholder="(sin cambios)"></div>
            </div>
        </div>
        <div class="wifi-network">
            <div class="wifi-network-header">
                <span class="wifi-network-title">Red WiFi 3</span>
                <span class="wifi-priority">Respaldo</span>
            </div>
            <div class="input-row">
                <div class="form-group"><label class="form-label">SSID</label><input type="text" class="form-input" id="wifiSsid3"></div>
                <div class="form-group"><label class="form-label">Password</label><input type="password" class="form-input" id="wifiPwd3" placeholder="(sin cambios)"></div>
            </div>
        </div>
        <button class="btn btn-save" onclick="saveWifi()" style="width:100%">Guardar WiFi</button>
    </div>

    <!-- Display Tab -->
    <div class="tab-content" id="tab-display">
        <div class="section-title">
            <svg viewBox="0 0 24 24"><path d="M20 3H4c-1.1 0-2 .9-2 2v11c0 1.1.9 2 2 2h3l-1 1v2h12v-2l-1-1h3c1.1 0 2-.9 2-2V5c0-1.1-.9-2-2-2zm0 13H4V5h16v11z"/></svg>
            Configuracion de Pantalla
        </div>
        <div class="cards">
            <div class="card">
                <div class="card-header"><svg viewBox="0 0 24 24" width="20" height="20" fill="var(--text-secondary)"><path d="M20 15.31L23.31 12 20 8.69V4h-4.69L12 .69 8.69 4H4v4.69L.69 12 4 15.31V20h4.69L12 23.31 15.31 20H20v-4.69zM12 18c-3.31 0-6-2.69-6-6s2.69-6 6-6 6 2.69 6 6-2.69 6-6 6z"/></svg><h3>Brillo</h3></div>
                <div class="card-body">
                    <div class="form-group">
                        <label class="form-label">Valor: <span class="range-value" id="brightVal">32</span></label>
                        <input type="range" class="form-input" id="displayBright" min="1" max="255" value="32" oninput="$('brightVal').textContent=this.value;sendBright(this.value)">
                    </div>
                </div>
            </div>
            <div class="card">
                <div class="card-header"><svg viewBox="0 0 24 24" width="20" height="20" fill="var(--text-secondary)"><path d="M7.34 6.41L.86 12.9l6.49 6.48 6.49-6.48-6.5-6.49zM3.69 12.9l3.66-3.66L11 12.9l-3.66 3.66-3.65-3.66zm15.67-6H11.6l2 2h5.76l-2.38 5.76c-.18.44.37.85.76.56l4.67-3.54-3.05-4.78z"/></svg><h3>Rotacion</h3></div>
                <div class="card-body">
                    <select class="form-select" id="displayRotation" onchange="updateRotationPreview()">
                        <option value="0">0 grados (normal)</option>
                        <option value="1">90 grados</option>
                        <option value="2">180 grados</option>
                        <option value="3">270 grados</option>
                    </select>
                    <div style="display:flex;justify-content:center;margin-top:12px">
                        <div id="rotation-preview" style="width:48px;height:48px;background:#000;border:2px solid var(--border);border-radius:4px;display:flex;align-items:center;justify-content:center;transition:transform 0.3s">
                            <span style="color:#0f0;font-family:monospace;font-size:12px">12:00</span>
                        </div>
                    </div>
                </div>
            </div>
            <div class="card">
                <div class="card-header"><svg viewBox="0 0 24 24" width="20" height="20" fill="var(--text-secondary)"><path d="M7.52 21.48A10.487 10.487 0 011.55 13H.05C.56 19.16 5.71 24 12 24l.66-.03-3.81-3.81-1.33 1.32zm.89-6.52c-.19 0-.37-.03-.52-.08-.16-.06-.29-.13-.4-.24-.11-.1-.2-.22-.26-.37-.06-.14-.09-.3-.09-.47h-1.3c0 .36.07.68.21.95s.33.5.56.69c.24.18.51.32.82.41.3.1.62.15.96.15.37 0 .72-.05 1.03-.15.32-.1.6-.25.83-.44s.42-.43.55-.72.2-.61.2-.97c0-.19-.02-.38-.07-.56-.05-.18-.12-.35-.23-.51-.1-.16-.24-.3-.4-.43-.17-.13-.37-.23-.61-.31.2-.09.37-.2.52-.33.15-.13.27-.27.37-.42.1-.15.17-.3.22-.46.05-.16.07-.32.07-.48 0-.36-.06-.68-.18-.96-.12-.28-.29-.51-.51-.69-.2-.19-.47-.33-.77-.43C9.1 8.05 8.76 8 8.39 8c-.36 0-.69.05-1 .16-.3.11-.57.26-.79.45-.21.19-.38.41-.51.67-.12.26-.18.54-.18.85h1.3c0-.17.03-.32.09-.45s.14-.25.25-.34c.11-.09.23-.17.38-.22.15-.05.3-.08.48-.08.4 0 .7.1.89.31.19.2.29.49.29.86 0 .18-.03.34-.08.49-.05.15-.14.27-.25.37-.11.1-.25.18-.41.24-.16.06-.36.09-.58.09H7.5v1.03h.77c.22 0 .42.02.6.07s.33.13.45.23c.12.11.22.24.29.4.07.16.1.35.1.57 0 .41-.12.72-.35.93-.23.23-.55.33-.95.33zm8.55-5.92c-.32-.33-.7-.59-1.14-.77-.43-.18-.92-.27-1.46-.27H12v8h2.3c.55 0 1.06-.09 1.51-.27.45-.18.84-.43 1.16-.76.32-.33.57-.73.74-1.19.17-.47.26-.99.26-1.57v-.4c0-.58-.09-1.1-.26-1.57-.18-.47-.43-.87-.75-1.2zm-.39 3.16c0 .42-.05.79-.14 1.13-.1.33-.24.62-.43.85-.19.23-.43.41-.71.53-.29.12-.62.18-.99.18h-.91V9.12h.97c.72 0 1.27.23 1.64.69.38.46.57 1.12.57 1.99v.4zM12 0l-.66.03 3.81 3.81 1.33-1.33c3.27 1.55 5.61 4.72 5.96 8.48h1.5C23.44 4.84 18.29 0 12 0z"/></svg><h3>Panel RGB/RBG</h3></div>
                <div class="card-body">
                    <div class="form-checkbox">
                        <input type="checkbox" id="swapBlueGreen" onchange="updateColorSwapPreview()">
                        <label for="swapBlueGreen">Intercambiar Blue/Green</label>
                    </div>
                    <div style="display:flex;gap:8px;margin-top:12px;justify-content:center">
                        <div style="text-align:center"><div style="width:24px;height:24px;background:#f00;border-radius:4px"></div><small>R</small></div>
                        <div style="text-align:center"><div id="color-g" style="width:24px;height:24px;background:#0f0;border-radius:4px;transition:background 0.3s"></div><small id="label-g">G</small></div>
                        <div style="text-align:center"><div id="color-b" style="width:24px;height:24px;background:#00f;border-radius:4px;transition:background 0.3s"></div><small id="label-b">B</small></div>
                    </div>
                    <p class="card-description" style="margin-top:8px;color:#f90">Requiere reinicio para aplicar</p>
                </div>
            </div>
            <div class="card">
                <div class="card-header"><svg viewBox="0 0 24 24" width="20" height="20" fill="var(--text-secondary)"><path d="M6.76 4.84l-1.8-1.79-1.41 1.41 1.79 1.79 1.42-1.41zM4 10.5H1v2h3v-2zm9-9.95h-2V3.5h2V.55zm7.45 3.91l-1.41-1.41-1.79 1.79 1.41 1.41 1.79-1.79zm-3.21 13.7l1.79 1.8 1.41-1.41-1.8-1.79-1.4 1.4zM20 10.5v2h3v-2h-3zm-8-5c-3.31 0-6 2.69-6 6s2.69 6 6 6 6-2.69 6-6-2.69-6-6-6zm-1 16.95h2V19.5h-2v2.95zm-7.45-3.91l1.41 1.41 1.79-1.8-1.41-1.41-1.79 1.8z"/></svg><h3>Brillo Automatico (LDR)</h3></div>
                <div class="card-body">
                    <p class="card-description">Valores del sensor LDR (0-4095). Max en 0 = desactivado.</p>
                    <div class="input-row">
                        <div class="form-group"><label class="form-label">Min</label><input type="number" class="form-input" id="autoBrightMin" min="0" max="4095"></div>
                        <div class="form-group"><label class="form-label">Max</label><input type="number" class="form-input" id="autoBrightMax" min="0" max="4095"></div>
                    </div>
                </div>
            </div>
            <div class="card">
                <div class="card-header"><svg viewBox="0 0 24 24" width="20" height="20" fill="var(--text-secondary)"><path d="M13 2.05v2.02c3.95.49 7 3.85 7 7.93 0 3.21-1.92 6-4.72 7.28L13 17v5h5l-1.22-1.22C19.91 19.07 22 15.76 22 12c0-5.18-3.95-9.45-9-9.95zM11 2.05C5.94 2.55 2 6.81 2 12c0 3.76 2.09 7.07 5.22 8.78L6 22h5v-5l-2.28 2.28C5.92 18 4 15.21 4 12c0-4.08 3.05-7.44 7-7.93V2.05z"/></svg><h3>Pin LDR</h3></div>
                <div class="card-body">
                    <p class="card-description">Puerto GPIO donde esta conectado el sensor de luz</p>
                    <div class="form-group">
                        <label class="form-label">GPIO Pin</label>
                        <div class="input-row">
                            <input type="number" class="form-input" id="ldrPin" min="0" max="39" value="35">
                            <button class="btn btn-primary" onclick="readLdrPin()">Leer</button>
                        </div>
                        <div style="margin-top:8px;font-size:13px">Valor actual: <strong id="ldrValue">--</strong></div>
                    </div>
                </div>
            </div>
        </div>
        <button class="btn btn-save" onclick="saveDisplay()" style="width:100%;margin-top:20px">Guardar Pantalla</button>
    </div>

    <!-- Time Tab -->
    <div class="tab-content" id="tab-time">
        <div class="section-title">
            <svg viewBox="0 0 24 24"><path d="M11.99 2C6.47 2 2 6.48 2 12s4.47 10 9.99 10C17.52 22 22 17.52 22 12S17.52 2 11.99 2zM12 20c-4.42 0-8-3.58-8-8s3.58-8 8-8 8 3.58 8 8-3.58 8-8 8zm.5-13H11v6l5.25 3.15.75-1.23-4.5-2.67z"/></svg>
            Hora y Fecha
        </div>
        <div class="cards">
            <div class="card">
                <div class="card-header"><svg viewBox="0 0 24 24" width="20" height="20" fill="var(--text-secondary)"><path d="M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm-1 17.93c-3.95-.49-7-3.85-7-7.93 0-.62.08-1.21.21-1.79L9 15v1c0 1.1.9 2 2 2v1.93zm6.9-2.54c-.26-.81-1-1.39-1.9-1.39h-1v-3c0-.55-.45-1-1-1H8v-2h2c.55 0 1-.45 1-1V7h2c1.1 0 2-.9 2-2v-.41c2.93 1.19 5 4.06 5 7.41 0 2.08-.8 3.97-2.1 5.39z"/></svg><h3>Zona Horaria</h3></div>
                <div class="card-body">
                    <select class="form-select" id="timeZone">
                        <option value="CST6">Mexico Centro (UTC-6)</option>
                        <option value="MST7">Mexico Pacifico (UTC-7)</option>
                        <option value="PST8">Mexico Noroeste (UTC-8)</option>
                        <option value="EST5">Este USA (UTC-5)</option>
                        <option value="CST6CDT,M3.2.0,M11.1.0">Centro USA (UTC-6 DST)</option>
                        <option value="PST8PDT,M3.2.0,M11.1.0">Pacifico USA (UTC-8 DST)</option>
                        <option value="GMT0">UTC/GMT</option>
                        <option value="CET-1">Europa Central (UTC+1)</option>
                        <option value="CST-8">China (UTC+8)</option>
                    </select>
                </div>
            </div>
            <div class="card">
                <div class="card-header"><svg viewBox="0 0 24 24" width="20" height="20" fill="var(--text-secondary)"><path d="M2 17h20v2H2v-2zm11.84-9.21c.1-.24.16-.51.16-.79 0-1.1-.9-2-2-2s-2 .9-2 2c0 .28.06.55.16.79C6.25 8.6 3.27 11.93 3 16h18c-.27-4.07-3.25-7.4-7.16-8.21z"/></svg><h3>Servidor NTP</h3></div>
                <div class="card-body">
                    <select class="form-select" id="ntpServer">
                        <option value="time.cloudflare.com">Cloudflare (recomendado)</option>
                        <option value="time.google.com">Google</option>
                        <option value="pool.ntp.org">NTP Pool</option>
                        <option value="time.nist.gov">NIST</option>
                    </select>
                </div>
            </div>
            <div class="card">
                <div class="card-header"><svg viewBox="0 0 24 24" width="20" height="20" fill="var(--text-secondary)"><path d="M11.99 2C6.47 2 2 6.48 2 12s4.47 10 9.99 10C17.52 22 22 17.52 22 12S17.52 2 11.99 2zM12 20c-4.42 0-8-3.58-8-8s3.58-8 8-8 8 3.58 8 8-3.58 8-8 8zm.5-13H11v6l5.25 3.15.75-1.23-4.5-2.67z"/></svg><h3>Formato de Hora</h3></div>
                <div class="card-body">
                    <p class="card-description">Elige entre formato de 24 horas o 12 horas (AM/PM)</p>
                    <div class="form-checkbox">
                        <input type="checkbox" id="use24hFormat">
                        <label for="use24hFormat">Usar formato 24 horas</label>
                    </div>
                </div>
            </div>
            <div class="card">
                <div class="card-header"><svg viewBox="0 0 24 24" width="20" height="20" fill="var(--text-secondary)"><path d="M12.87 15.07l-2.54-2.51.03-.03c1.74-1.94 2.98-4.17 3.71-6.53H17V4h-7V2H8v2H1v2h11.17C11.5 7.92 10.44 9.75 9 11.35 8.07 10.32 7.3 9.19 6.69 8h-2c.73 1.63 1.73 3.17 2.98 4.56l-5.09 5.02L4 19l5-5 3.11 3.11.76-2.04zM18.5 10h-2L12 22h2l1.12-3h4.75L21 22h2l-4.5-12zm-2.62 7l1.62-4.33L19.12 17h-3.24z"/></svg><h3>Idioma</h3></div>
                <div class="card-body">
                    <p class="card-description">Nombres de dias y meses en espanol (Lun, Mar, Ene, Feb...)</p>
                    <div class="form-checkbox">
                        <input type="checkbox" id="useSpanish">
                        <label for="useSpanish">Usar espanol</label>
                    </div>
                </div>
            </div>
            <div class="card">
                <div class="card-header"><svg viewBox="0 0 24 24" width="20" height="20" fill="var(--text-secondary)"><path d="M19.43 12.98c.04-.32.07-.64.07-.98s-.03-.66-.07-.98l2.11-1.65c.19-.15.24-.42.12-.64l-2-3.46c-.12-.22-.39-.3-.61-.22l-2.49 1c-.52-.4-1.08-.73-1.69-.98l-.38-2.65C14.46 2.18 14.25 2 14 2h-4c-.25 0-.46.18-.49.42l-.38 2.65c-.61.25-1.17.59-1.69.98l-2.49-1c-.23-.09-.49 0-.61.22l-2 3.46c-.13.22-.07.49.12.64l2.11 1.65c-.04.32-.07.65-.07.98s.03.66.07.98l-2.11 1.65c-.19.15-.24.42-.12.64l2 3.46c.12.22.39.3.61.22l2.49-1c.52.4 1.08.73 1.69.98l.38 2.65c.03.24.24.42.49.42h4c.25 0 .46-.18.49-.42l.38-2.65c.61-.25 1.17-.59 1.69-.98l2.49 1c.23.09.49 0 .61-.22l2-3.46c.12-.22.07-.49-.12-.64l-2.11-1.65zM12 15.5c-1.93 0-3.5-1.57-3.5-3.5s1.57-3.5 3.5-3.5 3.5 1.57 3.5 3.5-1.57 3.5-3.5 3.5z"/></svg><h3>Posix Timezone</h3></div>
                <div class="card-body">
                    <p class="card-description">Cadena Posix manual para evitar consultas remotas. <a href="https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv" target="_blank" style="color:var(--primary)">Ver lista</a></p>
                    <div class="form-group">
                        <label class="form-label">Posix String (opcional)</label>
                        <input type="text" class="form-input" id="manualPosix" placeholder="CST6CDT,M3.2.0,M11.1.0">
                    </div>
                </div>
            </div>
        </div>
        <button class="btn btn-save" onclick="saveTime()" style="width:100%;margin-top:20px">Guardar Hora y Fecha</button>
    </div>

    <!-- Canvas Tab -->
    <div class="tab-content" id="tab-canvas">
        <div class="section-title">
            <svg viewBox="0 0 24 24"><path d="M21 3H3c-1.1 0-2 .9-2 2v14c0 1.1.9 2 2 2h18c1.1 0 2-.9 2-2V5c0-1.1-.9-2-2-2zm0 16H3V5h18v14zM5 15h14v3H5z"/></svg>
            Canvas - Caratulas
        </div>

        <div class="cards">
            <!-- Storage & Stored Clockfaces -->
            <div class="card" style="grid-column: 1 / -1;">
                <div class="card-header"><svg viewBox="0 0 24 24" width="20" height="20" fill="var(--text-secondary)"><path d="M4 6H2v14c0 1.1.9 2 2 2h14v-2H4V6zm16-4H8c-1.1 0-2 .9-2 2v12c0 1.1.9 2 2 2h12c1.1 0 2-.9 2-2V4c0-1.1-.9-2-2-2zm-1 9h-4v4h-2v-4H9V9h4V5h2v4h4v2z"/></svg><h3>Caratulas Guardadas en el Reloj</h3></div>
                <div class="card-body">
                    <div style="display:flex;justify-content:space-between;align-items:center;margin-bottom:8px">
                        <span>Almacenamiento:</span>
                        <span id="storageInfo" style="font-size:13px;color:var(--text-secondary)">Cargando...</span>
                    </div>
                    <div class="storage-bar"><div class="storage-bar-fill" id="storageBar" style="width:0%"></div></div>

                    <div class="form-checkbox" style="margin:15px 0">
                        <input type="checkbox" id="rotationEnabled" onchange="onRotationToggle()">
                        <label for="rotationEnabled">Activar rotacion automatica</label>
                        <div id="rotationConfig" style="display:none;margin-left:20px">
                            <label>cada <input type="number" id="rotationInterval" min="1" max="1440" value="60" style="width:60px;padding:4px;border:1px solid var(--border);border-radius:4px"> min</label>
                        </div>
                    </div>

                    <p class="card-description">Haz clic para seleccionar. Arrastra para ordenar la rotacion.</p>
                    <div class="clockface-grid" id="clockfaceList"></div>
                    <input type="hidden" id="canvasFile">
                    <input type="hidden" id="rotationList">
                </div>
            </div>

            <!-- Upload -->
            <div class="card">
                <div class="card-header"><svg viewBox="0 0 24 24" width="20" height="20" fill="var(--text-secondary)"><path d="M9 16h6v-6h4l-7-7-7 7h4v6zm-4 2h14v2H5v-2z"/></svg><h3>Subir Caratula</h3></div>
                <div class="card-body">
                    <p class="card-description">Sube un archivo JSON desde tu computadora</p>
                    <input type="file" id="clockfaceFile" accept=".json" style="margin-bottom:10px">
                    <button class="btn btn-save" onclick="uploadClockface()" style="width:100%">Subir al Reloj</button>
                </div>
            </div>

            <!-- Download from repo -->
            <div class="card">
                <div class="card-header"><svg viewBox="0 0 24 24" width="20" height="20" fill="var(--text-secondary)"><path d="M19.35 10.04C18.67 6.59 15.64 4 12 4 9.11 4 6.6 5.64 5.35 8.04 2.34 8.36 0 10.91 0 14c0 3.31 2.69 6 6 6h13c2.76 0 5-2.24 5-5 0-2.64-2.05-4.78-4.65-4.96zM17 13l-5 5-5-5h3V9h4v4h3z"/></svg><h3>Descargar del Repositorio</h3></div>
                <div class="card-body">
                    <div class="form-group">
                        <label class="form-label">Fuente</label>
                        <select class="form-select" id="repoSource" onchange="onRepoSourceChange()">
                            <option value="github">GitHub XE1E</option>
                            <option value="local">Servidor Local</option>
                        </select>
                    </div>
                    <div id="localConfig" style="display:none">
                        <div class="input-row">
                            <div class="form-group"><label class="form-label">IP</label><input type="text" class="form-input" id="localServerHost" placeholder="192.168.1.100"></div>
                            <div class="form-group"><label class="form-label">Puerto</label><input type="number" class="form-input" id="localServerPort" value="8080"></div>
                        </div>
                    </div>
                    <div class="form-group">
                        <label class="form-label">Caratula</label>
                        <select class="form-select" id="repoClockface"></select>
                        <input type="text" class="form-input" id="customClockface" placeholder="nombre-caratula" style="display:none;margin-top:8px">
                    </div>
                    <button class="btn btn-save" onclick="downloadFromRepo()" style="width:100%">Descargar y Guardar</button>
                </div>
            </div>

            <!-- Night Mode -->
            <div class="card" style="grid-column: 1 / -1;">
                <div class="card-header"><svg viewBox="0 0 24 24" width="20" height="20" fill="var(--text-secondary)"><path d="M9 2c-1.05 0-2.05.16-3 .46 4.06 1.27 7 5.06 7 9.54 0 4.48-2.94 8.27-7 9.54.95.3 1.95.46 3 .46 5.52 0 10-4.48 10-10S14.52 2 9 2z"/></svg><h3>Modo Nocturno</h3></div>
                <div class="card-body">
                    <div style="display:flex;gap:20px;flex-wrap:wrap">
                        <div style="flex:1;min-width:280px">
                            <div class="form-checkbox" style="margin-bottom:15px">
                                <input type="checkbox" id="nightEnabled" onchange="toggleNightSettings()">
                                <label for="nightEnabled">Activar modo nocturno</label>
                            </div>
                            <div id="nightSettings" style="display:none">
                                <div class="input-row" style="margin-bottom:15px">
                                    <div class="form-group"><label class="form-label">Hora inicio</label><input type="time" class="form-input" id="nightStart" value="22:00"></div>
                                    <div class="form-group"><label class="form-label">Hora fin</label><input type="time" class="form-input" id="nightEnd" value="07:00"></div>
                                </div>
                                <div class="form-group" style="margin-bottom:15px">
                                    <label class="form-label">Brillo: <span id="nightBrightVal">8</span></label>
                                    <input type="range" class="form-input" id="nightBright" min="1" max="32" value="8" oninput="$('nightBrightVal').textContent=this.value;updateNightPreview();markCanvasChanged()">
                                </div>
                                <div class="form-group" style="margin-bottom:15px">
                                    <label class="form-label">Caratula nocturna</label>
                                    <select class="form-select" id="nightClockSelect" onchange="onNightClockChange()">
                                        <option value="_builtin">Reloj Nocturno (integrado)</option>
                                    </select>
                                    <input type="hidden" id="nightClock" value="_builtin">
                                </div>
                                <div class="form-group" style="margin-bottom:15px">
                                    <label class="form-label">Color de digitos</label>
                                    <input type="color" id="nightColor" value="#ff0000" onchange="updateNightPreview()" style="width:60px;height:36px;border:none;cursor:pointer;vertical-align:middle">
                                </div>
                                <div class="form-group" style="margin-bottom:10px">
                                    <label class="form-label">Colores brillantes</label>
                                    <div class="color-grid">
                                        <button type="button" class="color-btn" style="background:#ff0000" onclick="setNightColor('#ff0000')"></button>
                                        <button type="button" class="color-btn" style="background:#ff6600" onclick="setNightColor('#ff6600')"></button>
                                        <button type="button" class="color-btn" style="background:#00ff00" onclick="setNightColor('#00ff00')"></button>
                                        <button type="button" class="color-btn" style="background:#0088ff" onclick="setNightColor('#0088ff')"></button>
                                        <button type="button" class="color-btn" style="background:#ff00ff" onclick="setNightColor('#ff00ff')"></button>
                                        <button type="button" class="color-btn" style="background:#ffffff;border:1px solid #ccc" onclick="setNightColor('#ffffff')"></button>
                                    </div>
                                </div>
                                <div class="form-group">
                                    <label class="form-label">Colores tenues (para dormir)</label>
                                    <div class="color-grid">
                                        <button type="button" class="color-btn" style="background:#300000;border:1px solid #555" onclick="setNightColor('#300000')" title="Rojo tenue"></button>
                                        <button type="button" class="color-btn" style="background:#301500;border:1px solid #555" onclick="setNightColor('#301500')" title="Ambar tenue"></button>
                                        <button type="button" class="color-btn" style="background:#003000;border:1px solid #555" onclick="setNightColor('#003000')" title="Verde tenue"></button>
                                        <button type="button" class="color-btn" style="background:#000830;border:1px solid #555" onclick="setNightColor('#000830')" title="Azul tenue"></button>
                                    </div>
                                </div>
                            </div>
                        </div>
                        <div style="flex:0 0 200px">
                            <label class="form-label">Vista previa</label>
                            <div class="night-preview" style="height:120px">
                                <span class="time" id="nightPreviewTime" style="color:#ff0000;font-size:42px">23:45</span>
                                <canvas id="nightThumb" width="64" height="64" style="display:none;image-rendering:pixelated;width:80px;height:80px"></canvas>
                            </div>
                            <p id="nightPreviewStatus" style="text-align:center;font-size:12px;color:var(--text-secondary);margin-top:8px">Desactivado</p>
                        </div>
                    </div>
                </div>
            </div>

            <!-- Info -->
            <div class="card" style="grid-column: 1 / -1;">
                <div class="card-header"><svg viewBox="0 0 24 24" width="20" height="20" fill="var(--text-secondary)"><path d="M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm1 15h-2v-6h2v6zm0-8h-2V7h2v2z"/></svg><h3>Informacion</h3></div>
                <div class="card-body">
                    <p class="card-description">
                        El Canvas Clockface usa archivos JSON que definen como se muestra el reloj.<br>
                        Puedes crear tus propios disenos usando el <strong>Editor de Caratulas</strong>.
                    </p>
                    <div style="margin-top:15px">
                        <a href="https://xe1e.github.io/Clockwise-XE1E/clockface-editor/" target="_blank" class="btn btn-primary" style="text-decoration:none;display:inline-block">Abrir Editor de Caratulas</a>
                    </div>
                </div>
            </div>
        </div>
        <button class="btn btn-save" id="canvasSaveBtn" onclick="saveClock()" style="width:100%;margin-top:20px">Aplicar Cambios</button>
    </div>

    <!-- System Tab -->
    <div class="tab-content" id="tab-system">
        <div class="section-title">
            <svg viewBox="0 0 24 24"><path d="M19.43 12.98c.04-.32.07-.64.07-.98s-.03-.66-.07-.98l2.11-1.65c.19-.15.24-.42.12-.64l-2-3.46c-.12-.22-.39-.3-.61-.22l-2.49 1c-.52-.4-1.08-.73-1.69-.98l-.38-2.65C14.46 2.18 14.25 2 14 2h-4c-.25 0-.46.18-.49.42l-.38 2.65c-.61.25-1.17.59-1.69.98l-2.49-1c-.23-.09-.49 0-.61.22l-2 3.46c-.13.22-.07.49.12.64l2.11 1.65c-.04.32-.07.65-.07.98s.03.66.07.98l-2.11 1.65c-.19.15-.24.42-.12.64l2 3.46c.12.22.39.3.61.22l2.49-1c.52.4 1.08.73 1.69.98l.38 2.65c.03.24.24.42.49.42h4c.25 0 .46-.18.49-.42l.38-2.65c.61-.25 1.17-.59 1.69-.98l2.49 1c.23.09.49 0 .61-.22l2-3.46c.12-.22.07-.49-.12-.64l-2.11-1.65zM12 15.5c-1.93 0-3.5-1.57-3.5-3.5s1.57-3.5 3.5-3.5 3.5 1.57 3.5 3.5-1.57 3.5-3.5 3.5z"/></svg>
            Sistema
        </div>
        <div class="cards">
            <div class="card">
                <div class="card-header"><svg viewBox="0 0 24 24" width="20" height="20" fill="var(--text-secondary)"><path d="M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm1 15h-2v-6h2v6zm0-8h-2V7h2v2z"/></svg><h3>Informacion</h3></div>
                <div class="card-body">
                    <table style="width:100%;font-size:14px">
                        <tr><td style="padding:5px 0">Firmware:</td><td><strong id="sysName">ClockWise-XE1E</strong></td></tr>
                        <tr><td style="padding:5px 0">Version:</td><td><strong id="sysVersion">1.0.0</strong></td></tr>
                        <tr><td style="padding:5px 0">MAC:</td><td><strong id="sysMac">--:--:--:--:--:--</strong></td></tr>
                    </table>
                </div>
            </div>
            <div class="card">
                <div class="card-header"><svg viewBox="0 0 24 24" width="20" height="20" fill="var(--text-secondary)"><path d="M13 3h-2v10h2V3zm4.83 2.17l-1.42 1.42C17.99 7.86 19 9.81 19 12c0 3.87-3.13 7-7 7s-7-3.13-7-7c0-2.19 1.01-4.14 2.58-5.42L6.17 5.17C4.23 6.82 3 9.26 3 12c0 4.97 4.03 9 9 9s9-4.03 9-9c0-2.74-1.23-5.18-3.17-6.83z"/></svg><h3>Acciones</h3></div>
                <div class="card-body">
                    <div style="display:flex;flex-direction:column;gap:10px">
                        <button class="btn btn-save" onclick="restartDevice()" style="width:100%">Reiniciar Dispositivo</button>
                        <button class="btn" style="background:var(--warning);color:#333;width:100%" onclick="if(confirm('Borrar configuracion WiFi? El dispositivo entrara en modo AP.'))resetWifi()">Borrar Config WiFi</button>
                        <button class="btn btn-restart" onclick="if(confirm('ATENCION: Esto borrara TODA la configuracion. Continuar?'))resetDevice()" style="width:100%">Reset de Fabrica</button>
                    </div>
                </div>
            </div>
            <div class="card">
                <div class="card-header"><svg viewBox="0 0 24 24" width="20" height="20" fill="var(--text-secondary)"><path d="M19 9h-4V3H9v6H5l7 7 7-7zM5 18v2h14v-2H5z"/></svg><h3>Actualizacion OTA</h3></div>
                <div class="card-body">
                    <p class="card-description">Actualiza el firmware sin conectar cables</p>
                    <div class="form-group">
                        <label class="form-label">Archivo .bin</label>
                        <input type="file" class="form-input" id="ota-file" accept=".bin" style="padding:8px">
                    </div>
                    <button class="btn btn-save" onclick="uploadOTA()" style="width:100%;margin-top:10px">Subir Firmware</button>
                </div>
            </div>
            <div class="card">
                <div class="card-header"><svg viewBox="0 0 24 24" width="20" height="20" fill="var(--text-secondary)"><path d="M3.9 12c0-1.71 1.39-3.1 3.1-3.1h4V7H7c-2.76 0-5 2.24-5 5s2.24 5 5 5h4v-1.9H7c-1.71 0-3.1-1.39-3.1-3.1zM8 13h8v-2H8v2zm9-6h-4v1.9h4c1.71 0 3.1 1.39 3.1 3.1s-1.39 3.1-3.1 3.1h-4V17h4c2.76 0 5-2.24 5-5s-2.24-5-5-5z"/></svg><h3>Enlaces</h3></div>
                <div class="card-body">
                    <div style="display:flex;flex-direction:column;gap:10px">
                        <a href="https://xe1e.github.io/Clockwise-XE1E/clockface-editor/" target="_blank" class="btn btn-primary" style="text-decoration:none;text-align:center">Editor de Caratulas</a>
                        <a href="https://github.com/XE1E/Clockwise-XE1E" target="_blank" class="btn btn-primary" style="text-decoration:none;text-align:center">Repositorio GitHub</a>
                    </div>
                </div>
            </div>
        </div>
    </div>
</div>

<div class="toast" id="toast">Guardado</div>

<script>
const $=id=>document.getElementById(id);
let settings={},storedClockfaces=[],thumbCache={},canvasChanged=false;

// Tabs
document.querySelectorAll('.tab').forEach(tab=>{
    tab.onclick=()=>{
        document.querySelectorAll('.tab').forEach(t=>t.classList.remove('active'));
        document.querySelectorAll('.tab-content').forEach(p=>p.classList.remove('active'));
        tab.classList.add('active');
        $('tab-'+tab.dataset.tab).classList.add('active');
    };
});

function toast(msg){const t=$('toast');t.textContent=msg||'Guardado';t.style.display='block';setTimeout(()=>t.style.display='none',2500);}

async function api(action,params={}){
    const q=Object.entries(params).map(([k,v])=>k+'='+encodeURIComponent(v)).join('&');
    return fetch('/api/'+action+(q?'?'+q:''),{method:'POST'});
}
function restartDevice(){api('restart');toast('Reiniciando...');}

// System info
let uptimeStart=0;
async function loadSystemInfo(){
    try{
        const r=await fetch('/api/system');
        const d=await r.json();
        const usedRAM=d.totalHeap-d.freeHeap;
        const pctRAM=Math.round(usedRAM/d.totalHeap*100);
        $('ram-info').textContent=pctRAM+'% ('+Math.round(usedRAM/1024)+'/'+Math.round(d.totalHeap/1024)+'KB)';
        if(d.mac&&$('sysMac'))$('sysMac').textContent=d.mac;
        uptimeStart=Date.now()-d.uptimeMs;
        updateUptime();
    }catch(e){console.error(e);}
}
function updateUptime(){
    var el=$('uptime');if(!el)return;
    const s=Math.floor((Date.now()-uptimeStart)/1000);
    const h=Math.floor(s/3600),m=Math.floor((s%3600)/60);
    el.textContent=(h?h+'h ':'')+(m?m+'m ':'')+s%60+'s';
}
setInterval(updateUptime,1000);

// Preview functions
function updateRotationPreview(){
    var el=$('displayRotation'),prev=$('rotation-preview');
    if(el&&prev){var r=parseInt(el.value)||0;prev.style.transform='rotate('+r*90+'deg)';}
}
function updateColorSwapPreview(){
    var chk=$('swapBlueGreen'),cg=$('color-g'),cb=$('color-b'),lg=$('label-g'),lb=$('label-b');
    if(chk&&cg&&cb&&lg&&lb){
        var sw=chk.checked;
        cg.style.background=sw?'#00f':'#0f0';
        cb.style.background=sw?'#0f0':'#00f';
        lg.textContent=sw?'B':'G';
        lb.textContent=sw?'G':'B';
    }
}
function resetDevice(){api('reset');toast('Restaurando...');}
function resetWifi(){api('resetwifi');toast('WiFi borrado. Reiniciando...');}
function uploadOTA(){alert('Funcion no Habilitada');return false;}

// Color helpers
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

// Storage & Clockfaces
async function loadStorageInfo(){
    try{
        const r=await fetch('/api/storage');
        if(!r.ok)throw new Error('HTTP '+r.status);
        const d=await r.json();
        const pct=Math.round(d.used/d.total*100);
        const freeKB=Math.round(d.free/1024);
        const usedKB=Math.round(d.used/1024);
        const totalKB=Math.round(d.total/1024);
        $('storageInfo').textContent=freeKB+'KB libres ('+pct+'% usado)';
        $('storageBar').style.width=pct+'%';
        $('storageBar').style.background=pct>90?'#ea4335':pct>70?'#fbbc04':'#34a853';
        $('spiffs-info').textContent=pct+'% ('+usedKB+'/'+totalKB+'KB)';
    }catch(e){$('storageInfo').textContent='Error';console.error(e);}
}

async function loadStoredClockfaces(){
    try{
        const r=await fetch('/api/clockfaces/list');
        if(!r.ok)throw new Error('HTTP '+r.status);
        storedClockfaces=await r.json();
        renderClockfaceGrid();
        buildNightClockSelect();
        loadThumbsSequential();
    }catch(e){$('clockfaceList').innerHTML='<span style="color:#ea4335">Error al cargar</span>';console.error(e);}
}

let thumbQueue=[];
let thumbLoading=false;

async function loadThumbsSequential(){
    if(thumbLoading||thumbQueue.length===0)return;
    thumbLoading=true;
    while(thumbQueue.length>0){
        const{name,canvas}=thumbQueue.shift();
        if(thumbCache[name]){drawThumb(canvas,thumbCache[name]);continue;}
        try{
            await new Promise(r=>setTimeout(r,50));
            const r=await fetch('/api/clockfaces/get?name='+encodeURIComponent(name));
            if(!r.ok){canvas.style.background='#333';continue;}
            const cf=await r.json();
            let img=null;
            // 1. Check for dedicated thumbnail field first
            if(cf.thumbnail)img=cf.thumbnail;
            // 2. Then check setup images
            if(!img&&cf.setup)for(const el of cf.setup){if(el.image){img=el.image;break;}}
            // 3. Then check loop images
            if(!img&&cf.loop)for(const el of cf.loop){if(el.image){img=el.image;break;}}
            // 4. Finally check first sprite frame
            if(!img&&cf.sprites&&cf.sprites[0]&&cf.sprites[0][0])img=cf.sprites[0][0].image;
            thumbCache[name]={bg:cf.bgColor||0,img};
            drawThumb(canvas,thumbCache[name]);
        }catch(e){canvas.style.background='#333';console.error(e);}
    }
    thumbLoading=false;
}

function renderClockfaceGrid(){
    const cont=$('clockfaceList');
    const rot=$('rotationEnabled').checked;
    const selected=($('rotationList').value||'').split(',').filter(s=>s);

    if(storedClockfaces.length===0){
        cont.innerHTML='<span style="color:var(--text-secondary)">No hay caratulas. Sube o descarga una.</span>';
        return;
    }

    const sorted=[...selected.filter(n=>storedClockfaces.some(c=>c.name===n)),...storedClockfaces.filter(c=>!selected.includes(c.name)).map(c=>c.name)];

    thumbQueue=[];
    cont.innerHTML=sorted.map(name=>{
        const cf=storedClockfaces.find(c=>c.name===name)||{name,size:0};
        const isSelected=selected.includes(name);
        const orderNum=isSelected?selected.indexOf(name)+1:0;
        return '<div class="clockface-item'+(isSelected?' selected':'')+'" data-name="'+name+'">'
            +(orderNum?'<span class="order-num">'+orderNum+'</span>':'')
            +'<button class="delete-btn" onclick="event.stopPropagation();deleteClockface(\''+name+'\')">×</button>'
            +'<canvas width="64" height="64"></canvas>'
            +'<div class="name">'+name+'</div>'
            +'<div class="size">'+Math.round(cf.size/1024)+'KB</div>'
            +'</div>';
    }).join('');

    cont.querySelectorAll('.clockface-item').forEach(el=>{
        const name=el.dataset.name;
        const canvas=el.querySelector('canvas');

        if(thumbCache[name])drawThumb(canvas,thumbCache[name]);
        else thumbQueue.push({name,canvas});

        el.onclick=e=>{if(!e.target.classList.contains('delete-btn'))toggleClockfaceSelect(name);};

        if(rot){
            el.draggable=true;
            el.ondragstart=e=>{dragItem=el;el.classList.add('dragging');e.dataTransfer.effectAllowed='move';};
            el.ondragend=()=>{el.classList.remove('dragging');dragItem=null;};
            el.ondragover=e=>{e.preventDefault();e.dataTransfer.dropEffect='move';};
            el.ondrop=e=>{
                e.preventDefault();
                if(!dragItem||dragItem===el)return;
                const list=Array.from(cont.children);
                const fromIdx=list.indexOf(dragItem),toIdx=list.indexOf(el);
                if(fromIdx<toIdx)el.after(dragItem);else el.before(dragItem);
                updateSelectionFromGrid();
            };
        }
    });
}

function queueThumb(name,canvas){
    thumbQueue.push({name,canvas});
    loadThumbsSequential();
}

function drawThumb(canvas,data){
    const ctx=canvas.getContext('2d');
    ctx.fillStyle=rgb565ToHex(data.bg);
    ctx.fillRect(0,0,64,64);
    if(data.img){
        const img=new Image();
        img.onload=()=>{ctx.imageSmoothingEnabled=false;ctx.drawImage(img,0,0,64,64);};
        img.src='data:image/png;base64,'+data.img;
    }
}

let dragItem=null;

function toggleClockfaceSelect(name){
    const selected=($('rotationList').value||'').split(',').filter(s=>s);
    const idx=selected.indexOf(name);
    if(idx>=0)selected.splice(idx,1);
    else selected.push(name);
    $('rotationList').value=selected.join(',');
    if(selected.length>0)$('canvasFile').value=selected[0];
    renderClockfaceGrid();
    markCanvasChanged();
}

function updateSelectionFromGrid(){
    const items=Array.from($('clockfaceList').querySelectorAll('.clockface-item.selected'));
    const names=items.map(el=>el.dataset.name);
    $('rotationList').value=names.join(',');
    if(names.length>0)$('canvasFile').value=names[0];
    renderClockfaceGrid();
    markCanvasChanged();
}

function onRotationToggle(){
    const rot=$('rotationEnabled').checked;
    $('rotationConfig').style.display=rot?'inline':'none';
    renderClockfaceGrid();
    if(thumbQueue.length>0)loadThumbsSequential();
    markCanvasChanged();
}

async function deleteClockface(name){
    if(!confirm('Eliminar '+name+'?'))return;
    try{
        await fetch('/api/clockfaces/delete?name='+encodeURIComponent(name),{method:'POST'});
        toast('Eliminado');
        loadStorageInfo();
        loadStoredClockfaces();
    }catch(e){toast('Error');}
}

async function uploadClockface(){
    const fileInput=$('clockfaceFile');
    if(!fileInput.files.length){toast('Selecciona archivo');return;}
    const file=fileInput.files[0];
    const name=file.name.replace('.json','');
    try{
        const text=await file.text();
        JSON.parse(text);
        // Check if exists
        const chk=await fetch('/api/clockfaces/exists?name='+encodeURIComponent(name));
        const exists=await chk.json();
        if(exists.exists&&!confirm(name+' ya existe ('+Math.round(exists.size/1024)+'KB). Sobrescribir?'))return;
        const r=await fetch('/api/clockfaces/upload?name='+encodeURIComponent(name),{method:'POST',headers:{'Content-Type':'application/json'},body:text});
        if(r.ok){toast('Subido: '+name);fileInput.value='';loadStorageInfo();loadStoredClockfaces();}
        else toast('Error al subir');
    }catch(e){toast('JSON invalido');}
}

const cfGHPages=['pac-man','nyan-cat','night-clock','donkey-kong','star-wars','goomba_move','clock-club','retro-computer','snoopy3','christmassnoopy','pepsi-final-2','world-clock','mario-clock'];

function onRepoSourceChange(){
    const isLocal=$('repoSource').value==='local';
    $('localConfig').style.display=isLocal?'block':'none';
    $('repoClockface').style.display=isLocal?'none':'block';
    $('customClockface').style.display=isLocal?'block':'none';
    if(!isLocal)$('repoClockface').innerHTML=cfGHPages.map(c=>'<option value="'+c+'">'+c+'</option>').join('');
}

async function downloadFromRepo(){
    const isLocal=$('repoSource').value==='local';
    const name=isLocal?$('customClockface').value.trim():$('repoClockface').value;
    if(!name){toast('Selecciona caratula');return;}
    // Check if exists first
    try{
        const chk=await fetch('/api/clockfaces/exists?name='+encodeURIComponent(name));
        const exists=await chk.json();
        if(exists.exists&&!confirm(name+' ya existe ('+Math.round(exists.size/1024)+'KB). Sobrescribir?'))return;
    }catch(e){}
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

// Canvas change tracking
function markCanvasChanged(){
    if(!canvasChanged){
        canvasChanged=true;
        $('canvasSaveBtn').classList.add('btn-warning');
        $('canvasSaveBtn').textContent='⚠ Aplicar Cambios (sin guardar)';
    }
}

function clearCanvasChanged(){
    canvasChanged=false;
    $('canvasSaveBtn').classList.remove('btn-warning');
    $('canvasSaveBtn').textContent='Aplicar Cambios';
}

// Night mode
function toggleNightSettings(){
    const enabled=$('nightEnabled').checked;
    $('nightSettings').style.display=enabled?'block':'none';
    updateNightPreview();
    markCanvasChanged();
}

function setNightColor(c){
    $('nightColor').value=c;
    updateNightPreview();
    markCanvasChanged();
}

function updateNightPreview(){
    const enabled=$('nightEnabled').checked;
    const color=enabled?$('nightColor').value:'#333333';
    const brightness=enabled?$('nightBright').value:8;
    const preview=$('nightPreviewTime');
    preview.style.color=color;
    // Minimum opacity 0.15 so it's always visible even at lowest brightness
    const opacity=enabled?Math.max(0.15,brightness/32):0.3;
    preview.style.opacity=opacity;

    // Update status with clockface name
    const clockName=$('nightClockSelect').value==='_builtin'?'Reloj integrado':$('nightClockSelect').value;
    if(enabled){
        $('nightPreviewStatus').textContent=clockName+' | '+$('nightStart').value+' - '+$('nightEnd').value;
    }else{
        $('nightPreviewStatus').textContent='Desactivado';
    }

    // Show thumbnail of selected night clockface if not builtin
    updateNightThumb();
}

function updateNightThumb(){
    const sel=$('nightClockSelect').value;
    const thumbEl=$('nightThumb');
    if(!thumbEl)return;
    if(sel==='_builtin'){
        thumbEl.style.display='none';
        $('nightPreviewTime').style.display='block';
    }else if(thumbCache[sel]){
        thumbEl.style.display='block';
        $('nightPreviewTime').style.display='none';
        drawThumb(thumbEl,thumbCache[sel]);
    }
}

function buildNightClockSelect(){
    const sel=$('nightClockSelect');
    const current=$('nightClock').value||'_builtin';
    let html='<option value="_builtin">Reloj Nocturno (integrado)</option>';
    storedClockfaces.forEach(c=>{
        html+='<option value="'+c.name+'"'+(c.name===current?' selected':'')+'>'+c.name+'</option>';
    });
    sel.innerHTML=html;
    if(current!=='_builtin'&&!storedClockfaces.some(c=>c.name===current)){
        sel.value='_builtin';
        $('nightClock').value='_builtin';
    }
    updateNightPreview();
}

function onNightClockChange(){
    $('nightClock').value=$('nightClockSelect').value;
    updateNightPreview();
    markCanvasChanged();
}

// Brightness live update
let brightTimeout=null;
function sendBright(val){
    clearTimeout(brightTimeout);
    brightTimeout=setTimeout(()=>fetch('/api/set?displayBright='+val,{method:'POST'}),100);
}

// Save functions
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

async function readLdrPin(){
    var pin=$('ldrPin').value;
    try{
        var r=await fetch('/api/pin?pin='+pin);
        var d=await r.json();
        $('ldrValue').textContent=d.value;
    }catch(e){$('ldrValue').textContent='Error';}
}

async function saveTime(){
    await saveField('timeZone',$('timeZone').value);
    await saveField('ntpServer',$('ntpServer').value);
    await saveField('use24hFormat',$('use24hFormat').checked?'1':'0');
    await saveField('useSpanish',$('useSpanish').checked?'1':'0');
    await saveField('manualPosix',$('manualPosix').value);
    toast('Hora y Fecha guardada');
}

async function saveClock(){
    const btn=$('canvasSaveBtn');
    btn.disabled=true;
    const rot=$('rotationEnabled').checked;
    await saveField('rotationEnabled',rot?'1':'0');
    await saveField('rotationInterval',$('rotationInterval').value);

    if(rot){
        const list=$('rotationList').value;
        if(!list){toast('Selecciona al menos una caratula');btn.disabled=false;return;}
        await saveField('canvasFile',list.split(',')[0]);
        await saveField('rotationList',list);
    }else{
        const cf=$('canvasFile').value;
        if(!cf){toast('Selecciona una caratula');btn.disabled=false;return;}
        await saveField('canvasFile',cf);
        await saveField('rotationList','');
    }

    await saveField('nightEnabled',$('nightEnabled').checked?'1':'0');
    await saveField('nightStart',$('nightStart').value);
    await saveField('nightEnd',$('nightEnd').value);
    await saveField('nightBright',$('nightBright').value);
    await saveField('nightColor',hexToRgb565($('nightColor').value));
    await saveField('nightClock',$('nightClock').value);

    await fetch('/api/reload',{method:'POST'});
    clearCanvasChanged();
    btn.disabled=false;
    toast('Cambios aplicados');
}

// Load settings
async function load(){
    try{
        const r=await fetch('/api/settings');
        settings=await r.json();

        $('current-ssid').textContent=settings.wifiConnected||'-';
        $('wifi-rssi').textContent=settings.wifiRssi?'('+settings.wifiRssi+' dBm)':'';
        $('ip-address').textContent=location.hostname;
        $('fw-version').textContent=settings.version||'1.0.0';

        $('wifiSsid').value=settings.wifiSsid||'';
        $('wifiSsid2').value=settings.wifiSsid2||'';
        $('wifiSsid3').value=settings.wifiSsid3||'';

        $('displayBright').value=settings.displayBright||32;
        $('brightVal').textContent=settings.displayBright||32;
        $('displayRotation').value=settings.displayRotation||0;
        $('swapBlueGreen').checked=settings.swapBlueGreen==1;
        $('autoBrightMin').value=settings.autoBrightMin||0;
        $('autoBrightMax').value=settings.autoBrightMax||0;
        $('ldrPin').value=settings.ldrPin||35;

        $('timeZone').value=settings.timeZone||'CST6';
        $('ntpServer').value=settings.ntpServer||'time.cloudflare.com';
        $('use24hFormat').checked=settings.use24hFormat==1;
        $('useSpanish').checked=settings.useSpanish==1;
        $('manualPosix').value=settings.manualPosix||'';

        $('canvasFile').value=settings.canvasFile||'';
        $('rotationEnabled').checked=settings.rotationEnabled==1;
        $('rotationInterval').value=settings.rotationInterval||60;
        $('rotationList').value=settings.rotationList||'';
        $('rotationConfig').style.display=settings.rotationEnabled==1?'inline':'none';

        $('nightEnabled').checked=settings.nightEnabled==1;
        $('nightStart').value=settings.nightStart||'22:00';
        $('nightEnd').value=settings.nightEnd||'07:00';
        $('nightBright').value=settings.nightBright||8;
        $('nightBrightVal').textContent=settings.nightBright||8;
        $('nightColor').value=rgb565ToHex(settings.nightColor||63488);
        $('nightClock').value=settings.nightClock||'_builtin';
        toggleNightSettings();
        updateNightPreview();

        $('sysName').textContent=settings.name||'ClockWise-XE1E';
        $('sysVersion').textContent=settings.version||'1.0.0';

        $('localServerHost').value=settings.localServerHost||'192.168.1.100';
        $('localServerPort').value=settings.localServerPort||8080;

        loadStorageInfo();
        loadStoredClockfaces();
        loadSystemInfo();
        onRepoSourceChange();
    }catch(e){console.error(e);}
    // Always update previews even if settings load fails
    updateRotationPreview();
    updateColorSwapPreview();
    clearCanvasChanged();
}

load();
setInterval(()=>{var el=$('nightPreviewTime');if(el)el.textContent=new Date().toLocaleTimeString('es-MX',{hour:'2-digit',minute:'2-digit'});},1000);
</script>
</body>
</html>
)rawliteral";
