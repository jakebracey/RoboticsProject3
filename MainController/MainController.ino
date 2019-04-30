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

//Serial Definitions
#define serial3RX 50
#define serial3TX 51

typedef struct{
  int timeOfDay;
  int pillBay;
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
pillInfo bay1 = {0,1,0};
pillInfo bay2 = {0,2,0};
pillInfo bay3 = {0,3,0};
pillInfo bay4 = {0,4,0};


void setup() {
    //pin initializations
    pinMode(buttonA, INPUT_PULLUP);
    pinMode(buttonB, INPUT_PULLUP);
    pinMode(buttonC, INPUT_PULLUP);
    pinMode(toggleL, INPUT_PULLUP);
    pinMode(toggleR, INPUT_PULLUP);
    pinMode(SwitchSys, INPUT_PULLUP);

    Serial.begin(9600);
    serial3.begin(9600);
    tft.begin();
    tft.fillScreen(BLACK);
//    tft.setTextColor(YELLOW, BLACK);
//    tft.setCursor(70, 1);
//    tft.print("HH:MM:SS");

}

void loop() {
  
  checkSwitchSysState();

  
  if(SwitchSysState == LOW){
    
  }

  else{

    
  }
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
      checkButtons();
      
      
      displayStatic();
      displayStaticMenu(1,1);
}//END of Loop



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
  tft.setCursor(17,113);
  tft.setTextSize(2);
  tft.setTextColor(BLUE, YELLOW);
  tft.print(timeString);
}

void displayStaticMenu(int pill_slot, int pill_time){
  tft.setTextSize(1);
    if(pill_time == 1){
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
    if(pill_time == 2){
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
    if(pill_time == 3){
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
  }
  delay(50);
  }
  buttonAPrev = buttonAState;

  buttonBState = digitalRead(buttonB);
  if(buttonBState != buttonBPrev){
    if(buttonBState == HIGH){
    Serial.println("Button B Pressed");
  }
  delay(50);
  }
  buttonBPrev = buttonBState;

  buttonCState = digitalRead(buttonC);
  if(buttonCState != buttonCPrev){
    if(buttonCState == HIGH){
    Serial.println("Button C Pressed");
  }
  delay(50);
  }
  buttonCPrev = buttonCState;

  toggleLState = digitalRead(toggleL);
  if(toggleLState != toggleLPrev){
    if(toggleLState == HIGH){
    Serial.println("Toggle L Pressed");
  }
  delay(50);
  }
  toggleLPrev = toggleLState;

  toggleRState = digitalRead(toggleR);
  if(toggleRState != toggleRPrev){
    if(toggleRState == HIGH){
    Serial.println("Toggle R Pressed");
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
    }
    else{
      Serial.println("System Switched to OFF/HIGH");
    }
  delay(50);
  }
  SwitchSysPrev = SwitchSysState;
}
