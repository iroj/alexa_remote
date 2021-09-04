
#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

#include <fauxmoESP.h>
#include "credentials.h"
typedef struct DEVICE
{
  char *name;
  uint8_t id;
};
bool toggle = false;

#define DELAY_AFTER_SEND 2000
#define DELAY_BETWEEN_REPEATS 200

#define SERIAL_BAUDRATE 115200

#define IR_SEND_PIN 12

constexpr DEVICE TVAUDIO{"TV audio", 0};
constexpr DEVICE TVINPUT{"TV input", 1};
constexpr DEVICE TVPOWER{"TV power", 2};

IRsend irsend(IR_SEND_PIN);
fauxmoESP fauxmo;

uint16_t tvAudioCommand = 0x961;
uint16_t alexaAudioCommand = 0xa41;
uint64_t tvPowerCommand = 0x20DF10EF;
uint64_t tvInputCommand = 0x20DFF40B;

uint16_t sonyrepeats = 2;
uint16_t sonybits = 12;
uint16_t necbits = 32;
uint16_t tvinputrepeats = 1;

uint8_t mode = 0;

void deviceSetup(DEVICE device)
{
  fauxmo.addDevice(device.name);
  device.id = fauxmo.getDeviceId(device.name);
  Serial.println();
  Serial.printf("[DEVICE SETUP] %s ID: %d", device.name, device.id);
}

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

void alexaAudioOn()
{
  irsend.sendSony(alexaAudioCommand, sonybits, sonyrepeats);
  delay(DELAY_AFTER_SEND);
  Serial.println();
  Serial.println("[RESULT] ALEXA ON, TV OFF");
}

void tvAudioOn()
{
  irsend.sendSony(tvAudioCommand, sonybits, sonyrepeats);
  delay(DELAY_AFTER_SEND);
  Serial.println();
  Serial.println("[RESULT] ALEXA OFF, TV ON");
}

void tvToggle()
{
  irsend.sendNEC(tvPowerCommand, necbits);
  delay(DELAY_AFTER_SEND);
  Serial.println();
  Serial.println("[RESULT] TV POWER TOGGLED");
}

void tvInputSelect()
{
  Serial.println();
  Serial.printf("[RESULT] TV INPUT TOGGLED %d TIMES", tvinputrepeats);

  for (int count = 1; count <= tvinputrepeats; count++)
  {
    irsend.sendNEC(tvInputCommand, necbits);
      delay(DELAY_BETWEEN_REPEATS);
  }
}

void fauxmoSetup()
{
  deviceSetup(TVAUDIO);
  deviceSetup(TVINPUT);
  deviceSetup(TVPOWER);

  fauxmo.setPort(80);
  fauxmo.enable(true);

  fauxmo.onSetState([](unsigned char device_id, const char *device_name, bool state, unsigned char value)
                    {
                      Serial.println();
                      Serial.println();
                      Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);
                      switch (device_id)
                      {
                      case TVAUDIO.id:
                        if (state)
                        {

                          mode = 1;
                        }
                        else
                        {

                          mode = 2;
                        }
                        break;
                      case TVPOWER.id:
                        if (state)
                        {

                          mode = 3;
                        }
                        else
                        {

                          mode = 4;
                        }
                        break;
                      case TVINPUT.id:
                        handleTvInput(value);
                        break;
                      default:
                        break;
                      }
                    });
}

void handleTvInput(int hexValue)
{
  mode = 5;
  switch (hexValue)
  {
  case 4:
    tvinputrepeats = 1;
    break;
  case 6:
    tvinputrepeats = 2;
    break;
  case 9:
    tvinputrepeats = 3;
    break;
  case 11:
    tvinputrepeats = 4;
    break;
  case 14:
    tvinputrepeats = 5;
    break;
  default:
    tvinputrepeats = 1;
  }
  //reset tvinput value to 0;

  fauxmo.setState(TVINPUT.name, false, 255);
}

void sendIR()
{
  switch (mode)
  {
  case 1:
    tvAudioOn();
    break;
  case 2:
    alexaAudioOn();
    break;
  case 3:
    tvToggle();
    tvAudioOn();
    break;
  case 4:
    tvToggle();
    alexaAudioOn();
    break;
  case 5:
    tvInputSelect();
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

  Serial.println();
  Serial.printf("[IR] IR setup");
  irSetup();

  Serial.println();
  Serial.printf("[FAUXMO] Fauxmo setup");

  fauxmoSetup();
}

void loop()
{
  fauxmo.handle();
  static unsigned long last = millis();
  if (millis() - last > 500)
  {
    last = millis();
    ESP.getFreeHeap();
    if (mode != 0)
    {
      Serial.println();
      Serial.printf("[MODE] %d", mode);
      sendIR();
    }
  }
}
