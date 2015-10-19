#include <Wire.h>
#include <LSM303.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>
//#include <SdFat.h>
//#include <SD.h>  //lib is much nicer, but uses too much memory.

LSM303 compass;
TinyGPS gps;
//SdFat sd;

//const uint8_t chipSelect = SS;  // AKA pin 10.

//ArduinoOutStream cout(Serial);  // For SD card writing.

// store error strings in flash to save RAM
//#define error(s) sd.errorHalt_P(PSTR(s))

SoftwareSerial nssGPS(6,255);  // The wire to send stuff to GPS is not connected.

// initialize pins for the belt.

byte beltSel0 = 2;
byte beltSel1 = 3;
byte beltSel2 = 4;
byte beltEnable = 5;

byte manSwitch = 7;  // Compass vs GPS and compass.

long theHeading;  // Used long for averaging 100 values.
long gpsHeading;  // The GPS returns a long.  Not sure why yet.
int previousHeading;
int heading;

int compassCorrection;

float flat, flon;  // latitude and longitude GPS values.

unsigned long time;  // Later, perhaps get actual time from GPS.  
// For now, time since it was turned on.
unsigned long previousTime;

// Name of log file.
//char name[] = "BELT_L00.TXT";
 
void setup() {

  // initialize the SD card at SPI_HALF_SPEED to avoid bus errors with
  // breadboards.  use SPI_FULL_SPEED for better performance.
 // if (!sd.begin(chipSelect, SPI_HALF_SPEED)) sd.initErrorHalt();

  // todo:  make it run even if the SD card isn't in.

  pinMode(beltSel0, OUTPUT);
  pinMode(beltSel1, OUTPUT);
  pinMode(beltSel2, OUTPUT);
  pinMode(beltEnable, OUTPUT);  // PWM this pin.

  pinMode(manSwitch, INPUT);

  Serial.begin(115200);    // Remember to change the baud on the serial monitor.
  nssGPS.begin(4800);
  Wire.begin();            // I2C interface for compass.

  // Calibration values. Use the Calibrate example program to get the values for
  // your compass.

  // This should be re-done when everything is permanately mounted.

  //compass.m_min.x = -520; compass.m_min.y = -570; compass.m_min.z = -770;
  compass.m_min.x = -616; 
  compass.m_min.y = -673; 
  compass.m_min.z = -545;
  //compass.m_max.x = +540; compass.m_max.y = +500; compass.m_max.z = 180;
  compass.m_max.x = +480; 
  compass.m_max.y = +439; 
  compass.m_max.z = 482;

  compass.init();
  compass.enableDefault();

   // Name the data file with the first numbers that are not used.
  
//  for (uint8_t i = 0; i < 100; i++) {
//    name[6] = i/10 + '0';
//    name[7] = i%10 + '0';
//    if (sd.exists(name)) 
//      continue;
//    else
//      break;
//  }
//
//  Serial.print("Data file name: ");
//  Serial.println(name);

  float xVal = 0;   
  float yVal = 0;
  int nowHeading = 0;
  for (int i = 0;i<100;i++)  {  // 50 readings, 25 milli seconds in between.
    nowHeading = getCompassHeading();
    xVal = xVal + cos(nowHeading * 3.14159/180);  // degree to radian.
    yVal = yVal + sin(nowHeading * 3.14159/180);
    delay(10);
  }
  xVal = xVal / 100;
  yVal = yVal / 100;
  compassCorrection = (atan2(yVal,xVal)*180/3.14159);
  
  compassCorrection = 359-compassCorrection;
  
  if (compassCorrection < 0)
    compassCorrection +=359;
  
}


void loop() {

  theHeading = 0;    // This will get compass or GPS heading.
  
   // Averaging angles, (350 + 10)/2 = 180, but it should be 5.
   // See http://en.wikipedia.org/wiki/Mean_of_circular_quantities
   // for the formula used here.
  
  float xVal = 0;   
  float yVal = 0;
  int nowHeading;
  for (int i = 0;i<10;i++)  {  // 50 readings, 25 milli seconds in between.
    nowHeading = getCompassHeading();
    xVal = xVal + cos(nowHeading * 3.14159/180);  // degree to radian.
    yVal = yVal + sin(nowHeading * 3.14159/180);
    delay(10);
  }
  xVal = xVal / 10;
  yVal = yVal / 10;
  theHeading = (atan2(yVal,xVal)*180/3.14159);
  if (theHeading < 0)
    theHeading +=359;

  Serial.print("Compass heading:");
  Serial.println(theHeading);

//  Serial.println(nowHeading);

  Serial.println();
  Serial.print("free memory:");
  Serial.println(freemem());

  if (digitalRead(manSwitch)==HIGH){
    Serial.println("going to GPS");
    gpsHeading = getGPSHeading();
    theHeading = theHeading + gpsHeading;
    if (theHeading > 359)
      theHeading -=359;
  }

  time = millis();

  if((time-previousTime > 1000) || (abs(theHeading - previousHeading)> 6)){
    //vibrate when heading changes, or every 1 sec.
    vibrate();

    // Write to serial monitor.

    Serial.print("compass heading: ");
    Serial.println(theHeading);
    Serial.print("time: ");
    Serial.println(time);
//    Serial.print("Writing to log...");
//    
//    // Write to SD card.
//
//    ofstream sdout(name, ios::out | ios::app);  // Create or append file.
//
//    if (!sdout) error("open failed");
//
//    if (digitalRead(manSwitch)==HIGH){
//      sdout << "Latitude, Longitude: ";
//      sdout <<  setprecision(5)<<flat;
//      sdout <<", ";
//      sdout <<flon << endl;
//      sdout <<"GPS heading: ";
//      sdout << gpsHeading << endl;
//    }
//
//    sdout <<"compass heading: ";
//    sdout << theHeading<<endl;
//
//    sdout <<"time: ";
//    sdout <<time<<endl;
//
//    Serial.println("done.");
  } 
}


int getCompassHeading(){

  compass.read();
  heading=compass.heading((LSM303::vector){
    0,-1,0      }
  );

  heading = heading - compassCorrection;  //correction for how it is (was) mounted.
                            //this should be re-calibrated as well.

  if (heading < 0)
    heading = heading + 359;

  heading = 359-heading;  // Is it supposed to be mounted upside down???
  
  return (heading);
}

int getGPSHeading(){

  while (! feedgps()){
    feedgps();
  }

  gpsdump(gps);

  return (gpsHeading);
}

void vibrate(){

   //vib0
  if (theHeading > 348.75 || theHeading < 11.25){
    Serial.println("vibrate 0");
    digitalWrite (beltSel2,LOW);
    digitalWrite (beltSel1,LOW);
    digitalWrite (beltSel0,LOW);
    analogWrite (beltEnable,125);
    delay(500);
    digitalWrite(beltEnable,LOW);   
  }

  //vib1
  if (theHeading > 11.25 && theHeading < 33.75){
    Serial.println("vibrate 1");
    digitalWrite (beltSel2,LOW);
    digitalWrite (beltSel1,LOW);
    digitalWrite(beltSel0,LOW);
    digitalWrite(beltEnable,HIGH);
    for (int i=0;i< 50;i++){
      delay(5);
      digitalWrite(beltSel2,LOW);
      digitalWrite(beltSel1,LOW);
      digitalWrite(beltSel0,LOW);
      delay(5);
      digitalWrite(beltSel2,LOW);
      digitalWrite(beltSel1,LOW);
      digitalWrite(beltSel0,HIGH);
    }
    digitalWrite(beltEnable,LOW);
  }
 
   //vib2
  if (theHeading > 33.75 && theHeading < 56.25){
    Serial.println("vibrate 2");
    digitalWrite (beltSel2,LOW);
    digitalWrite (beltSel1,LOW);
    digitalWrite (beltSel0,HIGH);
    analogWrite (beltEnable,128);
    delay(500);
    digitalWrite(beltEnable,LOW);   
  }

  //vib3
  if (theHeading > 56.25 && theHeading < 78.75){
    Serial.println("vibrate 3");
    digitalWrite (beltSel2,LOW);
    digitalWrite (beltSel1,LOW);
    digitalWrite(beltSel0,HIGH);
    digitalWrite(beltEnable,HIGH);
    for (int i=0;i< 50;i++){
      delay(5);
      digitalWrite(beltSel2,LOW);
      digitalWrite(beltSel1,LOW);
      digitalWrite(beltSel0,HIGH);
      delay(5);
      digitalWrite(beltSel2,LOW);
      digitalWrite(beltSel1,HIGH);
      digitalWrite(beltSel0,LOW);
    }
    digitalWrite(beltEnable,LOW);
  }
  
  //vib4
  if (theHeading > 78.75 && theHeading < 101.25){
    Serial.println("vibrate 4");
    digitalWrite (beltSel2,LOW);
    digitalWrite (beltSel1,HIGH);
    digitalWrite (beltSel0,LOW);
    analogWrite (beltEnable,125);
    delay(500);
    digitalWrite(beltEnable,LOW);   
  }

  //vib5
  if (theHeading > 101.25 && theHeading < 123.75){
    Serial.println("vibrate 5");
    digitalWrite (beltSel2,LOW);
    digitalWrite (beltSel1,HIGH);
    digitalWrite(beltSel0,LOW);
    digitalWrite(beltEnable,HIGH);
    for (int i=0;i< 50;i++){
      delay(5);
      digitalWrite(beltSel2,LOW);
      digitalWrite(beltSel1,HIGH);
      digitalWrite(beltSel0,LOW);
      delay(5);
      digitalWrite(beltSel2,LOW);
      digitalWrite(beltSel1,HIGH);
      digitalWrite(beltSel0,HIGH);
    }
    digitalWrite(beltEnable,LOW);
  }
  
  //vib6
  if (theHeading > 123.75 && theHeading < 146.25){
    Serial.println("vibrate 6");
    digitalWrite (beltSel2,LOW);
    digitalWrite (beltSel1,HIGH);
    digitalWrite (beltSel0,HIGH);
    analogWrite (beltEnable,125);
    delay(500);
    digitalWrite(beltEnable,LOW);   
  }

  //vib7
  if (theHeading > 146.25 && theHeading < 168.75){
    Serial.println("vibrate 7");
    digitalWrite (beltSel2,LOW);
    digitalWrite (beltSel1,HIGH);
    digitalWrite(beltSel0,HIGH);
    digitalWrite(beltEnable,HIGH);
    for (int i=0;i< 50;i++){
      delay(5);
      digitalWrite(beltSel2,LOW);
      digitalWrite(beltSel1,HIGH);
      digitalWrite(beltSel0,HIGH);
      delay(5);
      digitalWrite(beltSel2,HIGH);
      digitalWrite(beltSel1,LOW);
      digitalWrite(beltSel0,LOW);
    }
    digitalWrite(beltEnable,LOW);
  }
  
  //vib8
  if (theHeading > 168.75 && theHeading < 191.25){
    Serial.println("vibrate 8");
    digitalWrite (beltSel2,HIGH);
    digitalWrite (beltSel1,LOW);
    digitalWrite (beltSel0,LOW);
    analogWrite (beltEnable,125);
    delay(500);
    digitalWrite(beltEnable,LOW);   
  }

  //vib9
  if (theHeading > 191.25 && theHeading < 213.75){
    Serial.println("vibrate 9");
    digitalWrite (beltSel2,HIGH);
    digitalWrite (beltSel1,LOW);
    digitalWrite(beltSel0,LOW);
    digitalWrite(beltEnable,HIGH);
    for (int i=0;i< 50;i++){
      delay(5);
      digitalWrite(beltSel2,HIGH);
      digitalWrite(beltSel1,LOW);
      digitalWrite(beltSel0,LOW);
      delay(5);
      digitalWrite(beltSel2,HIGH);
      digitalWrite(beltSel1,LOW);
      digitalWrite(beltSel0,HIGH);
    }
    digitalWrite(beltEnable,LOW);
  }
  
  //vib10
  if (theHeading > 213.75 && theHeading < 236.25){
    Serial.println("vibrate 10");
    digitalWrite (beltSel2,HIGH);
    digitalWrite (beltSel1,LOW);
    digitalWrite (beltSel0,HIGH);
    analogWrite (beltEnable,125);
    delay(500);
    digitalWrite(beltEnable,LOW);   
  }

  //vib11
  if (theHeading > 236.25 && theHeading < 258.75){
    Serial.println("vibrate 11");
    digitalWrite (beltSel2,HIGH);
    digitalWrite (beltSel1,LOW);
    digitalWrite(beltSel0,HIGH);
    digitalWrite(beltEnable,HIGH);
    for (int i=0;i< 50;i++){
      delay(5);
      digitalWrite(beltSel2,HIGH);
      digitalWrite(beltSel1,LOW);
      digitalWrite(beltSel0,HIGH);
      delay(5);
      digitalWrite(beltSel2,HIGH);
      digitalWrite(beltSel1,HIGH);
      digitalWrite(beltSel0,LOW);
    }
    digitalWrite(beltEnable,LOW);
  }
  
  //vib12
  if (theHeading > 258.75 && theHeading < 281.25){
    Serial.println("vibrate 12");
    digitalWrite (beltSel2,HIGH);
    digitalWrite (beltSel1,HIGH);
    digitalWrite (beltSel0,LOW);
    analogWrite (beltEnable,125);
    delay(500);
    digitalWrite(beltEnable,LOW);   
  }

  //vib13
  if (theHeading > 281.25 && theHeading < 303.75){
    Serial.println("vibrate 13");
    digitalWrite (beltSel2,HIGH);
    digitalWrite (beltSel1,HIGH);
    digitalWrite(beltSel0,LOW);
    digitalWrite(beltEnable,HIGH);
    for (int i=0;i< 50;i++){
      delay(5);
      digitalWrite(beltSel2,HIGH);
      digitalWrite(beltSel1,HIGH);
      digitalWrite(beltSel0,LOW);
      delay(5);
      digitalWrite(beltSel2,HIGH);
      digitalWrite(beltSel1,HIGH);
      digitalWrite(beltSel0,HIGH);
    }
    digitalWrite(beltEnable,LOW);
  }
  
  //vib14
  if (theHeading > 303.75 && theHeading < 326.25){
    Serial.println("vibrate 14");
    digitalWrite (beltSel2,HIGH);
    digitalWrite (beltSel1,HIGH);
    digitalWrite (beltSel0,HIGH);
    analogWrite (beltEnable,125);
    delay(500);
    digitalWrite(beltEnable,LOW);   
  }

  //vib15
  if (theHeading > 326.25 && theHeading < 348.75){
    Serial.println("vibrate 15");
    digitalWrite (beltSel2,HIGH);
    digitalWrite (beltSel1,HIGH);
    digitalWrite(beltSel0,HIGH);
    digitalWrite(beltEnable,HIGH);
    for (int i=0;i< 50;i++){
      delay(5);
      digitalWrite(beltSel2,HIGH);
      digitalWrite(beltSel1,HIGH);
      digitalWrite(beltSel0,HIGH);
      delay(5);
      digitalWrite(beltSel2,LOW);
      digitalWrite(beltSel1,LOW);
      digitalWrite(beltSel0,LOW);
    }
    digitalWrite(beltEnable,LOW);
  } 

  delay (250);  // Be sure the motors are not spinning before taking a compass reading.  
                // This could probably be reduced.

  previousTime = millis();

  previousHeading = theHeading;

}

// Feed data as it becomes available 
bool feedgps() {
  while (nssGPS.available()) {
    if (gps.encode(nssGPS.read()))
      return true;
  }
  return false;
}

void gpsdump(TinyGPS &gps) {
  unsigned long age;
  gps.f_get_position(&flat, &flon, &age);
  Serial.print(flat, 5); 
  Serial.print(", "); 
  Serial.println(flon, 5);

  // This is where you put in the coordinates of where GPS navigation is pointed.
  // Currently, middle of the lake at UR.
  gpsHeading = (TinyGPS::course_to(flat, flon, 37.57612, -77.53968));
  Serial.println(gpsHeading);

}

// this function will return the number of bytes currently free in RAM   
// if it returns a number less than 200, be worried.
// if it returns a negative number, it's about to crash.

extern int  __bss_end; 
extern int  *__brkval; 
int freemem()
{ 
  int free_memory; 
  if((int)__brkval == 0) 
    free_memory = ((int)&free_memory) - ((int)&__bss_end); 
  else 
    free_memory = ((int)&free_memory) - ((int)__brkval); 
  return free_memory; 
} 

