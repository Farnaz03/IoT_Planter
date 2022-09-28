#include "TFT_eSPI.h"

TFT_eSPI tft;  //Initializing TFT LCD
TFT_eSprite spr = TFT_eSprite(&tft);  //Initializing buffer

bool conn_status = false;

int sensorPin = A0;
int sensorValue = 0;

int light = 0;
int mic = 0;

unsigned long send_loop_millis_start;
unsigned long send_loop_millis_current;
const unsigned long update_interval = 60000; //1 mns; 10 mns = 600000

Disk91_LoRaE5 lorae5(&Serial); // Where the AT command and debut traces are printed

#define Frequency DSKLORAE5_ZONE_EU868

uint8_t deveui[] = { };
uint8_t appeui[] = { };
uint8_t appkey[] = { };

void data_decord(int val, uint8_t data[2])
{
  data[0] = val >> 8 & 0xFF;
  data[1] = val & 0xFF;
}

void tht_lcd_setup() {
  tft.begin();
  tft.setRotation(3);
  spr.createSprite(TFT_HEIGHT, TFT_WIDTH);

  //pinMode(WIO_LIGHT, INPUT);  //Set light sensor pin as INPUT
  //pinMode(WIO_MIC, INPUT);  //Set Microphone sensor pin as INPUT
}

void tht_lcd_loop(int sensorValue, bool conn_status) {
  //light = analogRead(WIO_LIGHT);   //Assign variable to store light sensor values
  //mic = (analogRead(WIO_MIC)) / 10; //Assign variable to store microphone sensor values
  //Serial.print("light, mic: "); Serial.println(light, mic);
  //digitalWrite(LCD_BACKLIGHT, HIGH);

  //Setting the title header
  spr.fillSprite(TFT_WHITE);  //Fill background with white color
  spr.fillRect(0, 0, 320, 60, TFT_DARKCYAN); //Rectangle fill with dark green
  spr.setTextColor(TFT_WHITE);  //Setting text color
  spr.setTextSize(3);  //Setting text size
  spr.drawString("IoT Into the Wild", 10, 25); //Drawing a text string
  //spr.drawFastVLine(150, 50, 190, TFT_DARKGREEN); //Drawing vertical line
  //spr.drawFastHLine(0, 140, 320, TFT_DARKGREEN); //Drawing horizontal line

  //Setting soil moisture
  spr.setTextColor(TFT_BLACK);
  spr.setTextSize(2);
  spr.drawString("Soil moisture", 5, 65);
  spr.setTextSize(3);
  spr.drawNumber(sensorValue, 50, 95); //Display soil moisture values
  spr.drawString("wfv", 100, 95);

  //Setting conn status
  spr.setTextSize(2);
  spr.drawString("LoRa Status", 15, 160);
  spr.setTextSize(3);
  if (conn_status) {
    spr.fillCircle(120, 200, 15, TFT_OLIVE);
    spr.drawString(":)", 45, 190); //Display lora connection status
  }
  else {
    spr.fillCircle(120, 200, 15, TFT_ORANGE);
    spr.drawString(":(", 45, 190); //Display lora connection status
  }

  // display soil status
  spr.setTextSize(2);
  if (sensorValue < 200) {
    spr.fillRect(180, 60, 200, 190, TFT_ORANGE);
    spr.drawString("Dry soil", 200, 95);
    spr.setTextSize(3);
    spr.drawString(":(", 225, 140);
    spr.setTextSize(2);
    spr.drawString("Water me", 200, 190);
  }
  else if ((sensorValue > 199) && (sensorValue <= 400)) {
    spr.fillRect(180, 60, 200, 190, TFT_MAROON);
    spr.drawString("Damp soil", 200, 95);
    spr.setTextSize(3);
    spr.drawString(":|", 225, 140);
    spr.setTextSize(2);
    spr.drawString("Water soon", 200, 190);
  }
  else if ((sensorValue > 400) && (sensorValue <= 699)) {
    spr.fillRect(180, 60, 200, 190, TFT_DARKGREEN);
    spr.drawString("Wet soil", 200, 95);
    spr.setTextSize(3);
    spr.drawString(":)", 225, 140);
    spr.setTextSize(2);
    spr.drawString("Wet", 200, 190);
  }

  /*
    //Setting Microphone
    spr.setTextSize(2);
    spr.drawString("Microphone", 180, 65);
    spr.setTextSize(3);
    spr.drawNumber(mic, 205, 95); //Display microphone values
    spr.drawString("dB", 245, 95);

    //Setting light
    spr.setTextSize(2);
    spr.drawString("Light", 200, 160);
    spr.setTextSize(3);
    light = map(light, 0, 1023, 0, 100); //Map sensor values
    spr.drawNumber(light, 205, 190); //Display sensor values as percentage
    spr.drawString("%", 245, 190);
  */
  spr.pushSprite(0, 0); //Push to LCD
  //digitalWrite(LCD_BACKLIGHT, LOW);
}
