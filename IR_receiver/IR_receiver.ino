#include <IRremote.h>

const int IR_RECEIVE_PIN = 2;
bool toggle = false;
decode_type_t sony = SONY;
IRrecv irrecv;


void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  pinMode(7, OUTPUT);
}

void loop() {
  if (irrecv.decode()) {
    irrecv.printIRResultShort(&Serial);

    Serial.println(irrecv.decodedIRData.decodedRawData);

    if (irrecv.decodedIRData.protocol == sony ){
      if(irrecv.decodedIRData.decodedRawData == 2085) {
        digitalWrite(7, HIGH);
      }
      if(irrecv.decodedIRData.decodedRawData == 2153) {
        digitalWrite(7, LOW);
      }
    } 
    IrReceiver.resume();
  }


  delay(1000);
}
