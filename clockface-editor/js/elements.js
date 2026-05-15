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
            case 'circle':
                element = new CircleElement(data.x, data.y);
                element.radius = data.radius || 5;
                element.color = data.color || 65535;
                break;
            case 'fillcircle':
                element = new FillCircleElement(data.x, data.y);
                element.radius = data.radius || 5;
                element.color = data.color || 65535;
                break;
            case 'sprite':
                element = new SpriteElement(data.x, data.y);
                element.sprite = data.sprite || 0;
                element.frameDelay = data.frameDelay || 100;
                element.loopDelay = data.loopDelay || 0;
                element.moveX = data.moveX || 0;
                element.moveY = data.moveY || 0;
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

        // Time in words support
        if (text === 'Hw') {
            return this.hourToWords(now.getHours());
        }
        if (text === 'iw') {
            return this.minuteToWords(now.getMinutes());
        }

        const diasSemana = ['Domingo', 'Lunes', 'Martes', 'Miercoles', 'Jueves', 'Viernes', 'Sabado'];
        const diasCortos = ['Dom', 'Lun', 'Mar', 'Mie', 'Jue', 'Vie', 'Sab'];
        const meses = ['Enero', 'Febrero', 'Marzo', 'Abril', 'Mayo', 'Junio',
                       'Julio', 'Agosto', 'Septiembre', 'Octubre', 'Noviembre', 'Diciembre'];
        const mesesCortos = ['Ene', 'Feb', 'Mar', 'Abr', 'May', 'Jun',
                             'Jul', 'Ago', 'Sep', 'Oct', 'Nov', 'Dic'];

        const replacements = {
            'l': diasSemana[now.getDay()],
            'D': diasCortos[now.getDay()],
            'F': meses[now.getMonth()],
            'M': mesesCortos[now.getMonth()],
            'Y': now.getFullYear().toString(),
            'y': now.getFullYear().toString().slice(-2),
            'H': now.getHours().toString().padStart(2, '0'),
            'h': (now.getHours() % 12 || 12).toString().padStart(2, '0'),
            'i': now.getMinutes().toString().padStart(2, '0'),
            's': now.getSeconds().toString().padStart(2, '0'),
            'A': now.getHours() >= 12 ? 'PM' : 'AM',
            'd': now.getDate().toString().padStart(2, '0'),
            'j': now.getDate().toString(),
            'm': (now.getMonth() + 1).toString().padStart(2, '0')
        };

        let result = '';
        for (let i = 0; i < text.length; i++) {
            const char = text[i];
            if (replacements.hasOwnProperty(char)) {
                result += replacements[char];
            } else {
                result += char;
            }
        }
        return result;
    }

    hourToWords(h) {
        const hours = [
            'twelve', 'one', 'two', 'three', 'four', 'five',
            'six', 'seven', 'eight', 'nine', 'ten', 'eleven', 'twelve'
        ];
        let hour12 = h % 12;
        if (hour12 === 0) hour12 = 12;
        if (h === 0) return 'mid\nnight';
        if (h === 12) return 'noon';
        return hours[hour12];
    }

    minuteToWords(m) {
        const ones = [
            '', 'one', 'two', 'three', 'four', 'five',
            'six', 'seven', 'eight', 'nine', 'ten',
            'eleven', 'twelve', 'thirteen', 'fourteen', 'fifteen',
            'sixteen', 'seventeen', 'eighteen', 'nineteen'
        ];
        const tens = ['', '', 'twenty', 'thirty', 'forty', 'fifty'];

        if (m === 0) return "o'clock";
        if (m === 30) return 'thirty';
        if (m < 10) return 'oh ' + ones[m];
        if (m < 20) return ones[m];

        const t = Math.floor(m / 10);
        const o = m % 10;
        if (o === 0) return tens[t];
        return tens[t] + '\n' + ones[o];
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
            color: this.color,
            id: this.id
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
            color: this.color,
            id: this.id
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
            color: this.color,
            id: this.id
        };
    }
}

class CircleElement extends ClockfaceElement {
    constructor(x = 0, y = 0) {
        super('circle', x, y);
        this.radius = 5;
        this.color = 65535;
    }

    toJSON() {
        return {
            type: this.type,
            x: this.x,
            y: this.y,
            radius: this.radius,
            color: this.color,
            id: this.id
        };
    }
}

class FillCircleElement extends ClockfaceElement {
    constructor(x = 0, y = 0) {
        super('fillcircle', x, y);
        this.radius = 5;
        this.color = 65535;
    }

    toJSON() {
        return {
            type: this.type,
            x: this.x,
            y: this.y,
            radius: this.radius,
            color: this.color,
            id: this.id
        };
    }
}

class SpriteElement extends ClockfaceElement {
    constructor(x = 0, y = 0) {
        super('sprite', x, y);
        this.sprite = 0;
        this.frameDelay = 100;
        this.loopDelay = 0;
        this.moveX = 0;
        this.moveY = 0;
        this.inLoop = true;
        this.currentFrame = 0;
        this.frameImages = [];
    }

    toJSON() {
        const json = {
            type: this.type,
            x: this.x,
            y: this.y,
            sprite: this.sprite,
            frameDelay: this.frameDelay,
            id: this.id
        };
        if (this.loopDelay > 0) json.loopDelay = this.loopDelay;
        if (this.moveX !== 0) json.moveX = this.moveX;
        if (this.moveY !== 0) json.moveY = this.moveY;
        return json;
    }

    async loadFrames(sprites) {
        if (!sprites || !sprites[this.sprite]) return;
        const spriteData = sprites[this.sprite];
        this.frameImages = [];

        for (const frame of spriteData.frames || []) {
            if (frame.image) {
                const img = new Image();
                await new Promise((resolve, reject) => {
                    img.onload = resolve;
                    img.onerror = reject;
                    img.src = frame.image.startsWith('data:') ? frame.image : `data:image/png;base64,${frame.image}`;
                });
                this.frameImages.push(img);
            }
        }
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

        if (direction === 'up' && index < this.elements.length - 1) {
            [this.elements[index], this.elements[index + 1]] =
            [this.elements[index + 1], this.elements[index]];
            return true;
        }
        if (direction === 'down' && index > 0) {
            [this.elements[index], this.elements[index - 1]] =
            [this.elements[index - 1], this.elements[index]];
            return true;
        }
        return false;
    }

    addSprite(frames = []) {
        const sprite = { frames: frames };
        this.sprites.push(sprite);
        return this.sprites.length - 1;
    }

    removeSprite(index) {
        if (index >= 0 && index < this.sprites.length) {
            this.sprites.splice(index, 1);
            for (const el of this.elements) {
                if (el.type === 'sprite') {
                    if (el.sprite === index) el.sprite = 0;
                    else if (el.sprite > index) el.sprite--;
                }
            }
            return true;
        }
        return false;
    }

    addFrameToSprite(spriteIndex, imageBase64) {
        if (spriteIndex >= 0 && spriteIndex < this.sprites.length) {
            this.sprites[spriteIndex].frames.push({ image: imageBase64 });
            return true;
        }
        return false;
    }

    removeFrameFromSprite(spriteIndex, frameIndex) {
        if (spriteIndex >= 0 && spriteIndex < this.sprites.length) {
            const sprite = this.sprites[spriteIndex];
            if (frameIndex >= 0 && frameIndex < sprite.frames.length) {
                sprite.frames.splice(frameIndex, 1);
                return true;
            }
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
            sprites: this.sprites.map(s => s.frames || s),
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

        if (json.sprites && Array.isArray(json.sprites)) {
            cf.sprites = json.sprites.map(sprite => {
                if (Array.isArray(sprite)) {
                    return { frames: sprite };
                }
                return sprite.frames ? sprite : { frames: [] };
            });
        } else {
            cf.sprites = [];
        }

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
