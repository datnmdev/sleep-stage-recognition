
const socket = new WebSocket('ws://localhost:8080')

socket.addEventListener('open', (event) => {
    alert('Đã kết nối đến ws server')

    socket.send("Hello, I'm browser")

    const connectStatusElements = document.querySelectorAll('#connectStatus div')
    for (let element of connectStatusElements) {
        if (element.getAttribute('data-status') == '0') {
            element.classList.add('hidden')
        } else {
            element.classList.remove('hidden')
        }
    }

    const mcbtn = document.getElementById('mcbtn')
    mcbtn.addEventListener('click', () => {
        const selectedDevices = document.querySelector('input[name="devices"]:checked').value
        const connectStatus = document.querySelector('#connectStatus div:not(.hidden)').getAttribute('data-status')
        const connectStatusOfDevices = document.querySelector('#connectStatusOfDevices div:not(.hidden)').getAttribute('data-status')

        if (!isNaN(connectStatus) && !isNaN(connectStatusOfDevices)) {
            const data = {
                header: 'MEASUREMENT_CMD',
                choice: Number(selectedDevices)
            }

            if (Number(selectedDevices) === 0) {
                socket.send(JSON.stringify(data))
                const loaderElement = document.getElementById('loader')
                loaderElement.classList.remove('hidden')
            } else {
                const box2Element = document.getElementById('box2')
                box2Element.classList.remove('hidden')
            }
        } else {
            alert('Vui lòng kết nối thiết bị đẩy đủ trước khi ra lệnh đo')
        }
    })

    const haCancelBtn = document.getElementById('ha-cancel-btn')
    haCancelBtn.addEventListener('click', () => {
        const comboBox = document.getElementById("humanAction")
        const data = {
            header: 'SAVING_REQUEST',
            isSave: false,
            subject: 'HumanAction',
            label: comboBox.options[comboBox.selectedIndex].value,
            choice: 0
        }

        socket.send(JSON.stringify(data))

        const box1 = document.getElementById('box1')
        box1.classList.add('hidden')
    })

    const haSubmitBtn = document.getElementById('ha-submit-btn')
    haSubmitBtn.addEventListener('click', () => {
        const comboBox = document.getElementById("humanAction")
        const data = {
            header: 'SAVING_REQUEST',
            isSave: true,
            subject: 'HumanAction',
            label: comboBox.options[comboBox.selectedIndex].value,
            choice: 0
        }

        socket.send(JSON.stringify(data))

        const box1 = document.getElementById('box1')
        box1.classList.add('hidden')
    })

    const ssCancelBtn = document.getElementById('ss-cancel-btn')
    ssCancelBtn.addEventListener('click', () => {
        const box2 = document.getElementById('box2')
        box2.classList.add('hidden')
    })

    const ssSubmitBtn = document.getElementById('ss-submit-btn')
    ssSubmitBtn.addEventListener('click', () => {
        const comboBox = document.getElementById("sleepStage")
        const data = {
            header: 'MEASUREMENT_CMD',
            choice: 1,
            label: comboBox.options[comboBox.selectedIndex].value
        }

        socket.send(JSON.stringify(data))

        const box2 = document.getElementById('box2')
        box2.classList.add('hidden')

        const box3 = document.getElementById('box3')
        const sleepStageLabel = document.getElementById('sleepStageLabel')
        sleepStageLabel.textContent = comboBox.options[comboBox.selectedIndex].value
        box3.classList.remove('hidden')
    })

    const stopBtn = document.getElementById('stopBtn')
    stopBtn.addEventListener('click', () => {
        const data = {
            header: 'RESTART_CMD',
        }

        socket.send(JSON.stringify(data))

        const box3 = document.getElementById('box3')
        box3.classList.add('hidden')
    })
})

socket.addEventListener('message', (event) => {
    console.log(event.data)
    const data = JSON.parse(event.data)

    switch (data.header) {
        case 'CONNECT_DEVICES_TO_WS_SERVER':
            const connectStatusOfDevicesElements = document.querySelectorAll('#connectStatusOfDevices div')

            if (data.data) {
                alert('Các thiết bị đã kết nối đên ws server')

                for (let element of connectStatusOfDevicesElements) {
                    if (element.getAttribute('data-status') == '0') {
                        element.classList.add('hidden')
                    } else {
                        element.classList.remove('hidden')
                    }
                }
            } else {
                alert('Các thiết bị đã ngắt kết nối đến ws server')
                for (let element of connectStatusOfDevicesElements) {
                    if (element.getAttribute('data-status') == '0') {
                        element.classList.remove('hidden')
                    } else {
                        element.classList.add('hidden')
                    }
                }
            }
            break

        case 'RECEIVED_DATA':
            const loaderElement = document.getElementById('loader')
            loaderElement.classList.add('hidden')

            if (data.choice === 0) {
                const box1 = document.getElementById('box1')
                box1.classList.remove('hidden')
            }
            break

        case 'COUNT':
            const count = document.getElementById('count')
            count.textContent = data.count
            break

        case 'SAVING_RESPONSE':
            alert(data.data ? 'Lưu thành công!' : 'Lưu thất bại!')
            break
    }
})

socket.addEventListener('close', (event) => {
    const loaderElement = document.getElementById('loader')
    loaderElement.classList.add('hidden')

    alert('Kết nối đã đóng')

    const connectStatusElements = document.querySelectorAll('#connectStatus div')
    for (let element of connectStatusElements) {
        if (element.getAttribute('data-status') == '0') {
            element.classList.remove('hidden')
        } else {
            element.classList.add('hidden')
        }
    }
})

socket.addEventListener('error', (event) => {
    alert('Đã xảy ra lỗi')
})
