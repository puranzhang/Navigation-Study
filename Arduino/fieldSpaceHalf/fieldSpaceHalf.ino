
int beltSel0 = 2;
int beltSel1 = 3;
int beltSel2 = 4;
int beltEnable = 5;

int beltVib;

void setup() {

  pinMode(beltSel0, OUTPUT);
  pinMode(beltSel1, OUTPUT);
  pinMode(beltSel2, OUTPUT);
  pinMode(beltEnable, OUTPUT);  // PWM this pin.
  
  beltVib = 0;
}

void loop() {
  
  if (beltVib >= 16)
    beltVib = 0;

   //vib0
  if (beltVib == 0){
    Serial.println("vibrate 0");
    digitalWrite (beltSel2,LOW);
    digitalWrite (beltSel1,LOW);
    digitalWrite (beltSel0,LOW);
    analogWrite (beltEnable,125);
    delay(500);
    digitalWrite(beltEnable,LOW);   
  }

  //vib1
  if (beltVib == 1){
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
  if (beltVib == 2){
    Serial.println("vibrate 2");
    digitalWrite (beltSel2,LOW);
    digitalWrite (beltSel1,LOW);
    digitalWrite (beltSel0,HIGH);
    analogWrite (beltEnable,128);
    delay(500);
    digitalWrite(beltEnable,LOW);   
  }

  //vib3
  if (beltVib == 3){
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
  if (beltVib == 4){
    Serial.println("vibrate 4");
    digitalWrite (beltSel2,LOW);
    digitalWrite (beltSel1,HIGH);
    digitalWrite (beltSel0,LOW);
    analogWrite (beltEnable,125);
    delay(500);
    digitalWrite(beltEnable,LOW);   
  }

  //vib5
  if (beltVib == 5){
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
  if (beltVib == 6){
    Serial.println("vibrate 6");
    digitalWrite (beltSel2,LOW);
    digitalWrite (beltSel1,HIGH);
    digitalWrite (beltSel0,HIGH);
    analogWrite (beltEnable,125);
    delay(500);
    digitalWrite(beltEnable,LOW);   
  }

  //vib7
  if (beltVib == 7){
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
  if (beltVib == 8){
    Serial.println("vibrate 8");
    digitalWrite (beltSel2,HIGH);
    digitalWrite (beltSel1,LOW);
    digitalWrite (beltSel0,LOW);
    analogWrite (beltEnable,125);
    delay(500);
    digitalWrite(beltEnable,LOW);   
  }

  //vib9
  if (beltVib == 9){
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
  if (beltVib == 10){
    Serial.println("vibrate 10");
    digitalWrite (beltSel2,HIGH);
    digitalWrite (beltSel1,LOW);
    digitalWrite (beltSel0,HIGH);
    analogWrite (beltEnable,125);
    delay(500);
    digitalWrite(beltEnable,LOW);   
  }

  //vib11
  if (beltVib == 11){
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
  if (beltVib == 12){
    Serial.println("vibrate 12");
    digitalWrite (beltSel2,HIGH);
    digitalWrite (beltSel1,HIGH);
    digitalWrite (beltSel0,LOW);
    analogWrite (beltEnable,125);
    delay(500);
    digitalWrite(beltEnable,LOW);   
  }

  //vib13
  if (beltVib == 13){
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
  if (beltVib == 14){
    Serial.println("vibrate 14");
    digitalWrite (beltSel2,HIGH);
    digitalWrite (beltSel1,HIGH);
    digitalWrite (beltSel0,HIGH);
    analogWrite (beltEnable,125);
    delay(500);
    digitalWrite(beltEnable,LOW);   
  }

  //vib15
  if (beltVib == 15){
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
beltVib++;
delay(1000);
  
}
