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

        this.init();
    }

    init() {
        this.updateCanvasSize();
        this.bindEvents();
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

        document.getElementById('btn-copy-json').addEventListener('click', () => {
            const textarea = document.getElementById('export-json');
            textarea.select();
            document.execCommand('copy');
            alert('JSON copiado al portapapeles');
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
