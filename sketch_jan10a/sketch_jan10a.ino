#include <SparkFun_ADXL345.h>        // SparkFun ADXL345 Library
#include <SoftwareSerial.h>           // Serial library

#define BUZZ 10
#define ARM 6
#define A5 440
SoftwareSerial BlueT(4, 5); // RX | TX
char inputChar;
bool armed = false;
int x,y,z;
int x1, y1, z1;




/*********** COMMUNICATION SELECTION ***********/
/*    Comment Out The One You Are Not Using    */
//ADXL345 adxl = ADXL345(10);           // USE FOR SPI COMMUNICATION, ADXL345(CS_PIN);
ADXL345 adxl = ADXL345();             // USE FOR I2C COMMUNICATION

/********* functions *******************/

bool detectMotion(int s, int s2,  int s3, int e, int e2, int e3)
{
  return ((s-e)*(s-e)>=4)||((s2-e2)*(s2-e2)>=4)||((s3-e3)*(s3-e3)>=4);
}

/******************** SETUP ********************/
/*          Configure ADXL345 Settings         */
void setup(){
  
  Serial.begin(9600);                 // Start the serial terminal
  Serial.println("testing alARM");
  
  
  adxl.powerOn();                     // Power on the ADXL345

  adxl.setRangeSetting(16);           // Give the range settings
                                      // Accepted values are 2g, 4g, 8g or 16g
                                      // Higher Values = Wider Measurement Range
                                      // Lower Values = Greater Sensitivity

   
  adxl.setActivityXYZ(1, 0, 0);       // Set to activate movement detection in the axes "adxl.setActivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)
  adxl.setActivityThreshold(75);      // 62.5mg per increment   // Set activity   // Inactivity thresholds (0-255)
 
  adxl.setInactivityXYZ(1, 0, 0);     // Set to detect inactivity in all the axes "adxl.setInactivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)
  adxl.setInactivityThreshold(75);    // 62.5mg per increment   // Set inactivity // Inactivity thresholds (0-255)
  adxl.setTimeInactivity(10);         // How many seconds of no activity is inactive?

  //adxl.setTapDetectionOnXYZ(0, 0, 1); // Detect taps in the directions turned ON "adxl.setTapDetectionOnX(X, Y, Z);" (1 == ON, 0 == OFF)
 
  // Set values for what is considered a TAP and what is a DOUBLE TAP (0-255)
  // adxl.setTapThreshold(50);           // 62.5 mg per increment
  // adxl.setTapDuration(15);            // 625 μs per increment
  // adxl.setDoubleTapLatency(80);       // 1.25 ms per increment
  // adxl.setDoubleTapWindow(200);       // 1.25 ms per increment
 
  // // Set values for what is considered FREE FALL (0-255)
  // adxl.setFreeFallThreshold(7);       // (5 - 9) recommended - 62.5mg per increment
  // adxl.setFreeFallDuration(30);       // (20 - 70) recommended - 5ms per increment
 
  // Setting all interupts to take place on INT1 pin
  //adxl.setImportantInterruptMapping(1, 1, 1, 1, 1);     // Sets "adxl.setEveryInterruptMapping(single tap, double tap, free fall, activity, inactivity);" 
                                                        // Accepts only 1 or 2 values for pins INT1 and INT2. This chooses the pin on the ADXL345 to use for Interrupts.
                                                        // This library may have a problem using INT2 pin. Default to INT1 pin.
  
  // Turn on Interrupts for each mode (1 == ON, 0 == OFF)
  adxl.InactivityINT(1);
  adxl.ActivityINT(1);
  // adxl.FreeFallINT(1);
  // adxl.doubleTapINT(1);
  // adxl.singleTapINT(1);
  
//attachInterrupt(digitalPinToInterrupt(interruptPin), ADXL_ISR, RISING);   // Attach Interrupt

  pinMode(BUZZ, OUTPUT);
  pinMode(ARM, OUTPUT);
  BlueT.begin(9600);
  BlueT.write("Welcome to BlueTooth alarm. Enter 'a' to arm and 'd' to disarm \n");

}

/****************** MAIN CODE ******************/
/*     Accelerometer Readings and Interrupt    */


void loop(){

  if(BlueT.available())
  {
    inputChar = BlueT.read();
    Serial.println(inputChar);
    while(inputChar != 'a' || inputChar != 'd')
    {
      BlueT.println("Please enter a valid command: \'a\' for arm, \'d\' for disarm");
      delay(1000);
      inputChar = BlueT.read(); 
    }

    if(inputChar == 'a')
    {
      BlueT.println("It is armed");
      armed = true;
      digitalWrite(ARM, HIGH);
    }
    else
    {
      BlueT.println("It is disarmed");
      armed = false;
      digitalWrite(ARM, LOW);
    }
  } 
  // Accelerometer Readings
  if(armed)
  {
    adxl.readAccel(&x, &y, &z);// Read the accelerometer values and store them in variables declared above x,y,z
    delay(100);
    adxl.readAccel(&x1,&y1,&z1);
    while(detectMotion(x, y, z, x1, y1, z1))
    {
      tone(BUZZ, A5);
      BlueT.write("Detected motion, please check your stuff \n");
      delay(500);
      noTone(BUZZ);
      delay(500);
      adxl.readAccel(&x, &y, &z);// Read the accelerometer values and store them in variables declared above x,y,z
      delay(100);
      adxl.readAccel(&x1,&y1,&z1);
    }
  }

  delay(1000);
  
  // Output Results to Serial
  /* UNCOMMENT TO VIEW X Y Z ACCELEROMETER VALUES */  
  // Serial.print(x);
  // Serial.print(", ");
  // Serial.print(y);
  // Serial.print(", ");
  // Serial.println(z); 
  //if (
 // tone(buzzer, 1000);
  
  
  
  //ADXL_ISR();
  // You may also choose to avoid using interrupts and simply run the functions within ADXL_ISR(); 
  //  and place it within the loop instead.  
  // This may come in handy when it doesn't matter when the action occurs. 

}
