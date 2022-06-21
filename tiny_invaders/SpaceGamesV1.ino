/**************************************************************************
 Space Games for 1602 console

 Author: John Bradnam (jbrad2089@gmail.com)
 
  2021-07-14
  - Created menu system for Space Invaders and Space Impact games
 
 --------------------------------------------------------------------------
 Arduino IDE:
 --------------------------------------------------------------------------
  BOARD: ATtiny1614/1604/814/804/414/404/214/204
  Chip: ATtiny1614
  Clock Speed: 8MHz (if using external power) or 1MHz (if using a battery)
  millis()/micros() Timer: "TCD0 (1-series only, default there)
  Programmer: jtag2updi (megaTinyCore)

  ATtiny1614 Pins mapped to Ardunio Pins

              +--------+
          VCC + 1   14 + GND
  (SS)  0 PA4 + 2   13 + PA3 10 (SCK)
        1 PA5 + 3   12 + PA2 9  (MISO)
  (DAC) 2 PA6 + 4   11 + PA1 8  (MOSI)
        3 PA7 + 5   10 + PA0 11 (UPDI)
  (RXD) 4 PB3 + 6    9 + PB0 7  (SCL)
  (TXD) 5 PB2 + 7    8 + PB1 6  (SDA)
              +--------+
  
 **************************************************************************/

#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <LiquidCrystal.h>
#include <TimerFreeTone.h>  // https://bitbucket.org/teckel12/arduino-timer-free-tone/wiki/Home
#include <EEPROM.h>
#include "button.h"

//LCD Screen
#define LCD_RS 0   //PA4
#define LCD_EN 1   //PA5
#define LCD_D4 2   //PA6
#define LCD_D5 3   //PA7
#define LCD_D6 4   //PB3
#define LCD_D7 5   //PB2

//Switches
#define SW_FIRE 6  //PB1
#define SW_XPAD 10 //PA3

//Other
#define SPEAKER 7  //PB0
#define LIGHT 8    //PA1
#define POWER 9    //PA2
 
//Initialize the LCD
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

//Buttons
enum buttonEnum { SW_NONE, SW_LEFT, SW_RIGHT, SW_DOWN, SW_UP };
Button* leftButton;
Button* rightButton;
Button* downButton;
Button* upButton;
Button* fireButton;

//EEPROM handling
#define EEPROM_ADDRESS 0
#define EEPROM_MAGIC 0x0BAD0DAD
typedef struct {
  uint32_t magic;
  uint32_t impactHighScore;
  uint32_t invaderHighScore;
} EEPROM_DATA;

EEPROM_DATA EepromData;       //Current EEPROM settings

enum modesEnum { MNU_MAIN, MNU_INVADERS, MNU_IMPACT };
modesEnum menuMode = MNU_MAIN;
modesEnum menuSelect = MNU_INVADERS;

#define SLEEP_TIMEOUT 15000
unsigned long sleepTimeOut;
volatile bool gameover = true;
int score = 0; //Player's score

//-------------------------------------------------------------------------
// Initialise Hardware
void setup()
{
  randomSeed(analogRead(SPEAKER));
  
  pinMode(POWER, OUTPUT);
  digitalWrite(POWER, HIGH);
  pinMode(LIGHT, OUTPUT);
  digitalWrite(LIGHT, HIGH);
  pinMode(SPEAKER, OUTPUT);

  //Initialise buttons
  leftButton = new Button(SW_LEFT, SW_XPAD, 250, 399, false); //22K/10K = 320
  rightButton = new Button(SW_RIGHT, SW_XPAD, 520, 700, false); //22K/30K = 590
  downButton = new Button(SW_DOWN, SW_XPAD, 400, 519, false); //22K/20K = 487
  upButton = new Button(SW_UP, SW_XPAD, 0, 100, false); //GND = 0
  fireButton = new Button(SW_FIRE);

  //Get last high scores
  readEepromData();
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  gameover = false;
  score = 0;
  switch (menuMode)
  {
    case MNU_INVADERS: invaderSetup(); break;
    case MNU_IMPACT:   impactSetup();  break;
  }
}

//-------------------------------------------------------------------------
// Main program loop
void loop()
{
  switch (menuMode)
  {
    case MNU_MAIN:
      displayMenuScreen();
      sleepTimeOut = millis() + SLEEP_TIMEOUT;
      while(!fireButton->Pressed())
      {
        if (downButton->Pressed() && menuSelect == MNU_INVADERS)
        {
          menuSelect = MNU_IMPACT;
          displayMenuCursor();
          sleepTimeOut = millis() + SLEEP_TIMEOUT;
        }
        else if (upButton->Pressed() && menuSelect == MNU_IMPACT)
        {
          menuSelect = MNU_INVADERS;
          displayMenuCursor();
          sleepTimeOut = millis() + SLEEP_TIMEOUT;
        }
        else if (millis() > sleepTimeOut) 
        {
          Sleep();
          displayMenuScreen();
          sleepTimeOut = millis() + SLEEP_TIMEOUT;
          while (fireButton->State() == LOW); //Wait until fire button is released
        }
      };
      menuMode = menuSelect;
      gameover = false;
      score = 0;
      switch (menuMode)
      {
        case MNU_INVADERS: invaderSetup(); break;
        case MNU_IMPACT:   impactSetup();  break;
      }
      break;
      
    case MNU_INVADERS:
      gameover = invaderLoop();
      break;
    
    case MNU_IMPACT:
      gameover = impactLoop();
      break;
  }
  if (gameover)
  {
    switch (menuMode)
    {
      case MNU_INVADERS: invaderShutDown(); break;
      case MNU_IMPACT:   impactShutDown();  break;
    }
    menuMode = MNU_MAIN;
  }
}

//-------------------------------------------------------------------------
// display main menu cursor
void displayMenuCursor()
{
  lcd.setCursor(0, 0);
  lcd.print((menuSelect == MNU_INVADERS) ? ">" : " ");
  lcd.setCursor(0, 1);
  lcd.print((menuSelect == MNU_IMPACT) ? ">" : " ");
}

//-------------------------------------------------------------------------
// display main menu screen
void displayMenuScreen()
{
  lcd.setCursor(0, 0);
  lcd.print("  SPACE INVADERS");
  lcd.setCursor(0, 1);
  lcd.print("  SPACE IMPACT  ");
  displayMenuCursor();
}

//--------------------------------------------------------------------
// Set all LCD pins to INPUTs or OUTPUTs
void setLcdPins(int state) 
{
  static char Outputs[] = {LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7};
  for (int i=0; i<6; i++) 
  {
    pinMode(Outputs[i], state);
  }
}

//--------------------------------------------------------------------
// Handle pin change interrupt when to wake up processor
void wakeUpProcessor()
{
}

//--------------------------------------------------------------------
// Put the processor to sleep
void Sleep() 
{
  switch (menuMode)
  {
    case MNU_INVADERS: invaderShutDown(); break;
    case MNU_IMPACT:   impactShutDown();  break;
  }
  
  attachInterrupt(SW_FIRE, wakeUpProcessor, CHANGE);    //Used to wake up processor
  lcd.clear();
  setLcdPins(INPUT);
  digitalWrite(LIGHT, LOW);
  digitalWrite(POWER, LOW);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // sleep mode is set here
  sleep_enable();
  sleep_mode();                         // System actually sleeps here
  sleep_disable();                      // System continues execution here when watchdog timed out
  // Continue after sleep
  pinMode(POWER, OUTPUT);
  digitalWrite(POWER, HIGH);
  pinMode(LIGHT, OUTPUT);
  digitalWrite(LIGHT, HIGH);
  setLcdPins(OUTPUT);
  lcd.begin(16, 2);
  detachInterrupt(SW_FIRE);

  gameover = false;
  score = 0;
  switch (menuMode)
  {
    case MNU_INVADERS: invaderSetup(); break;
    case MNU_IMPACT:   impactSetup();  break;
  }
}

//------------------------------------------------------------------

void playHitTone()
{
  TimerFreeTone(SPEAKER, 300, 150); 
}

//------------------------------------------------------------------

void playMissTone()
{
  TimerFreeTone(SPEAKER, 50, 150); 
}

//-----------------------------------------------------------------------------------
//Play the sound for the start of a round
void playStartRound() 
{
  #define MAX_NOTE 4978               // Maximum high tone in hertz. Used for siren.
  #define MIN_NOTE 31                 // Minimum low tone in hertz. Used for siren.
  
  for (int note = MIN_NOTE; note <= MAX_NOTE; note += 5)
  {                       
    TimerFreeTone(SPEAKER, note, 1);
  }
}

//------------------------------------------------------------------

//Play a high note as a sign you lost
void playWinSound()
{
  //TimerFreeTone(SPEAKER,880,300);
  TimerFreeTone(SPEAKER,880,100); //A5
  TimerFreeTone(SPEAKER,988,100); //B5
  TimerFreeTone(SPEAKER,523,100); //C5
  TimerFreeTone(SPEAKER,988,100); //B5
  TimerFreeTone(SPEAKER,523,100); //C5
  TimerFreeTone(SPEAKER,587,100); //D5
  TimerFreeTone(SPEAKER,523,100); //C5
  TimerFreeTone(SPEAKER,587,100); //D5
  TimerFreeTone(SPEAKER,659,100); //E5
  TimerFreeTone(SPEAKER,587,100); //D5
  TimerFreeTone(SPEAKER,659,100); //E5
  TimerFreeTone(SPEAKER,659,100); //E5
  delay(250);
}

//------------------------------------------------------------------------------------------------------------------

//Play wah wah wah wahwahwahwahwahwah
void playLoseSound()
{
  delay(400);
  //wah wah wah wahwahwahwahwahwah
  for(double wah=0; wah<4; wah+=6.541)
  {
    TimerFreeTone(SPEAKER, 440+wah, 50);
  }
  TimerFreeTone(SPEAKER, 466.164, 100);
  delay(80);
  for(double wah=0; wah<5; wah+=4.939)
  {
    TimerFreeTone(SPEAKER, 415.305+wah, 50);
  }
  TimerFreeTone(SPEAKER, 440.000, 100);
  delay(80);
  for(double wah=0; wah<5; wah+=4.662)
  {
    TimerFreeTone(SPEAKER, 391.995+wah, 50);
  }
  TimerFreeTone(SPEAKER, 415.305, 100);
  delay(80);
  for(int j=0; j<7; j++)
  {
    TimerFreeTone(SPEAKER, 391.995, 70);
    TimerFreeTone(SPEAKER, 415.305, 70);
  }
  delay(400);
}

//---------------------------------------------------------------
//Write the EepromData structure to EEPROM
void writeEepromData()
{
  //This function uses EEPROM.update() to perform the write, so does not rewrites the value if it didn't change.
  EEPROM.put(EEPROM_ADDRESS,EepromData);
}

//---------------------------------------------------------------
//Read the EepromData structure from EEPROM, initialise if necessary
void readEepromData()
{
  //Eprom
  EEPROM.get(EEPROM_ADDRESS,EepromData);
  if (EepromData.magic != EEPROM_MAGIC)
  {
    EepromData.magic = EEPROM_MAGIC;
    EepromData.impactHighScore = 0;
    EepromData.invaderHighScore = 0;
    writeEepromData();
  }
}