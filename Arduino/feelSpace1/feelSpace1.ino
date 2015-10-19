#include <Wire.h>
#include <LSM303.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <SD.h>


LSM303 compass;
TinyGPS gps;
File myFile;

SoftwareSerial nssGPS(6,255);

// initialize pins for the belt.

int beltSel0 = 2;
int beltSel1 = 3;
int beltSel2 = 4;
int beltEnable = 5;

int manSwitch = 7;

int theHeading;
long gpsHeading;
int previousHeading;

float flat, flon;

unsigned long time;
unsigned long previousTime;

char name[] = "BELT_L00.CSV";

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
  Serial.print("Initializing SD card...");
  pinMode(10, OUTPUT);
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("SD initialization done.");

  // Name the data file
  for (uint8_t i = 0; i < 100; i++) {
    name[6] = i/10 + '0';
    name[7] = i%10 + '0';
    if (SD.exists(name)) 
      continue;
    else
      break;
  }

  Serial.print("Data file name: ");
  Serial.println(name);

  //time = millis();


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

  time = millis();

  theHeading = 0;  

  for (int i = 0;i<100;i++)  {
    theHeading = theHeading + getCompassHeading();
    delay(10);
  }

  theHeading = theHeading / 100;
  
  Serial.println(theHeading);

  if (digitalRead(manSwitch)==1){
    Serial.println("going to GPS");
    gpsHeading = getGPSHeading();
    theHeading = theHeading + gpsHeading;
    if (theHeading > 359)
      theHeading -=359;
  }


  if((time-previousTime > 15000) || (abs(theHeading - previousHeading)> 25)){
    theHeading = 360-theHeading;
    vibrate();

    // And write it to SD card
    myFile = SD.open(name, FILE_WRITE);

    // if the file opened okay, write to it:
    if (myFile) {
      Serial.print("Writing to log...");

      if (digitalRead(manSwitch)==1){
        myFile.print("Latitude, Longitude: ");
        myFile.print(flat, 5);
        myFile.print(", ");
        myFile.println(flon, 5);
        myFile.print("GPS heading: ");
        myFile.println(gpsHeading);
      }

      myFile.print("compass heading: ");
      myFile.println(theHeading);

      myFile.print("time: ");
      myFile.println(time);

      myFile.close();
      Serial.println("done.");
    } 
    else {
      // if the file didn't open, print an error:
      Serial.println("error opening log file");
    }
  }

}


int mode(int *x,int n){

  int i = 0;
  int count = 0;
  int maxCount = 0;
  int mode = 0;
  int bimodal;
  int prevCount = 0;

  while(i<(n-1)){

    prevCount=count;
    count=0;

    while(x[i]==x[i+1]){

      count++;
      i++;
    }

    if(count>prevCount&count>maxCount){
      mode=x[i];
      maxCount=count;
      bimodal=0;
    }
    if(count==0){
      i++;
    }
    if(count==maxCount){//If the dataset has 2 or more modes.
      bimodal=1;
    }
    if(mode==0||bimodal==1){//Return the median if there is no mode.
      mode=x[(n/2)];
    }
    return mode;
  }

} 

//Sorting function
// sort function (Author: Bill Gentles, Nov. 12, 2010)
void isort(int *a, int n){
  // *a is an array pointer function

    for (int i = 1; i < n; ++i)
  {
    int j = a[i];
    int k;
    for (k = i - 1; (k >= 0) && (j < a[k]); k--)
    {
      a[k + 1] = a[k];
    }
    a[k + 1] = j;
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
  }

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

  gpsHeading = (TinyGPS::course_to(flat, flon, 37.57360, -77.43248));  // This is where you put in the coordinates of where GPS navigation is pointed.
  Serial.println(gpsHeading);

}


