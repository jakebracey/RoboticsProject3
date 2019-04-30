//Libraries
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <stdio.h>

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
#define SCREEN_COLOR    BLACK

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


//Global Variables
float p = 3.1415926;
Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, CS, DC, DIN, CLK, RST);
SoftwareSerial serial3(serial3RX, serial3TX);
String timeString;
String timeA = "07:59:30";
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

    Serial.begin(9600);
    serial3.begin(9600);
    tft.begin();
    tft.fillScreen(SCREEN_COLOR);
    bay[1]={1,0,10};
    bay[2]={2,0,0};
    bay[3]={3,0,0};
    bay[4]={4,0,0};
    
}

void loop() {
  //changeBays(0);
  checkSwitchSysState();

  //ON - Pill Dispensing Mode
  if(SwitchSysState == LOW){
    displayStatic();
  }
  
  //OFF - Setup Mode
  else{
    
      displayStaticMenu(bay[currentBay]);
      checkButtons();
  }
         
      
      
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
         delay(300);
         tft.fillRoundRect(0 , 0, 128, 30, 0, SCREEN_COLOR);
         tft.fillRoundRect(0 , 98, 128, 30, 0, RED);
         delay(300);
      }
    break;
    case 1:
    case 2:
    case 3:
    case 4:
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

void displayStatic(){
  if (serial3.available()>0) {
    timeString = getSerialString(serial3);
    Serial.println(timeString);
    if(timeString == timeA){
        Serial.println("hey there");
      }
  }
  if (Serial.available()) {
    serial3.write(Serial.read());
  }
  tft.setCursor(17,113);
  tft.setTextSize(2);
  tft.setTextColor(BLUE, YELLOW);
  tft.print(timeString);
}

void displayStaticMenu(pillInfo pill){
  tft.setTextSize(2);
  tft.setTextColor(GREEN, BLACK);
  tft.setCursor(28,40);
  tft.print("Bay: ");
  tft.print(pill.pillBay);
  tft.setCursor(5,60);
  tft.print("Pills: ");
  tft.print(pill.numOfPills);
  
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
