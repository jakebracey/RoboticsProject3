//Libraries
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <stdio.h>
#include <Math.h>

//Screen Dimensions & Color Definitions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128
#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0  
#define WHITE           0xFFFF
#define SCREEN_COLOR    MAGENTA

//Screen PIN definitions
#define CLK 2
#define DIN 3
#define DC 4
#define CS 5
#define RST 6

//Switch and Button Definitions
#define buttonA   7
#define buttonB   8
#define buttonC   9
#define toggleL   10
#define toggleR   11
#define SwitchSys 12
#define scaleUP 20 
#define scaleDOWN 21

//Serial Definitions
#define serial3RX 50
#define serial3TX 51
#define serial2RX 50
#define serial2TX 51

typedef struct{
  int pillBay;
  int timeOfDay;
  int numOfPills;
}pillInfo;

int buttonAState   = 0;
int buttonBState   = 0;
int buttonCState   = 0;
int toggleLState   = 0;
int toggleRState   = 0;
int SwitchSysState = 0;
int buttonAPrev    = 1;
int buttonBPrev    = 1;
int buttonCPrev    = 1;
int toggleLPrev    = 1;
int toggleRPrev    = 1;
int SwitchSysPrev  = 1;

//Scale Definitions
#include <HX711.h>
#define ScaleCalibration 377 //previously determined
#define Scale_Data  19
#define Scale_Clock  18
HX711 scale;
#define weight_bottle 104
#define weight_pill 6.25

//Global Variables
float p = 3.1415926;
Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, CS, DC, DIN, CLK, RST);
SoftwareSerial serial3(serial3RX, serial3TX);
SoftwareSerial serial2(serial2RX, serial2TX);
String timeString;
String timeA = "08:00:00";
String timeB = "08:02:00";
String timeC = "08:03:00";
pillInfo bay[5];
int currentBay = 1;

void setup() {
    //pin initializations
    pinMode(buttonA, INPUT_PULLUP);
    pinMode(buttonB, INPUT_PULLUP);
    pinMode(buttonC, INPUT_PULLUP);
    pinMode(toggleL, INPUT_PULLUP);
    pinMode(toggleR, INPUT_PULLUP);
    pinMode(SwitchSys, INPUT_PULLUP);
    pinMode(scaleUP, INPUT_PULLUP);
    pinMode(scaleDOWN, INPUT_PULLUP);

    //scale initialization
    scale.begin(Scale_Data, Scale_Clock);
    scale.set_scale(ScaleCalibration);
    delay(1000);
    scale.tare(); //zero out scale
    delay(1000);
    
    Serial.begin(9600);
    serial2.begin(9600);
    serial3.begin(9600);
    tft.begin();
    tft.fillScreen(SCREEN_COLOR);
    bay[1]={1,0,0};
    bay[2]={2,0,0};
    bay[3]={3,1,0};
    bay[4]={4,1,0};
    
}

void loop() {
  checkSwitchSysState();
  //ON - Pill Dispensing Mode
  if(SwitchSysState == LOW){
    displayStaticRun();
    if(timeA == timeString){
      displayPillAlert();
      Serial.println("timeA Pill");
    }
    if(timeB == timeString){
      displayPillAlert();
      Serial.println("timeB Pill");
    }
    if(timeC == timeString){
      displayPillAlert();
      Serial.println("timeC Pill");
    }
  }
  
  //OFF - Setup Mode
  else{
      serial2.listen();
      displayStaticMenu(bay[currentBay]);
      delay(500);
      bay[currentBay].numOfPills = getPillsFromScale();
      checkButtons();
  }

//    tft.setTextSize(2);
//    tft.setTextColor(RED, BLACK);
//    tft.setCursor(30,0); 
//    int reading = scale.get_units();
//    String weight = "--";
//    weight+=abs(reading);
//    weight+="--";
//    tft.print(weight); 
      
}//END of Loop


void changeBays(int bayNumber){
  tft.fillScreen(SCREEN_COLOR);
  tft.setTextSize(2);
  tft.setTextColor(RED, BLACK);
  tft.setCursor(10,40);
  tft.print("Moving To");
  switch(bayNumber){
    case 0:
      tft.setCursor(10,40);
      tft.print("Moving To");
      tft.setCursor(14,70);
      tft.print("Stand By");
      while(digitalRead(scaleDOWN) == HIGH){
         tft.fillRoundRect(0 , 0, 128, 30, 0, RED);
         tft.fillRoundRect(0 , 98, 128, 30, 0, SCREEN_COLOR);
         delay(400);
         tft.fillRoundRect(0 , 0, 128, 30, 0, SCREEN_COLOR);
         tft.fillRoundRect(0 , 98, 128, 30, 0, RED);
         delay(400);
      }
    break;
    case 1 ... 4:
      tft.setCursor(34,70);
      tft.print("Bay ");
      tft.print(bayNumber);
      while(digitalRead(scaleUP) == HIGH){
         tft.fillRoundRect(0 , 0, 128, 30, 0, RED);
         tft.fillRoundRect(0 , 98, 128, 30, 0, SCREEN_COLOR);
         delay(300);
         tft.fillRoundRect(0 , 0, 128, 30, 0, SCREEN_COLOR);
         tft.fillRoundRect(0 , 98, 128, 30, 0, RED);
         delay(300);
      }
    break;
  }
  tft.fillScreen(SCREEN_COLOR);
}

String getSerialString(SoftwareSerial &ser){
  String dataT = "";
    
    while (ser.available())
    {
        char character = ser.read(); // Receive a single character from the software serial port
        if (character == '\n')
        {
            dataT.trim();
            return dataT;
        }
        dataT.concat(character); // Add the received character to the receive buffer
  }
  dataT.trim();
  return dataT;
}

void displayStaticRun(){
  serial3.listen();
  if (serial3.available()>0) {
    String tempString = getSerialString(serial3);
    if(tempString.length()==8){
      timeString = tempString;
    }
    //timeString = getSerialString(serial3);
    Serial.println(timeString);
  }
  if (Serial.available()) {
    serial3.write(Serial.read());
  }
  tft.setCursor(17,113);
  tft.setTextSize(2);
  tft.setTextColor(BLUE, BLACK);
  tft.print(timeString);
  tft.setTextSize(1);
  tft.setCursor(5,13);
  tft.print("PB-1");
  tft.drawFastVLine(29,10,23,WHITE);
  tft.setCursor(36,13);
  tft.print("PB-2");
  tft.drawFastVLine(62,10,23,WHITE);
  tft.setCursor(67,13); 
  tft.print("PB-3");
  tft.drawFastVLine(94,10,23,WHITE);
  tft.setCursor(98,13);
  tft.print("PB-4");
  tft.setTextColor(RED, BLACK);
  tft.setCursor(16,1);
  tft.print("--- Schedule ---");
  tft.setTextSize(1);
  for(int i=1;i<=4;i++){
    tft.setTextColor(GREEN, BLACK);
    tft.setCursor(0+(i-1)*32,23);
    switch(bay[i].timeOfDay){    
      case 1:
        tft.print(timeA.substring(0,5));
      break;
      case 2:
        tft.print(timeB.substring(0,5));
      break;
      case 3:
        tft.print(timeC.substring(0,5));
      break;
      default:
        tft.setTextColor(RED, BLACK);
        tft.setCursor(8+(i-1)*31,23);
        tft.print("N/A");
      break;
    }
   
  }
  tft.setTextColor(BLUE, BLACK);
  tft.setCursor(5,57);
  tft.print("PB-1");
  tft.setCursor(36,57);
  tft.print("PB-2");
  tft.setCursor(67,57);
  tft.print("PB-3");
  tft.setCursor(98,57);
  tft.print("PB-4");
  tft.setTextColor(RED, BLACK);
  tft.setCursor(2,45);
  tft.print("-- Pills Remaining --");
  tft.setTextSize(2);
  for(int i=1;i<=4;i++){
    switch(bay[i].numOfPills){    
      case 0:
        tft.setTextColor(RED, BLACK);
        tft.setCursor(12+(i-1)*31,67);
        tft.print(bay[i].numOfPills);
      break;
      case 1 ... 6:
        tft.setTextColor(YELLOW, BLACK);
        tft.setCursor(8+(i-1)*33,67);
        tft.print(bay[i].numOfPills);
      break;
      case 7 ... 9:
        tft.setTextColor(GREEN, BLACK);
        tft.setCursor(8+(i-1)*33,67);
        tft.print(bay[i].numOfPills);
      break;
      default:
        tft.setTextColor(GREEN, BLACK);
        tft.setCursor(6+(i-1)*31,67);
        tft.print(bay[i].numOfPills);
      break;
    }
  }
}

void displayPillAlert(){
  tft.fillScreen(YELLOW);
  tft.setTextSize(2);
  tft.setTextColor(BLACK, YELLOW);
  tft.setCursor(23,70);
  tft.print("ALERT!!");
  tft.setTextSize(1);
  tft.setCursor(14,100);
  tft.print("Incorrect Number");
  tft.setCursor(21,112);
  tft.print("of Pills Taken");
  tft.fillTriangle(53, 15, 63, 0, 73, 15, BLACK);
  tft.setTextSize(2);
  tft.setCursor(52,20);
  tft.print("OK");
  while(digitalRead(buttonB) == HIGH){
    delay(500);
    //checkButtonB();
  }
  tft.fillScreen(SCREEN_COLOR);
}

int getPillsFromScale(){
  int currentPills=0;
  float reading = scale.get_units();
  float weight = (abs(reading) - weight_bottle) / weight_pill;
  if(weight < 0){
    weight = 0;
  }
  currentPills = round(weight);
  return currentPills;
}

void displayStaticMenu(pillInfo pill){
  tft.setTextSize(2);
  tft.setTextColor(GREEN, BLACK);
  tft.setCursor(28,40);
  tft.print("Bay: ");
  tft.print(pill.pillBay);
  tft.setCursor(35,70);
  tft.print("Pills");
  tft.drawFastHLine(35,88,60,WHITE);
  tft.setCursor(70,100);
  String tempStr;
  tempStr += pill.numOfPills;
  tempStr += "    ";
  tft.print(tempStr);
  
  tft.setTextSize(1);
    if(pill.timeOfDay == 1){
      tft.setTextColor(GREEN, BLACK);
      tft.fillTriangle(5, 15, 15, 0, 25, 15, GREEN);
      tft.setCursor(0,20);
      tft.print(timeA.substring(0,5));
    }else{
      tft.setTextColor(BLUE, BLACK);
      tft.fillTriangle(5, 15, 15, 0, 25, 15, BLUE);
      tft.setCursor(0,20);
      tft.print(timeA.substring(0,5));
    }
    if(pill.timeOfDay == 2){
      tft.setTextColor(GREEN, BLACK);
      tft.fillTriangle(53, 15, 63, 0, 73, 15, GREEN);
      tft.setCursor(48,20);
      tft.print(timeB.substring(0,5));
    }else{
      tft.setTextColor(BLUE, BLACK);
      tft.fillTriangle(53, 15, 63, 0, 73, 15, BLUE);
      tft.setCursor(48,20);
      tft.print(timeB.substring(0,5));
    }
    if(pill.timeOfDay == 3){
      tft.setTextColor(GREEN, BLACK);
      tft.fillTriangle(100, 15, 110, 0, 120, 15, GREEN);
      tft.setCursor(95,20);
      tft.print(timeC.substring(0,5));
    }else{
      tft.setTextColor(BLUE, BLACK);
      tft.fillTriangle(100, 15, 110, 0, 120, 15, BLUE);
      tft.setCursor(95,20);
      tft.print(timeC.substring(0,5));
    }
}

void checkButtonB(){
  buttonBState = digitalRead(buttonB);
  if(buttonBState != buttonBPrev){
    if(buttonBState == HIGH){
    Serial.println("Button B Pressed");
  }
  delay(50);
  }
  buttonBPrev = buttonBState;
}

void controlSecondary(int bayNumber, bool goUP){
  switch(bayNumber){
    case 0:
        serial2.print(currentBay);
        if(goUP){
          serial2.println("U");
        }else{
          serial2.println("D");
        }
    break;
  }
}

void checkButtons(){
  buttonAState = digitalRead(buttonA);
  if(buttonAState != buttonAPrev){
    if(buttonAState == HIGH){
    Serial.println("Button A Pressed");
    bay[currentBay].timeOfDay = 1;
  }
  delay(50);
  }
  buttonAPrev = buttonAState;

  buttonBState = digitalRead(buttonB);
  if(buttonBState != buttonBPrev){
    if(buttonBState == HIGH){
    Serial.println("Button B Pressed");
    bay[currentBay].timeOfDay = 2;
  }
  delay(50);
  }
  buttonBPrev = buttonBState;

  buttonCState = digitalRead(buttonC);
  if(buttonCState != buttonCPrev){
    if(buttonCState == HIGH){
    Serial.println("Button C Pressed");
    bay[currentBay].timeOfDay = 3;
  }
  delay(50);
  }
  buttonCPrev = buttonCState;

  toggleLState = digitalRead(toggleL);
  if(toggleLState != toggleLPrev){
    if(toggleLState == HIGH){
      if(currentBay == 1)
        currentBay = 4;
      else
        currentBay = currentBay - 1;
    Serial.println("Toggle L Pressed");
    changeBays(currentBay);
  }
  delay(50);
  }
  toggleLPrev = toggleLState;

  toggleRState = digitalRead(toggleR);
  if(toggleRState != toggleRPrev){
    if(toggleRState == HIGH){
      if(currentBay == 4)
        currentBay = 1;
      else
        currentBay = currentBay + 1;
    Serial.println("Toggle R Pressed");
    changeBays(currentBay);
  }
  delay(50);
  }
  toggleRPrev = toggleRState;
  
}

void checkSwitchSysState(){
  SwitchSysState = digitalRead(SwitchSys);
  if(SwitchSysState != SwitchSysPrev){
    if(SwitchSysState == LOW){
      Serial.println("System Switched to ON/LOW");
      tft.fillScreen(SCREEN_COLOR);
    }
    else{
      Serial.println("System Switched to OFF/HIGH");
      tft.fillScreen(SCREEN_COLOR);
    }
  delay(50);
  }
  SwitchSysPrev = SwitchSysState;
}
