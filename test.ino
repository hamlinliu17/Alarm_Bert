#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SparkFun_ADXL345.h>

#define LED_PIN 12

//gps constants===========================
// Choose two Arduino pins to use for software serial
int RXPin = 0;
int TXPin = 1;
int GPSBaud = 9600;

// Create a TinyGPS++ object
TinyGPSPlus gps;

// Create a software serial port called "gpsSerial"
SoftwareSerial gpsSerial(RXPin, TXPin);
///////////////////////////////////////////////////////////////////////////////

//Bluetooth stuff ====================
SoftwareSerial BT(5, 4); // RX | TX
char input_char;
int state = 1;

//Accelerometer constants
const int buzzer = 10;
ADXL345 adxl = ADXL345(); //I2C Communication

void setup()
{
 //Bluetooth setup
 Serial.begin(9600);
 gpsSerial.begin(GPSBaud);
 BT.begin(9600); //Default Baud for comm, it may be different for your Module.
 Serial.println("The bluetooth gates are open.\n Connect to HC-05 from any other bluetooth device with 1234 as pairing key!.");
 BT.println("The bluetooth gates are open.\n Connect to HC-05 from any other bluetooth device with 1234 as pairing key!.");
 pinMode(LED_PIN, OUTPUT);
 digitalWrite(LED_PIN, LOW);

 //GPS
 // Start the software serial port at the GPS's default baud

 //Accelerometer SETUP
 adxl.powerOn();                     // Power on the ADXL345

 adxl.setRangeSetting(16);           // Give the range settings
                                     // Accepted values are 2g, 4g, 8g or 16g
                                     // Higher Values = Wider Measurement Range
                                     // Lower Values = Greater Sensitivity
                                     adxl.setSpiBit(0);                  // Configure the device to be in 4 wire SPI mode when set to '0' or 3 wire SPI mode when set to 1
                                                                         // Default: Set to 1
  pinMode(buzzer,OUTPUT);                                                                     // SPI pins on the ATMega328: 11, 12 and 13 as reference in SPI Library

}
/////////////////////////////////////////////////////////////////////////////
void displayInfo()
{
 if (gps.location.isValid())
 {
   BT.print("Latitude: ");
   BT.println(gps.location.lat(), 6);
   BT.print("Longitude: ");
   BT.println(gps.location.lng(), 6);
   BT.print("Altitude: ");
   BT.println(gps.altitude.meters());
   BT.print("http://www.google.com/maps/place/");
   BT.print(gps.location.lat(), 6);
   BT.print(",");
   BT.println(gps.location.lng(), 6);
 }
 else
 {
   BT.println("Location: Not Available");
 }

 BT.print("Date: ");
 if (gps.date.isValid())
 {
   BT.print(gps.date.month());
   BT.print("/");
   BT.print(gps.date.day());
   BT.print("/");
   BT.println(gps.date.year());
 }
 else
 {
   BT.println("Not Available");
 }

 BT.print("Time: ");
 if (gps.time.isValid())
 {
   if (gps.time.hour() < 10) Serial.print(F("0"));
   BT.print(gps.time.hour());
   BT.print(":");
   if (gps.time.minute() < 10) Serial.print(F("0"));
   BT.print(gps.time.minute());
   BT.print(":");
   if (gps.time.second() < 10) Serial.print(F("0"));
   BT.print(gps.time.second());
   BT.print(".");
   if (gps.time.centisecond() < 10) Serial.print(F("0"));
   BT.println(gps.time.centisecond());
 }
 else
 {
   BT.println("Not Available");
 }

}
//////////////////////////////////////////////////////////////////////////////
void read_BT()
{
 BT.listen();

 BT.println("a to arm, d to disarm");
 while(!BT.available())
 {}


 input_char = BT.read();


 while(input_char != 'a' && input_char != 'd')
 {
   BT.println("Invalid Command. Enter the letter a to arm or d to disarm");
   while(BT.available() == 0)
   {}
   input_char = BT.read();
 }

 if(input_char == 'a')
   {
     state = 2;
     BT.println("Armed");
     digitalWrite(LED_PIN, HIGH);
     // if(printer) {Serial.println("armed");}
   }

  if(input_char == 'd')
   {
     state = 1;
     BT.println("Disarmed");
     digitalWrite(LED_PIN, LOW);
     // if(printer) {Serial.println("disarmed");}
   }

 return;
}

void readDisarm()
{
 char choice;
 BT.listen();
 if(state > 1)
 {
   BT.println("Would you like to disarm? y or n");
   while(BT.available() == 0)
   {}
   choice = BT.read();
   while(choice != 'y' && choice != 'n')
   {
     BT.println("Invalid Command. Enter y or n");
     while(BT.available() == 0)
     {}
     choice = BT.read();
   }

   if(choice == 'y')
   {
     BT.println("Disarmed");
     state = 1;
   }

 }
}
//////////////////////////////////////////////////////////////////////////////
int x,y,z;
int x1, y1, z1;
void read_AC()
{
   int quart = 500;

   adxl.readAccel(&x, &y, &z);// Read the accelerometer values and store them in variables declared above x,y,z
   delay(100);
   adxl.readAccel(&x1,&y1,&z1);
   if(((x-x1)*(x-x1)>=4)||((y-y1)*(y-y1)>=4)||((z-z1)*(z-z1)>=4))
   {

     tone(buzzer, 440);
     delay(2*quart);
     noTone(buzzer);
     delay(10);
     state = 3;

   }

}
//////////////////////////////////////////////////////////////////////////////
void read_GPS()
{
 // This sketch displays information every time a new sentence is correctly encoded.
 gpsSerial.listen();
 if (gpsSerial.available() > 0)
 {
   if (gps.encode(gpsSerial.read()))
     displayInfo();
 }

 // If 5000 milliseconds pass and there are no characters coming in
 // over the software serial port, show a "No GPS detected" error
 else if (millis() > 5000 && gps.charsProcessed() < 10)
 {
   BT.println("No GPS detected");
 }
}
//////////////////////////////////////////////////////////////////////////////
void loop()
{
 //Sanity check
 if(Serial.available())
 {
   BT.write(Serial.read());
 }

 if(state == 1)
 {
   //read from Bluetooth
   //// c = 2 or c = 1
   read_BT();
 }

 if(state == 2)
 {
   //listen to Bluetooth
   //// c = 1
   readDisarm();
   //listen to accel
   //// c = 3
   while(state == 2)
   {
     read_AC();
   }

 }

 if(state == 3)
 {
   //listen to GPS
   read_GPS();
   //listen to Bluetooth
   //// c = 1
   readDisarm();

 }

 // BT.listen();
 // if(BT.available()){
 //   input_char = BT.read();
 //
 //   while(input_char != 'a' || input_char != 'd')
 //   {
 //     BT.println("Invalid Command. Enter the letter a to arm or d to disarm");
 //     delay(1000);
 //     input_char = BT.read();
 //   }
 //   //
 //   //
 //   Serial.print(input_char);
 // }
 //
 //
 //
 // if(arm == true)
 // {
 //
 // }


// Feed all data from termial to bluetooth
// if (Serial.available())
// {
//
//   BT.write(Serial.read());
// }

}
