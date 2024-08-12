# Sleep Stage Recognition
Sleep plays an extremely important role in both the physical and mental health of individuals. Monitoring and distinguishing between the two basic states of sleep and wakefulness can provide numerous benefits for improving sleep quality, diagnosing and treating sleep disorders, and supporting effective healthcare. Recognizing this importance, the topic "Monitoring Human States of Sleep and Wakefulness" has been proposed to research and develop an automated, accurate, and efficient sleep-wake state monitoring system.
This system has the potential for widespread application in various fields, including personal sleep management, healthcare for the elderly, children, and patients, ensuring security in homes, offices, public areas, and more. With the rapid development of sensor technology and signal processing, it is entirely feasible to monitor sleep-wake states accurately, efficiently, and cost-effectively.
This system promises to bring many practical benefits such as improving sleep quality, early detection of sleep disorders, supporting healthcare, and ensuring security. Researching and developing this topic contributes to enhancing the quality of life, protecting human health, and providing numerous benefits to the community.
## Machine learning model used
- Support Vector Machine (SVM)
## Tools and technologies used
- C/C++
- Python
- Colab
- Javascript
- Arduino IDE
- Visual Studio Code IDE
- Websocket
- Socket I/O
- FFT (Fast Fourier Transform)
## Relatived
- Link to source code for signal measurement: https://drive.google.com/file/d/1p9veVa27KJtBMyoCWuAsqESB48pgBxWb/view?usp=drive_link
## Preparing equipment
- MPU6050 module
- MAX30102 sensor
- LCD 16x2 and module I2C
- ESP32 WROOM 32
## Component connection diagram
![image](https://github.com/user-attachments/assets/0184b926-c505-4b2d-975f-01f2575aa5f9)
- Steps for circuit wiring:
  - Connect the VCC pin of the MPU6050 to the 3v3 pin of the ESP32S Wroom 32.
  - Connect the GND pin of the MPU6050 to the GND pin of the ESP32S Wroom 32.
  - Connect the SCL pin of the MPU6050 to the D22 pin of the ESP32S Wroom 32.
  - Connect the SDA pin of the MPU6050 to the D21 pin of the ESP32S Wroom 32.
  - Connect the VIN pin of the MAX30102 to the VIN pin of the ESP32 Wroom 32.
  - Connect the GND pin of the MAX30102 to the GND pin of the ESP32 Wroom 32.
  - Connect the SCL pin of the MAX30102 to the D22 pin of the ESP32S Wroom 32.
  - Connect the SDA pin of the MAX30102 to the D21 pin of the ESP32S Wroom 32.
  - Connect the GND pin of the I2C Module to the GND pin of the ESP32S Wroom 32.
  - Connect the VCC pin of the I2C Module to the 3v3 pin of the ESP32S Wroom 32.
  - Connect the SCL pin of the I2C Module to the D22 pin of the ESP32S Wroom 32.
  - Connect the SDA pin of the I2C Module to the D21 pin of the ESP32S Wroom 32.
## Data collection and aggregation
### Developing software to support data collection
- Measurement System Model
![image](https://github.com/user-attachments/assets/1955c6c0-ca3a-45bc-a1e2-e7d7715c6d06)
  - The software system operates as follows:
    - The WebSocket Server will connect to a Wi-Fi network, open a WebSocket connection on a specified port, and then wait for WebSocket Clients to connect.
    - The WebSocket Clients (including ESP32S Wroom 32 and Web UI) connect to the same Wi-Fi network as the WebSocket Server. They establish a connection to the WebSocket Server using the local IP address and the corresponding open port.
    - Once the WebSocket Server and the WebSocket clients are successfully connected, users can issue measurement commands through the Web Interface (Web UI).
    - When a user issues a measurement command, a request will be sent to the WebSocket Server and await a response. This request is in JSON format and includes:
      - Header: Helps the server identify the user's command.
      - Body: Contains detailed content of the command (the body content varies depending on the header).
    - Upon receiving the user's request, the server will analyze it and issue a corresponding command (also in JSON format), which is sent to the ESP32S Wroom 32 and waits for a response.
    - The ESP32S Wroom 32 receives the request from the WebSocket Server, analyzes it, and processes the command. It then sends the result back to the WebSocket Server in JSON format.
    - The WebSocket Server receives the result from the ESP32S Wroom 32 and forwards it to the user.
    - The user receives a notification along with the returned result and can issue a subsequent request (e.g., saving the signal file).
    - If the user commands the system to save the signal file, the WebSocket Server will receive the request and perform the task of writing the signal data to a file in the server’s file system, and then send a notification to the user.
- Application startup process
![image](https://github.com/user-attachments/assets/88c9373f-5820-4221-96be-dc9dff4d4045)
### Conduct measurement
- Measuring signals from the MPU6050 module
  - Description: Measure acceleration and angular velocity signals along the x, y, and z axes. The measurement results will be used to train a model for recognizing basic human activities (Human Activity Recognition).
  - Parameters:
    - Sampling frequency: 50 Hz
    - Duration: 5.12s
  - Results obtained
    ![image](https://github.com/user-attachments/assets/0d8819c6-4430-474e-ae3c-15135185ebfd)
- Measuring signals from the MPU6050 and MAX30102 modules
  - Description: Measure acceleration and angular velocity signals along the x, y, and z axes, as well as PPG (Photoplethysmogram) signals. The measurement results will be used to train a model for recognizing two basic human states—awake and asleep (One's Basic Human State Recognition - Awake and Asleep).
  - Parameters:
    - Sampling frequency: 50 Hz
    - Duration: 20.48s
    - Results obtained
      ![image](https://github.com/user-attachments/assets/52637809-a642-491e-bf11-2e6068ab21cf)
## Data preprocessing
### Data preprocessing for the signal dataset used to train the human activity recognition model
- Feature selection for training preparation (24 features total):
  - Mean Values (mean): tAcc-mean()-X, tAcc-mean()-Y, tAcc-mean()-Z, tGyro-mean()-X, tGyro-mean()-Y, tGyro-mean()-Z.
  - Standard Deviation (std): tAcc-std()-X, tAcc-std()-Y, tAcc-std()-Z, tGyro-std()-X, tGyro-std()-Y, tGyro-std()-Z.
  - Maximum Values (max): tAcc-max()-X, tAcc-max()-Y, tAcc-max()-Z, tGyro-max()-X, tGyro-max()-Y, tGyro-max()-Z.
  - Minimum Values (min): tAcc-min()-X, tAcc-min()-Y, tAcc-min()-Z, tGyro-min()-X, tGyro-min()-Y, tGyro-min()-Z.
### Data preprocessing for the signal dataset used to train the model for recognizing two basic human states – awake and asleep (one's basic human state recognition - awake and asleep)





