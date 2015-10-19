#include <Wire.h>
#include <LSM303.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>
//#include <SD.h>
#include <SdFat.h>

LSM303 compass;
TinyGPS gps;
//File myFile;

SoftwareSerial nssGPS(6,255);

// initialize pins for the belt.

int beltSel0 = 2;
int beltSel1 = 3;
int beltSel2 = 4;
int beltEnable = 5;

int manSwitch = 7;

boolean useGPS;

long theHeading;
long gpsHeading;
int previousHeading;

float flat, flon;

unsigned long time;
unsigned long previousTime;

//char name[] = "BELT_L00.TXT";

int heading;

void setup() {

  pinMode(beltSel0, OUTPUT);
  pinMode(beltSel1, OUTPUT);
  pinMode(beltSel2, OUTPUT);
  pinMode(beltEnable, OUTPUT);

  pinMode(manSwitch, INPUT);

  Serial.begin(115200);    // Remember to change the baud on the serial monitor.

  nssGPS.begin(4800);

  Wire.begin();            // Initialize compass.
  compass.init();
  compass.enableDefault();

  // Initialize SD card
//  Serial.print("Initializing SD card...");
//  pinMode(10, OUTPUT);
//  if (!SD.begin(10)) {
//    Serial.println("initialization failed!");
//    return;
//  }
//  Serial.println("SD initialization done.");
//
//  // Name the data file
//  for (uint8_t i = 0; i < 100; i++) {
//    name[6] = i/10 + '0';
//    name[7] = i%10 + '0';
//    if (SD.exists(name)) 
//      continue;
//    else
//      break;
//  }
//
//  Serial.print("Data file name: ");
//  Serial.println(name);


  // Calibration values. Use the Calibrate example program to get the values for
  // your compass.
  //compass.m_min.x = -520; compass.m_min.y = -570; compass.m_min.z = -770;
  compass.m_min.x = -616; 
  compass.m_min.y = -673; 
  compass.m_min.z = -545;
  //compass.m_max.x = +540; compass.m_max.y = +500; compass.m_max.z = 180;
  compass.m_max.x = +480; 
  compass.m_max.y = +439; 
  compass.m_max.z = 482;
}


void loop() {
  
//  if (digitalRead(manSwitch)==HIGH)
//    useGPS = true;
//  else
    useGPS = false;  

  theHeading = 0;
  float xVal = 0;  
  float yVal = 0;
  int nowHeading;
  
 for (int i = 0;i<50;i++)  {
   nowHeading = getCompassHeading();
   xVal = xVal + cos(nowHeading * 3.14159/180);
   yVal = yVal + sin(nowHeading * 3.14159/180);
   
   delay(25);
 }
  
  xVal = xVal / 50;
  yVal = yVal / 50;
  
  theHeading = (atan2(yVal,xVal)*180/3.14159);
  
  if (theHeading < 0)
    theHeading +=359;
  
  Serial.println(theHeading);
  
  Serial.println(nowHeading);
  
  Serial.println();
  Serial.println();
  
     Serial.println(freemem());

  
  Serial.println(useGPS);

  if (useGPS){
    Serial.println("going to GPS");
    gpsHeading = getGPSHeading();
    theHeading = theHeading + gpsHeading;
    if (theHeading > 359)
      theHeading -=359;
  }

    time = millis();

  if((time-previousTime > 15000) || (abs(theHeading - previousHeading)> 20)){
    //theHeading = 360-theHeading;
    vibrate();

    // And write it to SD card
    
      Serial.print("compass heading: ");
      Serial.println(theHeading);

      Serial.print("time: ");
      Serial.println(time);
      
         Serial.println(freemem());


//     
//    myFile = SD.open(name, FILE_WRITE);
//
//    // if the file opened okay, write to it:
//    if (myFile) {
//      Serial.print("Writing to log...");
//
//      if (useGPS){
//        myFile.print("Latitude, Longitude: ");
//        myFile.print(flat, 5);
//        myFile.print(", ");
//        myFile.println(flon, 5);
//        myFile.print("GPS heading: ");
//        myFile.println(gpsHeading);
//      }
//
//      myFile.print("compass heading: ");
//      myFile.println(theHeading);
//
//      myFile.print("time: ");
//      myFile.println(time);
//
//      myFile.close();
//      Serial.println("done.");
//    } 
//    else {
//      // if the file didn't open, print an error:
//      Serial.println("error opening log file");
//    }
  }

}
 

int getCompassHeading(){


  compass.read();
  heading=compass.heading((LSM303::vector){
    0,-1,0    }
  );
  //isort(heading,5);
  //int theHeading = mode(heading,5);
  //Serial.println(theHeading);
  
 heading = heading - 271;  //correction for how it's currently mounted.
 
  
  if (heading < 0)
    heading = heading + 359;
    
   heading = 359-heading;
  

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
  // vib0
  if (theHeading > 337.5 || theHeading < 22.5){
    Serial.println("vibrate 0");
    digitalWrite (beltSel2,LOW);
    digitalWrite (beltSel1,LOW);
    digitalWrite (beltSel0,LOW);
    digitalWrite (beltEnable,HIGH);
    delay(500);
    digitalWrite(beltEnable,LOW);
   // delay(500);

  }
  //vib1
  if (theHeading > 22.5 && theHeading < 67.5){
    Serial.println("vibrate 1");
    digitalWrite (beltSel2,LOW);
    digitalWrite (beltSel1,LOW);
    digitalWrite (beltSel0,HIGH);
    digitalWrite (beltEnable,HIGH);
    delay(500);
    digitalWrite(beltEnable,LOW);
   // delay(500);

  }
  //vib2
  if (theHeading > 67.5 && theHeading < 112.5){
    Serial.println("vibrate 2");
    digitalWrite (beltSel2,LOW);
    digitalWrite (beltSel1,HIGH);
    digitalWrite (beltSel0,LOW);
    digitalWrite (beltEnable,HIGH);
    delay(500);
    digitalWrite(beltEnable,LOW);
  //  delay(500);

  }
  //vib3
  if (theHeading > 112.5 && theHeading < 157.5){
    Serial.println("vibrate 3");
    digitalWrite (beltSel2,LOW);
    digitalWrite (beltSel1,HIGH);
    digitalWrite (beltSel0,HIGH);
    digitalWrite (beltEnable,HIGH);
    delay(500);
    digitalWrite(beltEnable,LOW);
   // delay(500);

  }
  //vib4
  if (theHeading > 157.5 && theHeading < 202.5){
    Serial.println("vibrate 4");
    digitalWrite (beltSel2,HIGH);
    digitalWrite (beltSel1,LOW);
    digitalWrite (beltSel0,LOW);
    digitalWrite (beltEnable,HIGH);
    delay(500);
    digitalWrite(beltEnable,LOW);
  }
  //vib5
  if (theHeading > 202.5 && theHeading < 247.5){
    Serial.println("vibrate 5");
    digitalWrite (beltSel2,HIGH);
    digitalWrite (beltSel1,LOW);
    digitalWrite (beltSel0,HIGH);
    digitalWrite (beltEnable,HIGH);
    delay(500);
    digitalWrite(beltEnable,LOW);
  //  delay(500);

  }
  //vib6
  if (theHeading > 247.5 && theHeading < 292.5){
    Serial.println("vibrate 6");
    digitalWrite (beltSel2,HIGH);
    digitalWrite (beltSel1,HIGH);
    digitalWrite (beltSel0,LOW);
    digitalWrite (beltEnable,HIGH);
    delay(500);
    digitalWrite(beltEnable,LOW);
  //  delay(500);

  }
  //vib7
  if (theHeading > 292.5 && theHeading < 337.5){
    Serial.println("vibrate 7");
    digitalWrite (beltSel2,HIGH);
    digitalWrite (beltSel1,HIGH);
    digitalWrite (beltSel0,HIGH);
    digitalWrite (beltEnable,HIGH);
    delay(500);
    digitalWrite(beltEnable,LOW);
  //  delay(500);

  }
  
  delay (1000);

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

  gpsHeading = (TinyGPS::course_to(flat, flon, 37.43277, -77.41583));  // This is where you put in the coordinates of where GPS navigation is pointed.
  Serial.println(gpsHeading);

}


// this function will return the number of bytes currently free in RAM      
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

