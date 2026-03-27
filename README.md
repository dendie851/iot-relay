# IoT Smart Relay Platform

Imagine leaving your home or farm and suddenly remembering you forgot to turn off the water pump or the main lights. Driving all the way back just to push a button is frustrating and wastes valuable time. What if you could control and monitor your essential electrical devices from anywhere in the world, directly from your smartphone or laptop? 

That's exactly why the **IoT Smart Relay** was built! It is a complete, real-time Internet of Things (IoT) solution designed to seamlessly bridge the gap between physical hardware (like lights, pumps, and fans) and modern web technology. By using an ESP32 microcontroller, it allows you to remotely monitor statuses and toggle physical relay switches with low latency and high reliability.


## 📑 Table of Contents
- [🎥 Video Demonstration](#-video-demonstration)
- [📁 Project Folder Structure](#-project-folder-structure)
- [🏗️ Architecture & Workflow](#️-architecture--workflow)
- [📸 Step-by-Step Guide & Screenshots](#-step-by-step-guide--screenshots)
  - [1. Docker Compose Configuration](#1-docker-compose-configuration)
  - [2. MySQL Server Setup](#2-mysql-server-setup)
  - [3. MQTT Broker (Mosquitto)](#3-mqtt-broker-mosquitto)
  - [4. Backend API](#4-backend-api)
  - [5. Frontend Interfaces](#5-frontend-interfaces)
  - [6. ESP32 Edge Device Setup](#6-esp32-edge-device-setup)
  - [7. ESP32 Network Modes](#7-esp32-network-modes)
  - [8. Final Demonstration (Proof of Concept)](#8-final-demonstration-proof-of-concept)
- [🚀 How to Run](#-how-to-run)


## 🎥 Video Demonstration

Want to see the system in action? Check out the video demonstrations below:
- **Local Demo Video:** [video.mp4](video-demo/video.mp4)
- **YouTube Shorts Demo:** [Watch on YouTube](https://www.youtube.com/watch?v=mQ4HX40wBAk)

## 📁 Project Folder Structure

Here is a quick overview of what each folder in this repository contains:
- `apps-backend/`: Source code for the Node.js API server.
- `apps-frontend/`: Source code for the web-based user dashboard.
- `design/`: Contains the architecture diagram and design assets.
- `microcontroller/`: The C++ (Arduino IDE) firmware code to be uploaded to the ESP32.
- `mosquitto/`: Configuration files and logs for the MQTT Broker.
- `mysql-data/`: Local directory mapped to store the MySQL database files.
- `nginx/`: Configuration files for the Nginx Web Server.
- `sql/`: Database scripts (`.sql` files) used to initialize the tables.
- `ss/`: Screenshots used for this documentation.
- `video-demo/`: Contains the local `.mp4` video demonstration.
- `docker-compose.yaml`: The main configuration file to orchestrate the server containers.

## 🏗️ Architecture & Workflow

The following diagram shows how the system is structured:

![Architecture](design/architecture.jpg)  

### ⚙️ Core Components
1. **Frontend (Nginx / Web Dashboard):** The user-facing interface, easily accessible on both desktop and mobile devices. It allows users to click buttons to control the relays and view real-time data.
2. **Backend API (Node.js):** The central bridge of the application. It receives HTTP commands from the frontend, records transactions to the database, and forwards the action to the message broker.
3. **Database (MySQL):** The storage engine. It securely saves device lists, historical logs, and the current active state of every smart relay.
4. **MQTT Broker (Mosquitto):** The lightning-fast IoT messaging server. It handles the real-time (Publish/Subscribe) communication channel between the Backend API and the ESP32 hardware.
5. **Edge Device (ESP32 Microcontroller):** The physical "brain" on the site. Connected to the internet via WiFi, it constantly listens to the MQTT broker. When it receives a command, it physically triggers the connected relay module.

### 🔄 How Data Flows (The Workflow)
1. **User Action:** A user clicks the "Turn ON" button on the Web Dashboard for a specific relay.
2. **API Request:** The Frontend sends an HTTP POST request to the Node.js Backend API containing the command.
3. **Data Recording:** The Backend updates the relay's status to "ON" securely inside the MySQL Database.
4. **Message Publishing:** Instantly, the Backend publishes an "ON" message to a specific MQTT topic on the Mosquitto Broker.
5. **Hardware Execution:** The ESP32, which is subscribed to that exact MQTT topic, receives the message in real-time and physically switches the relay to the ON position.

## 📸 Step-by-Step Guide & Screenshots

Below is the step-by-step documentation showing how the environment is set up, the ESP32 configuration, and system demonstrations.

### 1. Docker Compose Configuration
Setting up the server environment using Docker Compose.

- ![Docker Compose 1](ss/1-docker-compose-1.jpg)  
  *This screenshot shows the first part of the `docker-compose.yaml` file, defining the setup for the MQTT Broker (Mosquitto) and MySQL database.*

- ![Docker Compose 2](ss/2-docker-compose-2.jpg)  
  *This screenshot shows the second part of the `docker-compose.yaml` file, defining the setup for the Node.js Backend and Nginx Frontend servers.*

### 2. MySQL Server Setup
Configuring the MySQL database to store sensor data and relay statuses.

- ![MySQL Setup 1](ss/3-env-server-mysql-1.jpg)  
  *This screenshot displays the MySQL database container running successfully in the terminal environment.*

- ![MySQL Setup 2](ss/4-env-server-mysql-2.jpg)  
  *Here we can see the initialized database tables (`iot_db`), which are ready to store the sensor logs and smart relay status data.*

### 3. MQTT Broker (Mosquitto)
Setting up the MQTT broker for real-time communication between the ESP32 and the backend server.

- ![MQTT 1](ss/5-env-server-mqtt-1.jpg)  
  *This shows the Mosquitto MQTT container running properly and listening for connections on the default ports.*

- ![MQTT 2](ss/5-env-server-mqtt-2.jpg)  
  *This proves that the MQTT broker is successfully receiving connections and capable of subscribing/publishing topic messages.*

### 4. Backend API
The backend provides APIs to list relays and publish relay statuses.

- ![API List Relay](ss/6-env-server-backend-api-list-relay-1.jpg)  
  *This screenshot shows a successful API test using an HTTP client (like Postman or cURL) to get the list of available smart relays from the backend.*

- ![API Pub Status](ss/7-env-server-backend-api-pub-status-relay.jpg)  
  *This demonstrates sending a POST request to the API to update or publish a new status (ON/OFF) for a specific relay.*

- ![Database Validation](ss/8-env-server-backend-api-pub-status-relay-di-database-mysql-status-update.jpg)  
  *This photo verifies that after the API request is made, the new relay status is officially saved and updated inside the MySQL database.*

- ![MQTT Broker Validation](ss/9-env-server-backend-api-pub-status-relay-di-mqtt-broker-topic-status-update.jpg)  
  *This verifies that the API not only updates the database, but also successfully publishes the new command directly to the MQTT broker so the ESP32 can receive it.*

### 5. Frontend Interfaces
The web interface is served via Nginx and is accessible on both desktop and mobile devices.

- ![Frontend Desktop 1](ss/10-env-server-frontend-web-desktop-1.jpg)  
  *This image displays the main web dashboard as seen on a desktop browser. It shows the real-time sensor monitoring values and the control buttons for the relays.*

- ![ESP32 AP Connection](ss/10-env-site-edge-esp32-mode-station-ap-connection.jpg)  
  *This screenshot displays a laptop searching for and successfully noticing the ESP32 device's local WiFi Access Point (AP) network.*

- ![Frontend Desktop 2](ss/11-env-server-frontend-web-desktop-2.jpg)  
  *Another view of the desktop web dashboard layout showing active states, demonstrating the data tracking user interface.*

- ![Frontend Mobile](ss/12-env-server-frontend-mobile.jpg)  
  *This shows how the frontend dashboard easily adapts to a mobile phone screen using responsive design.*

- ![Frontend Check MQTT Status](ss/13-env-server-frontend-cek-status-mqtt.jpg)  
  *This image shows the frontend interface's built-in feature correctly verifying that it stays connected to the MQTT broker.*

### 6. ESP32 Edge Device Setup
Configuring the ESP32 microcontroller from device setup to uploading the code via Arduino IDE.

- ![Set Preference in Arduino IDE](ss/14-env-site-edge-set-preference-esp-32.jpg)  
  *This shows the Arduino IDE's preference window being configured with the necessary URLs to support ESP32 board development.*

- ![Set Board Configuration](ss/15-env-site-edge-set-board-esp-32.jpg)  
  *This displays the exact ESP32 Board model and the COM port being selected in the Arduino IDE before uploading the code.*

- ![Serial Port Monitoring](ss/15-env-site-edge-show-monitoring-serial-port-esp-32.jpg)  
  *This screenshot shows the Serial Monitor in the Arduino IDE, which displays the device's diagnostic logs and real-time outputs.*

- ![Uploading Code](ss/17-env-site-edge-porcess-upload-code-to-esp-32.jpg)  
  *This photo captures the exact sequence of compiling and uploading the C++ firmware from the computer into the ESP32 hardware.*

### 7. ESP32 Network Modes
The ESP32 acts both as an Access Point (AP) and connects to the WiFi router as a Station (Client).

- ![Mode AP 1](ss/18-env-site-edge-esp32-mode-station-ap.jpg)  
  *Here we can see the ESP32 turning on its own internal WiFi (Access Point Mode) so that users can connect directly to it for configuration.*

- ![Mode AP 2](ss/19-env-site-edge-esp32-mode-station-ap-2.jpg)  
  *This screenshot presents the web portal hosted straight from the ESP32, which allows users to view available nearby WiFi networks.*

- ![Mode AP 3](ss/20-env-site-edge-esp32-mode-station-ap-3.jpg)  
  *This shows the configuration page where the user inputs their home router's WiFi username and password into the ESP32.*

- ![Mode AP 4](ss/21-env-site-edge-esp32-mode-station-ap-4.jpg)  
  *This confirms that the ESP32 has successfully saved the custom WiFi credentials into its memory for future use.*

- ![WiFi Client Connected](ss/22-env-site-edge-esp32-mode-client-wifi-connected.jpg)  
  *This screenshot shows the ESP32 disabling its Access Point mode and successfully joining the main WiFi router (Station Mode), obtaining a local IP address.*

### 8. Final Demonstration (Proof of Concept)
Demonstrating the working IoT flow end-to-end.

- ![Demonstration 1](ss/23-demontration-poc-1.jpg)  
  *This is a live picture of the actual hardware Proof of Concept (PoC) showing the ESP32 connected with the relay modules and active sensors.*

- ![Demonstration 2](ss/24-demontration-poc-2.jpg)  
  *Another live demonstration angle displaying the hardware successfully executing logical commands sent over the network.*

- ![Demonstration 3](ss/25-demontration-poc-3.jpg)  
  *The final validation image showing the entire physical system working perfectly in sync with the cloud/server platform.*

## 🚀 How to Run

1. Clone this repository to your local machine.
2. Make sure you have **Docker** and **Docker Compose** installed.
3. Open your terminal in the project directory and run the following command to start the servers:
   ```bash
   docker-compose up -d
   ```
4. Access the web interface by opening `http://localhost` in your browser.
