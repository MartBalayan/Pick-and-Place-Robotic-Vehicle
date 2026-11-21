# Pick-and-Place Robotic Vehicle

### Wireless Gesture Control | Embedded Systems | Robotics | Bluetooth Low Energy

A gesture-controlled robotic vehicle with a wearable glove interface, wireless communication and a six-servo robotic mechanism.

The system uses hand gestures and wrist movements to control vehicle navigation and robotic arm movements. Two Arduino Nano 33 BLE-compatible microcontroller boards communicate wirelessly through Bluetooth Low Energy (BLE), enabling real-time transmission of control commands between the wearable controller and the robotic platform.

The project combines embedded C/C++ programming, sensor integration, wireless communication and robotic motion control to create an intuitive human-machine interface for a mobile robotic system.

---

## 1. Project Overview

Traditional robotic systems often rely on joysticks, remote controllers or predefined control interfaces. This project explores an alternative approach using a wearable glove that translates hand movements and gestures into commands for a robotic vehicle.

The system consists of two main subsystems:

- **Wearable Controller:** A sensor-equipped glove that detects finger bending and hand orientation to generate movement and robotic arm commands.
- **Robotic Vehicle:** A mobile platform equipped with two DC motors and a six-servo robotic mechanism, receiving and executing commands wirelessly.

The glove processes sensor measurements locally and transmits the resulting control commands over Bluetooth Low Energy. The vehicle interprets these commands and controls its drive motors or robotic mechanism according to the selected operating mode.

## 2. System Architecture

The system uses a distributed embedded architecture with two microcontrollers communicating through BLE.

### 2.1 Wearable Gesture Controller

The wearable controller integrates five flex-sensor inputs and an onboard inertial measurement unit (IMU).

The flex sensors detect finger bending, while the IMU provides acceleration measurements used to calculate hand orientation and identify movement gestures.

The controller processes these measurements to determine the intended operation.

**Main functions:**
- Acquisition of five flex-sensor signals.
- Hand orientation estimation using accelerometer measurements.
- Pitch and roll calculation.
- Gesture recognition and classification.
- Operating-mode selection.
- Wireless transmission of control commands.

### 2.2 Wireless Communication

Bluetooth Low Energy provides the communication link between the wearable controller and the robotic vehicle.

The system uses a BLE central/peripheral architecture:

- The wearable controller operates as the BLE central device.
- The robotic vehicle operates as the BLE peripheral device.
- A custom BLE service and characteristic are used to exchange control information.
- A compact four-byte data packet communicates the selected operating mode and corresponding movement commands.

This approach separates gesture recognition from motor control, allowing each microcontroller to handle its own dedicated tasks.

### 2.3 Robotic Vehicle and Manipulator

The robotic platform consists of a two-motor drive system and a six-servo robotic mechanism.

The vehicle controller receives BLE commands and translates them into motor and servo movements.

**Main functions:**
- Independent PWM control of two DC motors.
- Direction control for vehicle navigation.
- Six-channel servo actuation.
- Servo position initialization and angular limits.
- Operating-mode-dependent motor and manipulator control.

---

## 3. Operating Modes

The system implements three main operating modes.

### Vehicle Mode

Hand orientation is used to control the movement of the robotic vehicle.

The accelerometer measurements are converted into pitch and roll angles, which determine the vehicle's direction and motor speeds.

The controller supports:
- Forward and backward movement.
- Left and right turning.
- Variable motor speed based on hand orientation.
- Differential motor speed control for steering.

### Robotic Arm Mode

Finger bending and wrist orientation are used to generate robotic mechanism commands.

The flex sensors identify finger configurations, while wrist orientation determines the corresponding movement commands.

The vehicle controller translates these commands into individual servo adjustments.

The six servo motors provide multiple degrees of actuation for the robotic mechanism.

### Standby Mode

A dedicated operating mode disables vehicle movement and prevents the robotic mechanism from receiving active movement commands.

This allows the operator to switch between the different control functions.

---

## 4. Hardware Components

| Component | Function |
|---|---|
| Arduino Nano 33 BLE-compatible board (controller) | Wearable sensor acquisition and gesture processing |
| Arduino Nano 33 BLE-compatible board (vehicle) | Vehicle and robotic mechanism control |
| Five flex sensors | Finger bending detection |
| BMI270/BMM150 IMU | Hand orientation and movement measurements |
| Two DC motors | Vehicle propulsion |
| Six servo motors | Robotic mechanism actuation |
| Motor driver circuitry | DC motor direction and speed control |
| RGB LED | Operating-mode indication |
| Push button | Operating-mode selection |

---

## 5. Embedded Software Development

The project is implemented in C++ using the Arduino development environment.

The software is divided into two independent programs.

### Wearable Controller — `code_glove.cpp`

Responsible for:
- Flex-sensor acquisition using analog inputs.
- IMU initialization and acceleration measurements.
- Pitch and roll calculations.
- Gesture detection.
- Vehicle movement detection.
- BLE device discovery and connection.
- Transmission of control packets.

### Vehicle Controller — `code_vechicle.cpp`

Responsible for:
- BLE peripheral initialization.
- Receiving and decoding control packets.
- DC motor PWM and direction control.
- Six-servo initialization and movement.
- Servo angular limit enforcement.
- Operating-mode management.

---

## 6. Technologies and Engineering Skills

| Category | Technologies |
|---|---|
| Embedded Systems | Arduino Nano 33 BLE |
| Programming | Embedded C/C++ |
| Wireless Communication | Bluetooth Low Energy |
| Sensor Integration | Flex Sensors, BMI270/BMM150 IMU |
| Motion Processing | Accelerometer-based pitch and roll estimation |
| Motor Control | PWM and H-bridge motor control |
| Robotics | DC Motors and Servo Actuation |
| Human-Machine Interface | Wearable Gesture Controller |

---

## 7. Repository Structure

```text
Pick-and-Place-Robotic-Vehicle/
│
├── code_glove.cpp
│   └── Wearable controller firmware
│
├── code_vechicle.cpp
│   └── Robotic vehicle firmware
│
├── ES_Capstone_Pick and Place
│   Robotic Vehicle_M.Balayan_A.Karapetyan_v2.0.pdf
│   └── Project documentation
│
└── README.md
```

---

## 8. Project Documentation

The complete engineering report contains additional information about the project requirements, design approach, hardware implementation and development process.

[**Read the Project Documentation (PDF)**](./ES_Capstone_Pick%20and%20Place%20Robotic%20Vehicle_M.Balayan_A.Karapetyan_v2.0.pdf)

## 9. Project Information

| Attribute | Description |
|---|---|
| Project | Pick-and-Place Robotic Vehicle |
| Project Type | Engineering Capstone Project |
| Main Application | Gesture-Controlled Robotics |
| Controller | Wearable Sensor Glove |
| Communication | Bluetooth Low Energy |
| Vehicle Drive | Two DC Motors |
| Robotic Mechanism | Six Servo Motors |
| Programming Language | C++ |
| Development Platform | Arduino |

---

**Explore the implementation:** The repository includes the firmware for both the wearable gesture controller and the robotic vehicle, together with the project documentation.
