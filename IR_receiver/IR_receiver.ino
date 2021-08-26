#include <IRremote.h>
#define STATUS_LED_PIN 7
#define IR_RECEIVE_PIN 6

#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

decode_type_t sony = SONY;
void setup()
{
  Serial.begin(9600);
  lcd.begin(16,2);
  IrReceiver.enableIRIn();
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  pinMode(STATUS_LED_PIN, OUTPUT);

  lcd.print("   Iroj's IR ");
  lcd.setCursor(0,1); //Display position
  lcd.print("  Code Detector");
}

void loop()
{

  if (IrReceiver.decode())
  {
    IrReceiver.printIRResultShort(&Serial);
     lcd.clear();
    lcd.print("Protocol:             ");
    lcd.setCursor(10,0);
    lcd.print(getProtocolString(IrReceiver.decodedIRData.protocol));
    lcd.setCursor(0,1);
    lcd.print("Code:                   ");
    lcd.setCursor(6,1);
    lcd.print(IrReceiver.decodedIRData.decodedRawData, HEX);
    if (IrReceiver.decodedIRData.protocol == sony)
    {
      if (IrReceiver.decodedIRData.decodedRawData == 2085)
      {
        digitalWrite(STATUS_LED_PIN, LOW);
      }
      if (IrReceiver.decodedIRData.decodedRawData == 2153)
      {
        digitalWrite(STATUS_LED_PIN, HIGH);
      }
    }
    IrReceiver.resume();
  }
}
