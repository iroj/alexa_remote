#include <IRremote.h>

const int IR_RECEIVE_PIN = 2;
bool toggle = false;
decode_type_t sony = SONY;

void setup()
{
  Serial.begin(9600);
  IrReceiver.enableIRIn();
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  pinMode(7, OUTPUT);
}

void loop()
{

  if (IrReceiver.decode())
  {
    IrReceiver.printIRResultShort(&Serial);
    IrReceiver.printIRResultRawFormatted(&Serial);
    IrReceiver.compensateAndPrintIRResultAsCArray(&Serial);
    Serial.println(IrReceiver.decodedIRData.decodedRawData);

    if (IrReceiver.decodedIRData.protocol == sony)
    {
      if (IrReceiver.decodedIRData.decodedRawData == 2085)
      {
        digitalWrite(7, HIGH);
      }
      if (IrReceiver.decodedIRData.decodedRawData == 2153)
      {
        digitalWrite(7, LOW);
      }
    }
    IrReceiver.resume();
  }

  delay(1000);
}
