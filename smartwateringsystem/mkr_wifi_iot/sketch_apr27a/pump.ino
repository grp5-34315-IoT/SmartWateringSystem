const int waterPumpPin2 = 2;
const int waterPumpPin3 = 3;
const int waterPumpPin4 = 4;

void setup(){
pinMode(2, OUTPUT);
pinMode(3, OUTPUT);
pinMode(4, OUTPUT);
pinMode(waterPumpPin2, OUTPUT);
pinMode(waterPumpPin3, OUTPUT);
pinMode(waterPumpPin4, OUTPUT);

Serial.begin(9600);
}

void minuteOne(){
  pump1ON();
  pump2ON();
  pump3ON();
  delay(10000);
 pump1OFF();
  pump2OFF();
   pump3OFF();

}
void pump1ON(){
  digitalWrite(waterPumpPin2, LOW);
}
void pump1OFF(){
  digitalWrite(waterPumpPin2, HIGH);
}

void pump2ON(){
  digitalWrite(waterPumpPin3, LOW);
}
void pump2OFF(){
  digitalWrite(waterPumpPin3, HIGH);
}

void pump3ON(){
  digitalWrite(waterPumpPin4, LOW);
}
void pump3OFF(){
  digitalWrite(waterPumpPin4, HIGH);
}

void loop() {
  if(Serial.available()){
    char command = Serial.read();
    if(command == 'A'){
            Serial.println("PUMP1ON");
            pump1ON();
    } else if (command == 'a'){
      Serial.println("PUMP1OFF");
            pump1OFF();

    } else if(command == 'B'){
            Serial.println("PUMP2ON");
            pump2ON();
    } else if (command == 'b'){
      Serial.println("PUMP2OFF");
            pump2OFF();

    } else if(command == 'C'){
            Serial.println("PUMP3ON");
            pump3ON();


    } else if (command == 'c'){
      Serial.println("PUMP3OFF");
            pump3OFF();

    } else if (command == 'D'){
      Serial.println("1Minute");
      minuteOne();
    }
  }
}