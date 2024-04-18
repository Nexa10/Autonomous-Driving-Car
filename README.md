# Autonomous Vehicle Technology in Small Robotics

## Project Overview
This project explores the feasibility of integrating autonomous vehicle technology into small robotics using Computer Vision (CV) within the context of limited computational power. Our prototype utilizes a variety of components to navigate indoor environments, potentially applicable in settings such as restaurants, senior care homes, and hotels to improve service efficiency and reduce human workload.

## Components

- **K66F Board**: Acts as the central unit for input/output operations.
- **ESP-32 CAM**: Serves as a camera web server, streaming visual data for CV processing.
- **ESP32 Module**: Interfaces with the K66F board for command reception and sensor data transmission via UART.
- **Motor Drive & Motors**: Control movement with PWM signals from the K66F.
- **Ultrasonic Sensor**: Measures distance to obstacles, with integrated alert systems based on proximity.
- **Light Sensor Unit**: Includes a photoresistor for ambient light detection, influencing the LED ring light output.
- **LED Ring Light**: Provides visual feedback based on sensor inputs.
- **Speaker**: Emits warning sounds when approaching obstacles.
- **Wi-Fi Module**: Facilitates wireless communication using UDP WebSocket for real-time data transmission.

## System Operation

The system operates through multiple concurrent threads handling motor control, sensor data reading, and actuator management. The CV server processes visual data received from the ESP-32 CAM, directing movement through wireless communications to the ESP32, which then relays commands to the K66F for execution.

### Navigation

Our approach does not rely on predefined maps but instead uses real-time computer vision techniques to navigate. By processing images to detect paths and obstacles, the system adjusts its route dynamically, ensuring responsive and adaptable navigation within complex environments.

## Challenges and Solutions

- **CV Sensitivity**: Calibration of the CV algorithm to adapt to variable lighting and environmental conditions.
- **Motor Control**: Managing speed and direction to accurately follow the computed path, overcoming limitations with the current motor drive’s PWM range.
- **Communication Latency**: Optimization of Wi-Fi and UART protocols to minimize delays in command execution.

## Future Directions

Further integration of sensors, enhanced CV algorithms, and improvements in power management and motor control are planned to enhance system reliability and performance. The use of 3D printing for structural components and potential integration of additional sensing technologies like LiDAR are also under consideration.

## Conclusion

Our project demonstrates the potential of autonomous technologies in small-scale robotics, highlighting the critical role of computer vision and connectivity in creating effective autonomous systems. Continued development and testing are required to fully realize these capabilities in practical applications.

## Appendices

- **Demo Videos and Images**: Visuals of the car in operation, demonstrating its ability to navigate and respond to environmental cues.
- <img width="468" alt="image" src="https://github.com/Nexa10/Autonomous-Driving-Car/assets/90815170/c0c37f8d-2a6a-405f-992f-5f64e3613715">
- <img width="468" alt="image" src="https://github.com/Nexa10/Autonomous-Driving-Car/assets/90815170/4df70782-c46c-4bd0-8cb3-05916d5c729f">



## Contributors

- **Dennis Audu**
- **Yiyuan Dong**

For more details, see our full project documentation and codebase in this repository.
