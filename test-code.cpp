/*
  At Doom's Gate (H1M1)
  Connect a piezo buzzer or speaker to pin 11 or select a new pin.
  More songs available at https://github.com/robsoncouto/arduino-songs

                                              Robson Couto, 2019
*/

#include <Adafruit_LiquidCrystal.h> //Carrega a biblioteca LiquidCrystal

Adafruit_LiquidCrystal lcd(0);

#define btnLEFT   3
#define btnRIGHT  4
#define btnFIRE 5
#define btnNONE   6

// change this to make the song slower or faster
int tempo = 225;

// change this to whichever pin you want to use
int buzzer = 2;


void setup() {
  lcd.begin(16, 2); //Define o número de colunas e linhas do LCD
}

int val;

byte buttonPressed()
{
  int adc_key_in = analogRead(A0);      // read analogue value
  if (adc_key_in <= 350)  return btnLEFT;
  if (adc_key_in <= 550)  return btnRIGHT;
  if (adc_key_in <= 700)  return btnFIRE;

  return btnNONE;
}


void loop() { 
  val = analogRead(A0);
  delay(25);
  lcd.clear();
  lcd.setCursor(0, 0);

  switch(buttonPressed())
  {
    case btnRIGHT: {
      lcd.print("Button Right");
      break;
    }
    case btnLEFT:{
      lcd.print("Button Left");
      break;
    }
    // Shoot
    case btnFIRE:{
      lcd.print("Button FIRE");
      break;
    }
    default:
      break;
  }

  lcd.setCursor(0, 1);
  lcd.print(val);
  delay(25);
}
