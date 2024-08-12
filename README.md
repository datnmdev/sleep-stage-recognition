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
- Some graphs representing the recorded signal dataset with basic human activities.
  - Walking
    ![image](https://github.com/user-attachments/assets/ee805846-adda-49af-a5b5-03eaaccc0e6d)
    ![image](https://github.com/user-attachments/assets/348768f9-cf7c-4f1c-8efe-6e7fe62dcfc3)
    ![image](https://github.com/user-attachments/assets/8cb3c333-142f-4af2-9f3e-176e73f8826d)
    ![image](https://github.com/user-attachments/assets/c4c7f84f-4f6d-4340-ab86-ed6a90f45781)
    ![image](https://github.com/user-attachments/assets/0338ccb1-07c1-49d9-bdcc-dfef8989fea5)
    ![image](https://github.com/user-attachments/assets/4f7341d9-d07c-4b0c-8527-7288586580b1)
  - Standing
    ![image](https://github.com/user-attachments/assets/8e80d4f7-7833-4f2c-ab6c-3c415c140569)
    ![image](https://github.com/user-attachments/assets/ee0773f2-e9ec-4287-be2d-e219c770e66d)
    ![image](https://github.com/user-attachments/assets/6ed53c8c-ecc1-4712-aa31-207487aaccc2)
    ![image](https://github.com/user-attachments/assets/9e61d06e-277e-4496-8e3d-029f9472975a)
    ![image](https://github.com/user-attachments/assets/1fcb981c-2142-436d-a97f-0efcb3935b46)
    ![image](https://github.com/user-attachments/assets/730a12f9-bae4-497e-9be5-9125b4e6af68)
  - Sitting
    ![image](https://github.com/user-attachments/assets/24d1254a-21be-4a07-9623-e4a83095994c)
    ![image](https://github.com/user-attachments/assets/df733d5e-cd39-4d2f-b85c-277180b8c143)
    ![image](https://github.com/user-attachments/assets/73b760e7-24c8-4f3b-8b37-e46db32a2091)
    ![image](https://github.com/user-attachments/assets/e2b2d6b6-7d0c-4a6b-90d6-15d07c43197c)
    ![image](https://github.com/user-attachments/assets/5eff0092-a1fe-456e-beb7-956bfdc2f11b)
    ![image](https://github.com/user-attachments/assets/59b8975a-8116-409e-a46e-2f77fdfd148c)
  - Lying
    ![image](https://github.com/user-attachments/assets/9bc2e983-971b-487c-aceb-1c91ec0acda9)
    ![image](https://github.com/user-attachments/assets/fd70afad-09ee-4768-97c8-2b07f0e5e79b)
    ![image](https://github.com/user-attachments/assets/18011ee8-17f1-4341-ae1b-72a98cb33d1e)
    ![image](https://github.com/user-attachments/assets/46daf695-d8f8-4718-806c-9d5e2fcc0543)
    ![image](https://github.com/user-attachments/assets/ea43ed74-7fac-40f3-b14f-4cba87760425)
    ![image](https://github.com/user-attachments/assets/ecfaa818-167b-4645-b9fe-dff8aa5ccf89)
- Feature selection for training preparation (24 features total):
  - Mean Values (mean): tAcc-mean()-X, tAcc-mean()-Y, tAcc-mean()-Z, tGyro-mean()-X, tGyro-mean()-Y, tGyro-mean()-Z.
  - Standard Deviation (std): tAcc-std()-X, tAcc-std()-Y, tAcc-std()-Z, tGyro-std()-X, tGyro-std()-Y, tGyro-std()-Z.
  - Maximum Values (max): tAcc-max()-X, tAcc-max()-Y, tAcc-max()-Z, tGyro-max()-X, tGyro-max()-Y, tGyro-max()-Z.
  - Minimum Values (min): tAcc-min()-X, tAcc-min()-Y, tAcc-min()-Z, tGyro-min()-X, tGyro-min()-Y, tGyro-min()-Z.
### Data preprocessing for the signal dataset used to train the model for recognizing two basic human states – awake and asleep (one's basic human state recognition - awake and asleep)
- Some graphs representing the recorded signal dataset with the two basic human states – sleep and wakefulness
  - AWAKE:
    ![image](https://github.com/user-attachments/assets/21a17d81-67de-40a8-932d-85a0810b525e)
    ![image](https://github.com/user-attachments/assets/66c97f25-817e-40a0-9841-ca8534479398)
    ![image](https://github.com/user-attachments/assets/1af5840b-47d8-4f91-8ac2-32a8891c61db)
    ![image](https://github.com/user-attachments/assets/6b41b1b3-73bc-4fbb-9ffb-e5d485e3f3e9)
    ![image](https://github.com/user-attachments/assets/eb23299e-6c8a-4b21-ad82-cc19ffe584c3)
    ![image](https://github.com/user-attachments/assets/f9ea2c65-ffd0-4aa5-bb5f-9e205181b429)
    ![image](https://github.com/user-attachments/assets/2aed8796-90c7-41e5-8e1c-e521ab156be6)
  - ASLEEP:
    ![image](https://github.com/user-attachments/assets/6c495fad-1262-429e-8d7b-7072ac0b91fa)
    ![image](https://github.com/user-attachments/assets/275059e9-835d-4e47-ae74-9f4a6a7b8864)
    ![image](https://github.com/user-attachments/assets/1d8bd713-e203-4bae-bb94-2d31585bd63b)
    ![image](https://github.com/user-attachments/assets/6fe36136-813f-4f7a-914a-2fb4e7a52c3d)
    ![image](https://github.com/user-attachments/assets/e801b25e-dc89-4d3c-80c2-d5d09f3bd983)
    ![image](https://github.com/user-attachments/assets/bb192040-86d3-4c6a-8416-37f31ae6b46e)
    ![image](https://github.com/user-attachments/assets/98fb701d-17fd-46f0-a2a8-1c3acc8877bf)
- 




