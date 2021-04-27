import { Client } from 'discord-rpc'
import SerialPort = require('serialport')

import { Commands, handler } from './handlers'

const clientId = '297197616062529538'

const rpc = new Client({
  transport: 'ipc'
})

let currentTimeout: ReturnType<typeof setTimeout>

export const initRPC = (port: SerialPort): void => {
  rpc.on('ready', () => {
    rpc.subscribe('NOTIFICATION_CREATE', undefined, () => {
      console.log('notification recieved')
      let color: number[]
      let mode: number
      handler.once(Commands[Commands.GET_COLOR] + ':0', (...args) => color = args)
      handler.once(Commands[Commands.GET_MODE] + ':0', arg => mode = arg)
      port.write([
        Commands.GET_COLOR, 0,
        Commands.GET_MODE, 0,
        Commands.SET_COLOR, 0, 114, 137, 218,
        Commands.SET_MODE, 0, 0x2
      ])
      clearTimeout(currentTimeout)
      currentTimeout = setTimeout(() => {
        // Return to previous state
        port.write([Commands.SET_MODE, 0, mode, Commands.SET_COLOR, 0, ...color])
        console.log('notification should have cleared')
      }, 60 * 1000)
    })
  })

  // @ts-expect-error redirectoUri isn't included in types
  rpc.login({ clientId, scopes: ['rpc', 'rpc.notifications.read'], redirectUri: 'http://localhost', clientSecret: 'sAYJzNkwZRBGe3rAlCr3aXJakHf5Q4kH' }).catch(console.error)
}
