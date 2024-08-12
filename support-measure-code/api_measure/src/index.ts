import express from 'express'
import { WebSocket } from 'ws'
import http from 'http'
import path from 'path'
import fs from 'fs'

const app = express()
const server = http.createServer(app)
const wss = new WebSocket.Server({ port: 8080 })

const dataPath = path.join(process.cwd(), 'data')
const clients: Set<WebSocket> = new Set()
const chunkAllSignalsData: any[] = []
const mpu6050SignalsData: any[] = []
let count = 0
let sleepStageLabel = 'AWAKE'

function sendToClient(socket: WebSocket, message: string) {
    for (const client of clients) {
        client.send(String(message))
    }
}

wss.on('connection', (socket) => {
    console.log('Client đã kết nối')

    // Thêm client mới vào danh sách
    clients.add(socket)

    // Xử lý tin nhắn từ client
    socket.on('message', async (message: any) => {
        console.log(`Nhận tin nhắn: ${String(message)}`)

        try {
            const obj = JSON.parse(message)
    
            switch (obj!.header) {
    
            case 'CONNECT_DEVICES_TO_WS_SERVER':
            case 'MEASUREMENT_CMD': {
                const obj = JSON.parse(message)
                if (obj.choice === 1) {
                    sleepStageLabel = obj.label
                }
                console.log(sleepStageLabel);
                sendToClient(socket, message)
                break
            }

            case 'RESTART_CMD': {
                chunkAllSignalsData.splice(0, chunkAllSignalsData.length)
                sendToClient(socket, message)
                break
            }

            case 'ALL_SIGNALS_DATA': {
                chunkAllSignalsData.push(obj)

                if (obj?.ppgSignal) {
                    if (chunkAllSignalsData.length > 7) {
                        chunkAllSignalsData.splice(0, chunkAllSignalsData.length)

                        sendToClient(socket, JSON.stringify({
                            header: 'MEASUREMENT_CMD',
                            choice: 1
                        }))
                        
                        return
                    }

                    const now = new Date().getTime()

                    const ppgData = chunkAllSignalsData.pop()
                    fs.writeFile(path.join(dataPath, 'SleepStage', sleepStageLabel, `${'ppgSignal'}_${now}.csv`), ppgData['ppgSignal'].split(',').join(','), (err) => {
                        if (err) console.log(err)
                    })

                    const mpu6050Data = {
                        ...chunkAllSignalsData.pop().mpu6050Signals,
                        ...chunkAllSignalsData.pop().mpu6050Signals,
                        ...chunkAllSignalsData.pop().mpu6050Signals,
                        ...chunkAllSignalsData.pop().mpu6050Signals,
                        ...chunkAllSignalsData.pop().mpu6050Signals,
                        ...chunkAllSignalsData.pop().mpu6050Signals,
                    }
        
                    for (const key of Object.keys(mpu6050Data)) {
                        if (key !== 'header') {
                            fs.writeFile(path.join(dataPath, 'SleepStage', sleepStageLabel, `${key}_${now}.csv`), mpu6050Data[key].split(',').join(','), (err) => {
                                if (err) console.log(err)
                            })
                        }
                    }

                    ++count
                    console.log(`Đã đo được: ${count} tập tín hiệu`)
                    sendToClient(socket, JSON.stringify({
                        header: 'COUNT',
                        count
                    }))
                    
                    if (count % 5 == 0) {
                        chunkAllSignalsData.splice(0, chunkAllSignalsData.length)
                        sendToClient(socket, JSON.stringify({
                            header: 'RESTART_CMD'
                        }))
                    } else {
                        sendToClient(socket, JSON.stringify({
                            header: 'MEASUREMENT_CMD',
                            choice: 1
                        }))
                    }
                }
                break
            }
    
            case 'MPU6050_SIGNALS_DATA': {
                mpu6050SignalsData.push(obj)
                const data = {
                    header: 'RECEIVED_DATA',
                    choice: 0
                }
                sendToClient(socket, JSON.stringify(data))
                break
            }
    
            case 'SAVING_REQUEST': {
                const success = {
                    header: 'SAVING_RESPONSE',
                    data: true
                }
    
                const failure = {
                    header: 'SAVING_RESPONSE',
                    data: false
                }
    
                if (obj!.isSave) {
                    if (obj!.choice === 0) {
                        try {
                            const data = mpu6050SignalsData.pop()
                            const now = new Date().getTime()
            
                            for (const key of Object.keys(data)) {
                                if (key !== 'header') {
                                    await fs.promises.writeFile(path.join(dataPath, obj.subject, obj.label, `${key}_${now}.csv`), data[key].split(',').join(','))
                                }
                            }
                            sendToClient(socket, JSON.stringify(success))
                        } catch (error) {
                            sendToClient(socket, JSON.stringify(failure))
                        }
                    } 
                }
                break
            }}
        } catch (error) {
            console.log('')
        }
    })

    // Xử lý sự kiện đóng kết nối
    socket.on('error', () => {
        console.log('Client đã ngắt kết nối')
        clients.delete(socket)
    })
})

server.listen(8000, () => {
    console.log('listening on port 8000')
})