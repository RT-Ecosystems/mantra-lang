/**
 * MANTRA WebAssembly Runtime
 * Enables MANTRA language to run in browsers
 * 
 * Usage:
 *   const mantra = await MantraWASM.init();
 *   const result = await mantra.run('dikhao "Hello from Browser!"');
 */

class MantraWASM {
    constructor() {
        this.module = null;
        this.memory = null;
        this.exports = {};
        this.initialized = false;
    }

    /**
     * Initialize WASM module
     */
    static async init(wasmPath = 'mantra.wasm') {
        const instance = new MantraWASM();

        try {
            // Fetch and instantiate WASM module
            const response = await fetch(wasmPath);
            const wasmBytes = await response.arrayBuffer();

            const importObject = {
                env: {
                    // Memory
                    memory: new WebAssembly.Memory({ initial: 256, maximum: 512 }),

                    // Console output
                    js_console_log: (ptr, len) => {
                        const message = instance.readString(ptr, len);
                        console.log('[MANTRA]', message);
                        instance.outputCallback?.(message);
                    },

                    js_console_error: (ptr, len) => {
                        const message = instance.readString(ptr, len);
                        console.error('[MANTRA ERROR]', message);
                        instance.errorCallback?.(message);
                    },

                    // DOM manipulation
                    js_dom_getElementById: (ptr, len) => {
                        const id = instance.readString(ptr, len);
                        const element = document.getElementById(id);
                        return element ? instance.storeObject(element) : 0;
                    },

                    js_dom_setInnerHTML: (objId, ptr, len) => {
                        const element = instance.getObject(objId);
                        const html = instance.readString(ptr, len);
                        if (element) element.innerHTML = html;
                    },

                    js_dom_setValue: (objId, ptr, len) => {
                        const element = instance.getObject(objId);
                        const value = instance.readString(ptr, len);
                        if (element) element.value = value;
                    },

                    js_dom_getValue: (objId) => {
                        const element = instance.getObject(objId);
                        if (element) {
                            return instance.storeString(element.value);
                        }
                        return 0;
                    },

                    js_dom_addEventListener: (objId, eventPtr, eventLen, callbackId) => {
                        const element = instance.getObject(objId);
                        const event = instance.readString(eventPtr, eventLen);
                        if (element) {
                            element.addEventListener(event, () => {
                                instance.exports.wasm_callback(callbackId);
                            });
                        }
                    },

                    // HTTP requests
                    js_http_get: async (urlPtr, urlLen) => {
                        const url = instance.readString(urlPtr, urlLen);
                        try {
                            const response = await fetch(url);
                            const text = await response.text();
                            return instance.storeString(text);
                        } catch (e) {
                            return instance.storeString('Error: ' + e.message);
                        }
                    },

                    js_http_post: async (urlPtr, urlLen, bodyPtr, bodyLen) => {
                        const url = instance.readString(urlPtr, urlLen);
                        const body = instance.readString(bodyPtr, bodyLen);
                        try {
                            const response = await fetch(url, {
                                method: 'POST',
                                headers: { 'Content-Type': 'application/json' },
                                body: body
                            });
                            const text = await response.text();
                            return instance.storeString(text);
                        } catch (e) {
                            return instance.storeString('Error: ' + e.message);
                        }
                    },

                    // Timer
                    js_setTimeout: (callbackId, delay) => {
                        setTimeout(() => {
                            instance.exports.wasm_callback(callbackId);
                        }, delay);
                    },

                    // Math
                    js_math_random: () => Math.random(),
                    js_math_floor: (x) => Math.floor(x),
                    js_math_ceil: (x) => Math.ceil(x),
                    js_math_sqrt: (x) => Math.sqrt(x),
                    js_math_pow: (x, y) => Math.pow(x, y),

                    // Date
                    js_date_now: () => Date.now(),

                    // LocalStorage
                    js_localStorage_getItem: (keyPtr, keyLen) => {
                        const key = instance.readString(keyPtr, keyLen);
                        const value = localStorage.getItem(key);
                        return value ? instance.storeString(value) : 0;
                    },

                    js_localStorage_setItem: (keyPtr, keyLen, valuePtr, valueLen) => {
                        const key = instance.readString(keyPtr, keyLen);
                        const value = instance.readString(valuePtr, valueLen);
                        localStorage.setItem(key, value);
                    },

                    // JSON
                    js_json_parse: (ptr, len) => {
                        const str = instance.readString(ptr, len);
                        try {
                            const obj = JSON.parse(str);
                            return instance.storeString(JSON.stringify(obj));
                        } catch (e) {
                            return instance.storeString('null');
                        }
                    },

                    js_json_stringify: (ptr, len) => {
                        const obj = instance.readString(ptr, len);
                        try {
                            return instance.storeString(JSON.stringify(obj));
                        } catch (e) {
                            return instance.storeString('null');
                        }
                    }
                }
            };

            const wasmModule = await WebAssembly.instantiate(wasmBytes, importObject);
            instance.module = wasmModule;
            instance.exports = wasmModule.instance.exports;
            instance.memory = wasmModule.instance.exports.memory;
            instance.initialized = true;

            // Initialize runtime
            instance.exports.mantra_init();

            return instance;
        } catch (error) {
            console.error('Failed to initialize MANTRA WASM:', error);
            throw error;
        }
    }

    /**
     * Run MANTRA code
     */
    async run(code) {
        if (!this.initialized) {
            throw new Error('MANTRA WASM not initialized');
        }

        // Store code in WASM memory
        const codePtr = this.storeString(code);

        // Run code
        const resultPtr = this.exports.mantra_run(codePtr);

        // Read result
        const result = this.readString(resultPtr);

        return result;
    }

    /**
     * Run MANTRA file
     */
    async runFile(url) {
        const response = await fetch(url);
        const code = await response.text();
        return this.run(code);
    }

    /**
     * Set output callback
     */
    onOutput(callback) {
        this.outputCallback = callback;
    }

    /**
     * Set error callback
     */
    onError(callback) {
        this.errorCallback = callback;
    }

    // Helper methods
    readString(ptr, len) {
        const memory = new Uint8Array(this.memory.buffer);
        const bytes = memory.slice(ptr, ptr + len);
        return new TextDecoder().decode(bytes);
    }

    storeString(str) {
        const encoder = new TextEncoder();
        const bytes = encoder.encode(str);
        const ptr = this.exports.mantra_alloc(bytes.length + 1);
        const memory = new Uint8Array(this.memory.buffer);
        memory.set(bytes, ptr);
        memory[ptr + bytes.length] = 0;
        return ptr;
    }

    storeObject(obj) {
        if (!this.objects) this.objects = new Map();
        const id = ++this.objectId || 1;
        this.objects.set(id, obj);
        return id;
    }

    getObject(id) {
        return this.objects?.get(id) || null;
    }
}

// Export for global use
window.MantraWASM = MantraWASM;

// Auto-initialize if script tag has data-autoinit
if (document.currentScript?.dataset?.autoinit) {
    MantraWASM.init().then(mantra => {
        window.mantra = mantra;
        console.log('MANTRA WASM auto-initialized');
    });
}
