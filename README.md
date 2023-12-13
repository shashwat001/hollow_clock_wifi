# Hollow Clock

This Arduino project implements a hollow clock using an ESP8266, and other libraries. The clock's hands move to display the current time, and it is designed to be configurable for different WiFi networks.
The project is based on this project on [instructables](https://www.instructables.com/Hollow-Clock-4).

![Hollow Clock by me](https://github.com/shashwat001/hollow_clock_wifi/assets/1008906/a3bc4948-c823-44b2-983d-1fa43c9bf22a)


## Features

- Displays the time on a hollow clock face using stepper motors.
- Connects to multiple WiFi networks for flexibility.
- Automatically set to the current time.
- UI Dashboard for debugging.

## Hardware Requirements

- ESP8266 microcontroller
- Stepper motors
- Other hardware components as required

## Installation and Setup

1. Clone this repository:

2. Open the `hollow_clock_wifi.c` file in the Arduino IDE.

3. Copy the `config.CUSTOMIZE_ME.h` to `config.h` and update the proper wifi credentials.

4. Compile and upload the code to your ESP8266.

## Usage

1. Set the current hands position to 12 O'clock.

2. Power on the ESP8266 with the uploaded code.

3. The clock hands should move slowly to display the current time.

## License
This project is licensed under the MIT License.


