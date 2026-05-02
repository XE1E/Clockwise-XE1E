class CanvasRenderer {
    constructor(canvas) {
        this.canvas = canvas;
        this.ctx = canvas.getContext('2d');
        this.ctx.imageSmoothingEnabled = false;
        this.width = 64;
        this.height = 64;

        this.fontMap = {
            'picopixel': 'picopixel',
            'tomthumb': 'tomthumb',
            'square': 'square',
            'medium': 'medium',
            'big': 'big',
            'bold': 'bold',
            '': 'picopixel'
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
            case 'circle':
                this.renderCircle(element);
                break;
            case 'fillcircle':
                this.renderFillCircle(element);
                break;
            case 'sprite':
                this.renderSprite(element);
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
        const mappedFont = this.fontMap[fontName] || 'picopixel';
        const fgRgb = ColorUtils.rgb565ToRgb(fgColor);
        const bgRgb = ColorUtils.rgb565ToRgb(bgColor);

        const fgColorStr = `rgb(${fgRgb.r}, ${fgRgb.g}, ${fgRgb.b})`;
        const bgColorStr = bgColor !== 0 ? `rgb(${bgRgb.r}, ${bgRgb.g}, ${bgRgb.b})` : null;

        PixelFonts.drawText(this.ctx, mappedFont, text, x, y, fgColorStr, bgColorStr);
    }

    renderImage(element) {
        if (element.imageData) {
            this.ctx.drawImage(element.imageData, element.x, element.y);
        } else if (element.image && !element._loading) {
            element._loading = true;
            const img = new Image();
            img.onload = () => {
                element.imageData = img;
                element.width = img.width;
                element.height = img.height;
                element._loading = false;
            };
            img.onerror = () => {
                element._loading = false;
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

    renderCircle(element) {
        const rgb = ColorUtils.rgb565ToRgb(element.color);
        this.ctx.strokeStyle = `rgb(${rgb.r}, ${rgb.g}, ${rgb.b})`;
        this.ctx.lineWidth = 1;
        this.ctx.beginPath();
        this.ctx.arc(element.x + 0.5, element.y + 0.5, element.radius, 0, 2 * Math.PI);
        this.ctx.stroke();
    }

    renderFillCircle(element) {
        const rgb = ColorUtils.rgb565ToRgb(element.color);
        this.ctx.fillStyle = `rgb(${rgb.r}, ${rgb.g}, ${rgb.b})`;
        this.ctx.beginPath();
        this.ctx.arc(element.x, element.y, element.radius, 0, 2 * Math.PI);
        this.ctx.fill();
    }

    renderSprite(element) {
        if (element.frameImages && element.frameImages.length > 0) {
            const frameIndex = element.currentFrame % element.frameImages.length;
            const img = element.frameImages[frameIndex];
            if (img) {
                this.ctx.drawImage(img, element.x, element.y);
            }
        } else {
            this.ctx.strokeStyle = '#ff00ff';
            this.ctx.lineWidth = 1;
            this.ctx.strokeRect(element.x + 0.5, element.y + 0.5, 15, 15);
            this.ctx.beginPath();
            this.ctx.moveTo(element.x, element.y);
            this.ctx.lineTo(element.x + 16, element.y + 16);
            this.ctx.moveTo(element.x + 16, element.y);
            this.ctx.lineTo(element.x, element.y + 16);
            this.ctx.stroke();
        }
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
                const mappedFont = this.fontMap[element.font] || 'picopixel';
                const metrics = PixelFonts.measureText(mappedFont, text);
                return {
                    x: element.x,
                    y: element.y + metrics.yOffset,
                    width: metrics.width,
                    height: metrics.height
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
            case 'circle':
            case 'fillcircle':
                return {
                    x: element.x - element.radius,
                    y: element.y - element.radius,
                    width: element.radius * 2,
                    height: element.radius * 2
                };
            case 'sprite': {
                let w = 16, h = 16;
                if (element.frameImages && element.frameImages.length > 0 && element.frameImages[0]) {
                    w = element.frameImages[0].width || 16;
                    h = element.frameImages[0].height || 16;
                }
                return { x: element.x, y: element.y, width: w, height: h };
            }
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
