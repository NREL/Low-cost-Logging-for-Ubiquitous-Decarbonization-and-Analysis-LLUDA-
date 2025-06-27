# Low-cost Logging for Ubiquitous Decarbonization and Analysis (LLUDA)

This repository features an Arduino-based GPS logger, called LLUDA, which uses the Adafruit Ultimate GPS FeatherWing with the Adafruit Feather M0 Adalogger, designed to capture and log GPS data in CSV format onto an SD card (embedded inside the Feather M0 Adalogger). The FeatherWing is powered through the vehicle's auxiliary power (from cigarette lighter) socket via a micro USB cable.

The data collected can inform a wide range of analyses, including route efficiency, vehicle utilization, and the feasibility of transitioning to electric or higher-efficiency vehicles. 
_________________________________________________

**This program is designed to:**
- Define variables for storing GPS data, managing files, and handling data logging tasks.
- Parse NMEA sentences from the GPS module, converting coordinates from NMEA format into decimal degrees, which are already in EPSG:4326.
- Establish a connection to the GPS module and SD card for data storage, and configure them for logging.
- Continuously read GPS data (date, time, latitude, longitude, altitude, and speed), record it in CSV format on the SD card, and manage file creation as needed (dependent on vehicle auxiliary power).

<p align="center">
  <img src="https://github.com/user-attachments/assets/b7b4a464-a94d-4c61-be4b-ff4a89e3d96f" alt="inside gps logger" width="300" height="auto" style="margin-right: 20px;"/>
  <img src="https://github.com/user-attachments/assets/66298cb2-057a-490a-a0c4-2d9fbe0657a7" alt="GPS antenna" width="153" height="auto"/>
</p>

**The logger records the following information at a rate of 1 Hz while a valid GPS fix (when connected to satellites) is being maintained:**
  - Date (MM/DD/YYYY)
  - Time (HH:MM:SS in UTC)
  - Latitude
  - Longitude
  - Altitude (m)
  - Speed (m/s)

Here’s an example snippet of the GPS data recorded on the SD card (in CSV format) using LLUDA:
<p align="center"> 
  <img src="https://github.com/user-attachments/assets/4aac474d-8868-47e4-87f2-a3bf1331902a" alt="csv output" width="500" height="auto"/>
</p>

### Casing + Soldering Adafruit FeatherWing
_________________________________________________

This repository also includes the STL file necessary for 3D printing the logger casing. You can find this in the `/STL Logger Casing` folder.

Use this link on how to solder the male and female header pins for the Adafruit FeatherWing under "Types of Headers": [Soldering Adafruit FeatherWing](https://learn.adafruit.com/how-to-solder-headers/feathers-and-featherwings?gad_source=1&gclid=CjwKCAjw_4S3BhAAEiwA_64YhotCzhuHvyhc-7wuLengRd8agaj9MzUqPYBRsM-82ovQWJ_1ttQ_nRoC2kgQAvD_BwE)

### LLUDA Materials
_________________________________________________

**The following materials are required to fabricate LLUDA:**
  - Adafruit Feather M0 Adalogger
  - Adafruit Ultimate GPS FeatherWing
  - Male & Female Header Pins
  - Lithium Battery CR1220 3V
  - Pelican Clear Case
  - Micro SD Card
  - Car Charger with 1 Amp Output
  - Short Micro USB Extension (Male to Female)
  - Micro USB Cable
  - Adafruit SMA to uFL/u.FL/IPX/IPEX RF Adapter Cable
  - GPS Antenna
  - 3D Printer Filament (Optional Casing)

**The purchase links for each of the items shown above are included in a spreadsheet, which is in this repository.**

### Deployment Instructions
_________________________________________________

**Before deploying the LLUDA on the respective vehicle, please record the following information:**
  - Logger Box ID (e.g., LOGGER 01, LOGGER 02)
  - Vehicle ID
  - Vehicle Make/Model/Year
  - Fuel Type (e.g., Diesel, Electric)
  - Route (if applicable)
  - Date Range – Record the specific date range each logger was used on each vehicle if applicable. This helps in identifying which data belongs to which vehicle.
  
<p align="center">
  <img src="https://github.com/user-attachments/assets/b700f4bd-75c1-489b-8a99-6f0921435e43" alt="outside gps logger" width="350" height="auto"/>
</p>

_________________________________________________

1) Open the Pelican case and verify that the SMA adapter cable is securely connected to the FeatherWing. Also, ensure that the micro USB extension is properly connected to the FeatherWing, as shown below.

<p align="center">
  <img src="https://github.com/user-attachments/assets/638ef98b-86d1-4aea-b3db-599b6f8b0f72" alt="inside gps logger with description" width="350" height="auto"/>
</p>

2) Connect the SMA adapter cable to the antenna (Figure A). Place the antenna on the vehicle’s dashboard and secure it using the fastener on the back. Connect the Micro USB to the Micro USB extender (Figure B).

<p align="center">
  <img src="https://github.com/user-attachments/assets/bd0cf600-0b87-48de-98f1-931fabeb38f0" alt="SMA adaptor cable and micro USB extension connection" width="450" height="auto"/>
</p>

3) Connect the USB end to the car adapter charger’s **1 Amp slot** (shown in the figure below) to avoid potential damage to the board from excessive voltage. Then, plug the adapter into the cigarette lighter port to power it using the vehicle's auxiliary power. 

<p align="center">
  <img src="https://github.com/user-attachments/assets/0ee28567-1bde-4d4d-b2af-1579a3ca2222" alt="car adapter charger" width="180" height="auto"/>
</p>

4) When the FeatherWing is powered (vehicle is on), a red LED will pulsate every second. This indicates that the logger is attempting to connect with GPS satellites. Once it has successfully connected, the red LED changes to pulsating every 15 seconds, meaning it has a GPS fix, and is actively recording the respective GPS data to the SD card. The location of the SD card in the M0 Adalogger is shown below.

<p align="center">
  <img src="https://github.com/user-attachments/assets/76d6b7ce-ce61-4d49-8673-8bef43ab3fc3" alt="sd card location" width="180" height="auto"/>
</p>

### Arduino Code Overview
_________________________________________________

#### Intitialization:
The `setup()` function does the following:
  - Initializes the GPS module and SD card for data logging. 
  - Starts serial communication at 115200 baud for debugging.
  - Begins the communication with the GPS module and sets GPS commands.
  - Finds an available filename for logging, creates the file (e.g, "GPS_0001.CSV"), and writes the header row, which includes the following: Date, Time, Latitude, Longitude, Altitude (m), Speed (m/s).
    
```cpp
void setup() {
  Serial.begin(115200);
  GPS.begin(9600);
  pinMode(13, OUTPUT);

  // Initialize GPS commands
  GPS.sendCommand("$PGCMD,33,0*6D"); // Turn off antenna update
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // Set update rate to 1 Hz
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA); // Request RMC and GGA sentences only

  // Initialize SD card
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed or not present!");
    return;
  }
  digitalWrite(13, HIGH);
  delay(100);
  Serial.println("Card initialized!");

  // Create new file for logging
  for (uint8_t i = 0; i < 30000; i++) {
    sprintf(filename + 4, "%04d.csv", i);
    if (!SD.exists(filename)) {
      break;
    }
  }
  dataFile = SD.open(filename, FILE_WRITE);
  Serial.print(F("Logging to: "));
  Serial.println(filename);
  dataFile.println(F("Date,Time,Latitude,Longitude,Altitude (m),Speed (m/s)"));
  dataFile.close();
}
```

#### Data Reading and Logging:
The `loop()` function performs the following tasks:
  - Checks for a valid GPS fix and proceeds only if the fix is valid.
  - Continuously reads GPS data and logs it to a CSV file on the SD card.
  - Creates new CSV files when the current file reaches its maximum length.
  - Prints GPS data to the Serial Monitor if `echo` is enabled, allowing for real-time data inspection.

```cpp
void loop() {
  readGPS();

  if (GPS.fix == 1) { // Only log data if GPS fix is obtained
    count++;
    if (count >= maxCSVLength) {
      Serial.println(F("Data file reached maximum CSV length!"));
      count = 0;
      dataFile.close();

      for (uint8_t i = 0; i < 30000; i++) {
        sprintf(filename + 4, "%04d.csv", i);
        if (!SD.exists(filename)) {
          break;
        }
      }
      Serial.println(F("Logging to: "));
      Serial.println(filename);
      dataFile = SD.open(filename, FILE_WRITE);
    }

    dataFile = SD.open(filename, FILE_WRITE);
    PrintGPS();
    dataFile.close();
    delay(500);

    if (echo == 1) {
      gps_conversion(GPS.latitude, GPS.longitude);
      Serial.print("Date: "); Serial.print(GPS.month, DEC); Serial.print('/'); Serial.print(GPS.day, DEC); Serial.print("/20"); Serial.println(GPS.year, DEC);
      Serial.print("Time: "); if (GPS.hour < 10) Serial.print('0'); Serial.print(GPS.hour, DEC); Serial.print(':'); if (GPS.minute < 10) Serial.print('0'); Serial.print(GPS.minute, DEC); Serial.print(':'); if (GPS.seconds < 10) Serial.print('0'); Serial.println(GPS.seconds, DEC);
      Serial.print("Latitude: "); Serial.println(deg, 6);
      Serial.print("Longitude: "); Serial.println(deg1, 6);
      Serial.print("Speed (m/s): "); Serial.println(GPS.speed * 0.514444);
      Serial.print("Altitude (m): "); Serial.println(GPS.altitude);
      Serial.println("");
    }
  } else {
    Serial.println("Waiting for fix...");
  }
}
```

#### Data Conversion:
The `gps_conversion()` function converts GPS coordinates from degrees-minutes format to decimal degrees. Then it uses the `PrintGPS()` function to print the converted GPS coordinates to the Serial Monitor and store it on the SD card, including date, time, GPS coordinates, altitude, and speed.

```cpp
void PrintGPS() {
  gps_conversion(GPS.latitude, GPS.longitude);

  dataFile.print(GPS.month, DEC); dataFile.print('/'); dataFile.print(GPS.day, DEC); dataFile.print("/20");
  dataFile.print(GPS.year, DEC); dataFile.print(", ");
  if (GPS.hour < 10) dataFile.print('0'); dataFile.print(GPS.hour, DEC); dataFile.print(':');
  if (GPS.minute < 10) dataFile.print('0'); dataFile.print(GPS.minute, DEC); dataFile.print(':');
  if (GPS.seconds < 10) dataFile.print('0'); dataFile.print(GPS.seconds, DEC);
  dataFile.print(", ");
  dataFile.print(deg, 6); dataFile.print(", ");
  dataFile.print(deg1, 6); dataFile.print(", ");
  dataFile.print(GPS.altitude); dataFile.print(", ");
  dataFile.print(GPS.speed * 0.514444); dataFile.println();
}

void gps_conversion(float latitude, float longitude) {
  degWhole = float(int(latitude / 100));
  degDec = (latitude - degWhole * 100) / 60;
  deg = degWhole + degDec;
  if (GPS.lat == 'S') deg = (-1) * deg;

  degWhole = float(int(longitude / 100));
  degDec = (longitude - degWhole * 100) / 60;
  deg1 = degWhole + degDec;
  if (GPS.lon == 'W') deg1 = (-1) * deg1;
}
```

### Usage
_________________________________________________

The software is designed to assist both researchers and industry professionals, with no programming knowledge required. After assembling the FeatherWing, users need to:
  - Connect the Hardware: Ensure the SD card module is properly inserted into the Adafruit Ultimate GPS FeatherWing.
  - Upload the Code: Use the Arduino IDE to upload the provided .ino file to your FeatherWing by connecting it to your computer via a micro USB cable.
  - Monitor Output: Open the Serial Monitor in the Arduino IDE to view real-time GPS data and confirm that everything is working correctly.
  - Check SD Card: The GPS data is logged to the SD card in CSV format. Remove the SD card to view and analyze the data on your computer.

_________________________________________________

**Copyright (c) 2025 Alliance for Sustainable Energy, LLC**
