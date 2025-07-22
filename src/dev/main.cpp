#include <Arduino.h>
#include "pin_config.h"
#include "TFT_eSPI.h"    //Display library /* Please use the TFT library provided in the library. */

// Create objects
TFT_eSPI tft = TFT_eSPI();

// Static values
static int dHeight = 170;
static int dWidth = 320;
static int maxSat = 12;
static int battCHG = 80;

// Display variables
int brightness;
int size;
int x;
int gpsRefresh = 1000;

// Speed variables
int currentSpeed;
int displaySpeed = 0;
char mph[4];
int steps;
unsigned long stepDelay;

// Direction variables
char strCard[4];

// Misc variables
int i;

void setup()
{
   // Establish monitoring port
   Serial.begin(115200);

  // Set up display
  tft.begin();
  //tft.setRotation(1);
  tft.setRotation(3);
  tft.setTextSize(2);
  tft.fillScreen(TFT_BLACK);
  delay(500);
  tft.fillScreen(TFT_WHITE);
  delay(500);
  tft.fillScreen(TFT_BLACK);
  ledcSetup(0, 2000, 8);

  // Turn on backlight
  ledcAttachPin(PIN_LCD_BL, 0);
  ledcWrite(0, 250);

}

void updateSpeed()
{
  // display the prior value in black to "erase" it
 // Serial.printf("Clearing display: (currentSpeed %d != displaySpeed %d) with old display value %s \n", currentSpeed, displaySpeed, mph);
  tft.setTextColor(TFT_BLACK, TFT_BLACK);
  x = (3 - strlen(mph)) * 110; // each character is 110 pixels
  size = tft.drawString(mph, x, 0, 8);

  // Increment or decrement the displaySpeed value by one
  // and set the smartDelay appropriately so we make it to the currentSpeed within
  // the GPS refresh time.

  if (currentSpeed == -1) {
    //Invalid speed; just set display to -1 
    displaySpeed = -1;
    sprintf(mph, "-");
  }
  else if (displaySpeed < currentSpeed) {
    ++displaySpeed;
    sprintf(mph, "%d", displaySpeed);
  }
  else if (displaySpeed > currentSpeed) {
    --displaySpeed;
    sprintf(mph, "%d", displaySpeed);
  }
  
  // display the new value
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  x = (3 - strlen(mph)) * 110; // each character is 110 pixels
  size = tft.drawString(mph, x, 0, 8);
}

void loop()
{
//--------------------------------------
  Serial.println("\n\n--SPEED-- \n");
  
  Serial.printf("Starting speed: %d \n", currentSpeed);

  //--------------------------------------
  //This is to simulate the speed 
  if (currentSpeed < 7 ) 
  { 
    currentSpeed = 13;
    //Serial.printf("Reset speed to start increase: %d \n", currentSpeed);
  }
  if (currentSpeed > 104 )
  {
    currentSpeed = 112;
    //Serial.printf("Reset speed to start decrease: %d \n", currentSpeed);
  } 
  
  if (currentSpeed % 13 == 0 ) currentSpeed += 13;
  if (currentSpeed % 7 == 0 ) currentSpeed -= 7;
  //--------------------------------------
  
  Serial.printf("New Speed to display: %d \n", currentSpeed);

  if (currentSpeed <= 1 and displaySpeed <= 1 and currentSpeed >-1)
  {
    ledcWrite(0, 0);
  }

  if (displaySpeed <= 1 and currentSpeed > 1)
  {
    ledcWrite(0, 250);
  }


  // ONLY if the value has changed
  if (currentSpeed != displaySpeed)
  {
    
    if (displaySpeed == -1) {
      //Previous speed was invalid speed; overwrite with '-'
      sprintf(mph, "-");
    }
    else {
      sprintf(mph, "%d", displaySpeed);
    }

    
    Serial.printf("Calculating steps for delay (currentSpeed %i - displaySpeed %i) \n", currentSpeed, displaySpeed);

    if (currentSpeed == -1) {
      steps = 1;  
    }
    else {
      //steps = abs(currentSpeed - displaySpeed) + 1;
      steps = abs(currentSpeed - displaySpeed);
    }
    
    stepDelay = gpsRefresh / steps /2;
    Serial.printf("steps for delay: %d  stepDelay: %dms \n", steps, stepDelay);

  }

  unsigned long start1 = millis();
  
  do 
  {
    int effectiveDelay;
    unsigned long start = millis();
    updateSpeed();
    
    effectiveDelay = (stepDelay-(millis() - start));
    if (effectiveDelay > 0) {
      delay(effectiveDelay);
      //Serial.printf("EffectiveDelay=%d \n", effectiveDelay);
    };

  } while (currentSpeed != displaySpeed);
  Serial.printf("TimeToUpdate: %d \n", millis() - start1);
  Serial.printf("Additional delay: %d \n", gpsRefresh - (millis() - start1));
  delay(gpsRefresh - (millis() - start1));

  Serial.printf("Time Elapsed: %d", start1 - millis());
  delay (1000);
}  

/*TFT_BLACK       0x0000
TFT_NAVY        0x000F
TFT_DARKGREEN   0x03E0
TFT_DARKCYAN    0x03EF
TFT_MAROON      0x7800
TFT_PURPLE      0x780F
TFT_OLIVE       0x7BE0
TFT_LIGHTGREY   0xC618
TFT_DARKGREY    0x7BEF
TFT_BLUE        0x001F
TFT_GREEN       0x07E0
TFT_CYAN        0x07FF
TFT_RED         0xF800
TFT_MAGENTA     0xF81F
TFT_YELLOW      0xFFE0
TFT_WHITE       0xFFFF
TFT_ORANGE      0xFDA0
TFT_GREENYELLOW 0xB7E0
TFT_PINK        0xFC9F
*/