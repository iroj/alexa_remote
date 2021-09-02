
#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

#include <fauxmoESP.h>
#include "credentials.h"

bool toggle = false;

#define DELAY_AFTER_SEND 2000
#define DELAY_BETWEEN_REPEATS 26

#define SERIAL_BAUDRATE 115200

#define IR_SEND_PIN 12

#define TV "TV audio"

IRsend irsend(IR_SEND_PIN);
fauxmoESP fauxmo;

uint16_t tvCommand = 0x961;
uint16_t alexaCommand = 0xa41;

uint16_t repeats = 2;
uint16_t bits = 12;

// uint16_t tvModeAddress = 0x4;
// uint8_t tvModeCommand = 0x2F;
uint8_t mode = 0;

void wifiSetup()
{

  WiFi.mode(WIFI_STA);
  Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }
  Serial.println();
  Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}

void irSetup()
{
  irsend.begin();
}

void alexaOn()
{
  irsend.sendSony(alexaCommand, bits, repeats);
  delay(DELAY_AFTER_SEND);
  Serial.println("ALEXA ON, TV OFF");
}

void tvOn()
{
  irsend.sendSony(tvCommand, bits, repeats);
  delay(DELAY_AFTER_SEND);
  Serial.println("ALEXA OFF, TV ON");
}

void fauxmoSetup()
{
  fauxmo.addDevice(TV);
  fauxmo.setPort(80);
  fauxmo.enable(true);

  fauxmo.onSetState([](unsigned char device_id, const char *device_name, bool state, unsigned char value)
                    {
                      Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);
                      if (strcmp(device_name, TV) == 0)
                      {
                        if (state)
                        {

                          mode = 1;
                        }
                        else
                        {

                          mode = 2;
                        }
                      }
                    });
}

void sendIR()
{
  switch (mode)
  {
  case 1:
    tvOn();
    break;
  case 2:
    alexaOn();
    break;
  default:
    break;
  }
  mode = 0;
}

void setup()
{
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println();
  wifiSetup();

  Serial.printf("[IR] IR setup");
  Serial.println();
  irSetup();

  Serial.printf("[FAUXMO] Fauxmo setup");
  Serial.println();

  fauxmoSetup();
}

void loop()
{
  // tvOn();
  // delay(5000);
  // alexaOn();
  fauxmo.handle();
  static unsigned long last = millis();
  if (millis() - last > 500)
  {
    last = millis();
    ESP.getFreeHeap();
    if(mode!=0){
      Serial.println(mode);
      sendIR();
    }
  }
}
