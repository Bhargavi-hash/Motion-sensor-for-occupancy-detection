# Motion sensor for occupancy detection
Embedded systems workshop team project for Motion sensor for occupancy detection
### Team members
- Bhargavi K - 2020101077
- Nandini Reddy - 202010138
- Poorvaja B - 2020101096
- Ruchitha J - 2020101093
## Motivation
To create a product which uses IOT in detecting the persons occupancy in the room to decrease the False OFFs
and False ONs. This project includes collection of data from the setup place then using this data to train the ML
algorithm.

In the present times IOT has become a part of our life and people have become so busy. The purpose of
this project is to make human life easier so that they do not need to turn on the lights in the room every
time they enter a room. The system in this project analyzes the room environment and detects the
presence of a person.

## Overview of the system
The system consists of Esp32, Esp32 cam, SD card, Grid Eye Sensor and PIR sensor. The data from Grid Eye sensor
is sent to OneM2M server and NodeJS backend and then to Mongodb. The data is processed to draw a conclusion
of whether a person exists in the environment or not.

### System requirements
The system should be able to get the temperature grid and the
images of the room. The system have the capability to push data to
the cloud throughout the day. For this Purpose, it should access a
Wi-Fi client. The data is received in the onem2m, database and
Frontend.

### System specifications
The ESP32 microcontroller is used to integrate with Grid eye
sensor, ESP cam, PIR sensor. The microcontroller sends data
collected to onem2m and MongodB atlas. 2 microcontrollers are
used one for Grideye Sensor and other for Espcam data collection.
The Grideye sensor measure the temperature of a 8*8 grid a total of
64 temperature values. The PIR sensor detects whether there is
motion or not. The Espcam takes pictures at a regular interval of 5
minutes and stores the data in SD card.The whole setup is placed in
cardboard boxes as it is an inside environment it would be
sufficient.

## Dashboard
- The user will be able to view the room analytics on the dashboard.
- Parameters displayed on the dashboard
    - Room occupancy
    - Room temperature
    - Highest occupancy of the room
    - Average occupancy of the room
    - Graphical representations of the room occupancy status and room temperature
    - HeatMap of the room
## Link to view the app
https://bhargavi-hash-occupancy-detection-dashboard-dashboard-7yfya8.streamlitapp.com/
