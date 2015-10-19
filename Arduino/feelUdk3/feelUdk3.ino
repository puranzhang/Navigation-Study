//#include <Wire.h>    // I2C interface for compass.
//#include <LSM303.h>  // Compass software, modified to be not tilt compensated.
//#include <SoftwareSerial.h>  // GPS on soft serial.
//#include <TinyGPS.h>

//#include <SdFat.h>
//#include <SD.h>  //lib is much nicer, but uses too much memory.

//LSM303 compass;
//TinyGPS gps;
//SdFat sd;

//const uint8_t chipSelect = SS;  // AKA pin 10.

//ArduinoOutStream cout(Serial);  // For SD card writing.

// store error strings in flash to save RAM
//#define error(s) sd.errorHalt_P(PSTR(s))

//SoftwareSerial nssGPS(6,255);  // The wire to send stuff to GPS is not connected.

// initialize pins for the belt.

byte beltSel0 = 2;   // MUX pins
byte beltSel1 = 3;
byte beltSel2 = 4;
int beltEnable = 5;  // PWM enable pin

byte manSwitch = 7;  // Compass vs GPS and compass.

//long gpsHeading;  // The GPS returns a long.  Not sure why yet.
//boolean newGps;
int previousHeading;
int headingAt;    // Heading the compass is pointed at.
int headingTo;    // Heading of vibrator to point person to north.
int theHeading;// Heading of vibrator to point person to north.

int compassHeading;

int vibNum;  

int compassCorrection;  // Startup calibration, stand facing north.

int val_1;    // variable to read the value from the serial port
int buf;    // concatenated into this var.
int val; //possible three digit integer
static unsigned long lWaitMillis;


//float flat, flon;  // latitude and longitude GPS values.

unsigned long time;   // Later, perhaps get actual time from GPS...  
// For now, time since it was turned on.

unsigned long previousTime;

// Name of log file.
//char name[] = "BELT_L00.TXT";

void setup() {

    while (!Serial);  // wait for serial to begin.


  // initialize the SD card at SPI_HALF_SPEED to avoid bus errors with
  // breadboards.  use SPI_FULL_SPEED for better performance.
  //if (!sd.begin(chipSelect, SPI_FULL_SPEED)) sd.initErrorHalt();

  // todo:  make it run even if the SD card isn't in.

  pinMode(beltSel0, OUTPUT);  // MUX pins
  pinMode(beltSel1, OUTPUT);
  pinMode(beltSel2, OUTPUT);
  pinMode(beltEnable, OUTPUT);  // PWM this pin.

  pinMode(manSwitch, INPUT);    //compass/GPS switch

  //Serial.begin(115200);    // Remember to change the baud on the serial monitor.
  Serial.begin(9600);
  //nssGPS.begin(4800);
  //newGps = 0;              // Have new gps heading?
  //Wire.begin();            // I2C interface for compass.

  // Calibration values. Use the Calibrate example program to get the values for
  // your compass.

  // This should be re-done.

 
  // Name the data file with the first numbers that are not used.

//  for (uint8_t i = 0; i < 100; i++) {
//    name[6] = i/10 + '0';
//    name[7] = i%10 + '0';
//    if (sd.exists(name)) 
//      continue;
//    else
//      break;
//  }

//  Serial.print("Data file name: ");
//  Serial.println(name);

  // calibrate to however it's being worn.

  // Averaging angles, (350 + 10)/2 = 180, but it should be 5.
  // See http://en.wikipedia.org/wiki/Mean_of_circular_quantities
  // for the formula used here.


  float xVal = 0;   
  float yVal = 0;
  //headingAt = 0;
//  for (int i = 0;i<100;i++)  {  
//    headingAt = 180;
//    xVal = xVal + cos(headingAt * M_PI/180);  // degree to radian.
//    yVal = yVal + sin(headingAt * M_PI/180);
//    delay(10);
//  }
//  xVal = xVal / 100;
//  yVal = yVal / 100;
//  compassCorrection = (atan2(yVal,xVal)*180/M_PI);
//
//  compassCorrection += 12;  //11.25, but it's an int.  
//  //vibrators art centered on heading, not at the beginning of it.
//
//  if (compassCorrection < 0)
//    compassCorrection +=359;
//  if (compassCorrection > 359)
//    compassCorrection -=359;
}

void loop() {

  if (Serial.available()){


    val_1 = (Serial.read()-'0');
    if (val_1 ==49){
      //val = map(buf, 0, 179, 60, 120);     // scale it to use it with the servo (value between 0 and 180)
      //myservo.write(val);       // sets the servo position according to the scaled value
      //Serial.print ("buf:");
      //Serial.println (buf);
      headingAt = buf;
      Serial.print("Heading at:");
      Serial.println(headingAt);
      buf = 0;

      }else {  // concatenate one digit integers ...
    buf = buf * 10 + val_1;  
  }
  }
  else {
    // Why isn't serial available?
  }
  

      time = millis();

      if(abs(time-previousTime > 500)){
      //vibrate when heading changes, or every 1 sec.

        previousTime = millis();

        //float xVal = 0;   
        //float yVal = 0;
//      for (int i = 0;i<10;i++)  {  // 10 readings, 5 milli seconds in between.
//        headingAt =180;
//        xVal = xVal + cos(headingAt * M_PI/180);  // degree to radian.
//        yVal = yVal + sin(headingAt * M_PI/180);
//        delay(5);
//      }
        //xVal = xVal / 10;
        //yVal = yVal / 10;
        //headingAt = (atan2(yVal,xVal)*180/M_PI);
        if (headingAt < 0)
          headingAt +=359;

        compassHeading = headingAt;
       vibrate();
  } // end of if time

}

        void vibrate(){
          theHeading = headingAt;
//    vibNum = floor((headingTo-11.25)/ 22.5);  //349 degrees should still be the front vib.
//    if(vibNum == -1)
//      vibNum = 15;
//    if ((vibNum % 2)==0)
//      vibOne();
//    else
//      vibOne();
      vibrateSimple();
  }
  
  void vibOne(){
    int tempVibNum = vibNum;  //Pick out the bits of this int, use them for mux.
    tempVibNum /= 2;  
    digitalWrite (beltSel0,(tempVibNum%2));
    tempVibNum /= 2;  
    digitalWrite (beltSel1,(tempVibNum%2));
    tempVibNum /= 2;    
    digitalWrite (beltSel2,(tempVibNum%2));

    analogWrite (beltEnable,160);
    delay(100);

    digitalWrite(beltEnable,LOW);

    previousHeading = headingAt;

  //  if (digitalRead(manSwitch)==HIGH){
  //    gpsHeading = getGPSHeading();
  //    newGps = 1;
  //  }

  }
    void vibrateSimple(){
  // vib0
  theHeading = 360-compassHeading;  // Go from your heading to which side of the belt to use.
  if (theHeading > 337.5 || theHeading < 22.5){
    Serial.println("vibrate 0");
    digitalWrite (beltSel2,LOW);
    digitalWrite (beltSel1,LOW);
    digitalWrite (beltSel0,LOW);
    digitalWrite (beltEnable,HIGH);
    delay(100);
    digitalWrite(beltEnable,LOW);
  }
  //vib1
  if (theHeading > 22.5 && theHeading < 67.5){
    Serial.println("vibrate 1");
    digitalWrite (beltSel2,LOW);
    digitalWrite (beltSel1,LOW);
    digitalWrite (beltSel0,HIGH);
    digitalWrite (beltEnable,HIGH);
    delay(100);
    digitalWrite(beltEnable,LOW);
  }
  //vib2
  if (theHeading > 67.5 && theHeading < 112.5){
    Serial.println("vibrate 2");
    digitalWrite (beltSel2,LOW);
    digitalWrite (beltSel1,HIGH);
    digitalWrite (beltSel0,LOW);
    digitalWrite (beltEnable,HIGH);
    delay(100);
    digitalWrite(beltEnable,LOW);
  }
  //vib3
  if (theHeading > 112.5 && theHeading < 157.5){
    Serial.println("vibrate 3");
    digitalWrite (beltSel2,LOW);
    digitalWrite (beltSel1,HIGH);
    digitalWrite (beltSel0,HIGH);
    digitalWrite (beltEnable,HIGH);
    delay(100);
    digitalWrite(beltEnable,LOW);
  }
  //vib4
  if (theHeading > 157.5 && theHeading < 202.5){
    Serial.println("vibrate 4");
    digitalWrite (beltSel2,HIGH);
    digitalWrite (beltSel1,LOW);
    digitalWrite (beltSel0,LOW);
    digitalWrite (beltEnable,HIGH);
    delay(100);
    digitalWrite(beltEnable,LOW);
  }
  //vib5
  if (theHeading > 202.5 && theHeading < 247.5){
    Serial.println("vibrate 5");
    digitalWrite (beltSel2,HIGH);
    digitalWrite (beltSel1,LOW);
    digitalWrite (beltSel0,HIGH);
    digitalWrite (beltEnable,HIGH);
    delay(100);
    digitalWrite(beltEnable,LOW);
  }
  //vib6
  if (theHeading > 247.5 && theHeading < 292.5){
    Serial.println("vibrate 6");
    digitalWrite (beltSel2,HIGH);
    digitalWrite (beltSel1,HIGH);
    digitalWrite (beltSel0,LOW);
    digitalWrite (beltEnable,HIGH);
    delay(100);
    digitalWrite(beltEnable,LOW);
  }
  //vib7
  if (theHeading > 292.5 && theHeading < 337.5){
    Serial.println("vibrate 7");
    digitalWrite (beltSel2,HIGH);
    digitalWrite (beltSel1,HIGH);
    digitalWrite (beltSel0,HIGH);
    digitalWrite (beltEnable,HIGH);
    delay(100);
    digitalWrite(beltEnable,LOW);
  }

  previousTime = millis();

  previousHeading = theHeading;

}
  
//}


