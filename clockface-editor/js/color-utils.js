const ColorUtils = {
    hexToRgb(hex) {
        const result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
        return result ? {
            r: parseInt(result[1], 16),
            g: parseInt(result[2], 16),
            b: parseInt(result[3], 16)
        } : null;
    },

    rgbToHex(r, g, b) {
        return '#' + [r, g, b].map(x => {
            const hex = x.toString(16);
            return hex.length === 1 ? '0' + hex : hex;
        }).join('');
    },

    rgbToRgb565(r, g, b) {
        return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
    },

    rgb565ToRgb(rgb565) {
        const r = ((rgb565 >> 11) & 0x1F) << 3;
        const g = ((rgb565 >> 5) & 0x3F) << 2;
        const b = (rgb565 & 0x1F) << 3;
        return { r, g, b };
    },

    hexToRgb565(hex) {
        const rgb = this.hexToRgb(hex);
        if (!rgb) return 0;
        return this.rgbToRgb565(rgb.r, rgb.g, rgb.b);
    },

    rgb565ToHex(rgb565) {
        const rgb = this.rgb565ToRgb(rgb565);
        return this.rgbToHex(rgb.r, rgb.g, rgb.b);
    },

    commonColors: {
        'Negro': { hex: '#000000', rgb565: 0 },
        'Blanco': { hex: '#ffffff', rgb565: 65535 },
        'Rojo': { hex: '#ff0000', rgb565: 63488 },
        'Verde': { hex: '#00ff00', rgb565: 2016 },
        'Azul': { hex: '#0000ff', rgb565: 31 },
        'Amarillo': { hex: '#ffff00', rgb565: 65504 },
        'Cyan': { hex: '#00ffff', rgb565: 2047 },
        'Magenta': { hex: '#ff00ff', rgb565: 63519 },
        'Naranja': { hex: '#ff8000', rgb565: 64512 },
        'Rosa': { hex: '#ff0080', rgb565: 63504 }
    }
};
