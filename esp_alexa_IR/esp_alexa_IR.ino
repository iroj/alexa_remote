
#include <ESP8266WiFi.h>
#include <fauxmoESP.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>
#include "credentials.h"

bool toggle = false;
decode_type_t sony = SONY;

fauxmoESP fauxmo;

#define SERIAL_BAUDRATE 115200

#define IR_SEND_PIN 12
#define IR_RECEIVE_PIN 14
#define STATUS_LED_PIN 4
#define TV "TV audio"

IRsend irsend(IR_SEND_PIN);
IRrecv irrecv(IR_RECEIVE_PIN);
decode_results results;  

// uint8_t tvCode = 0x52;
// uint8_t alexaCode = 0x4B;

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
  irsend.begin();  
}

void irReceiverSetup(){
  irrecv.enableIRIn();  
}

void irStatusLEDSetup(){
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, HIGH);
}

void irReceiverLoop(){

   if (irrecv.decode(&results)) {
    // serialPrintUint64(results.value, HEX);
    Serial.println("");
     Serial.print(resultToHumanReadableBasic(&results));

      // if(results.value == alexaCode) {
      //   digitalWrite(STATUS_LED_PIN, HIGH);
      // }
      // if(results.value == tvCode) {
      //   digitalWrite(STATUS_LED_PIN, LOW);
      // }

    irrecv.resume();  

  }
}

void alexaOn(){
  irsend.sendSony(0x4B, 12, 0);
  //  IrSender.sendRaw_P(alexaRawSignal, sizeof(alexaRawSignal) / sizeof(alexaRawSignal[0]), 40);

  Serial.println("ALEXA ON, TV OFF");
}

void tvOn(){
  irsend.sendSony(0x52, 12, 0);  
  //  IrSender.sendRaw_P(tvRawSignal, sizeof(tvRawSignal) / sizeof(tvRawSignal[0]), 40);

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
