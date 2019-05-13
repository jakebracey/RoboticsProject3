
unsigned long SecInit = 7.99 * 3600;
unsigned long time_now;

void setup() {
  Serial.begin(9600);
  time_now = SecInit;
}

void loop() { 
  //writes the time to the serial port
  if((time_now / 3600)<10){
    Serial.print('0');
  }
  Serial.print(time_now / 3600);
  Serial.print(':');
  if(((time_now %3600)/60) < 10){
    Serial.print('0');
  }
  Serial.print((time_now %3600)/60);
  Serial.print(':');
  if(((time_now %3600)%60) < 10){
    Serial.print('0');
  }
  Serial.print((time_now %3600)%60);
  Serial.println();

  //delays the code to simulate time
  time_now = time_now + 1;
  delay(1000);
}
