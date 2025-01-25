#include <SD.h> 
#include <Wire.h>
#include <Adafruit_GPS.h> 

Adafruit_GPS GPS(&Serial1);

File dataFile;

String NMEA1;
String NMEA2;

char c;
float deg;
float deg1;
float degWhole;
float degDec;

int echo = 1;
int count = 1;

char filename[] = "GPS_LOG0000.CSV";
const int chipSelect = 4;
const int maxCSVLength = 86400; // Max CSV Length (in case vehicle is always powered)

void setup() {

  Serial.begin(115200);
  GPS.begin(9600);
  pinMode(13, OUTPUT);

  // Uncomment line below for use with computer to view serial output
  //while( ! Serial ) { delay (1);}

  GPS.sendCommand("$PGCMD,33,0*6D"); // Turn off antenna update
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // Set update rate to 10 Hz
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA); // Request RMaC nd GGA sentences only

  // SD Card
  Serial.print("Initializing SD card...");
  // Set default chip select pin to output
  // pinMode(chipSelect,OUTPUT);
  
  // Check if the card is present and can be initialized
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed or not present!");
    return;
  }
  else {
  digitalWrite(13,HIGH);
  delay(100);
  Serial.println("Card initialized!");
  }
  
  // Create new file, max is 30,000 files
  for (uint8_t i = 0; i < 30000; i++) {
    sprintf(filename + 4, "%04d.csv", i);
    if (!SD.exists(filename)) {
      break;
    }
  }

  dataFile = SD.open(filename,FILE_WRITE);
  
  Serial.print(F("Logging to: "));
  Serial.println(filename);
  
  dataFile.println(F("Date,Time,Latitude,Longitude,Altitude (m),Speed (m/s)"));
  dataFile.close();
}
  
void loop() {
      //delay(300);
      readGPS();
      
      //Serial.println(GPS.fix);
      if (GPS.fix == 1){
        count = count + 1;
        if (count >= maxCSVLength){
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
          dataFile = SD.open(filename,FILE_WRITE);
          }

      dataFile = SD.open(filename,FILE_WRITE);
      PrintGPS();
      dataFile.close();
      delay(500);

        if (echo == 1){
          gps_conversion(GPS.latitude,GPS.longitude);

          Serial.print("Date: ");
          Serial.print(GPS.month, DEC); Serial.print('/');
          Serial.print(GPS.day, DEC); Serial.print("/20");
          Serial.println(GPS.year, DEC);

          Serial.print("Time: ");

          if (GPS.hour < 10){Serial.print('0');}
          Serial.print(GPS.hour,DEC);
          Serial.print(':');
          if (GPS.minute < 10){Serial.print('0');}
          Serial.print(GPS.minute,DEC);
          Serial.print(':');
          if (GPS.seconds < 10){Serial.print('0');}
          Serial.println(GPS.seconds,DEC);

          Serial.print("Latitude: ");
          Serial.println(deg,6);

          Serial.print("Longitude: ");
          Serial.println(deg1,6);

          Serial.print("Speed (m/s): ");
          Serial.println(GPS.speed*0.514444);

          Serial.print("Altitude (m): ");
          Serial.println(GPS.altitude);
          Serial.println("");
        }
      }
      else{Serial.println("Waiting for fix...");}
}

void PrintGPS() {

  gps_conversion(GPS.latitude,GPS.longitude);

  dataFile.print(GPS.month, DEC); dataFile.print('/');
  dataFile.print(GPS.day, DEC); dataFile.print("/20");
  dataFile.print(GPS.year, DEC);
  dataFile.print(", ");
  
  if (GPS.hour < 10){dataFile.print('0');}
  dataFile.print(GPS.hour,DEC);dataFile.print(':');
  if (GPS.minute < 10) { dataFile.print('0'); }
  dataFile.print(GPS.minute, DEC); dataFile.print(':');
  if (GPS.seconds < 10) {dataFile.print('0'); }
  dataFile.print(GPS.seconds, DEC);
  dataFile.print(", ");

  dataFile.print(deg,6);
  dataFile.print(", ");
  dataFile.print(deg1,6);
  dataFile.print(", ");
  dataFile.print(GPS.altitude);
  dataFile.print(", ");
  dataFile.print(GPS.speed * 0.514444);
  dataFile.println();
  }

void readGPS(){
  while(!GPS.newNMEAreceived()){ // Loop until you have a good NMEA sentence 
    c=GPS.read();
  }
  GPS.parse(GPS.lastNMEA()); // Parse the last good NMEA sentence
  NMEA1 = GPS.lastNMEA();
  
  while(!GPS.newNMEAreceived()){ // Loop until you have a good NMEA sentence 
    c=GPS.read();
  }
  GPS.parse(GPS.lastNMEA()); // Parse the last good NMEA sentence
  NMEA2 = GPS.lastNMEA();
}

void gps_conversion(float latitude,float longitude){

  degWhole = float(int(latitude/100)); // Gives the whole degree part of latitude
  degDec = (latitude - degWhole*100)/60; // Gives the fractional part of latitude
  deg = degWhole+degDec; // Gives correct decimal form of latitude in degrees
  if (GPS.lat == 'S'){
    deg = (-1)*deg;
  }
  degWhole = float(int(longitude/100)); // Gives the whole degree part of longitude
  degDec = (longitude - degWhole*100)/60; // Gives the fractional part of longitude
  deg1 = degWhole+degDec; // Gives correct decimal form of longitude in degrees
  if (GPS.lon == 'W'){
    deg1 = (-1)*deg1;
  }
}