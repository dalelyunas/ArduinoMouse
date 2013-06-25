/*
 ArduinoMouse
 
 Uses mouse input data to perform several functions
 
 created 25 Jun 2013
 by David Alelyunas
 
 */

// Require mouse control library
#include <MouseController.h>
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

// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7
// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).

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
// Initialize USB Controller
USBHost usb;


// Attach mouse controller to USB
MouseController mouse(usb);



//values for total x traveled and total y traveled
int totalX;
int totalY;

//desired value for the leds to all be lit at
int maxValue = 0;

//time keeping values
float startTime = 0;
float currentTime = 0;
boolean startRecorded = false;

// This function intercepts mouse movements
void mouseMoved() {
  if(!startRecorded){
   startTime = millis();
   currentTime = (millis() - startTime)/1000;
   startRecorded = true;
  }
  
  totalX += mouse.getXChange();

  totalY += mouse.getYChange();

  Serial.print("Total X: ");
  Serial.print(totalX);
  Serial.print(", ");
  Serial.print("Total Y: ");
  Serial.print(totalY);
  Serial.print(" Time: ");
  Serial.print(currentTime); 
  Serial.println();
}



// This function intercepts mouse button press
void mousePressed() {

  if (mouse.getButton(LEFT_BUTTON)){
    maxValue = 0;
    Serial.println("Enter maximum value:");
    while(maxValue == 0){
      maxValue = Serial.parseInt();

    }
    Serial.print("Maximum Value: ");
    Serial.println(maxValue);
    
    startTime = millis();
  }
  /*if (mouse.getButton(MIDDLE_BUTTON)){
   Serial.print("M");
   
   }*/
  if (mouse.getButton(RIGHT_BUTTON)){
    Serial.println("Values Reset");
    totalX = 0;
    totalY = 0;
    
  }
}


// Checks to see which leds should light up based on a desired distance value
void checkLEDS() {
  if(abs(totalY) >= (maxValue * 1/3)){
    digitalWrite(4, HIGH); 
  }
  else{
    digitalWrite(4, LOW); 
  }
  if(abs(totalY) >= (maxValue * 2/3)){
    digitalWrite(3, HIGH); 
  }
  else{
    digitalWrite(3, LOW); 
  }
  if(abs(totalY) >= maxValue){
    digitalWrite(2, HIGH); 
  }
  else{
    digitalWrite(2, LOW); 
  }
}

void lcdDisplay(){
   //writes the totalY value to the LCD screen
  tft.fillScreen(BLACK);
  tft.setCursor(120,200);
  tft.setTextColor(GREEN);
  tft.setTextSize(3);
  tft.print(totalY);
  //writes the time elapsed since the start of the program in milliseconds
  tft.setCursor(120,230);
  tft.print(currentTime);
  
  
}

void setup()
{
  Serial.begin(115200);
  tft.begin();
  Serial.println("Program started");
  totalX = 0;
  totalY = 0;



  //reads an integer from the serial stream
  Serial.println("Enter maximum value:");
  while(maxValue == 0){
    maxValue = Serial.parseInt();

  }
  Serial.print("Maximum Value: ");
  Serial.println(maxValue);
  
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
  checkLEDS();
  lcdDisplay();

}



