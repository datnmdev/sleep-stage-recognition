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
- Detailed documentation of the project: https://docs.google.com/document/d/1BuN3-p7fkp3Hfp-SFHrpFxO1AkymMbJg/edit?usp=sharing&ouid=103410879762918091663&rtpof=true&sd=true
- Detailed slides: https://docs.google.com/presentation/d/1f31fUoMkN2M_UQsv2NxQa5PDGEpqZjTR/edit?usp=sharing&ouid=103410879762918091663&rtpof=true&sd=true
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
- Selecting features for training preparation (a total of 25 features):
  - Mean values of the signal set: tAcc-mean()-X, tAcc-mean()-Y, tAcc-mean()-Z, tGyro-mean()-X, tGyro-mean()-Y, tGyro-mean()-Z.
  - Standard deviation of the signal set: tAcc-std()-X, tAcc-std()-Y, tAcc-std()-Z, tGyro-std()-X, tGyro-std()-Y, tGyro-std()-Z.
  - Maximum values of the signal set: tAcc-max()-X, tAcc-max()-Y, tAcc-max()-Z, tGyro-max()-X, tGyro-max()-Y, tGyro-max()-Z.
  - Minimum values of the signal set: tAcc-min()-X, tAcc-min()-Y, tAcc-min()-Z, tGyro-min()-X, tGyro-min()-Y, tGyro-min()-Z.
  - Average heart rate value (bmp): ppg-bmp
    To calculate the bmp value from the PPG signal set, perform FFT (Fast Fourier Transform) on the PPG signal set.
    - Normalize the signal set: Find the mean value of the signal set, then subtract this mean value from all values in the signal set.
    - Perform FFT on the signal set: After performing FFT, the resulting graph can be represented as follows.
    ![image](https://github.com/user-attachments/assets/92a76920-300b-4ce8-abf0-dfec5062e27b)
    - Find the frequency with the highest amplitude value (freq): As mentioned in Chapter 1, the human heart rate ranges from 40 to 120 beats per minute. Therefore, to accurately calculate the bmp value, we need to examine the frequency range from 0.67 Hz to 2 Hz to find the frequency with the highest amplitude value.
    - Calculate the bmp value using the formula: freq x 60
## Evaluate the model
### Evaluate the model for basic human activity recognition
- Evaluate the model using the metrics: precision, recall, F1-score, and support
  ![image](https://github.com/user-attachments/assets/907a7851-6124-4ef8-9dee-474e77d2adf5)
- Evaluate the model using a heat map
  ![image](https://github.com/user-attachments/assets/22452869-6ac3-4c78-b72d-2ac9795536b1)
### Evaluate the model for recognizing two basic human states - awake and asleep (One's Basic Human State Recognition - Awake and Asleep)
- Evaluate the model using the metrics: precision, recall, F1-score, and support
  ![image](https://github.com/user-attachments/assets/4aef634f-d90d-43ce-8d9e-4e007bdaa01d)
- Evaluate the model using a heat map
  ![image](https://github.com/user-attachments/assets/cfbb6866-b1fa-4c35-84fc-05e3c196c758)
## Deploy the model to an embedded system
### Prepare
- Install the latest version of Arduino IDE.
- Install the board: esp32 (by Espressif Systems).
- Install the necessary libraries: Adafruit MPU6050 (by Adafruit), SparkFun 3010x Pulse and Proximity, arduinoFFT, LiquidCrystal_I2C.
- Two models: Human Activity Recognition and One's Basic Human State Recognition - Awake and Asleep (converted to .h files).
- Devices (as mentioned in the device preparation process).
- Knowledge of RTOs (Real-Time Operating Systems).
### Describe the functions
- Function to predict basic human activities (walking, standing, sitting, lying down): It will predict the activities that might have occurred every 20.48 seconds and display the results on a 16x2 LCD.
- Function to predict the current state between awake and asleep: It will predict the human state every 20.48 seconds and display the results to the user through a 16x2 LCD.
### Describe the workflow
![image](https://github.com/user-attachments/assets/ef72cd9a-8681-4348-b003-4ea68d6d2088) 
- All processes are managed by RTOs (Real-Time Operating Systems).
- The main process will initialize and run other processes, including: MPU6050_Task, MAX30102_Task, Predict_Task.
- MPU6050_Task is responsible for receiving requests and measuring IMU signals (acceleration and angular velocity).
  - When the process receives an execution notification, it will request a lock from the Semaphore Mutex Handler to be able to perform the assigned task.
  - The Semaphore Mutex Handler receives the request from the process and checks the lock. If the current lock value is 1 (i.e., the lock is available), it immediately provides the lock to the process and updates the lock value to 0. If the current lock value is 0 (i.e., the lock is not available), it will not respond, and the process must wait indefinitely until it receives the lock.
  - Once the process has received the lock, it will start measuring the IMU signals. After completing the measurement, it will place the measured data into the Queue via the Queue Handler.
  - After finishing the task, the process will return the lock to the Semaphore Mutex Handler and wait for the next execution notification.
- MAX30102_Task is responsible for receiving requests and measuring PPG signals.
  - When the process receives an execution notification, it will request a lock from the Semaphore Mutex Handler to be able to perform the assigned task.
  - The Semaphore Mutex Handler receives the request from the process and checks the lock. If the current lock value is 1 (i.e., the lock is available), it immediately provides the lock to the process and updates the lock value to 0. If the current lock value is 0 (i.e., the lock is not available), it will not respond, and the process must wait indefinitely until it receives the lock.
  - Once the process has received the lock, it will start measuring the PPG signals. After completing the measurement, it will place the measured data into the Queue via the Queue Handler.
  - After finishing the task, the process will return the lock to the Semaphore Mutex Handler and wait for the next execution notification.
- Predict_Task is responsible for receiving data shared by MPU6050_Task and MAX30102_Task, performing predictions, and displaying the results on a 16x2 LCD.
  - The process starts by notifying the two processes, MPU6050_Task and MAX30102_Task, to perform their tasks.
  - The process checks if both MPU6050_Task and MAX30102_Task have completed their tasks. If both processes indicate completion, it will request a lock from the Semaphore Mutex Handler to perform the next task.
  - The Semaphore Mutex Handler receives the request from the process and checks the lock. If the current lock value is 1 (i.e., the lock is available), it immediately provides the lock to the process and updates the lock value to 0. If the current lock value is 0 (i.e., the lock is not available), it will not respond, and the process must wait indefinitely until it receives the lock.
  - Once the process has received the lock, it will retrieve all data from the Queue via the Queue Handler, perform predictions based on the trained model, and display the results on the 16x2 LCD.
  - After completing the task, the process will return the lock to the Semaphore Mutex Handler and notify MPU6050_Task and MAX30102_Task to perform new tasks.
## Run the experiment
- Wear the device
  - ![image](https://github.com/user-attachments/assets/e6bf6238-0539-4e34-80fb-92b6ee3a1203)
- The actual results obtained
  - ![image](https://github.com/user-attachments/assets/756a435a-ab2f-4861-91ab-d152e224f0a9)
## Evaluate the actual results
- Based on the actual results obtained in the table above, we can calculate the prediction accuracy of the device for the two human states - awake and asleep. Specifically, when in the awake state, the device can predict accurately with an accuracy rate of up to 80%, and when in the asleep state, it can predict accurately with a rate of up to 75%.
- Based on these numbers, it is evident that the device has a strong potential for practical application. Implementing this device in real-world applications requires consideration of various factors, such as collecting diverse data from many individuals and adjusting model parameters to fit the collected data. However, with the current results, it is clear that this device and model have significant potential for practical application in human life.
## Conclusion
- Predicting the two basic human states - awake and asleep - based on embedded programming is a promising and important research field. With advancements in technology and machine learning algorithms, creating devices capable of accurately recognizing awake and asleep states not only contributes to improving quality of life but also opens up many applications in healthcare, industry, and personal health management. In this study, the embedded device was tested and evaluated, with results showing an accuracy of 80% for predicting awake states and 75% for asleep states. These results demonstrate the significant potential of the device for practical applications, although there are still some challenges to overcome.
- The results obtained from the embedded device indicate that it can predict the awake state with an accuracy of up to 80% and the asleep state with an accuracy of 75%. These are impressive figures, especially considering the complexity and variability of physiological data between different individuals. This level of accuracy can be considered reliable for many practical applications, such as personal health monitoring, sleep management, and other medical applications.
- With the current results, the device has significant potential for practical applications. Specific potential applications include:
  - Sleep Management: The device could be used to monitor and manage sleep, helping users better understand their sleep quality and provide recommendations for improvement.
  - Personal Health Care: The device could assist in monitoring users' awake and asleep states in real-time, providing crucial data to adjust work and rest schedules.
  - Medical Applications: In medicine, the device could support the diagnosis and treatment of sleep disorders, such as insomnia, sleep apnea, and circadian rhythm disorders.
  - Industry: In high-focus work environments, the device could alert users when signs of drowsiness are detected, helping to prevent workplace accidents and improve work performance.
- Although the initial results are promising, deploying the device in practical applications requires addressing several challenges. One of the biggest challenges is the diversity of physiological data between individuals. Data from different users can vary significantly due to factors such as age, gender, health status, and lifestyle. Therefore, collecting and analyzing data from a large and diverse sample is necessary to improve the device's accuracy.
-  Additionally, model parameters need to be continuously adjusted to match the collected data. Optimizing the prediction model requires intervention from experts in machine learning and data analysis. Furthermore, integrating the device into existing systems and ensuring user data security and privacy is a considerable challenge.

