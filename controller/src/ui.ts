import SerialPort = require('serialport')
import { Commands, handler, state } from './handlers'

export const initEvents = (port: SerialPort): void => {
  const cubeSelect = document.getElementById('cube-num') as HTMLInputElement
  const colorSelect = document.getElementById('color') as HTMLInputElement
  const modeSelect = document.getElementById('mode') as HTMLSelectElement

  const syncUI = () => {
    const cube = Number(cubeSelect.value)
    handler.once(Commands[Commands.GET_MODE] + ':' + cube.toString(), mode => {
      modeSelect.value = mode
    })
    handler.once(Commands[Commands.GET_COLOR] + ':' + cube.toString(), (r, g, b) => {
      let color = '#'
      color += r.toString(16).padStart(2, 0)
      color += g.toString(16).padStart(2, 0)
      color += b.toString(16).padStart(2, 0)
  
      colorSelect.value = color
    })
    port.write([Commands.GET_MODE, cube, Commands.GET_COLOR, cube])
  }

  syncUI()
  cubeSelect.addEventListener('change', syncUI)

  document.getElementById('update').addEventListener('click', () => {
    const cube = Number(cubeSelect.value)
    const mode = Number(modeSelect.value)
    const color = colorSelect.value.substr(1)
    const red = parseInt(color.substr(0, 2), 16)
    const green = parseInt(color.substr(2, 2), 16)
    const blue = parseInt(color.substr(4, 2), 16)
    port.write([Commands.SET_MODE, cube, mode, Commands.SET_COLOR, cube, red, green, blue])
  })

  document.getElementById('update-all').addEventListener('click', () => {
    const mode = Number(modeSelect.value)
    const color = colorSelect.value.substr(1)
    const red = parseInt(color.substr(0, 2), 16)
    const green = parseInt(color.substr(2, 2), 16)
    const blue = parseInt(color.substr(4, 2), 16)

    const cmd = <number[]>[]

    for (let i = 0; i < state.numCubes; i++) {
      cmd.push(Commands.SET_MODE, i, mode, Commands.SET_COLOR, i, red, green, blue)
    }
    // console.log(cmd)
    port.write(cmd)
  })

  handler.on(Commands[Commands.SET_COLOR], () => {
    const para = document.createElement('p')
    para.innerText = 'Color set'
    document.getElementById('log').prepend(para)
  })

  handler.on(Commands[Commands.SET_MODE], () => {
    const para = document.createElement('p')
    para.innerText = 'Mode set'
    document.getElementById('log').prepend(para)
  })
}
