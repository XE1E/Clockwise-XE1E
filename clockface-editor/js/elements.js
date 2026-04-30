class ClockfaceElement {
    constructor(type, x = 0, y = 0) {
        this.id = this.generateId();
        this.type = type;
        this.x = x;
        this.y = y;
        this.inLoop = false;
    }

    generateId() {
        return Math.random().toString(36).substring(2, 8);
    }

    toJSON() {
        return {
            type: this.type,
            x: this.x,
            y: this.y,
            id: this.id
        };
    }

    static fromJSON(data) {
        let element;
        switch (data.type) {
            case 'datetime':
                element = new DateTimeElement(data.x, data.y);
                element.content = data.content || 'H:i:s';
                element.font = data.font || '';
                element.fgColor = data.fgColor || 65535;
                element.bgColor = data.bgColor || 0;
                break;
            case 'text':
                element = new TextElement(data.x, data.y);
                element.content = data.content || 'Text';
                element.font = data.font || '';
                element.fgColor = data.fgColor || 65535;
                element.bgColor = data.bgColor || 0;
                break;
            case 'image':
                element = new ImageElement(data.x, data.y);
                element.image = data.image || '';
                break;
            case 'rect':
                element = new RectElement(data.x, data.y);
                element.width = data.width || 10;
                element.height = data.height || 10;
                element.color = data.color || 65535;
                break;
            case 'fillrect':
                element = new FillRectElement(data.x, data.y);
                element.width = data.width || 10;
                element.height = data.height || 10;
                element.color = data.color || 65535;
                break;
            case 'line':
                element = new LineElement(data.x, data.y);
                element.x1 = data.x1 || data.x + 10;
                element.y1 = data.y1 || data.y + 10;
                element.color = data.color || 65535;
                break;
            default:
                return null;
        }
        if (data.id) element.id = data.id;
        return element;
    }
}

class DateTimeElement extends ClockfaceElement {
    constructor(x = 0, y = 0) {
        super('datetime', x, y);
        this.content = 'H:i:s';
        this.font = 'square';
        this.fgColor = 65535;
        this.bgColor = 0;
        this.inLoop = true;
    }

    toJSON() {
        return {
            type: this.type,
            x: this.x,
            y: this.y,
            content: this.content,
            font: this.font,
            fgColor: this.fgColor,
            bgColor: this.bgColor,
            id: this.id
        };
    }

    getDisplayText() {
        const now = new Date();
        let text = this.content;
        text = text.replace(/H/g, now.getHours().toString().padStart(2, '0'));
        text = text.replace(/h/g, (now.getHours() % 12 || 12).toString().padStart(2, '0'));
        text = text.replace(/i/g, now.getMinutes().toString().padStart(2, '0'));
        text = text.replace(/s/g, now.getSeconds().toString().padStart(2, '0'));
        text = text.replace(/A/g, now.getHours() >= 12 ? 'PM' : 'AM');
        text = text.replace(/d/g, now.getDate().toString().padStart(2, '0'));
        text = text.replace(/m/g, (now.getMonth() + 1).toString().padStart(2, '0'));
        text = text.replace(/Y/g, now.getFullYear().toString());
        return text;
    }
}

class TextElement extends ClockfaceElement {
    constructor(x = 0, y = 0) {
        super('text', x, y);
        this.content = 'Text';
        this.font = 'picopixel';
        this.fgColor = 65535;
        this.bgColor = 0;
    }

    toJSON() {
        return {
            type: this.type,
            x: this.x,
            y: this.y,
            content: this.content,
            font: this.font,
            fgColor: this.fgColor,
            bgColor: this.bgColor,
            id: this.id
        };
    }
}

class ImageElement extends ClockfaceElement {
    constructor(x = 0, y = 0) {
        super('image', x, y);
        this.image = '';
        this.imageData = null;
        this.width = 0;
        this.height = 0;
    }

    toJSON() {
        return {
            type: this.type,
            x: this.x,
            y: this.y,
            image: this.image,
            id: this.id
        };
    }

    loadImage(base64) {
        return new Promise((resolve, reject) => {
            const img = new Image();
            img.onload = () => {
                this.imageData = img;
                this.width = img.width;
                this.height = img.height;
                resolve(img);
            };
            img.onerror = reject;
            img.src = base64.startsWith('data:') ? base64 : `data:image/png;base64,${base64}`;
        });
    }
}

class RectElement extends ClockfaceElement {
    constructor(x = 0, y = 0) {
        super('rect', x, y);
        this.width = 10;
        this.height = 10;
        this.color = 65535;
    }

    toJSON() {
        return {
            type: this.type,
            x: this.x,
            y: this.y,
            width: this.width,
            height: this.height,
            color: this.color
        };
    }
}

class FillRectElement extends ClockfaceElement {
    constructor(x = 0, y = 0) {
        super('fillrect', x, y);
        this.width = 10;
        this.height = 10;
        this.color = 65535;
    }

    toJSON() {
        return {
            type: this.type,
            x: this.x,
            y: this.y,
            width: this.width,
            height: this.height,
            color: this.color
        };
    }
}

class LineElement extends ClockfaceElement {
    constructor(x = 0, y = 0) {
        super('line', x, y);
        this.x1 = x + 10;
        this.y1 = y + 10;
        this.color = 65535;
    }

    toJSON() {
        return {
            type: this.type,
            x: this.x,
            y: this.y,
            x1: this.x1,
            y1: this.y1,
            color: this.color
        };
    }
}

class Clockface {
    constructor() {
        this.name = 'mi-clockface';
        this.version = 1;
        this.author = 'XE1E';
        this.bgColor = 0;
        this.delay = 250;
        this.elements = [];
        this.sprites = [];
    }

    addElement(element) {
        this.elements.push(element);
        return element;
    }

    removeElement(id) {
        const index = this.elements.findIndex(el => el.id === id);
        if (index !== -1) {
            this.elements.splice(index, 1);
            return true;
        }
        return false;
    }

    getElementById(id) {
        return this.elements.find(el => el.id === id);
    }

    moveElement(id, direction) {
        const index = this.elements.findIndex(el => el.id === id);
        if (index === -1) return false;

        if (direction === 'up' && index > 0) {
            [this.elements[index], this.elements[index - 1]] =
            [this.elements[index - 1], this.elements[index]];
            return true;
        }
        if (direction === 'down' && index < this.elements.length - 1) {
            [this.elements[index], this.elements[index + 1]] =
            [this.elements[index + 1], this.elements[index]];
            return true;
        }
        return false;
    }

    toJSON() {
        const setup = [];
        const loop = [];

        for (const el of this.elements) {
            const json = el.toJSON();
            if (el.inLoop) {
                loop.push(json);
            } else {
                setup.push(json);
            }
        }

        return {
            name: this.name,
            version: this.version,
            author: this.author,
            bgColor: this.bgColor,
            delay: this.delay,
            setup: setup,
            sprites: this.sprites,
            loop: loop
        };
    }

    static fromJSON(json) {
        const cf = new Clockface();
        cf.name = json.name || 'imported';
        cf.version = json.version || 1;
        cf.author = json.author || '';
        cf.bgColor = json.bgColor || 0;
        cf.delay = json.delay || 250;
        cf.sprites = json.sprites || [];

        if (json.setup) {
            for (const data of json.setup) {
                const el = ClockfaceElement.fromJSON(data);
                if (el) {
                    el.inLoop = false;
                    cf.elements.push(el);
                }
            }
        }

        if (json.loop) {
            for (const data of json.loop) {
                const el = ClockfaceElement.fromJSON(data);
                if (el) {
                    el.inLoop = true;
                    cf.elements.push(el);
                }
            }
        }

        return cf;
    }
}
