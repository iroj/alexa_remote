
#include <ESP8266WiFi.h>
#include <IRremote.h>

#include <fauxmoESP.h>
#include "credentials.h"

bool toggle = false;
decode_type_t sony = SONY;

fauxmoESP fauxmo;

#define SERIAL_BAUDRATE 115200

#define IR_SEND_PIN 12
#define IR_RECEIVE_PIN 14
#define STATUS_LED_PIN 4
#define TV "TV audio"

uint16_t sAddress = 0x10;
uint8_t tvCommand = 0x69;
uint8_t alexaCommand = 0x25;

// uint16_t tvModeAddress = 0x4;
// uint8_t tvModeCommand = 0x2F;

void wifiSetup() {

  WiFi.mode(WIFI_STA);
  Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();
  Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}

void irSetup(){
  IrSender.begin(IR_SEND_PIN, ENABLE_LED_FEEDBACK);  
}

void irReceiverSetup(){
  IrReceiver.enableIRIn();
  IrReceiver.begin(IR_RECEIVE_PIN);
}

void irStatusLEDSetup(){
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, HIGH);
}

void irReceiverLoop(){

  if (IrReceiver.decode()) {
    IrReceiver.printIRResultShort(&Serial);
    if (IrReceiver.decodedIRData.protocol == sony ){
      if(IrReceiver.decodedIRData.decodedRawData == 2085) {
        digitalWrite(STATUS_LED_PIN, HIGH);
      }
      if(IrReceiver.decodedIRData.decodedRawData == 2153) {
        digitalWrite(STATUS_LED_PIN, LOW);
      }
    } 
    IrReceiver.resume();
  }
}

void alexaOn(){
  IrSender.sendSony(sAddress, alexaCommand, 0);
  Serial.println("ALEXA ON, TV OFF");
}

void tvOn(){

  IrSender.sendSony(sAddress, tvCommand, 3);
// IrSender.sendNEC(tvModeAddress, tvModeCommand, 0);
  Serial.println("ALEXA OFF, TV ON");
}


void fauxmoSetup(){ 
  fauxmo.addDevice(TV);
  fauxmo.setPort(80);
  fauxmo.enable(true);

  fauxmo.onSetState([](unsigned char device_id, const char* device_name, bool state, unsigned char value) {
   Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);
     if (strcmp(device_name, TV) == 0) {
      if (state) {
        tvOn();
      }  else {
        alexaOn();
      }
    }
  });
}

void setup() {
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println();
  wifiSetup();

  Serial.printf("[IR] IR setup");
  Serial.println();
  irSetup();
  irReceiverSetup();
  irStatusLEDSetup();
  Serial.printf("[FAUXMO] Fauxmo setup");
  Serial.println();

  fauxmoSetup();
}

void loop() {
  fauxmo.handle();
  irReceiverLoop();
  delay(1000);
  static unsigned long last = millis();
  if (millis() - last > 5000) {
    last = millis();
    Serial.printf("[MAIN] Free heap: %d bytes\n", ESP.getFreeHeap());
  }
}
