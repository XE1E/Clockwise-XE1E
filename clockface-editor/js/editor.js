class ClockfaceEditor {
    constructor() {
        this.canvas = document.getElementById('editor-canvas');
        this.renderer = new CanvasRenderer(this.canvas);
        this.clockface = new Clockface();
        this.selectedId = null;
        this.currentTool = 'select';
        this.zoom = 8;
        this.isDragging = false;
        this.dragOffset = { x: 0, y: 0 };
        this.referenceImage = document.getElementById('reference-image');

        this.init();
    }

    init() {
        this.updateCanvasSize();
        this.bindEvents();
        this.bindReferenceImage();
        this.render();
        this.startClock();
    }

    updateCanvasSize() {
        this.canvas.style.width = `${64 * this.zoom}px`;
        this.canvas.style.height = `${64 * this.zoom}px`;

        const grid = document.querySelector('.canvas-grid');
        grid.style.backgroundSize = `${this.zoom}px ${this.zoom}px`;

        document.getElementById('zoom-level').textContent = `${this.zoom}x`;
        document.getElementById('canvas-zoom').textContent = `Zoom: ${this.zoom}x`;

        if (this.referenceImage && !this.referenceImage.classList.contains('hidden')) {
            this.referenceImage.style.width = `${64 * this.zoom}px`;
            this.referenceImage.style.height = `${64 * this.zoom}px`;
        }
    }

    bindReferenceImage() {
        const fileInput = document.getElementById('reference-file');
        const opacitySlider = document.getElementById('reference-opacity');
        const opacityValue = document.getElementById('opacity-value');
        const opacityControl = document.getElementById('opacity-control');
        const positionLabel = document.getElementById('ref-position-label');
        const behindCheckbox = document.getElementById('reference-behind');
        const clearBtn = document.getElementById('btn-clear-reference');
        const convertBtn = document.getElementById('btn-convert-reference');

        fileInput.addEventListener('change', (e) => {
            const file = e.target.files[0];
            if (!file) return;

            const reader = new FileReader();
            reader.onload = (event) => {
                this.referenceImage.src = event.target.result;
                this.referenceImage.classList.remove('hidden');
                this.referenceImage.style.width = `${64 * this.zoom}px`;
                this.referenceImage.style.height = `${64 * this.zoom}px`;
                opacityControl.style.display = 'flex';
                positionLabel.style.display = 'flex';
                convertBtn.style.display = 'inline-block';
                this.updateReferencePosition();
            };
            reader.readAsDataURL(file);
        });

        opacitySlider.addEventListener('input', (e) => {
            const opacity = e.target.value / 100;
            this.referenceImage.style.opacity = opacity;
            opacityValue.textContent = `${e.target.value}%`;
        });

        behindCheckbox.addEventListener('change', () => {
            this.updateReferencePosition();
        });

        clearBtn.addEventListener('click', () => {
            this.clearReferenceImage();
        });

        convertBtn.addEventListener('click', () => {
            this.convertReferenceToElement();
        });
    }

    clearReferenceImage() {
        this.referenceImage.src = '';
        this.referenceImage.classList.add('hidden');
        document.getElementById('opacity-control').style.display = 'none';
        document.getElementById('ref-position-label').style.display = 'none';
        document.getElementById('btn-convert-reference').style.display = 'none';
        document.getElementById('reference-file').value = '';
    }

    convertReferenceToElement() {
        if (!this.referenceImage.src) return;

        const tempCanvas = document.createElement('canvas');
        tempCanvas.width = 64;
        tempCanvas.height = 64;
        const tempCtx = tempCanvas.getContext('2d');
        tempCtx.imageSmoothingEnabled = false;

        const img = new Image();
        img.onload = async () => {
            const scale = Math.min(64 / img.width, 64 / img.height);
            const newWidth = Math.floor(img.width * scale);
            const newHeight = Math.floor(img.height * scale);
            const offsetX = Math.floor((64 - newWidth) / 2);
            const offsetY = Math.floor((64 - newHeight) / 2);

            tempCtx.fillStyle = '#000000';
            tempCtx.fillRect(0, 0, 64, 64);
            tempCtx.drawImage(img, offsetX, offsetY, newWidth, newHeight);

            const base64 = tempCanvas.toDataURL('image/png').split(',')[1];

            const element = new ImageElement(0, 0);
            element.image = base64;
            await element.loadImage(base64);

            this.clockface.elements.unshift(element);
            this.selectedId = element.id;

            this.clearReferenceImage();
            this.updatePropertiesPanel();
            this.updateLayersList();
            this.render();

            alert('Imagen convertida a 64x64 y agregada como fondo. Puedes ajustar su posicion.');
        };
        img.src = this.referenceImage.src;
    }

    updateReferencePosition() {
        const behind = document.getElementById('reference-behind').checked;
        this.referenceImage.classList.remove('behind', 'above');
        this.referenceImage.classList.add(behind ? 'behind' : 'above');
    }

    bindImageTool() {
        this.imageToolData = {
            originalImage: null,
            processedBase64: null,
            cropStart: null,
            cropEnd: null,
            displayScale: 1,
            isCropping: false
        };

        const cropCanvas = document.getElementById('crop-canvas');
        const cropSelection = document.getElementById('crop-selection');
        const cropContainer = cropCanvas.parentElement;

        document.getElementById('btn-image-tool').addEventListener('click', () => {
            document.getElementById('image-modal').classList.add('active');
            document.getElementById('image-preview-section').style.display = 'none';
            document.getElementById('btn-add-image').style.display = 'none';
            this.resetCropSelection();
        });

        document.getElementById('image-tool-file').addEventListener('change', (e) => {
            const file = e.target.files[0];
            if (!file) return;

            const reader = new FileReader();
            reader.onload = (event) => {
                const img = new Image();
                img.onload = () => {
                    this.imageToolData.originalImage = img;
                    document.getElementById('image-preview-section').style.display = 'block';
                    document.getElementById('btn-add-image').style.display = 'inline-block';

                    this.drawCropCanvas();
                    this.resetCropSelection();
                    this.updateImagePreview();
                };
                img.src = event.target.result;
            };
            reader.readAsDataURL(file);
        });

        cropCanvas.addEventListener('mousedown', (e) => {
            const rect = cropCanvas.getBoundingClientRect();
            this.imageToolData.cropStart = {
                x: e.clientX - rect.left,
                y: e.clientY - rect.top
            };
            this.imageToolData.isCropping = true;
            cropSelection.classList.add('active');
        });

        cropCanvas.addEventListener('mousemove', (e) => {
            if (!this.imageToolData.isCropping) return;

            const rect = cropCanvas.getBoundingClientRect();
            this.imageToolData.cropEnd = {
                x: Math.max(0, Math.min(cropCanvas.width, e.clientX - rect.left)),
                y: Math.max(0, Math.min(cropCanvas.height, e.clientY - rect.top))
            };

            const start = this.imageToolData.cropStart;
            const end = this.imageToolData.cropEnd;

            const left = Math.min(start.x, end.x);
            const top = Math.min(start.y, end.y);
            const width = Math.abs(end.x - start.x);
            const height = Math.abs(end.y - start.y);

            cropSelection.style.left = left + 'px';
            cropSelection.style.top = top + 'px';
            cropSelection.style.width = width + 'px';
            cropSelection.style.height = height + 'px';
        });

        cropCanvas.addEventListener('mouseup', () => {
            if (!this.imageToolData.isCropping) return;
            this.imageToolData.isCropping = false;

            const start = this.imageToolData.cropStart;
            const end = this.imageToolData.cropEnd;

            if (end && Math.abs(end.x - start.x) > 5 && Math.abs(end.y - start.y) > 5) {
                const scale = this.imageToolData.displayScale;
                const cropW = Math.round(Math.abs(end.x - start.x) / scale);
                const cropH = Math.round(Math.abs(end.y - start.y) / scale);
                document.getElementById('crop-info').textContent = `Seleccion: ${cropW}x${cropH} px`;
                this.updateImagePreview();
            } else {
                this.resetCropSelection();
            }
        });

        cropCanvas.addEventListener('mouseleave', () => {
            if (this.imageToolData.isCropping) {
                this.imageToolData.isCropping = false;
            }
        });

        document.getElementById('btn-reset-crop').addEventListener('click', () => {
            this.resetCropSelection();
            this.updateImagePreview();
        });

        document.getElementById('btn-preview-update').addEventListener('click', () => {
            this.updateImagePreview();
        });

        ['contrast-adjust', 'brightness-adjust', 'saturation-adjust'].forEach(id => {
            const el = document.getElementById(id);
            el.addEventListener('input', () => {
                document.getElementById(id.replace('-adjust', '-value')).textContent = el.value + '%';
            });
        });

        document.getElementById('btn-add-image').addEventListener('click', async () => {
            if (!this.imageToolData.processedBase64) return;

            const element = new ImageElement(0, 0);
            element.image = this.imageToolData.processedBase64;
            await element.loadImage(this.imageToolData.processedBase64);

            this.clockface.elements.unshift(element);
            this.selectedId = element.id;

            document.getElementById('image-modal').classList.remove('active');
            document.getElementById('image-tool-file').value = '';

            this.updatePropertiesPanel();
            this.updateLayersList();
            this.render();
        });
    }

    drawCropCanvas() {
        const img = this.imageToolData.originalImage;
        if (!img) return;

        const cropCanvas = document.getElementById('crop-canvas');
        const ctx = cropCanvas.getContext('2d');

        const maxW = 400, maxH = 300;
        const scale = Math.min(maxW / img.width, maxH / img.height, 1);
        this.imageToolData.displayScale = scale;

        cropCanvas.width = Math.round(img.width * scale);
        cropCanvas.height = Math.round(img.height * scale);

        ctx.imageSmoothingEnabled = true;
        ctx.drawImage(img, 0, 0, cropCanvas.width, cropCanvas.height);
    }

    resetCropSelection() {
        this.imageToolData.cropStart = null;
        this.imageToolData.cropEnd = null;
        document.getElementById('crop-selection').classList.remove('active');
        document.getElementById('crop-selection').style.cssText = '';
        document.getElementById('crop-info').textContent = 'Sin seleccion (usa imagen completa)';
    }

    updateImagePreview() {
        if (!this.imageToolData.originalImage) return;

        const img = this.imageToolData.originalImage;
        const canvas = document.getElementById('preview-64');
        const ctx = canvas.getContext('2d');

        const tempCanvas = document.createElement('canvas');
        tempCanvas.width = 64;
        tempCanvas.height = 64;
        const tempCtx = tempCanvas.getContext('2d');
        tempCtx.imageSmoothingEnabled = false;

        tempCtx.fillStyle = '#000';
        tempCtx.fillRect(0, 0, 64, 64);

        let sx = 0, sy = 0, sw = img.width, sh = img.height;

        const cropStart = this.imageToolData.cropStart;
        const cropEnd = this.imageToolData.cropEnd;
        const displayScale = this.imageToolData.displayScale;

        if (cropStart && cropEnd && displayScale) {
            sx = Math.round(Math.min(cropStart.x, cropEnd.x) / displayScale);
            sy = Math.round(Math.min(cropStart.y, cropEnd.y) / displayScale);
            sw = Math.round(Math.abs(cropEnd.x - cropStart.x) / displayScale);
            sh = Math.round(Math.abs(cropEnd.y - cropStart.y) / displayScale);
        }

        const scaleMode = document.getElementById('scale-mode').value;
        let dx = 0, dy = 0, dw = 64, dh = 64;

        if (scaleMode === 'contain') {
            const scale = Math.min(64 / sw, 64 / sh);
            dw = Math.floor(sw * scale);
            dh = Math.floor(sh * scale);
            dx = Math.floor((64 - dw) / 2);
            dy = Math.floor((64 - dh) / 2);
        } else if (scaleMode === 'cover') {
            const scale = Math.max(64 / sw, 64 / sh);
            const newSw = 64 / scale;
            const newSh = 64 / scale;
            sx += (sw - newSw) / 2;
            sy += (sh - newSh) / 2;
            sw = newSw;
            sh = newSh;
        }

        tempCtx.drawImage(img, sx, sy, sw, sh, dx, dy, dw, dh);

        const contrast = parseInt(document.getElementById('contrast-adjust').value) / 100;
        const brightness = parseInt(document.getElementById('brightness-adjust').value) / 100;
        const saturation = parseInt(document.getElementById('saturation-adjust').value) / 100;

        const imageData = tempCtx.getImageData(0, 0, 64, 64);
        const data = imageData.data;

        for (let i = 0; i < data.length; i += 4) {
            let r = data[i], g = data[i + 1], b = data[i + 2];

            r = ((r / 255 - 0.5) * contrast + 0.5) * 255 * brightness;
            g = ((g / 255 - 0.5) * contrast + 0.5) * 255 * brightness;
            b = ((b / 255 - 0.5) * contrast + 0.5) * 255 * brightness;

            const gray = 0.299 * r + 0.587 * g + 0.114 * b;
            r = gray + (r - gray) * saturation;
            g = gray + (g - gray) * saturation;
            b = gray + (b - gray) * saturation;

            data[i] = Math.max(0, Math.min(255, r));
            data[i + 1] = Math.max(0, Math.min(255, g));
            data[i + 2] = Math.max(0, Math.min(255, b));
        }

        const colorMode = document.getElementById('color-mode').value;
        if (colorMode !== 'full') {
            const levels = parseInt(colorMode);
            const step = 256 / levels;
            for (let i = 0; i < data.length; i += 4) {
                data[i] = Math.floor(data[i] / step) * step;
                data[i + 1] = Math.floor(data[i + 1] / step) * step;
                data[i + 2] = Math.floor(data[i + 2] / step) * step;
            }
        }

        tempCtx.putImageData(imageData, 0, 0);

        ctx.imageSmoothingEnabled = false;
        ctx.drawImage(tempCanvas, 0, 0, 128, 128);

        this.imageToolData.processedBase64 = tempCanvas.toDataURL('image/png').split(',')[1];
    }

    bindEvents() {
        this.canvas.addEventListener('mousedown', (e) => this.onMouseDown(e));
        this.canvas.addEventListener('mousemove', (e) => this.onMouseMove(e));
        this.canvas.addEventListener('mouseup', () => this.onMouseUp());
        this.canvas.addEventListener('mouseleave', () => this.onMouseUp());

        document.querySelectorAll('.tool-btn[data-tool]').forEach(btn => {
            btn.addEventListener('click', () => this.selectTool(btn.dataset.tool));
        });

        document.querySelectorAll('.tool-btn[data-action]').forEach(btn => {
            btn.addEventListener('click', () => this.doAction(btn.dataset.action));
        });

        document.getElementById('zoom-in').addEventListener('click', () => this.setZoom(this.zoom + 2));
        document.getElementById('zoom-out').addEventListener('click', () => this.setZoom(this.zoom - 2));

        document.getElementById('show-grid').addEventListener('change', (e) => {
            document.querySelector('.canvas-grid').classList.toggle('hidden', !e.target.checked);
        });

        document.getElementById('theme-name').addEventListener('input', (e) => {
            this.clockface.name = e.target.value;
        });
        document.getElementById('theme-author').addEventListener('input', (e) => {
            this.clockface.author = e.target.value;
        });
        document.getElementById('theme-delay').addEventListener('input', (e) => {
            this.clockface.delay = parseInt(e.target.value) || 250;
        });

        document.getElementById('bg-color').addEventListener('input', (e) => {
            const rgb565 = ColorUtils.hexToRgb565(e.target.value);
            this.clockface.bgColor = rgb565;
            document.getElementById('bg-color-value').textContent = rgb565;
            this.render();
        });

        this.bindElementProperties();
        this.bindModals();

        document.addEventListener('keydown', (e) => this.onKeyDown(e));
    }

    bindElementProperties() {
        const inputs = ['el-x', 'el-y', 'el-content', 'el-font', 'el-width', 'el-height', 'el-x1', 'el-y1', 'el-in-loop'];

        inputs.forEach(id => {
            const el = document.getElementById(id);
            if (!el) return;

            el.addEventListener('change', () => this.updateSelectedElement());
            if (el.type === 'number' || el.type === 'text') {
                el.addEventListener('input', () => this.updateSelectedElement());
            }
        });

        ['el-fgcolor', 'el-bgcolor', 'el-color'].forEach(id => {
            const el = document.getElementById(id);
            if (!el) return;

            el.addEventListener('input', (e) => {
                const rgb565 = ColorUtils.hexToRgb565(e.target.value);
                document.getElementById(`${id}-value`).textContent = rgb565;
                this.updateSelectedElement();
            });
        });

        document.getElementById('el-image-file').addEventListener('change', async (e) => {
            const file = e.target.files[0];
            if (!file) return;

            const reader = new FileReader();
            reader.onload = async (event) => {
                const element = this.clockface.getElementById(this.selectedId);
                if (element && element.type === 'image') {
                    const base64 = event.target.result.split(',')[1];
                    element.image = base64;
                    await element.loadImage(base64);
                    this.render();
                }
            };
            reader.readAsDataURL(file);
        });

        document.querySelectorAll('.btn-preset').forEach(btn => {
            btn.addEventListener('click', () => {
                const format = btn.dataset.format;
                document.getElementById('el-content').value = format;
                this.updateSelectedElement();
            });
        });
    }

    bindModals() {
        document.getElementById('btn-new').addEventListener('click', () => {
            if (confirm('Crear nuevo clockface? Se perderan los cambios no guardados.')) {
                this.clockface = new Clockface();
                this.selectedId = null;
                this.updateUI();
                this.render();
            }
        });

        document.getElementById('btn-import').addEventListener('click', () => {
            document.getElementById('import-json').value = '';
            document.getElementById('import-modal').classList.add('active');
        });

        document.getElementById('btn-export').addEventListener('click', () => {
            const json = JSON.stringify(this.clockface.toJSON(), null, 2);
            document.getElementById('export-json').value = json;
            document.getElementById('export-modal').classList.add('active');
        });

        document.getElementById('btn-do-import').addEventListener('click', () => {
            try {
                const json = JSON.parse(document.getElementById('import-json').value);
                this.clockface = Clockface.fromJSON(json);
                this.selectedId = null;
                this.loadImagesAsync();
                this.updateUI();
                this.render();
                document.getElementById('import-modal').classList.remove('active');
            } catch (e) {
                alert('Error al parsear JSON: ' + e.message);
            }
        });

        document.getElementById('btn-copy-json').addEventListener('click', async () => {
            const json = document.getElementById('export-json').value;
            try {
                await navigator.clipboard.writeText(json);
                alert('JSON copiado al portapapeles');
            } catch (e) {
                const textarea = document.getElementById('export-json');
                textarea.select();
                document.execCommand('copy');
                alert('JSON copiado al portapapeles');
            }
        });

        document.getElementById('btn-download-json').addEventListener('click', () => {
            const json = document.getElementById('export-json').value;
            const blob = new Blob([json], { type: 'application/json' });
            const url = URL.createObjectURL(blob);
            const a = document.createElement('a');
            a.href = url;
            a.download = `${this.clockface.name}.json`;
            a.click();
            URL.revokeObjectURL(url);
        });

        this.bindImageTool();

        document.querySelectorAll('.modal-close').forEach(btn => {
            btn.addEventListener('click', () => {
                document.querySelectorAll('.modal').forEach(m => m.classList.remove('active'));
            });
        });

        document.querySelectorAll('.modal').forEach(modal => {
            modal.addEventListener('click', (e) => {
                if (e.target === modal) modal.classList.remove('active');
            });
        });
    }

    async loadImagesAsync() {
        for (const el of this.clockface.elements) {
            if (el.type === 'image' && el.image) {
                await el.loadImage(el.image);
            }
        }
        this.render();
    }

    selectTool(tool) {
        this.currentTool = tool;
        document.querySelectorAll('.tool-btn[data-tool]').forEach(btn => {
            btn.classList.toggle('active', btn.dataset.tool === tool);
        });
    }

    doAction(action) {
        if (!this.selectedId) return;

        switch (action) {
            case 'delete':
                this.clockface.removeElement(this.selectedId);
                this.selectedId = null;
                break;
            case 'duplicate':
                const el = this.clockface.getElementById(this.selectedId);
                if (el) {
                    const json = el.toJSON();
                    json.x += 5;
                    json.y += 5;
                    const newEl = ClockfaceElement.fromJSON(json);
                    newEl.inLoop = el.inLoop;
                    this.clockface.addElement(newEl);
                    this.selectedId = newEl.id;
                }
                break;
            case 'move-up':
                this.clockface.moveElement(this.selectedId, 'up');
                break;
            case 'move-down':
                this.clockface.moveElement(this.selectedId, 'down');
                break;
        }
        this.updateLayersList();
        this.render();
    }

    setZoom(level) {
        this.zoom = Math.max(4, Math.min(16, level));
        this.updateCanvasSize();
    }

    getCanvasCoords(e) {
        const rect = this.canvas.getBoundingClientRect();
        const x = Math.floor((e.clientX - rect.left) / this.zoom);
        const y = Math.floor((e.clientY - rect.top) / this.zoom);
        return { x: Math.max(0, Math.min(63, x)), y: Math.max(0, Math.min(63, y)) };
    }

    onMouseDown(e) {
        const coords = this.getCanvasCoords(e);

        if (this.currentTool === 'select') {
            const hit = this.renderer.hitTest(coords.x, coords.y, this.clockface);
            if (hit) {
                this.selectedId = hit.id;
                this.isDragging = true;
                this.dragOffset = { x: coords.x - hit.x, y: coords.y - hit.y };
            } else {
                this.selectedId = null;
            }
        } else {
            this.createElement(coords);
        }

        this.updatePropertiesPanel();
        this.updateLayersList();
        this.render();
    }

    onMouseMove(e) {
        const coords = this.getCanvasCoords(e);
        document.getElementById('mouse-pos').textContent = `X: ${coords.x}, Y: ${coords.y}`;

        if (this.isDragging && this.selectedId) {
            const element = this.clockface.getElementById(this.selectedId);
            if (element) {
                element.x = Math.max(0, Math.min(63, coords.x - this.dragOffset.x));
                element.y = Math.max(0, Math.min(63, coords.y - this.dragOffset.y));
                this.updatePropertiesPanel();
                this.render();
            }
        }
    }

    onMouseUp() {
        this.isDragging = false;
    }

    onKeyDown(e) {
        if (e.target.tagName === 'INPUT' || e.target.tagName === 'TEXTAREA') return;

        if (e.key === 'Delete' || e.key === 'Backspace') {
            if (this.selectedId) {
                this.doAction('delete');
            }
        }

        if (e.key === 'Escape') {
            this.selectedId = null;
            this.updatePropertiesPanel();
            this.render();
        }

        if (this.selectedId) {
            const element = this.clockface.getElementById(this.selectedId);
            if (!element) return;

            let moved = false;
            switch (e.key) {
                case 'ArrowUp':
                    element.y = Math.max(0, element.y - 1);
                    moved = true;
                    break;
                case 'ArrowDown':
                    element.y = Math.min(63, element.y + 1);
                    moved = true;
                    break;
                case 'ArrowLeft':
                    element.x = Math.max(0, element.x - 1);
                    moved = true;
                    break;
                case 'ArrowRight':
                    element.x = Math.min(63, element.x + 1);
                    moved = true;
                    break;
            }

            if (moved) {
                e.preventDefault();
                this.updatePropertiesPanel();
                this.render();
            }
        }
    }

    createElement(coords) {
        let element;

        switch (this.currentTool) {
            case 'datetime':
                element = new DateTimeElement(coords.x, coords.y);
                break;
            case 'text':
                element = new TextElement(coords.x, coords.y);
                break;
            case 'image':
                element = new ImageElement(coords.x, coords.y);
                break;
            case 'rect':
                element = new RectElement(coords.x, coords.y);
                break;
            case 'fillrect':
                element = new FillRectElement(coords.x, coords.y);
                break;
            case 'line':
                element = new LineElement(coords.x, coords.y);
                break;
            default:
                return;
        }

        this.clockface.addElement(element);
        this.selectedId = element.id;
        this.selectTool('select');
    }

    updateSelectedElement() {
        if (!this.selectedId) return;

        const element = this.clockface.getElementById(this.selectedId);
        if (!element) return;

        element.x = parseInt(document.getElementById('el-x').value) || 0;
        element.y = parseInt(document.getElementById('el-y').value) || 0;

        if (element.type === 'datetime' || element.type === 'text') {
            element.content = document.getElementById('el-content').value;
            element.font = document.getElementById('el-font').value;
            element.fgColor = ColorUtils.hexToRgb565(document.getElementById('el-fgcolor').value);
            element.bgColor = ColorUtils.hexToRgb565(document.getElementById('el-bgcolor').value);
        }

        if (element.type === 'rect' || element.type === 'fillrect') {
            element.width = parseInt(document.getElementById('el-width').value) || 10;
            element.height = parseInt(document.getElementById('el-height').value) || 10;
            element.color = ColorUtils.hexToRgb565(document.getElementById('el-color').value);
        }

        if (element.type === 'line') {
            element.x1 = parseInt(document.getElementById('el-x1').value) || 0;
            element.y1 = parseInt(document.getElementById('el-y1').value) || 0;
            element.color = ColorUtils.hexToRgb565(document.getElementById('el-color').value);
        }

        element.inLoop = document.getElementById('el-in-loop').checked;

        this.render();
    }

    updatePropertiesPanel() {
        const panel = document.getElementById('element-properties');

        if (!this.selectedId) {
            panel.style.display = 'none';
            return;
        }

        const element = this.clockface.getElementById(this.selectedId);
        if (!element) {
            panel.style.display = 'none';
            return;
        }

        panel.style.display = 'block';
        document.getElementById('element-type').textContent = element.type;

        document.getElementById('el-x').value = element.x;
        document.getElementById('el-y').value = element.y;
        document.getElementById('el-in-loop').checked = element.inLoop;

        const showFields = {
            'fg-content': ['datetime', 'text'],
            'fg-format-presets': ['datetime'],
            'fg-font': ['datetime', 'text'],
            'fg-fgcolor': ['datetime', 'text'],
            'fg-bgcolor': ['datetime', 'text'],
            'fg-size': ['rect', 'fillrect'],
            'fg-endpoint': ['line'],
            'fg-color': ['rect', 'fillrect', 'line'],
            'fg-image': ['image']
        };

        for (const [fieldId, types] of Object.entries(showFields)) {
            const field = document.getElementById(fieldId);
            if (field) {
                field.style.display = types.includes(element.type) ? 'block' : 'none';
            }
        }

        if (element.type === 'datetime' || element.type === 'text') {
            document.getElementById('el-content').value = element.content;
            document.getElementById('el-font').value = element.font;
            document.getElementById('el-fgcolor').value = ColorUtils.rgb565ToHex(element.fgColor);
            document.getElementById('el-fgcolor-value').textContent = element.fgColor;
            document.getElementById('el-bgcolor').value = ColorUtils.rgb565ToHex(element.bgColor);
            document.getElementById('el-bgcolor-value').textContent = element.bgColor;
        }

        if (element.type === 'rect' || element.type === 'fillrect') {
            document.getElementById('el-width').value = element.width;
            document.getElementById('el-height').value = element.height;
            document.getElementById('el-color').value = ColorUtils.rgb565ToHex(element.color);
            document.getElementById('el-color-value').textContent = element.color;
        }

        if (element.type === 'line') {
            document.getElementById('el-x1').value = element.x1;
            document.getElementById('el-y1').value = element.y1;
            document.getElementById('el-color').value = ColorUtils.rgb565ToHex(element.color);
            document.getElementById('el-color-value').textContent = element.color;
        }
    }

    updateLayersList() {
        const list = document.getElementById('layers-list');
        list.innerHTML = '';

        for (let i = this.clockface.elements.length - 1; i >= 0; i--) {
            const el = this.clockface.elements[i];
            const item = document.createElement('div');
            item.className = 'layer-item' + (el.id === this.selectedId ? ' selected' : '');
            item.innerHTML = `
                <span>${el.type}${el.content ? ': ' + el.content.substring(0, 10) : ''}</span>
                <span class="layer-type">${el.inLoop ? 'loop' : 'setup'}</span>
            `;
            item.addEventListener('click', () => {
                this.selectedId = el.id;
                this.updatePropertiesPanel();
                this.updateLayersList();
                this.render();
            });
            list.appendChild(item);
        }
    }

    updateUI() {
        document.getElementById('theme-name').value = this.clockface.name;
        document.getElementById('theme-author').value = this.clockface.author;
        document.getElementById('theme-delay').value = this.clockface.delay;
        document.getElementById('bg-color').value = ColorUtils.rgb565ToHex(this.clockface.bgColor);
        document.getElementById('bg-color-value').textContent = this.clockface.bgColor;

        this.updatePropertiesPanel();
        this.updateLayersList();
    }

    render() {
        this.renderer.render(this.clockface, this.selectedId);
    }

    startClock() {
        setInterval(() => {
            const hasDateTime = this.clockface.elements.some(el => el.type === 'datetime');
            if (hasDateTime) {
                this.render();
            }
        }, 1000);
    }
}

document.addEventListener('DOMContentLoaded', () => {
    window.editor = new ClockfaceEditor();
});
