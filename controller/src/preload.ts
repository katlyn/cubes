console.log(process.versions)

import * as SerialPort from 'serialport'

import { handler } from './handlers'
import { initEvents } from './ui'
import { initRPC } from './discord'

// TODO: make this configurable
const port = new SerialPort('/dev/ttyACM0', {
  baudRate: 115200
}, () => {
  setTimeout(() => {
    port.write([0xBE])
  initEvents(port)
  initRPC(port)
  }, 3000)
})

port.on('error', console.error)

port.on('data', data => {
  console.log(data)
  handler.parseCommand(data)
})

const cleanup = () => {
  port.close()
}

process.on('SIGINT', cleanup)
process.on('SIGTERM', cleanup)

// @ts-expect-error Used for debugging I guess
global.port = port
// @ts-expect-error Used for debugging I guess
global.handler = handler
