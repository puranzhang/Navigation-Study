#include <Wire.h>    // I2C interface for compass.
#include <LSM303.h>  // Compass software, modified to be not tilt compensated.
#include <SoftwareSerial.h>  // GPS on soft serial.
#include <TinyGPS.h>

#include <SdFat.h>
//#include <SD.h>  //lib is much nicer, but uses too much memory.

LSM303 compass;
TinyGPS gps;
SdFat sd;

const uint8_t chipSelect = SS;  // AKA pin 10.

ArduinoOutStream cout(Serial);  // For SD card writing.

// store error strings in flash to save RAM
#define error(s) sd.errorHalt_P(PSTR(s))

SoftwareSerial nssGPS(6,255);  // The wire to send stuff to GPS is not connected.

// initialize pins for the belt.

byte beltSel0 = 2;   // MUX pins
byte beltSel1 = 3;
byte beltSel2 = 4;
int beltEnable = 5;  // PWM enable pin

byte manSwitch = 7;  // Compass vs GPS and compass.

long gpsHeading;  // The GPS returns a long.  Not sure why yet.
boolean newGps;
int previousHeading;
int headingAt;    // Heading the compass is pointed at.
int headingTo;    // Heading of vibrator to point person to north.

int compassHeading;

int vibNum;  

int compassCorrection;  // Startup calibration, stand facing north.

float flat, flon;  // latitude and longitude GPS values.

unsigned long time;   // Later, perhaps get actual time from GPS...  
// For now, time since it was turned on.

unsigned long previousTime;

// Name of log file.
char name[] = "BELT_L00.TXT";

void setup() {

  // initialize the SD card at SPI_HALF_SPEED to avoid bus errors with
  // breadboards.  use SPI_FULL_SPEED for better performance.
  if (!sd.begin(chipSelect, SPI_FULL_SPEED)) sd.initErrorHalt();

  // todo:  make it run even if the SD card isn't in.

  pinMode(beltSel0, OUTPUT);  // MUX pins
  pinMode(beltSel1, OUTPUT);
  pinMode(beltSel2, OUTPUT);
  pinMode(beltEnable, OUTPUT);  // PWM this pin.

  pinMode(manSwitch, INPUT);    //compass/GPS switch

  Serial.begin(115200);    // Remember to change the baud on the serial monitor.
  nssGPS.begin(4800);
  newGps = 0;              // Have new gps heading?
  Wire.begin();            // I2C interface for compass.

  // Calibration values. Use the Calibrate example program to get the values for
  // your compass.

  // This should be re-done.

  //compass.m_min.x = -520; compass.m_min.y = -570; compass.m_min.z = -770;
  //  compass.m_min.x = -616; 
  //  compass.m_min.y = -673; 
  //  compass.m_min.z = -545;
  //  //compass.m_max.x = +540; compass.m_max.y = +500; compass.m_max.z = 180;
  //  compass.m_max.x = +480; 
  //  compass.m_max.y = +439; 
  //  compass.m_max.z = 482;
  compass.m_min.x = -429; 
  compass.m_min.y = -509; 
  compass.m_min.z = -585;
  //compass.m_max.x = +540; compass.m_max.y = +500; compass.m_max.z = 180;
  compass.m_max.x = +110; 
  compass.m_max.y = +131; 
  compass.m_max.z = -441;

  compass.init();
  compass.enableDefault();

  // Name the data file with the first numbers that are not used.

  for (uint8_t i = 0; i < 100; i++) {
    name[6] = i/10 + '0';
    name[7] = i%10 + '0';
    if (sd.exists(name)) 
      continue;
    else
      break;
  }

  Serial.print("Data file name: ");
  Serial.println(name);

  // calibrate to however it's being worn.

  // Averaging angles, (350 + 10)/2 = 180, but it should be 5.
  // See http://en.wikipedia.org/wiki/Mean_of_circular_quantities
  // for the formula used here.


  float xVal = 0;   
  float yVal = 0;
  //headingAt = 0;
  for (int i = 0;i<100;i++)  {  
    headingAt = getCompassHeading();
    xVal = xVal + cos(headingAt * M_PI/180);  // degree to radian.
    yVal = yVal + sin(headingAt * M_PI/180);
    delay(10);
  }
  xVal = xVal / 100;
  yVal = yVal / 100;
  compassCorrection = (atan2(yVal,xVal)*180/M_PI);

  compassCorrection += 12;  //11.25, but it's an int.  
  //vibrators art centered on heading, not at the beginning of it.

  if (compassCorrection < 0)
    compassCorrection +=359;
  if (compassCorrection > 359)
    compassCorrection -=359;
}

void loop() {

  time = millis();

  if(abs(time-previousTime > 1500) || (abs(headingAt - previousHeading)> 6)){
    //vibrate when heading changes, or every 1 sec.

    previousTime = millis();

    float xVal = 0;   
    float yVal = 0;
    for (int i = 0;i<10;i++)  {  // 10 readings, 5 milli seconds in between.
      headingAt = getCompassHeading();
      xVal = xVal + cos(headingAt * M_PI/180);  // degree to radian.
      yVal = yVal + sin(headingAt * M_PI/180);
      delay(5);
    }
    xVal = xVal / 10;
    yVal = yVal / 10;
    headingAt = (atan2(yVal,xVal)*180/M_PI);
    if (headingAt < 0)
      headingAt +=359;

    compassHeading = headingAt;

    if ((digitalRead(manSwitch)==HIGH)&&!newGps){  // if gps
      Serial.println("GPS..");
      gpsHeading = getGPSHeading();
      newGps = 1;
    }
    if (digitalRead(manSwitch)==HIGH){
      headingAt = headingAt-gpsHeading;
      if (headingAt <0)
        headingAt +=359;
    }
    newGps = 0;

    vibrate();

    // Write to serial monitor.
    Serial.print("mem:");
    Serial.println(freemem());

    Serial.print("heading: ");
    Serial.println(headingAt);
    Serial.print("time: ");
    Serial.println(time);
    Serial.print("Write log..");

    // Write to SD card.

    ofstream sdout(name, ios::out | ios::app);  // Create or append file.

    if (!sdout) error("open failed");

    if (digitalRead(manSwitch)==HIGH){
      sdout << "Lat, Lon:";
      sdout <<  setprecision(5)<<flat;
      sdout <<", ";
      sdout <<flon << endl;
      sdout <<"GPS heading: ";
      sdout << gpsHeading << endl;
    }

    sdout <<"compass heading: ";
    sdout <<compassHeading<<endl;   
    sdout <<"time: ";
    sdout <<time<<endl;

    Serial.println("done.");
  }  
}


int getCompassHeading(){

  compass.read();
  headingAt=compass.heading((LSM303::vector){
    0,-1,0      }
  );

  headingAt = headingAt - compassCorrection; //Calibration from when first switched on.

  if (headingAt < 0)
    headingAt = headingAt + 359;
  return (headingAt);
}

int getGPSHeading(){
  while (! feedgps()){
    feedgps();
  }

  gpsdump(gps);
  return (gpsHeading);
}

void vibrate(){
  headingTo = 359-headingAt;
  vibNum = floor((headingTo-11.25)/ 22.5);  //349 degrees should still be the front vib.
  if(vibNum == -1)
    vibNum = 15;
  if ((vibNum % 2)==0)
    vibOne();
  else
    vibTwo();
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
  delay(500);

  digitalWrite(beltEnable,LOW);

  previousHeading = headingAt;

  if (digitalRead(manSwitch)==HIGH){
    gpsHeading = getGPSHeading();
    newGps = 1;
  }

}

void vibTwo(){

  byte beltSel0_0;  //switch between this vib
  byte beltSel1_0;
  byte beltSel2_0;

  byte beltSel0_1;  //and this one.
  byte beltSel1_1;
  byte beltSel2_1;

  int tempVibNum = vibNum;

  tempVibNum /= 2;  
  beltSel0_0 = tempVibNum%2;
  tempVibNum /= 2;  
  beltSel1_0 = tempVibNum%2;
  tempVibNum /= 2;  
  beltSel2_0 = tempVibNum%2;

  tempVibNum = vibNum;
  tempVibNum /= 2;
  tempVibNum +=1;
  //if (tempVibNum == 8)  //  This seems to make it crash,  I will figure out why.
  //  tempVibNum = 0;     // it also seems to work without it...
  beltSel0_1 = tempVibNum%2;
  tempVibNum /= 2;  
  beltSel1_1 = tempVibNum%2;
  tempVibNum /= 2;  
  beltSel2_1 = tempVibNum%2;

  for(int i = 0;i<4;i++){  //  500 ms or so of vibration.
    digitalWrite (beltSel0,beltSel0_0);
    digitalWrite (beltSel1,beltSel1_0);
    digitalWrite (beltSel2,beltSel2_0);
    digitalWrite (beltEnable, HIGH);
    delay(65);
    digitalWrite (beltSel0,beltSel0_1);
    digitalWrite (beltSel1,beltSel1_1);
    digitalWrite (beltSel2,beltSel2_1);
    delay(65);
  }

  digitalWrite (beltEnable, LOW);

  previousHeading = headingAt;

  if (digitalRead(manSwitch)==HIGH){
    gpsHeading = getGPSHeading();
    newGps = 1;
  }
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






