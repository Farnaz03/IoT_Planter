#include <Arduino.h>
#include "disk91_LoRaE5.h"
#include "tht_lib.h"
//#include "ntp_lib.h"
// ntp_lib.h had a conflict preventing tft from working
// might implement tft clock using ntp later
// perhaps a lora based ntp downlink or similar would work well instead of wifi


void setup(void)
{
  Serial.begin(9600);
  while (!Serial);

  send_loop_millis_start = millis();

  tht_lcd_setup();

  uint32_t start = millis();
  while ( !Serial && (millis() - start) < 1500 );  // Open the Serial Monitor to get started or wait for 1.5"
  // init the library, search the LORAE5 over the different WIO port available
  if ( ! lorae5.begin(DSKLORAE5_SWSERIAL_WIO_P1) ) {
    Serial.println("LoRa E5 Init Failed");
    conn_status = false;
    while (1);
  }

  // Setup the LoRaWan Credentials
  if ( ! lorae5.setup(
         DSKLORAE5_ZONE_EU868,     // LoRaWan Radio Zone EU868 here
         deveui,
         appeui,
         appkey
       ) ) {
    Serial.println("LoRa E5 Setup Failed");
    conn_status = false;
    while (1);
  }
  else {
    conn_status = true;
  }
  //setup_ntp();
}

void loop(void)
{
  sensorValue = analogRead(sensorPin);
  tht_lcd_loop(sensorValue, conn_status);
  
  //ntp_loop();
  
  //Serial.print("Soil Moisture: "); Serial.println(sensorValue);

  static uint8_t data[2] = { 0x00 };  //Use the data[] to store the values of the sensors

  data_decord(sensorValue, data);

  send_loop_millis_current = millis();
  if ((send_loop_millis_current - send_loop_millis_start) >= update_interval) {
    if ( lorae5.send_sync(              //Sending the sensor values out
           8,                            // LoRaWan Port
           data,                         // data array
           sizeof(data),                 // size of the data
           false,                        // we are not expecting a ack
           7,                            // Spread Factor
           14                            // Tx Power in dBm
         )
       ) {
      Serial.println("Uplink done");
      if ( lorae5.isDownlinkReceived() ) {
        Serial.println("A downlink has been received");
        if ( lorae5.isDownlinkPending() ) {
          Serial.println("More downlink are pending");
        }
      }
      conn_status = true;
    }
    else {
      conn_status = false;
    }
    send_loop_millis_start = send_loop_millis_current;
  }
  delay(3000);
}
