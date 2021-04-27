import { EventEmitter } from 'events'

export enum Commands {
  BEGIN = 0xBE,
  UPDATE = 0xCA,
  SET_MODE = 0xCB,
  GET_MODE = 0xCC,
  SET_COLOR = 0xCD,
  GET_COLOR = 0xCE
}

const argumentCounts = <{[key: number]: number}>{
  [Commands.BEGIN]: 3,
  [Commands.SET_MODE]: 0,
  [Commands.SET_COLOR]: 0,
  [Commands.GET_MODE]: 2,
  [Commands.GET_COLOR]: 4
}

export const state = {
  numCubes: 0,
  firmwareVersion: 0
}

class Handler extends EventEmitter {
  _command: number
  _args = <number[]> []

  constructor () {
    super()
    this.on(Commands[Commands.BEGIN], this._onBegin)
  }

  parseCommand (data: Buffer) {
    for (const byte of data) {
      if (this._command == void 0 && argumentCounts[byte] !== void 0) {
        this._command = byte
      } else if (this._command !== void 0) {
        this._args.push(byte)
      } else {
        console.warn("Recieved unknown byte: 0x" + byte.toString(16).toUpperCase())
      }

      if (this._args.length === argumentCounts[this._command]) {
        this.emit(Commands[this._command], ...this._args)
        if ([Commands.GET_COLOR, Commands.GET_MODE].includes(this._command)) {
          const cube = this._args.shift()
          this.emit(Commands[this._command] + ':' + cube.toString(), ...this._args)
        }
        this._args.splice(0, this._args.length) // Clear array
        this._command = void 0
      }
    }
  }

  _onBegin (ver: number, numCubes: number) {
    state.firmwareVersion = ver
    state.numCubes = numCubes
    document.getElementById('numCubes').innerText = numCubes.toString()
    document.getElementById('version').innerText = ver.toString()
  }
}

export const handler = new Handler
