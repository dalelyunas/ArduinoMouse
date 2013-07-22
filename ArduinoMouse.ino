/*
 ArduinoMouse
 
 Uses mouse input data to perform several functions
 
 created 25 Jun 2013
 by David Alelyunas
 
 */

// Require mouse control library
#include <MouseController.h>
#include <ads7843.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <tftlib.h> // Hardware-specific library

#define PSTR(a)  a
// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

/** ADS7843 pin map */
#ifdef ELECFREAKS_TFT_SHIELD_V2
#define DCLK     6
#define CS       5  
#define DIN      4 
#define DOUT     3
#define IRQ      2 
#elif defined ELECHOUSE_DUE_TFT_SHIELD_V1
/** elechouse TFT shield pin map */
#define DCLK     25
#define CS       26 
#define DIN      27 
#define DOUT     29
#define IRQ      30
#endif

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F 
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

TFTLCD tft;
ADS7843 touch(CS, DCLK, DIN, DOUT, IRQ);

// Initialize USB Controller
USBHost usb;

Point p;

// Attach mouse controller to USB
MouseController mouse(usb);

//variables to control the keyboard
int enterValue = 0;
int prevEnterValue = 0;
boolean pressed = true;

//values for total x traveled and total y traveled
int totalX;
int totalY;
float totalMM;
float prevMM;
int prevY;

//desired value for the leds to all be lit at
float maxValue = 0;

//time keeping values
float startTime = 0;
float currentTime = 0;
float prevTime;
boolean startRecorded = false;

float lastPercent;

boolean toggle = true;

// This function intercepts mouse movements
void mouseMoved() {
  if(!startRecorded){
    startTime = millis();
    currentTime = (millis() - startTime)/1000;
    startRecorded = true;
  }

  totalX += mouse.getXChange();
  int tempYChange = mouse.getYChange();
  
  totalY += tempYChange;
  totalMM = totalY/44.461;
  //Serial.print(" Time, ");
  //Serial.print(currentTime);
  //Serial.print(", ");

  // Serial.print("Total Y, ");
  Serial.println(totalMM);

  //Serial.print(" mm = ");
  //Serial.print(float(totalY)/45.0);

  //Serial.println();
}

// This function intercepts mouse button press
void mousePressed() {

  if (mouse.getButton(LEFT_BUTTON)){
    Serial.println("End");
  }
  if (mouse.getButton(RIGHT_BUTTON)){
    Serial.println("Values Reset");
    reset();
  }
}
//resets values
void reset(){
  totalX = 0;
  totalMM = 0;
  totalY = 0;
  //overallY = 0;
  startTime = millis();
}

// Checks to see which leds should light up based on a desired distance value
void checkLEDS() {
  if(abs(totalMM) >= 0 && abs(totalMM) < maxValue * 4/5) {
    digitalWrite(4, HIGH); 
  }
  else{
    digitalWrite(4, LOW); 
  }
  if(abs(totalMM) >= (maxValue * 4/5) && abs(totalMM) < maxValue){
    digitalWrite(3, HIGH); 
  }
  else{
    digitalWrite(3, LOW); 
  }
  if(abs(totalMM) >= maxValue){
    digitalWrite(2, HIGH); 
  }
  else{
    digitalWrite(2, LOW); 
  }
}

void lcdDisplay(){
  //writes the max value to the LCD screen

  tft.setCursor(20, 90);
  tft.print("EndDist: ");
  tft.print(maxValue);

  //writes the totalY value to the LCD screen
  tft.setCursor(130,30);
  tft.setTextColor(BLACK);
  tft.print(prevMM);
  tft.setCursor(130 ,30 );
  tft.setTextColor(GREEN);
  tft.print(totalMM);

  //writes the time elapsed since the start of the program in seconds
  if(startRecorded){   
    tft.setCursor(80,60);
    tft.setTextColor(BLACK);
    tft.print(prevTime);
    tft.setCursor(80,60);
    tft.setTextColor(GREEN);
    tft.print(currentTime);
  }
}

//displays a rectangle that displays progress towards the maxValue in 200ths
void displayProgress(){
  float percent = abs(totalMM)/float(maxValue);

  if(percent < 1){
    tft.fillRect(20+ lastPercent*200,180,201 - lastPercent*200,20,BLACK);
    tft.fillRect(20,180,200*percent,20,GREEN);
  }
  else if (percent > 1.00){
    tft.setTextColor(RED);
    tft.fillRect(20,180,200,20,RED); 
  }
  lastPercent = percent;
}

//displays the keyboard
void displayTouchKeyboard(){
  int x = 20;
  int y = 220;
  int number = 0;
  int side = 30;
  tft.setCursor(20, 150);
  tft.print("Enter Max Value:");

  for(int i = 0;i<2;i++){
    for(int z = 0; z<5;z++){
      tft.drawRect(x+40*z,y+40*i,side,side,GREEN);
      tft.setCursor(x+40*z+10,y+40*i+10);
      tft.print(number);  
      number++;
    }    
  }

  tft.drawRect(x,y+80,90,side,GREEN);
  tft.drawRect(x+100,y+80,90,side,GREEN);
  tft.setCursor(x+10, y+90);
  tft.print("Enter");
  tft.setCursor(x+110, y+90);
  tft.print("Back");
}

//determines when keys are pressed and what to do when a key is pressed
void interpretKeys(){

  String temp = "";
  temp += enterValue;

  if(temp.length()<7){
    if(((p.x-310)/14 < 42 && (p.x-310)/14 > 16) && ((p.y-150)/9> 229 && (p.y-150)/9< 253)){
      if(pressed){
        enterValue = enterValue*10 + 0;
        pressed = false;
      }
    }
    else if(((p.x-310)/14 < 83 && (p.x-310)/14 > 58) && ((p.y-150)/9> 229 && (p.y-150)/9< 253)){
      if(pressed){
        enterValue = enterValue*10 + 1;
        pressed = false;
      }
    }
    else if(((p.x-310)/14 < 127 && (p.x-310)/14 > 98) && ((p.y-150)/9> 229 && (p.y-150)/9< 253)){
      if(pressed){
        enterValue = enterValue*10 + 2;
        pressed = false;
      }
    }
    else if(((p.x-310)/14 < 171 && (p.x-310)/14 > 145) && ((p.y-150)/9> 229 && (p.y-150)/9< 253)){
      if(pressed){
        enterValue = enterValue*10 + 3;
        pressed = false;
      }
    }
    else if(((p.x-310)/14 < 212 && (p.x-310)/14 > 185) && ((p.y-150)/9> 229 && (p.y-150)/9< 253)){
      if(pressed){
        enterValue = enterValue*10 + 4;
        pressed = false;
      }
    }
    else if(((p.x-310)/14 < 42 && (p.x-310)/14 > 16) && ((p.y-150)/9> 270 && (p.y-150)/9< 294)){
      if(pressed){
        enterValue = enterValue*10 + 5;
        pressed = false;
      }
    }
    else if(((p.x-310)/14 < 83 && (p.x-310)/14 > 58) && ((p.y-150)/9> 270 && (p.y-150)/9< 294)){
      if(pressed){
        enterValue = enterValue*10 + 6;
        pressed = false;
      }
    }
    else if(((p.x-310)/14 < 127 && (p.x-310)/14 > 98) && ((p.y-150)/9> 270 && (p.y-150)/9< 294)){
      if(pressed){
        enterValue = enterValue*10 + 7;
        pressed = false;
      }
    }
    else if(((p.x-310)/14 < 171 && (p.x-310)/14 > 145) && ((p.y-150)/9> 270 && (p.y-150)/9< 294)){
      if(pressed){
        enterValue = enterValue*10 + 8;
        pressed = false;
      }
    }
    else if(((p.x-310)/14 < 212 && (p.x-310)/14 > 185) && ((p.y-150)/9> 270 && (p.y-150)/9< 294)){
      if(pressed){
        enterValue = enterValue*10 + 9;
        pressed = false;
      }
    }
  }
  if(((p.x-310)/14 < 212 && (p.x-310)/14 > 123) && ((p.y-150)/9> 310 && (p.y-150)/9< 338)){
    if(pressed){
      enterValue = enterValue/10;
      pressed = false;
    }
  }
  if(((p.x-310)/14 < 108 && (p.x-310)/14 > 16) && ((p.y-150)/9> 310 && (p.y-150)/9< 338)){
    if(pressed){
      maxValue = enterValue;
      pressed = false;
    }
  }
  if(((p.x-310)/14 == -22) && ((p.y-150)/9 == -16)){
    pressed = true;
  }
}

void determineToggle(){

  if(((p.x-310)/14 < 106 && (p.x-310)/14 > 15) && ((p.y-150)/9> 371 && (p.y-150)/9< 397)){
    if(pressed){
      toggle = !toggle;
      tft.fillScreen(BLACK);
      pressed = false;
    }
  }
  if(((p.x-310)/14 == -22) && ((p.y-150)/9 == -16)){
    pressed = true;
  }
}

void setup()
{
  Serial.begin(115200);
  tft.begin();
  touch.begin();
  //Serial.setTimeout(50); 
  Serial.println();
  Serial.println();

  Serial.println("Program started");
  totalX = 0;
  totalY = 0;
  totalMM = 0;

  tft.setTextColor(GREEN);
  tft.setTextSize(2);

  //reads an integer from the keyboard interface
  Serial.println("Enter Max Value:");

  while(maxValue == 0){
    uint8_t flag;
    p = touch.getpos(&flag);

    if(toggle){
      tft.drawRect(20,360, 90,30,GREEN);
      tft.setCursor(30,370);
      tft.print("Toggle");
      tft.setCursor(20, 40);
      tft.print("Waiting for input");
      if(Serial.available()){
        maxValue = Serial.parseFloat();
      }
    }
    else{   
      tft.drawRect(20,360, 90,30,GREEN);
      tft.setCursor(30,370);
      tft.print("Toggle");
      displayTouchKeyboard();
      interpretKeys();
      if(enterValue != prevEnterValue){
        tft.fillScreen(BLACK);
      }
      tft.setCursor(20,180);
      tft.print(enterValue);
      prevEnterValue = enterValue;

    }
    determineToggle();
  }

  tft.fillScreen(BLACK);  

  Serial.print("Maximum Value: ");
  Serial.println(maxValue);
  Serial.println();

  tft.setCursor(20,30);
  tft.print("Distance: ");

  tft.setCursor(20,60);
  tft.print("Time: ");

  pinMode(3, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(4,OUTPUT);

  digitalWrite(4, LOW);
  digitalWrite(3, LOW);
  digitalWrite(2, LOW);
}

void loop()
{
  currentTime = (millis() - startTime)/1000;

  // Process USB tasks
  usb.Task();
  uhd_set_vbof_active_high();

  lcdDisplay();
  displayProgress();
  checkLEDS();
  prevY = totalY;
  prevMM = totalMM;
  prevTime = currentTime;
}










