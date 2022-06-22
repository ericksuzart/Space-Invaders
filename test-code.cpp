#include <LiquidCrystal.h>

const int rs = 13, enable = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8;
LiquidCrystal lcd(rs, enable, d4, d5, d6, d7);

#define btnLEFT 3
#define btnRIGHT 4
#define btnFIRE 5
#define btnNONE 6


void setup() {
  lcd.begin(16, 2); //Define o número de colunas e linhas do LCD
  Serial.begin(9600);
}

int val;

byte buttonPressed()
{
  int adc_key_in = analogRead(A0);      // read analogue value
  Serial.println(adc_key_in);
  if (adc_key_in > 150)  return btnLEFT;
  if (adc_key_in > 490)  return btnRIGHT;
  if (adc_key_in > 390)  return btnFIRE;

  return btnNONE;
}


void loop() { 
  val = analogRead(A0);
  delay(100);
  lcd.clear();
  lcd.setCursor(0, 0);

  switch(buttonPressed())
  {
    case btnRIGHT: {
      lcd.print("Right");
      Serial.println("Right");
      break;
    }
    case btnLEFT:{
      lcd.print("Left");
      Serial.println("Left");
      break;
    }
    // Shoot
    case btnFIRE:{
      lcd.print("FIRE");
      Serial.println("FIRE");
      break;
    }
    default:
      lcd.print("NONE");
      Serial.println("NONE");
      break;
  }

  lcd.setCursor(0, 1);
  lcd.print(val);
  delay(100);
}
