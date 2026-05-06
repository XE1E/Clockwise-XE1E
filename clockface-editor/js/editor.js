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
        this.spriteEditorState = {
            selectedSpriteIndex: -1,
            previewInterval: null,
            currentPreviewFrame: 0
        };

        this.init();
    }

    init() {
        this.updateCanvasSize();
        this.loadCustomFonts().then(() => {
            this.populateFontDropdown();
        });
        this.populateFontDropdown();
        this.bindEvents();
        this.bindReferenceImage();
        this.render();
        this.startClock();
    }

    async loadCustomFonts() {
        try {
            const loaded = await PixelFonts.loadFromFolder('fonts/');
            if (loaded.length > 0) {
                console.log('[Editor] Custom fonts loaded:', loaded);
            }
        } catch (e) {
            console.warn('[Editor] Could not load custom fonts:', e);
        }
    }

    populateFontDropdown() {
        const select = document.getElementById('el-font');
        if (!select || typeof PixelFonts === 'undefined') return;

        select.innerHTML = '<option value="">Default (picopixel)</option>';

        for (const [name, font] of Object.entries(PixelFonts)) {
            if (typeof font !== 'object' || !font.glyphs) continue;

            let maxW = 0, minYOff = 0, maxYEnd = 0;
            for (const g of font.glyphs) {
                if (!g || g[1] === 0) continue;
                const [off, w, h, xAdv, xOff, yOff] = g;
                if (w > maxW) maxW = w;
                if (yOff < minYOff) minYOff = yOff;
                const yEnd = yOff + h;
                if (yEnd > maxYEnd) maxYEnd = yEnd;
            }
            const totalH = maxYEnd - minYOff;

            const opt = document.createElement('option');
            opt.value = name;
            opt.textContent = `${name} (${maxW}x${totalH})`;
            select.appendChild(opt);
        }
    }

    updateCanvasSize() {
        this.canvas.style.width = `${64 * this.zoom}px`;
        this.canvas.style.height = `${64 * this.zoom}px`;

        const wrapper = document.querySelector('.canvas-wrapper');
        const grid = document.querySelector('.canvas-grid');

        grid.style.backgroundSize = `${this.zoom}px ${this.zoom}px`;
        wrapper.style.setProperty('--pixel-size', `${this.zoom}px`);

        document.getElementById('zoom-level').textContent = `${this.zoom}x`;

        if (this.referenceImage && !this.referenceImage.classList.contains('hidden')) {
            this.referenceImage.style.width = `${64 * this.zoom}px`;
            this.referenceImage.style.height = `${64 * this.zoom}px`;
        }
    }

    bindReferenceImage() {
        const popup = document.getElementById('reference-popup');
        const openBtn = document.getElementById('btn-open-reference');
        const closeBtn = document.getElementById('btn-close-reference');
        const fileInput = document.getElementById('reference-file');
        const opacitySlider = document.getElementById('reference-opacity');
        const referenceOptions = document.getElementById('reference-options');
        const behindCheckbox = document.getElementById('reference-behind');
        const clearBtn = document.getElementById('btn-clear-reference');
        const convertBtn = document.getElementById('btn-convert-reference');

        openBtn.addEventListener('click', () => {
            popup.style.display = 'flex';
            if (this.referenceImage.src && !this.referenceImage.classList.contains('hidden')) {
                referenceOptions.style.display = 'flex';
            }
        });

        closeBtn.addEventListener('click', () => {
            popup.style.display = 'none';
        });

        popup.addEventListener('click', (e) => {
            if (e.target === popup) popup.style.display = 'none';
        });

        fileInput.addEventListener('change', (e) => {
            const file = e.target.files[0];
            if (!file) return;

            const reader = new FileReader();
            reader.onload = (event) => {
                this.referenceImage.src = event.target.result;
                this.referenceImage.classList.remove('hidden');
                this.referenceImage.style.width = `${64 * this.zoom}px`;
                this.referenceImage.style.height = `${64 * this.zoom}px`;
                referenceOptions.style.display = 'flex';
                this.updateReferencePosition();
            };
            reader.readAsDataURL(file);
        });

        opacitySlider.addEventListener('input', (e) => {
            const opacity = e.target.value / 100;
            this.referenceImage.style.opacity = opacity;
        });

        behindCheckbox.addEventListener('change', () => {
            this.updateReferencePosition();
        });

        clearBtn.addEventListener('click', () => {
            this.clearReferenceImage();
        });

        convertBtn.addEventListener('click', () => {
            this.convertReferenceToElement();
            popup.style.display = 'none';
        });
    }

    clearReferenceImage() {
        this.referenceImage.src = '';
        this.referenceImage.classList.add('hidden');
        document.getElementById('reference-options').style.display = 'none';
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

        document.getElementById('rounded-pixels').addEventListener('change', (e) => {
            document.querySelector('.canvas-wrapper').classList.toggle('rounded-pixels', e.target.checked);
        });

        document.getElementById('theme-name').addEventListener('input', (e) => {
            this.clockface.name = e.target.value;
        });
        document.getElementById('theme-version').addEventListener('input', (e) => {
            this.clockface.version = parseInt(e.target.value) || 1;
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
        const inputs = ['el-x', 'el-y', 'el-content', 'el-font', 'el-width', 'el-height', 'el-x1', 'el-y1', 'el-radius', 'el-in-loop'];

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

        document.getElementById('el-font').addEventListener('change', () => {
            this.updateFontPreview();
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
                    this.updatePropertiesPanel();
                    this.render();
                }
            };
            reader.readAsDataURL(file);
        });

        document.getElementById('btn-apply-img-size').addEventListener('click', () => {
            this.scaleSelectedImage();
        });

        document.querySelectorAll('.btn-preset').forEach(btn => {
            btn.addEventListener('click', () => {
                const format = btn.dataset.format;
                document.getElementById('el-content').value = format;
                this.updateSelectedElement();
            });
        });
    }

    bindThumbnailGenerator() {
        this.generatedThumbs = [];

        document.getElementById('btn-thumbnail').addEventListener('click', () => {
            this.generateThumbnail();
        });

        document.getElementById('btn-batch-thumbs').addEventListener('click', () => {
            this.openBatchThumbsModal();
        });

        document.getElementById('batch-file-input').addEventListener('change', (e) => {
            if (e.target.files.length > 0) {
                this.processBatchThumbnails(Array.from(e.target.files));
            }
        });

        document.getElementById('btn-select-folder').addEventListener('click', () => {
            this.selectFolderForThumbnails();
        });

        document.getElementById('btn-download-all-thumbs').addEventListener('click', () => {
            this.downloadAllThumbs();
        });

        document.getElementById('btn-save-to-folder').addEventListener('click', () => {
            this.saveThumbsToFolder();
        });
    }

    generateThumbnail() {
        const tempCanvas = document.createElement('canvas');
        tempCanvas.width = 64;
        tempCanvas.height = 64;
        const tempCtx = tempCanvas.getContext('2d');
        tempCtx.imageSmoothingEnabled = false;

        tempCtx.drawImage(this.canvas, 0, 0);

        const link = document.createElement('a');
        link.download = `${this.clockface.name}.png`;
        link.href = tempCanvas.toDataURL('image/png');
        link.click();
    }

    openBatchThumbsModal() {
        document.getElementById('batch-thumbs-modal').classList.add('active');
        document.getElementById('batch-file-input').value = '';
        document.getElementById('batch-results').innerHTML = '<p>Selecciona archivos JSON o una carpeta con clockfaces.</p>';
        document.getElementById('batch-actions').style.display = 'none';
        this.generatedThumbs = [];
    }

    async selectFolderForThumbnails() {
        if (!('showDirectoryPicker' in window)) {
            alert('Tu navegador no soporta seleccion de carpetas. Usa Chrome o Edge.');
            return;
        }

        try {
            const dirHandle = await window.showDirectoryPicker();
            const files = [];

            for await (const entry of dirHandle.values()) {
                if (entry.kind === 'file' && entry.name.endsWith('.json')) {
                    const file = await entry.getFile();
                    files.push(file);
                }
            }

            if (files.length === 0) {
                alert('No se encontraron archivos .json en la carpeta seleccionada.');
                return;
            }

            this.processBatchThumbnails(files);
        } catch (e) {
            if (e.name !== 'AbortError') {
                console.error('Error selecting folder:', e);
            }
        }
    }

    async processBatchThumbnails(files) {
        const results = document.getElementById('batch-results');
        const actions = document.getElementById('batch-actions');
        results.innerHTML = '<p>Procesando...</p>';
        actions.style.display = 'none';

        const tempCanvas = document.createElement('canvas');
        tempCanvas.width = 64;
        tempCanvas.height = 64;
        const tempRenderer = new CanvasRenderer(tempCanvas);

        let processed = 0;
        let errors = 0;
        this.generatedThumbs = [];

        for (const file of files) {
            try {
                const text = await file.text();
                const json = JSON.parse(text);
                const clockface = Clockface.fromJSON(json);

                for (const el of clockface.elements) {
                    if (el.type === 'image' && el.image) {
                        await el.loadImage(el.image);
                    }
                    if (el.type === 'sprite') {
                        await el.loadFrames(clockface.sprites);
                    }
                }

                await new Promise(resolve => setTimeout(resolve, 100));
                tempRenderer.render(clockface, null);

                const dataUrl = tempCanvas.toDataURL('image/png');
                this.generatedThumbs.push({
                    name: clockface.name,
                    dataUrl: dataUrl
                });

                processed++;
            } catch (e) {
                console.error(`Error processing ${file.name}:`, e);
                errors++;
            }
        }

        let html = `<p>Procesados: ${processed}, Errores: ${errors}</p>`;
        html += '<div class="batch-thumbs-grid">';

        for (const thumb of this.generatedThumbs) {
            html += `
                <div class="batch-thumb-item">
                    <img src="${thumb.dataUrl}" alt="${thumb.name}">
                    <span>${thumb.name}</span>
                    <a href="${thumb.dataUrl}" download="${thumb.name}.png" class="btn btn-small">PNG</a>
                </div>
            `;
        }

        html += '</div>';
        results.innerHTML = html;

        if (this.generatedThumbs.length > 0) {
            actions.style.display = 'flex';
        }
    }

    async downloadAllThumbs() {
        if (this.generatedThumbs.length === 0) {
            alert('No hay thumbnails generados.');
            return;
        }

        for (const thumb of this.generatedThumbs) {
            const link = document.createElement('a');
            link.download = `${thumb.name}.png`;
            link.href = thumb.dataUrl;
            link.click();
            await new Promise(resolve => setTimeout(resolve, 200));
        }
    }

    async saveThumbsToFolder() {
        if (this.generatedThumbs.length === 0) {
            alert('No hay thumbnails generados.');
            return;
        }

        if (!('showDirectoryPicker' in window)) {
            alert('Tu navegador no soporta guardar en carpeta. Usa "Descargar Todos".');
            return;
        }

        try {
            const dirHandle = await window.showDirectoryPicker({ mode: 'readwrite' });

            for (const thumb of this.generatedThumbs) {
                const fileHandle = await dirHandle.getFileHandle(`${thumb.name}.png`, { create: true });
                const writable = await fileHandle.createWritable();

                const response = await fetch(thumb.dataUrl);
                const blob = await response.blob();

                await writable.write(blob);
                await writable.close();
            }

            alert(`${this.generatedThumbs.length} thumbnails guardados correctamente.`);
        } catch (e) {
            if (e.name !== 'AbortError') {
                console.error('Error saving to folder:', e);
                alert('Error al guardar: ' + e.message);
            }
        }
    }

    bindSpriteEditor() {
        document.getElementById('btn-edit-sprites').addEventListener('click', () => {
            this.openSpriteModal();
        });

        document.getElementById('btn-add-sprite').addEventListener('click', () => {
            this.clockface.addSprite([]);
            this.updateSpriteList();
            this.updateSpriteSelect();
        });

        document.getElementById('add-frame-file').addEventListener('change', async (e) => {
            const files = Array.from(e.target.files);
            if (files.length === 0) return;

            const spriteIndex = this.spriteEditorState.selectedSpriteIndex;
            if (spriteIndex < 0) return;

            for (const file of files) {
                const base64 = await this.fileToBase64(file);
                const resized = await this.resizeImageTo64(base64);
                this.clockface.addFrameToSprite(spriteIndex, resized);
            }

            this.updateFramesList();
            this.updateSpriteList();
            this.loadSpriteFrames();
            e.target.value = '';
        });

        document.getElementById('btn-play-sprite').addEventListener('click', () => {
            this.toggleSpritePreview();
        });

        ['el-sprite-index', 'el-frame-delay', 'el-loop-delay', 'el-move-x', 'el-move-y'].forEach(id => {
            const el = document.getElementById(id);
            if (el) {
                el.addEventListener('change', () => this.updateSelectedElement());
                if (el.type === 'number') {
                    el.addEventListener('input', () => this.updateSelectedElement());
                }
            }
        });
    }

    openSpriteModal() {
        document.getElementById('sprite-modal').classList.add('active');
        this.updateSpriteList();
        if (this.clockface.sprites.length > 0) {
            this.selectSpriteForEdit(0);
        } else {
            document.getElementById('frames-section').style.display = 'none';
        }
    }

    updateSpriteList() {
        const list = document.getElementById('sprite-list');
        list.innerHTML = '';

        if (this.clockface.sprites.length === 0) {
            list.innerHTML = '<div class="empty-sprites">No hay sprites. Crea uno nuevo.</div>';
            return;
        }

        this.clockface.sprites.forEach((sprite, index) => {
            const item = document.createElement('div');
            item.className = 'sprite-item' + (index === this.spriteEditorState.selectedSpriteIndex ? ' selected' : '');
            item.innerHTML = `
                <div class="sprite-item-info">
                    <span class="sprite-item-name">Sprite ${index}</span>
                    <span class="sprite-item-frames">${sprite.frames.length} frames</span>
                </div>
                <button class="sprite-item-delete" title="Eliminar">&times;</button>
            `;
            item.querySelector('.sprite-item-info').addEventListener('click', () => {
                this.selectSpriteForEdit(index);
            });
            item.querySelector('.sprite-item-delete').addEventListener('click', (e) => {
                e.stopPropagation();
                if (confirm(`Eliminar Sprite ${index}?`)) {
                    this.clockface.removeSprite(index);
                    this.updateSpriteList();
                    this.updateSpriteSelect();
                    if (this.spriteEditorState.selectedSpriteIndex >= this.clockface.sprites.length) {
                        this.spriteEditorState.selectedSpriteIndex = this.clockface.sprites.length - 1;
                    }
                    if (this.clockface.sprites.length > 0) {
                        this.selectSpriteForEdit(Math.max(0, this.spriteEditorState.selectedSpriteIndex));
                    } else {
                        document.getElementById('frames-section').style.display = 'none';
                    }
                }
            });
            list.appendChild(item);
        });
    }

    selectSpriteForEdit(index) {
        this.spriteEditorState.selectedSpriteIndex = index;
        this.stopSpritePreview();

        document.querySelectorAll('.sprite-item').forEach((el, i) => {
            el.classList.toggle('selected', i === index);
        });

        document.getElementById('frames-section').style.display = 'block';
        document.getElementById('current-sprite-index').textContent = index;
        this.updateFramesList();
        this.loadSpriteFrames();
    }

    updateFramesList() {
        const list = document.getElementById('frames-list');
        const spriteIndex = this.spriteEditorState.selectedSpriteIndex;

        if (spriteIndex < 0 || !this.clockface.sprites[spriteIndex]) {
            list.innerHTML = '<div class="empty-frames">Selecciona un sprite</div>';
            return;
        }

        const sprite = this.clockface.sprites[spriteIndex];
        list.innerHTML = '';

        if (sprite.frames.length === 0) {
            list.innerHTML = '<div class="empty-frames">Sin frames. Agrega imagenes.</div>';
            return;
        }

        sprite.frames.forEach((frame, frameIndex) => {
            const item = document.createElement('div');
            item.className = 'frame-item';
            const imgSrc = frame.image.startsWith('data:') ? frame.image : `data:image/png;base64,${frame.image}`;
            item.innerHTML = `
                <img src="${imgSrc}" alt="Frame ${frameIndex}">
                <span class="frame-item-index">${frameIndex}</span>
                <button class="frame-item-delete" title="Eliminar">&times;</button>
            `;
            item.querySelector('.frame-item-delete').addEventListener('click', (e) => {
                e.stopPropagation();
                this.clockface.removeFrameFromSprite(spriteIndex, frameIndex);
                this.updateFramesList();
                this.updateSpriteList();
                this.loadSpriteFrames();
            });
            list.appendChild(item);
        });
    }

    async loadSpriteFrames() {
        const spriteIndex = this.spriteEditorState.selectedSpriteIndex;
        if (spriteIndex < 0 || !this.clockface.sprites[spriteIndex]) return;

        for (const el of this.clockface.elements) {
            if (el.type === 'sprite' && el.sprite === spriteIndex) {
                await el.loadFrames(this.clockface.sprites);
            }
        }

        this.spriteEditorState.currentPreviewFrame = 0;
        this.drawSpritePreviewFrame();
        document.getElementById('sprite-frame-info').textContent =
            `Frame: 0/${this.clockface.sprites[spriteIndex].frames.length}`;
    }

    drawSpritePreviewFrame() {
        const spriteIndex = this.spriteEditorState.selectedSpriteIndex;
        if (spriteIndex < 0 || !this.clockface.sprites[spriteIndex]) return;

        const sprite = this.clockface.sprites[spriteIndex];
        const canvas = document.getElementById('sprite-preview-canvas');
        const ctx = canvas.getContext('2d');

        ctx.fillStyle = '#000';
        ctx.fillRect(0, 0, 128, 128);

        if (sprite.frames.length === 0) return;

        const frame = sprite.frames[this.spriteEditorState.currentPreviewFrame];
        if (!frame || !frame.image) return;

        const img = new Image();
        img.onload = () => {
            ctx.imageSmoothingEnabled = false;
            ctx.drawImage(img, 0, 0, 128, 128);
        };
        img.src = frame.image.startsWith('data:') ? frame.image : `data:image/png;base64,${frame.image}`;
    }

    toggleSpritePreview() {
        const btn = document.getElementById('btn-play-sprite');
        if (this.spriteEditorState.previewInterval) {
            this.stopSpritePreview();
            btn.textContent = 'Play';
        } else {
            this.startSpritePreview();
            btn.textContent = 'Stop';
        }
    }

    startSpritePreview() {
        const spriteIndex = this.spriteEditorState.selectedSpriteIndex;
        if (spriteIndex < 0 || !this.clockface.sprites[spriteIndex]) return;

        const sprite = this.clockface.sprites[spriteIndex];
        if (sprite.frames.length === 0) return;

        this.spriteEditorState.previewInterval = setInterval(() => {
            this.spriteEditorState.currentPreviewFrame =
                (this.spriteEditorState.currentPreviewFrame + 1) % sprite.frames.length;
            this.drawSpritePreviewFrame();
            document.getElementById('sprite-frame-info').textContent =
                `Frame: ${this.spriteEditorState.currentPreviewFrame}/${sprite.frames.length}`;
        }, 100);
    }

    stopSpritePreview() {
        if (this.spriteEditorState.previewInterval) {
            clearInterval(this.spriteEditorState.previewInterval);
            this.spriteEditorState.previewInterval = null;
        }
        document.getElementById('btn-play-sprite').textContent = 'Play';
    }

    updateSpriteSelect() {
        const select = document.getElementById('el-sprite-index');
        select.innerHTML = '';

        if (this.clockface.sprites.length === 0) {
            select.innerHTML = '<option value="0">Sin sprites</option>';
            return;
        }

        this.clockface.sprites.forEach((sprite, index) => {
            const opt = document.createElement('option');
            opt.value = index;
            opt.textContent = `Sprite ${index} (${sprite.frames.length} frames)`;
            select.appendChild(opt);
        });
    }

    fileToBase64(file) {
        return new Promise((resolve, reject) => {
            const reader = new FileReader();
            reader.onload = () => resolve(reader.result);
            reader.onerror = reject;
            reader.readAsDataURL(file);
        });
    }

    resizeImageTo64(base64) {
        return new Promise((resolve) => {
            const img = new Image();
            img.onload = () => {
                const canvas = document.createElement('canvas');
                canvas.width = 64;
                canvas.height = 64;
                const ctx = canvas.getContext('2d');
                ctx.imageSmoothingEnabled = false;

                const scale = Math.min(64 / img.width, 64 / img.height);
                const w = Math.floor(img.width * scale);
                const h = Math.floor(img.height * scale);
                const x = Math.floor((64 - w) / 2);
                const y = Math.floor((64 - h) / 2);

                ctx.fillStyle = '#000';
                ctx.fillRect(0, 0, 64, 64);
                ctx.drawImage(img, x, y, w, h);

                resolve(canvas.toDataURL('image/png').split(',')[1]);
            };
            img.src = base64;
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
        this.bindSpriteEditor();
        this.bindThumbnailGenerator();

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
            if (el.type === 'sprite') {
                await el.loadFrames(this.clockface.sprites);
            }
        }
        this.render();
    }

    async scaleSelectedImage() {
        if (!this.selectedId) return;
        const element = this.clockface.getElementById(this.selectedId);
        if (!element || element.type !== 'image' || !element.imageData) return;

        const newWidth = parseInt(document.getElementById('el-img-width').value) || 64;
        const newHeight = parseInt(document.getElementById('el-img-height').value) || 64;

        const tempCanvas = document.createElement('canvas');
        tempCanvas.width = newWidth;
        tempCanvas.height = newHeight;
        const tempCtx = tempCanvas.getContext('2d');
        tempCtx.imageSmoothingEnabled = false;

        tempCtx.drawImage(element.imageData, 0, 0, newWidth, newHeight);

        const base64 = tempCanvas.toDataURL('image/png').split(',')[1];
        element.image = base64;
        await element.loadImage(base64);

        this.updatePropertiesPanel();
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
            case 'circle':
                element = new CircleElement(coords.x, coords.y);
                break;
            case 'fillcircle':
                element = new FillCircleElement(coords.x, coords.y);
                break;
            case 'sprite':
                if (this.clockface.sprites.length === 0) {
                    alert('Primero crea un sprite en el editor de sprites (boton Editar)');
                    return;
                }
                element = new SpriteElement(coords.x, coords.y);
                element.loadFrames(this.clockface.sprites);
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

        if (element.type === 'circle' || element.type === 'fillcircle') {
            element.radius = parseInt(document.getElementById('el-radius').value) || 5;
            element.color = ColorUtils.hexToRgb565(document.getElementById('el-color').value);
        }

        if (element.type === 'sprite') {
            const newSpriteIndex = parseInt(document.getElementById('el-sprite-index').value) || 0;
            if (newSpriteIndex !== element.sprite) {
                element.sprite = newSpriteIndex;
                element.loadFrames(this.clockface.sprites);
            }
            element.frameDelay = parseInt(document.getElementById('el-frame-delay').value) || 100;
            element.loopDelay = parseInt(document.getElementById('el-loop-delay').value) || 0;
            element.moveX = parseInt(document.getElementById('el-move-x').value) || 0;
            element.moveY = parseInt(document.getElementById('el-move-y').value) || 0;
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
            'fg-colors-grid': ['datetime', 'text'],
            'fg-size': ['rect', 'fillrect'],
            'fg-radius': ['circle', 'fillcircle'],
            'fg-endpoint': ['line'],
            'fg-color': ['rect', 'fillrect', 'line', 'circle', 'fillcircle'],
            'fg-image': ['image'],
            'fg-image-size': ['image'],
            'fg-sprite-select': ['sprite'],
            'fg-sprite-timing': ['sprite'],
            'fg-sprite-move': ['sprite']
        };

        for (const [fieldId, types] of Object.entries(showFields)) {
            const field = document.getElementById(fieldId);
            if (field) {
                const displayStyle = fieldId === 'fg-colors-grid' ? 'grid' : 'block';
                field.style.display = types.includes(element.type) ? displayStyle : 'none';
            }
        }

        if (element.type === 'datetime' || element.type === 'text') {
            document.getElementById('el-content').value = element.content;
            document.getElementById('el-font').value = element.font;
            document.getElementById('el-fgcolor').value = ColorUtils.rgb565ToHex(element.fgColor);
            document.getElementById('el-fgcolor-value').textContent = element.fgColor;
            document.getElementById('el-bgcolor').value = ColorUtils.rgb565ToHex(element.bgColor);
            document.getElementById('el-bgcolor-value').textContent = element.bgColor;
            this.updateFontPreview();
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

        if (element.type === 'circle' || element.type === 'fillcircle') {
            document.getElementById('el-radius').value = element.radius;
            document.getElementById('el-color').value = ColorUtils.rgb565ToHex(element.color);
            document.getElementById('el-color-value').textContent = element.color;
        }

        if (element.type === 'image') {
            document.getElementById('el-img-width').value = element.width || 64;
            document.getElementById('el-img-height').value = element.height || 64;
        }

        if (element.type === 'sprite') {
            this.updateSpriteSelect();
            document.getElementById('el-sprite-index').value = element.sprite;
            document.getElementById('el-frame-delay').value = element.frameDelay;
            document.getElementById('el-loop-delay').value = element.loopDelay;
            document.getElementById('el-move-x').value = element.moveX;
            document.getElementById('el-move-y').value = element.moveY;
        }
    }

    updateLayersList() {
        const list = document.getElementById('layers-list');
        list.innerHTML = '';

        for (let i = this.clockface.elements.length - 1; i >= 0; i--) {
            const el = this.clockface.elements[i];
            const item = document.createElement('div');
            item.className = 'layer-item' + (el.id === this.selectedId ? ' selected' : '');

            let label = el.type;
            if (el.content) {
                label += ': ' + el.content.substring(0, 10);
            } else if (el.type === 'sprite') {
                label += ` #${el.sprite}`;
            }

            item.innerHTML = `
                <span>${label}</span>
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
        document.getElementById('theme-version').value = this.clockface.version;
        document.getElementById('theme-author').value = this.clockface.author;
        document.getElementById('theme-delay').value = this.clockface.delay;
        document.getElementById('bg-color').value = ColorUtils.rgb565ToHex(this.clockface.bgColor);
        document.getElementById('bg-color-value').textContent = this.clockface.bgColor;

        this.updatePropertiesPanel();
        this.updateLayersList();
        this.updateSpriteSelect();
    }

    updateFontPreview() {
        const canvas = document.getElementById('font-preview');
        if (!canvas) return;

        const ctx = canvas.getContext('2d');
        ctx.imageSmoothingEnabled = false;
        ctx.fillStyle = '#000';
        ctx.fillRect(0, 0, canvas.width, canvas.height);

        const fontName = document.getElementById('el-font').value || 'picopixel';
        const font = PixelFonts[fontName] || PixelFonts.picopixel;
        const fontHeight = font ? font.yAdvance : 7;
        const y = Math.floor((canvas.height - fontHeight) / 2) + fontHeight;
        PixelFonts.drawText(ctx, fontName, '12:45', 2, y, '#fff', null);
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

        setInterval(() => {
            let hasAnimatedSprite = false;
            for (const el of this.clockface.elements) {
                if (el.type === 'sprite' && el.frameImages && el.frameImages.length > 1) {
                    el.currentFrame = (el.currentFrame + 1) % el.frameImages.length;
                    hasAnimatedSprite = true;
                }
            }
            if (hasAnimatedSprite) {
                this.render();
            }
        }, 100);
    }
}

document.addEventListener('DOMContentLoaded', () => {
    window.editor = new ClockfaceEditor();
});
