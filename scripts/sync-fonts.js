#!/usr/bin/env node
/**
 * sync-fonts.js - Sincroniza fuentes de pixel-fonts.js al firmware
 *
 * Lee las fuentes definidas en clockface-editor/js/pixel-fonts.js y genera:
 * 1. Archivos .h individuales para cada fuente (formato Adafruit GFX)
 * 2. Template de includes para Clockface.h
 * 3. Template de setFont() para Clockface.cpp
 *
 * Uso: node scripts/sync-fonts.js [--dry-run]
 */

const fs = require('fs');
const path = require('path');

// Configuracion
const CONFIG = {
    pixelFontsPath: 'clockface-editor/js/pixel-fonts.js',
    fontsOutputDir: 'firmware/clockfaces/cw-cf-0x07/fonts',
    templatesOutputDir: 'firmware/clockfaces/cw-cf-0x07/fonts',
    // Mapeo de nombres en pixel-fonts.js a nombres en firmware
    fontNameMapping: {
        'picopixel': { headerName: 'Picopixel', fileName: 'picopixel_gen', alias: 'picopixel' },
        'nocturno': { headerName: 'Nocturno', fileName: 'nocturno_gen', alias: 'nocturno' },
        'tiny5': { headerName: 'Tiny5', fileName: 'tiny5_gen', alias: 'tiny5' },
        // Agregar mas mapeos segun sea necesario
    },
    // Fuentes a excluir (ya existen manualmente o no se quieren sincronizar)
    excludeFonts: ['drawChar', 'drawText', 'measureText', 'getMetrics']
};

// Parsear argumentos
const args = process.argv.slice(2);
const dryRun = args.includes('--dry-run');
const verbose = args.includes('--verbose') || args.includes('-v');

/**
 * Lee y parsea pixel-fonts.js extrayendo el objeto PixelFonts
 */
function parsePixelFonts(filePath) {
    const content = fs.readFileSync(filePath, 'utf8');

    // Extraer el objeto PixelFonts usando eval en un contexto aislado
    // NOTA: Esto es seguro porque controlamos el archivo fuente
    const match = content.match(/const\s+PixelFonts\s*=\s*\{/);
    if (!match) {
        throw new Error('No se encontro el objeto PixelFonts en el archivo');
    }

    // Encontrar el inicio y contar llaves para encontrar el final
    const startIdx = match.index + match[0].length - 1;
    let braceCount = 1;
    let idx = startIdx + 1;

    while (braceCount > 0 && idx < content.length) {
        if (content[idx] === '{') braceCount++;
        else if (content[idx] === '}') braceCount--;
        idx++;
    }

    const objectStr = content.substring(startIdx, idx);

    // Evaluar el objeto (confiamos en el archivo fuente)
    const PixelFonts = eval('(' + objectStr + ')');

    return PixelFonts;
}

/**
 * Convierte nombre a PascalCase para el header C++
 */
function toPascalCase(str) {
    return str.split(/[-_]/).map(word =>
        word.charAt(0).toUpperCase() + word.slice(1).toLowerCase()
    ).join('');
}

/**
 * Genera el contenido del archivo .h para una fuente
 */
function generateFontHeader(fontName, fontData) {
    const pascalName = toPascalCase(fontName);
    const upperName = fontName.toUpperCase().replace(/-/g, '_');

    const lines = [];

    // Header guard y comentarios
    lines.push(`// ${pascalName} - Auto-generated from pixel-fonts.js`);
    lines.push(`// Do not edit manually - run 'npm run sync-fonts' to regenerate`);
    lines.push('');
    lines.push('#pragma once');
    lines.push('');
    lines.push('#include <Adafruit_GFX.h>');
    lines.push('');

    // Bitmaps
    lines.push(`const uint8_t ${pascalName}Bitmaps[] PROGMEM = {`);
    const bitmaps = fontData.bitmaps;
    const bytesPerLine = 16;
    for (let i = 0; i < bitmaps.length; i += bytesPerLine) {
        const chunk = bitmaps.slice(i, i + bytesPerLine);
        const hexStr = chunk.map(b => '0x' + b.toString(16).padStart(2, '0').toUpperCase()).join(',');
        const isLast = i + bytesPerLine >= bitmaps.length;
        lines.push(`    ${hexStr}${isLast ? '' : ','}`);
    }
    lines.push('};');
    lines.push('');

    // Glyphs
    lines.push(`/* {offset, width, height, advance, xOffset, yOffset} */`);
    lines.push(`const GFXglyph ${pascalName}Glyphs[] PROGMEM = {`);
    fontData.glyphs.forEach((glyph, idx) => {
        const [offset, width, height, advance, xOff, yOff] = glyph;
        const charCode = fontData.first + idx;
        const charDisplay = (charCode >= 0x20 && charCode < 0x7F)
            ? String.fromCharCode(charCode)
            : `0x${charCode.toString(16).toUpperCase()}`;
        const isLast = idx === fontData.glyphs.length - 1;
        lines.push(`    {${offset}, ${width}, ${height}, ${advance}, ${xOff}, ${yOff}}${isLast ? '' : ','} /* ${charDisplay} */`);
    });
    lines.push('};');
    lines.push('');

    // Font struct
    lines.push(`const GFXfont ${pascalName} PROGMEM = {`);
    lines.push(`    (uint8_t *)${pascalName}Bitmaps,`);
    lines.push(`    (GFXglyph *)${pascalName}Glyphs,`);
    lines.push(`    0x${fontData.first.toString(16).toUpperCase()}, 0x${fontData.last.toString(16).toUpperCase()}, ${fontData.yAdvance}`);
    lines.push('};');
    lines.push('');

    return lines.join('\n');
}

/**
 * Genera template de includes
 */
function generateIncludesTemplate(fonts) {
    const lines = [
        '// === AUTO-GENERATED FONT INCLUDES ===',
        '// Copy these lines to Clockface.h',
        '// Generated by: npm run sync-fonts',
        ''
    ];

    fonts.forEach(({ fileName }) => {
        lines.push(`#include "fonts/${fileName}.h"`);
    });

    lines.push('');
    lines.push('// === END AUTO-GENERATED ===');

    return lines.join('\n');
}

/**
 * Genera template de setFont()
 */
function generateSetFontTemplate(fonts) {
    const lines = [
        '// === AUTO-GENERATED SETFONT CASES ===',
        '// Copy these lines to Clockface.cpp setFont() function',
        '// Generated by: npm run sync-fonts',
        ''
    ];

    fonts.forEach(({ alias, headerName }, idx) => {
        const elsePrefix = idx === 0 ? 'if' : 'else if';
        lines.push(`${elsePrefix} (strcmp(fontName, "${alias}") == 0)`);
        lines.push('{');
        lines.push(`    Locator::getDisplay()->setFont(&${headerName});`);
        lines.push('}');
    });

    lines.push('');
    lines.push('// === END AUTO-GENERATED ===');

    return lines.join('\n');
}

/**
 * Valida una fuente
 */
function validateFont(fontName, fontData) {
    const errors = [];

    if (!fontData.bitmaps || !Array.isArray(fontData.bitmaps)) {
        errors.push(`${fontName}: falta array 'bitmaps'`);
    }
    if (!fontData.glyphs || !Array.isArray(fontData.glyphs)) {
        errors.push(`${fontName}: falta array 'glyphs'`);
    }
    if (typeof fontData.first !== 'number') {
        errors.push(`${fontName}: falta 'first' (primer caracter)`);
    }
    if (typeof fontData.last !== 'number') {
        errors.push(`${fontName}: falta 'last' (ultimo caracter)`);
    }
    if (typeof fontData.yAdvance !== 'number') {
        errors.push(`${fontName}: falta 'yAdvance'`);
    }

    return errors;
}

/**
 * Funcion principal
 */
function main() {
    console.log('=== Sincronizacion de Fuentes ===');
    console.log(`Modo: ${dryRun ? 'DRY-RUN (no se escribiran archivos)' : 'NORMAL'}`);
    console.log('');

    // Verificar que existe el archivo fuente
    if (!fs.existsSync(CONFIG.pixelFontsPath)) {
        console.error(`ERROR: No se encuentra ${CONFIG.pixelFontsPath}`);
        process.exit(1);
    }

    // Parsear pixel-fonts.js
    console.log(`Leyendo ${CONFIG.pixelFontsPath}...`);
    let PixelFonts;
    try {
        PixelFonts = parsePixelFonts(CONFIG.pixelFontsPath);
    } catch (err) {
        console.error(`ERROR parseando pixel-fonts.js: ${err.message}`);
        process.exit(1);
    }

    // Filtrar fuentes validas (excluir funciones y fuentes en la lista de exclusion)
    const fontNames = Object.keys(PixelFonts).filter(name => {
        if (CONFIG.excludeFonts.includes(name)) return false;
        if (typeof PixelFonts[name] === 'function') return false;
        if (!PixelFonts[name].bitmaps) return false;
        return true;
    });

    console.log(`Fuentes encontradas: ${fontNames.length}`);
    fontNames.forEach(name => console.log(`  - ${name}`));
    console.log('');

    // Validar fuentes
    const allErrors = [];
    fontNames.forEach(name => {
        const errors = validateFont(name, PixelFonts[name]);
        allErrors.push(...errors);
    });

    if (allErrors.length > 0) {
        console.error('ERRORES de validacion:');
        allErrors.forEach(err => console.error(`  - ${err}`));
        process.exit(1);
    }

    // Crear directorio de salida si no existe
    if (!dryRun && !fs.existsSync(CONFIG.fontsOutputDir)) {
        fs.mkdirSync(CONFIG.fontsOutputDir, { recursive: true });
    }

    // Generar archivos .h
    const generatedFonts = [];
    fontNames.forEach(fontName => {
        const fontData = PixelFonts[fontName];
        const mapping = CONFIG.fontNameMapping[fontName] || {
            headerName: toPascalCase(fontName),
            fileName: fontName.toLowerCase().replace(/-/g, '_') + '_gen',
            alias: fontName
        };

        const headerContent = generateFontHeader(fontName, fontData);
        const outputPath = path.join(CONFIG.fontsOutputDir, `${mapping.fileName}.h`);

        if (dryRun) {
            console.log(`[DRY-RUN] Generaria: ${outputPath}`);
            if (verbose) {
                console.log('--- Contenido ---');
                console.log(headerContent.substring(0, 500) + '...');
                console.log('-----------------');
            }
        } else {
            fs.writeFileSync(outputPath, headerContent);
            console.log(`Generado: ${outputPath}`);
        }

        generatedFonts.push({
            fontName,
            ...mapping
        });
    });

    // Generar templates
    const includesTemplate = generateIncludesTemplate(generatedFonts);
    const setFontTemplate = generateSetFontTemplate(generatedFonts);

    const includesPath = path.join(CONFIG.templatesOutputDir, '_INCLUDES_TEMPLATE.txt');
    const setFontPath = path.join(CONFIG.templatesOutputDir, '_SETFONT_TEMPLATE.txt');

    if (dryRun) {
        console.log(`[DRY-RUN] Generaria: ${includesPath}`);
        console.log(`[DRY-RUN] Generaria: ${setFontPath}`);
    } else {
        fs.writeFileSync(includesPath, includesTemplate);
        fs.writeFileSync(setFontPath, setFontTemplate);
        console.log(`Generado: ${includesPath}`);
        console.log(`Generado: ${setFontPath}`);
    }

    console.log('');
    console.log('=== Sincronizacion completada ===');
    console.log('');
    console.log('Proximos pasos:');
    console.log('1. Revisar los archivos generados en firmware/clockfaces/cw-cf-0x07/fonts/');
    console.log('2. Copiar los includes de _INCLUDES_TEMPLATE.txt a Clockface.h');
    console.log('3. Copiar los cases de _SETFONT_TEMPLATE.txt a Clockface.cpp setFont()');
    console.log('4. Compilar el firmware para verificar');
}

// Ejecutar
main();
