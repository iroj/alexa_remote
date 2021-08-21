#include <IRremote.h>

const int IR_SEND_PIN = 12;
uint16_t sAddress1 = 0x10;
uint8_t sCommand1 = 0x69;

uint16_t sAddress2 = 0x10;
uint8_t sCommand2 = 0x25;


void setup() {
  pinMode(IR_SEND_PIN, OUTPUT);
  IrSender.begin(IR_SEND_PIN, ENABLE_LED_FEEDBACK);
}

void loop() {
  IrSender.sendSony(sAddress1,sCommand1,0);
  Serial.print('OUT1');
  delay(3000); 
  IrSender.sendSony(sAddress2,sCommand2,0);
  Serial.print('OUT2');
  delay(3000); 
}
