# RF Remote (Manual Version)

This is the repository for my ESP8266-controlled RF transceiver, operating at 433 MHz. The code in this repository is written on [Arduino IDE](https://www.arduino.cc/en/software/) using the Arduino format `.ino` _(based on C/C++)_.

## Hardware
- ESP8266 Microcontroller (NodeMCU)

  ![ESP8266](https://bizweb.dktcdn.net/thumb/grande/100/005/602/products/dien-tu-360-95c67cad-4300-4262-8e90-9024e938804d.jpg?v=1500977760473)
- RF Transmitter & Receiver (FS1000A 433MHz Combo) _(Requires external antennas (straight copper wire or custom antennas, best length 17.4 cm) for best reception)_ 

  ![RF Transceiver Combo](https://bizweb.dktcdn.net/100/408/906/products/12r2r21313312313-1684808102072.jpg?v=1684808112890)
- 0.98' I2C OLED display

  ![OLED Display](https://nshopvn.com/wp-content/uploads/2019/03/man-hinh-lcd-oled-0-96-inch-giao-tiep-i2c-white-9w56-1.jpg)
- Reachargeable Li-ion 9V battery
- Mini 3A Buck converter (Regulate from 9V -> 5V for ESP8266 and RF receiver)

  ![3A Converter](https://ae01.alicdn.com/kf/Sb35bffdaaa9346eaad88f7bce19750d6c.jpg)
- Common electrical components: resistors, capacitors, push buttons, jumper wires, LEDs (for status)

## Libraries used:
- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306) _(For controlling and driving the OLED display)_
- [rc-switch](https://github.com/sui77/rc-switch) _(For RF transmitting & signal processing)_
- Ticker _(For task scheduling)_
- LittleFS _(For file management on ESP8266)_

## Project Structure:
```
    ├── ManualUnlock.ino - Main entry point for the firmware
    ├── OLED_Display.ino - Contains functions to drive the OLED display 
    ├── Receiver.ino - Handle processing of received RF signal in Receiver mode
    ├── Transmitter.ino - Handle RF transmission & Button mapping for Transmit mode
    ├── Settings.ino - Button mapping & events handlers for Setting mode
    ├── lib.h - Abstract definition of functions, objects and constants for the entire app
    ├── reset.ino - Ticker script for scheduling OLED display refresh
    ├── README.md - This documents
```

## Demo
- Product introduction & Testing on a rolling door: https://youtu.be/Vt_Nx-8a0FA

_Created by Minh Hai Hoang. @ 2025_
