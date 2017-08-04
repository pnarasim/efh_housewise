# Project Housewise 
## By Shambhavi, Poornima, Priya K, Priya N

## Brief Introduction

Our plan is to demonstrate our understanding of embedded systems by implementing a household monitoring solution. 

We would like to create a small (and maybe exandable) network of sensors that could report data on the parameter they are sensing and collect this information in a centrally accessible location. 

### High Level Implementation Details

We plan to set up bluey boards with sound and dust sensors (as a measure of air quality) respectively. They will broadcast the sensor data in BLE beacons at regular intervals - the method of sending this data out (though BLE beacons or some other) is still being disucssed.

We plan to set up an Edison board that will monitor these BLE beacons and extract the sensor data from them. In addition we also plan to report on the meter readings from the water and electricity meters at regular intervals. 

We will then store this data (from the sound sensor, air quality sensor, meter readings) in a database locally or on the cloud.

We may also present graphs and daily, weekly averages etc of this data.

Regarding the water and electricity meters, we have two options for getting this data
1. Use cameras to capture an image of the meter reading and then an OCR library to decipher the readings
2. Use actual water or electricity flow sensors to determine the usage.

The camera solution comes with problems such as occlusion of the meter for humans, also difficulty of reading in low light or if a leaf or dust has covered the meter.

The flow sensors also have their own challenges of requiring a plumber or electrician to help setup the sensor inline. We are still debating/researching these solutions.


### User Benefits
As responsible citizens and homeowners, we should all be constantly congnizant of our consumption of water, electricity. We should also be aware of the quality of air and noise pollution in our homes and take necessary action if these cross acceptable limits.

A system like this could provide the user with information required to make responsible decisions about their lifestyle choices.

## Deliverables
1. A frontend showing water or electricity usage patterns. Also information on the air and sound quality in this house.
2. A small network of sensors collecting relevant data.
3. An infrastructure to collect the data from the sensors and push into a centrally accessible database.

### Minimal Demo
1. At least 1 sensor sending out data through BLE beacons
2. This data being received and displayed in a central location (Edison, cloud or even a phone app)

### Enhanced Demo
Everything described above

## Block Diagram of System
### Parts
#### Intel Edison board
#### Electronut Bluey boards
#### REES52 Sound Detection sensor Module
#### SHARP Air detection sensor dust sensor GP2Y1010AU0F KG017
## Risks
1. Camera positioning, low light readings, occlusion of meter
2. Communication of sensor data through BLE beacons 
3. Figuring out the hardware

## Timeline
