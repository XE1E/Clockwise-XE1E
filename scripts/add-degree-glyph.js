#!/usr/bin/env node
/**
 * add-degree-glyph.js
 *
 * Agrega el glifo del simbolo de grado en el codepoint 0x7F (un solo glifo,
 * extendiendo el rango de 0x7E a 0x7F, sin glifos de relleno) a un conjunto
 * de fuentes, tanto en pixel-fonts.js (editor) como en los .h del firmware.
 *
 * El glifo se auto-dimensiona a partir del digito '0' de cada fuente.
 * Solo procesa fuentes que terminen LIMPIAMENTE en 0x7E en AMBOS lados;
 * cualquier otra se salta con aviso (idempotente: si ya esta en 0x7F, salta).
 *
 * Uso: node scripts/add-degree-glyph.js [--dry-run]
 */

const fs = require('fs');
const path = require('path');

const dryRun = process.argv.includes('--dry-run');

const PIXEL_FONTS = 'clockface-editor/js/pixel-fonts.js';

// jsKey en pixel-fonts.js  ->  archivo .h del firmware
const TARGETS = [
  { jsKey: 'medium',    h: 'firmware/clockfaces/cw-cf-0x07/fonts/minute7pt7b.h' },
  { jsKey: 'big',       h: 'firmware/clockfaces/cw-cf-0x07/fonts/hour8pt7b.h' },
  { jsKey: 'tiny5',     h: 'firmware/clockfaces/cw-cf-0x07/fonts/tiny5.h' },
  // Omitidas a proposito (casos especiales, se trataran aparte):
  //  - square (atari): en pixel-fonts.js solo llega a 0x3A (sin letras).
  //  - 8x13 / haxor-12: existen en firmware pero NO en pixel-fonts.js (editor).
  //  - tomthumb: TomThumb.h tiene bloques #if dentro de los arrays.
  //  - picopixel: formato Adafruit fontconvert (array glifos en una linea,
  //    comentarios con comillas) incompatible con el regex append de este script.
];

// --- parse de PixelFonts (igual que sync-fonts.js) -----------------------
function parsePixelFonts(content) {
  const m = content.match(/const\s+PixelFonts\s*=\s*\{/);
  if (!m) throw new Error('No se encontro PixelFonts');
  const start = m.index + m[0].length - 1;
  let depth = 1, i = start + 1;
  while (depth > 0 && i < content.length) {
    if (content[i] === '{') depth++;
    else if (content[i] === '}') depth--;
    i++;
  }
  return eval('(' + content.substring(start, i) + ')');
}

// --- empaqueta filas de bits (MSB-first, continuo, padding al final) ------
function packBits(rows) {
  const bits = [];
  for (const row of rows) for (const b of row) bits.push(b ? 1 : 0);
  while (bits.length % 8 !== 0) bits.push(0);
  const out = [];
  for (let i = 0; i < bits.length; i += 8) {
    let byte = 0;
    for (let j = 0; j < 8; j++) if (bits[i + j]) byte |= (1 << (7 - j));
    out.push(byte);
  }
  return out;
}

// --- disena el glifo de grado a partir del digito '0' ---------------------
function designDegree(fontData) {
  const zero = fontData.glyphs[0x30 - fontData.first]; // [off,w,h,adv,xoff,yoff]
  if (!zero) return null;
  const digitH = zero[2];
  const yoff = zero[5];
  let s = digitH >= 10 ? 4 : (digitH >= 5 ? 3 : 2);
  if (s > digitH) s = digitH;

  const rows = [];
  for (let r = 0; r < s; r++) {
    const row = [];
    for (let c = 0; c < s; c++) {
      // anillo hueco si s>=3, bloque relleno si s==2
      row.push(s >= 3 ? ((r === 0 || r === s - 1 || c === 0 || c === s - 1) ? 1 : 0) : 1);
    }
    rows.push(row);
  }
  return { w: s, h: s, xAdvance: s + 1, xOffset: 0, yOffset: yoff, bytes: packBits(rows) };
}

const bytesC = (bytes) => bytes.map(b => '0x' + b.toString(16).toUpperCase().padStart(2, '0')).join(', ');

// --- edita un .h del firmware (append-only) -------------------------------
function patchHeader(file, deg) {
  let c = fs.readFileSync(file, 'utf8');

  const structRe = /const GFXfont\s+\w+\s+PROGMEM\s*=\s*\{[\s\S]*?\(uint8_t\s*\*\)(\w+),\s*\(GFXglyph\s*\*\)(\w+),\s*0x20,\s*(0x[0-9A-Fa-f]+)\s*,\s*(\d+)\s*\}/;
  const sm = c.match(structRe);
  if (!sm) return { ok: false, msg: 'no se hallo GFXfont' };
  const [, bmpName, glyName, lastHex] = sm;
  const last = parseInt(lastHex, 16);
  if (last === 0x7F) return { ok: false, msg: 'ya tiene 0x7F (saltado)' };
  if (last !== 0x7E) return { ok: false, msg: `last=0x${last.toString(16)} (no 0x7E, saltado)` };

  // contar bytes del bitmap = offset del nuevo glifo
  const bmRe = new RegExp(`const uint8_t\\s+${bmpName}\\[\\]\\s*PROGMEM\\s*=\\s*\\{([\\s\\S]*?)\\};`);
  const bm = c.match(bmRe);
  if (!bm) return { ok: false, msg: 'no se hallo array Bitmaps' };
  const offset = (bm[1].match(/0x[0-9A-Fa-f]{2}/g) || []).length;

  // 1) append bytes al bitmap
  // (usar funcion de reemplazo: el contenido puede traer '$' en comentarios
  //  y String.replace interpretaria $&, $', $` como patrones)
  let bmInner = bm[1].replace(/\s*$/, '');
  if (!bmInner.endsWith(',')) bmInner += ',';
  const bmNew = `const uint8_t ${bmpName}[] PROGMEM = {${bmInner}\n  ${bytesC(deg.bytes)}\n};`;
  c = c.replace(bm[0], () => bmNew);

  // 2) append glifo. Detecta el estilo: algunos fonts terminan cada entrada
  //    con "}," (coma final) y otros usan coma inicial "  ,{...}". Hay que
  //    ignorar el comentario final al decidir si ya existe la coma separadora.
  const glRe = new RegExp(`const GFXglyph\\s+${glyName}\\[\\]\\s*PROGMEM\\s*=\\s*\\{([\\s\\S]*?)\\};`);
  const gl = c.match(glRe);
  if (!gl) return { ok: false, msg: 'no se hallo array Glyphs' };
  const glInner = gl[1].replace(/\s*$/, '');
  const stripped = glInner.replace(/\/\/[^\n]*$/, '').replace(/\s*$/, '');
  const lead = stripped.endsWith(',') ? '  ' : '  ,';   // coma separadora si falta
  const glyphLine = `${lead}{ ${offset}, ${deg.w}, ${deg.h}, ${deg.xAdvance}, ${deg.xOffset}, ${deg.yOffset} }  // 0x7F grado`;
  const glNew = `const GFXglyph ${glyName}[] PROGMEM = {${glInner}\n${glyphLine}\n};`;
  c = c.replace(gl[0], () => glNew);

  // 3) bump last 0x7E -> 0x7F dentro del struct GFXfont ya capturado (robusto al formato)
  const newStruct = sm[0].replace(/(0x20,\s*)0x7[eE]/, '$10x7F');
  c = c.replace(sm[0], () => newStruct);

  if (!dryRun) fs.writeFileSync(file, c);
  return { ok: true, msg: `offset=${offset}, glifo ${deg.w}x${deg.h} yoff=${deg.yOffset}` };
}

// --- edita pixel-fonts.js (append-only, por fuente) -----------------------
function patchPixelFonts(content, jsKey, fontData, deg) {
  const k = `(?:${jsKey}|'${jsKey}'|"${jsKey}")`;
  const offset = fontData.bitmaps.length;

  // bitmaps (array de una linea, sin ']' interno)
  const bmRe = new RegExp(`(${k}:\\s*\\{\\s*bitmaps:\\s*\\[)([^\\]]*)(\\])`);
  if (!bmRe.test(content)) return { ok: false, msg: 'no bitmaps en JS', content };
  content = content.replace(bmRe, (m, a, body, c) => `${a}${body.replace(/\s*$/, '')}, ${deg.bytes.map(b => b).join(', ')}${c}`);

  // glyphs (array multilinea; cierra con ']' seguido de ', first:' o newline+first)
  const glRe = new RegExp(`(${k}:\\s*\\{[\\s\\S]*?glyphs:\\s*\\[)([\\s\\S]*?)(\\]\\s*,\\s*first:)`);
  if (!glRe.test(content)) return { ok: false, msg: 'no glyphs en JS', content };
  const glyphArr = `[${offset}, ${deg.w}, ${deg.h}, ${deg.xAdvance}, ${deg.xOffset}, ${deg.yOffset}]`;
  content = content.replace(glRe, (m, a, body, c) => `${a}${body.replace(/\s*$/, '')},\n            ${glyphArr}\n        ${c}`);

  // last 0x7E -> 0x7F (el primer last: tras la key)
  const lastRe = new RegExp(`(${k}:\\s*\\{[\\s\\S]*?last:\\s*)0x7[eE]`);
  content = content.replace(lastRe, '$10x7F');

  return { ok: true, msg: `offset=${offset}`, content };
}

// --- main -----------------------------------------------------------------
let pf = fs.readFileSync(PIXEL_FONTS, 'utf8');
const PixelFonts = parsePixelFonts(pf);

console.log(`=== add-degree-glyph ${dryRun ? '(DRY-RUN)' : ''} ===\n`);

for (const t of TARGETS) {
  const data = PixelFonts[t.jsKey];
  if (!data || !data.bitmaps) { console.log(`SKIP ${t.jsKey}: no existe en pixel-fonts.js`); continue; }
  if (data.last === 0x7F)     { console.log(`SKIP ${t.jsKey}: pixel-fonts ya en 0x7F`); continue; }
  if (data.last !== 0x7E)     { console.log(`SKIP ${t.jsKey}: pixel-fonts last=0x${data.last.toString(16)} (no 0x7E)`); continue; }

  const deg = designDegree(data);
  if (!deg) { console.log(`SKIP ${t.jsKey}: sin digito '0'`); continue; }

  // .h firmware
  const hRes = patchHeader(t.h, deg);
  if (!hRes.ok) { console.log(`SKIP ${t.jsKey}: .h ${hRes.msg}`); continue; }

  // pixel-fonts.js
  const jsRes = patchPixelFonts(pf, t.jsKey, data, deg);
  if (!jsRes.ok) { console.log(`SKIP ${t.jsKey}: JS ${jsRes.msg} (.h SI modificado!)`); continue; }
  pf = jsRes.content;

  console.log(`OK   ${t.jsKey}: .h ${hRes.msg} | JS ${jsRes.msg} | bytes=[${bytesC(deg.bytes)}]`);
}

if (!dryRun) fs.writeFileSync(PIXEL_FONTS, pf);
console.log(`\n${dryRun ? 'DRY-RUN: no se escribio nada.' : 'Hecho.'}`);
