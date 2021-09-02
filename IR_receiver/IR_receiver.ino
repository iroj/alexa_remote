#include <Arduino.h>
#include <IRremote.h>

#define STATUS_LED_PIN 7
#define IR_RECEIVE_PIN 6
IRrecv IrReceiver(IR_RECEIVE_PIN);

#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
decode_results results;

decode_type_t sony = SONY;
void setup()
{
  Serial.begin(9600);
  lcd.begin(16, 2);

  IrReceiver.enableIRIn();  // Start the receiver
  pinMode(STATUS_LED_PIN, OUTPUT);

  lcd.print("   Iroj's IR ");
  lcd.setCursor(0, 1); //Display position
  lcd.print("  Code Detector");
}

void loop()
{
 if (IrReceiver.decode()) {  // Grab an IR code
    IrReceiver.printIRResultShort(&Serial);  
    lcd.clear();
    lcd.print("Protocol:             ");
    lcd.setCursor(10, 0);
    lcd.print(IrReceiver.getProtocolString());
    lcd.setCursor(0, 1);
    lcd.print("Code:                   ");
    lcd.setCursor(6, 1);
    lcd.print(IrReceiver.results.value, HEX);
    IrReceiver.resume();
  }
}
