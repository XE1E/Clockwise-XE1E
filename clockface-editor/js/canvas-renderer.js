class CanvasRenderer {
    constructor(canvas) {
        this.canvas = canvas;
        this.ctx = canvas.getContext('2d');
        this.ctx.imageSmoothingEnabled = false;
        this.width = 64;
        this.height = 64;

        this.fonts = {
            'picopixel': { size: 3, lineHeight: 5 },
            'square': { size: 5, lineHeight: 7 },
            'medium': { size: 6, lineHeight: 8 },
            'big': { size: 8, lineHeight: 10 },
            '': { size: 5, lineHeight: 7 }
        };
    }

    clear(bgColor = 0) {
        const rgb = ColorUtils.rgb565ToRgb(bgColor);
        this.ctx.fillStyle = `rgb(${rgb.r}, ${rgb.g}, ${rgb.b})`;
        this.ctx.fillRect(0, 0, this.width, this.height);
    }

    render(clockface, selectedId = null) {
        this.clear(clockface.bgColor);

        for (const element of clockface.elements) {
            this.renderElement(element);
        }

        if (selectedId) {
            const selected = clockface.getElementById(selectedId);
            if (selected) {
                this.drawSelection(selected);
            }
        }
    }

    renderElement(element) {
        switch (element.type) {
            case 'datetime':
                this.renderDateTime(element);
                break;
            case 'text':
                this.renderText(element);
                break;
            case 'image':
                this.renderImage(element);
                break;
            case 'rect':
                this.renderRect(element);
                break;
            case 'fillrect':
                this.renderFillRect(element);
                break;
            case 'line':
                this.renderLine(element);
                break;
        }
    }

    renderDateTime(element) {
        const text = element.getDisplayText();
        this.drawText(text, element.x, element.y, element.font, element.fgColor, element.bgColor);
    }

    renderText(element) {
        this.drawText(element.content, element.x, element.y, element.font, element.fgColor, element.bgColor);
    }

    drawText(text, x, y, fontName, fgColor, bgColor) {
        const font = this.fonts[fontName] || this.fonts[''];
        const fgRgb = ColorUtils.rgb565ToRgb(fgColor);
        const bgRgb = ColorUtils.rgb565ToRgb(bgColor);

        this.ctx.font = `${font.size}px monospace`;

        if (bgColor !== 0) {
            const metrics = this.ctx.measureText(text);
            this.ctx.fillStyle = `rgb(${bgRgb.r}, ${bgRgb.g}, ${bgRgb.b})`;
            this.ctx.fillRect(x, y, metrics.width, font.lineHeight);
        }

        this.ctx.fillStyle = `rgb(${fgRgb.r}, ${fgRgb.g}, ${fgRgb.b})`;
        this.ctx.fillText(text, x, y + font.size);
    }

    renderImage(element) {
        if (element.imageData) {
            this.ctx.drawImage(element.imageData, element.x, element.y);
        } else if (element.image) {
            const img = new Image();
            img.onload = () => {
                element.imageData = img;
                element.width = img.width;
                element.height = img.height;
                this.ctx.drawImage(img, element.x, element.y);
            };
            img.src = element.image.startsWith('data:') ?
                element.image : `data:image/png;base64,${element.image}`;
        }
    }

    renderRect(element) {
        const rgb = ColorUtils.rgb565ToRgb(element.color);
        this.ctx.strokeStyle = `rgb(${rgb.r}, ${rgb.g}, ${rgb.b})`;
        this.ctx.lineWidth = 1;
        this.ctx.strokeRect(element.x + 0.5, element.y + 0.5, element.width - 1, element.height - 1);
    }

    renderFillRect(element) {
        const rgb = ColorUtils.rgb565ToRgb(element.color);
        this.ctx.fillStyle = `rgb(${rgb.r}, ${rgb.g}, ${rgb.b})`;
        this.ctx.fillRect(element.x, element.y, element.width, element.height);
    }

    renderLine(element) {
        const rgb = ColorUtils.rgb565ToRgb(element.color);
        this.ctx.strokeStyle = `rgb(${rgb.r}, ${rgb.g}, ${rgb.b})`;
        this.ctx.lineWidth = 1;
        this.ctx.beginPath();
        this.ctx.moveTo(element.x + 0.5, element.y + 0.5);
        this.ctx.lineTo(element.x1 + 0.5, element.y1 + 0.5);
        this.ctx.stroke();
    }

    drawSelection(element) {
        this.ctx.strokeStyle = '#00ff00';
        this.ctx.lineWidth = 1;
        this.ctx.setLineDash([2, 2]);

        let bounds = this.getElementBounds(element);
        this.ctx.strokeRect(bounds.x - 1, bounds.y - 1, bounds.width + 2, bounds.height + 2);

        this.ctx.setLineDash([]);
    }

    getElementBounds(element) {
        switch (element.type) {
            case 'datetime':
            case 'text': {
                const text = element.type === 'datetime' ? element.getDisplayText() : element.content;
                const font = this.fonts[element.font] || this.fonts[''];
                this.ctx.font = `${font.size}px monospace`;
                const metrics = this.ctx.measureText(text);
                return {
                    x: element.x,
                    y: element.y,
                    width: Math.ceil(metrics.width),
                    height: font.lineHeight
                };
            }
            case 'image':
                return {
                    x: element.x,
                    y: element.y,
                    width: element.width || 10,
                    height: element.height || 10
                };
            case 'rect':
            case 'fillrect':
                return {
                    x: element.x,
                    y: element.y,
                    width: element.width,
                    height: element.height
                };
            case 'line':
                return {
                    x: Math.min(element.x, element.x1),
                    y: Math.min(element.y, element.y1),
                    width: Math.abs(element.x1 - element.x) || 1,
                    height: Math.abs(element.y1 - element.y) || 1
                };
            default:
                return { x: element.x, y: element.y, width: 10, height: 10 };
        }
    }

    hitTest(x, y, clockface) {
        for (let i = clockface.elements.length - 1; i >= 0; i--) {
            const element = clockface.elements[i];
            const bounds = this.getElementBounds(element);

            if (x >= bounds.x && x < bounds.x + bounds.width &&
                y >= bounds.y && y < bounds.y + bounds.height) {
                return element;
            }
        }
        return null;
    }
}
