# Multiprotocol Communication Over Serial Port COM USB
This is a very large project initiated many years ago
Multiprotocol Communication Over Serial Port COM USB

This project work with [RFID-CloverWiever](https://github.com/mabyre/RFID-CloverWiever "RFID-CloverWiever")

RFID-CloverWiever is the Graphique Interface written in Qt/C++.

This project is made up of three parts.

- **CSL Lib** that is for Communication and Security Layer, this part implement the communication over USB serial port
- **PMLite** is for Protocol Machinerie Light that deal with different platforme implementation
- **TestPThread** is the upper part of this project, it test and use the two others, it take the place of the User Interface when there is no