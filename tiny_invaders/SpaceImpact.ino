/**************************************************************************
 Space Impact Game

 Author: John Bradnam (jbrad2089@gmail.com)
 
 Modified code from Space Impact LCD game by MOHD SOHAIL
 (https://www.youtube.com/channel/UCaXI2PcsTlH5g0et67kdD6g)
 (https://www.hackster.io/mohammadsohail0008/space-impact-lcd-game-ce5c74)
 
 2021-07-14
  - Create program for ATtiny1614
  - Replaced joystick with X-Pad
  - Made fire and movement processing interrupt driven
  - Made LCD backlight and power enabled via code
  - Added processor sleep mode for battery powered systems
  - Added more sounds
  - Added EEPROM functions to store high score 
 
 **************************************************************************/

//Logical screen
volatile uint8_t area[4][15] = 
{
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

#define CUSTOM_CHARACTERS 8
const byte cc[CUSTOM_CHARACTERS][8] =
{  
  {B11100, B01111, B11100, B00000, B00000, B00000, B00000, B00000},
  {B00000, B00000, B00000, B00000, B11100, B01111, B11100, B00000},
  {B11100, B01111, B11100, B00000, B11100, B10100, B11100, B00000},
  {B11100, B10100, B11100, B00000, B11100, B01111, B11100, B00000},
  {B11100, B10100, B11100, B00000, B11100, B10100, B11100, B00000},
  {B00000, B00000, B00000, B00000, B00100, B10010, B01000, B00000},
  {B00100, B10010, B01000, B00000, B11100, B10100, B11100, B00000},
  {B11100, B10100, B11100, B00000, B00100, B10010, B01000, B00000}
};

#if (F_CPU == 1000000L)
  //A 1MHz clock uses less battery power when running
  #define MAX_GAME_DELAY 50
  #define MIN_GAME_DELAY 10
  #define STEP_GAME_DELAY 5
#else
  //Assume 8MHz if running via external power
  #define MAX_GAME_DELAY 200
  #define MIN_GAME_DELAY 50
  #define STEP_GAME_DELAY 5
#endif

volatile uint8_t fireLoad = 0;
volatile uint8_t fireConsumption = 0;
volatile bool xPadButtonDown = false;

//-------------------------------------------------------------------------
// Initialise Hardware
void impactSetup() 
{
  attachInterrupt(SW_FIRE, fireButtonInterrupt, CHANGE);    //Used to wake up processor and to fire bullet

  //Define custom characters
  for (int i = 0; i < CUSTOM_CHARACTERS; i++)
  {
    lcd.createChar(i, &cc[i][0]);
  }
  lcd.home();
  displayImpactInitialScreens();

  //Set up background player control
  TCB1.CCMP = 10000;
  TCB1.INTCTRL = TCB_CAPT_bm;
  TCB1.CTRLA = TCB_ENABLE_bm;
}

//-------------------------------------------------------------------------
// Initialise Hardware for space invaders
void impactInitialise()
{
}

//-------------------------------------------------------------------------
// Release Hardware used for space invaders
void impactShutDown()
{
  TCB1.CTRLA = TCB1.CTRLA & ~TCB_ENABLE_bm;
  detachInterrupt(SW_FIRE);
}

//--------------------------------------------------------------------
// Handle pin change interrupt when SW_FIRE is pressed
void fireButtonInterrupt()
{
  if (!gameover && fireButton->State() == LOW && fireLoad >= fireConsumption) {
    fireLoad -= fireConsumption;
    for (uint8_t y=0; y<4; y++) 
    {
      for (uint8_t x=0; x<14; x++) 
      {
        if (area[y][x]==1) // spaceship
        { 
          area[y][x+1] += 4;
        }
      }
    }
  }
}

//-------------------------------------------------------------------------
//Timer B Interrupt handler interrupt each mS - output segments
ISR(TCB1_INT_vect)
{
  if (xPadButtonDown)
  {
    xPadButtonDown = (leftButton->State() == HIGH || rightButton->State() == HIGH || upButton->State() == HIGH || downButton->State() == HIGH);
  }
  else if (!gameover)
  {
    bool doBreak = false;
    for (uint8_t y=0; y<4; y++) 
    {
      for (uint8_t x=0; x<15; x++) 
      {
        if (area[y][x]==1) 
        {
          doBreak = true;
          if (leftButton->State() == HIGH && x > 0) 
          {
            area[y][x] = 0;
            area[y][x-1] += 1;
            xPadButtonDown = true;
          } 
          else if (rightButton->State() == HIGH && x < 14) 
          {
            if (area[y][x+1]!=4) 
            {
              area[y][x] = 0;
              area[y][x+1] += 1;
              xPadButtonDown = true;
            }
          } 
          else if (upButton->State() == HIGH && y > 0) 
          {
            area[y][x] = 0;
            area[y-1][x] += 1;
            xPadButtonDown = true;
          } 
          else if (downButton->State() == HIGH && y < 3) 
          {
            area[y][x] = 0;
            area[y+1][x] += 1;
            xPadButtonDown = true;
          }
        }
        if (doBreak) 
        {
          break;
        }
      }
      if (doBreak) 
      {
        break;
      }
    }
  }
  
  //Clear interrupt flag
  TCB1.INTFLAGS |= TCB_CAPT_bm; //clear the interrupt flag(to reset TCB1.CNT)
}

//-------------------------------------------------------------------------
// Main program loop
bool impactLoop()
{
  //Setup the game
  lcd.clear();
  delay(500);
  playStartRound(); 

  //Clear playing field
  for (uint8_t y=0; y<4; y++) 
  {
    for (uint8_t x=0; x<15; x++) 
    {
      area[y][x] = 0;
    }
  }
  area[0][0] = 1;         //Put ship in top left corner
  uint8_t sleep = MAX_GAME_DELAY;
  uint8_t junkRisk = 10;
  fireLoad = 0;
  fireConsumption = 9;
  uint8_t life = 3;
  unsigned long count = 0;

  lcd.setCursor(0,0);
  lcd.print(life);
  lcd.setCursor(0,1);
  lcd.print(fireLoad);
      
  draw();
  gameover = false;
  
  while (!gameover) 
  {
    for (uint8_t y=0; y<4; y++) 
    {
      for (uint8_t x=15; x>0; x--) 
      {
        if (area[y][x-1]==4) 
        {
          area[y][x-1] = 0;
          if (x<15) 
          {
            area[y][x] += 4;
          }
        }
      }
    }
    
    if (fireLoad<9) 
    {
      fireLoad++;
      lcd.setCursor(0,1);
      lcd.print(fireLoad);
    }

    draw();
    delay(sleep);
    
    for (uint8_t y=0; y<4; y++) 
    {
      for (uint8_t x=0; x<15; x++) 
      {
        if (area[y][x]==2) 
        {
          area[y][x] = 0;
          if (x>0) 
          {
            area[y][x-1] += 2;
          }
        }
      }
    }

    for (uint8_t y=0; y<4; y++) {
      if (random(100) < junkRisk) {
        area[y][14] += 2;
      }
    }
  
    draw();
    delay(sleep);

    for (uint8_t y=0; y<4; y++) 
    {
      for (uint8_t x=0; x<15; x++) 
      {
        if (area[y][x]==3) // collided ship
        { 
          area[y][x] = 1;
          blinkShip();
          life--;
          lcd.setCursor(0, 0);
          lcd.print(life);
          if(life==0) 
          {
            impactGameOver();
            gameover = true;
          }
        } 
        else if (area[y][x]>4) 
        {
          for (uint8_t i=0; i<10; i++) 
          {
            digitalWrite(SPEAKER,HIGH);
            delay(3);
            digitalWrite(SPEAKER,LOW);
            delay(3);
          }
          score+=10;
          area[y][x] -= 6;
        }
      }
    }
  
    score++;
    count++;

    if (count % 100 == 0)
    {
      sleep = max(MIN_GAME_DELAY, sleep - STEP_GAME_DELAY);
      junkRisk +=3;
      fireConsumption--;
    }
  }
  return gameover;
}

//-------------------------------------------------------------------------
// Transfer the logical screen to the physical screen
void draw() 
{
  for (uint8_t y=0; y<4; y+=2) 
  {
    for (uint8_t x=0; x<15; x++) 
    {
      lcd.setCursor(x+1, y/2);
      
      if (area[y][x]==1) 
      {
        if (area[y+1][x]==0) 
        {
          lcd.print(char(0));
        } 
        else if (area[y+1][x]==2 || area[y+1][x]==6 || area[y+1][x]==10)  // down obstacle
        {
          lcd.print(char(2));
        }
      } else if (area[y][x]==2 || area[y][x]==6 || area[y][x]==10) 
      {
        if (area[y+1][x]==0) 
        {
          lcd.write(0b11011111); //upper Small box
        } 
        else if (area[y+1][x]==1) 
        {
          lcd.print(char(3));
        } 
        else if (area[y+1][x]==2) 
        {
          lcd.print(char(4));
        }
        else if (area[y+1][x]==4 || area[y+1][x]==6 || area[y+1][x]==10) 
        {
          lcd.print(char(7));
        }
      } 
      else if (area[y][x]==4) 
      {
        if (area[y+1][x]==0) 
        {
          lcd.write(0b11011110); //bullet
        }
        else if (area[y+1][x]==2 || area[y+1][x]==6 || area[y+1][x]==10) 
        {
          lcd.print(char(6)); //bullet + junk
        }
      } 
      else if (area[y][x]==0) // above nothing 
      { 
        if (area[y+1][x]==0) // nothing below
        { 
          lcd.print(" ");
        } 
        else if (area[y+1][x]==1) // below ship
        { 
          lcd.print(char(1));
        } 
        else if (area[y+1][x]==2 || area[y+1][x]==6 || area[y+1][x]==10) 
        {
          lcd.write(0b10100001); //lower Small box
        } 
        else if (area[y+1][x]==4) 
        {
          lcd.print(char(5));
        }
      } else {
        lcd.print(" ");
      }
    }
  }
}

//-------------------------------------------------------------------------
// Flash the ship
void blinkShip() 
{
  for (uint8_t y=0; y<4; y++) 
  {
    for (uint8_t x=0; x<15; x++) 
    {
      if (area[y][x]==1) //Found ship
      {

        for (uint8_t i=0; i<3; i++) 
        {
          lcd.setCursor(x+1, y>1);
          if (y==0 || y==2) 
          {
            if (area[y+1][x]==0) 
            {
              lcd.print(" ");
            } 
            else 
            {
              lcd.write(0b10100001);
            }
          } 
          else 
          {
            if (area[y-1][x]==0) {
              lcd.print(" ");
            } 
            else 
            {
              lcd.write(0b11011111);
            }
          }
          for (uint8_t i=0; i<10; i++) 
          {
            digitalWrite(SPEAKER,HIGH);
            delay(25);
            digitalWrite(SPEAKER,LOW);
            delay(5);
          }
          
          lcd.setCursor(x+1, y>1);
          if (y==0 || y==2) 
          {
            if (area[y+1][x]==0) 
            {
              lcd.print(char(0));
            } 
            else 
            {
              lcd.print(char(2));
            }
          } 
          else 
          {
            if (area[y-1][x]==0) 
            {
              lcd.print(char(1));
            } 
            else 
            {
              lcd.print(char(3));
            }
          }
          for (uint8_t i=0; i<10; i++) 
          {
            digitalWrite(SPEAKER,HIGH);
            delay(25);
            digitalWrite(SPEAKER,LOW);
            delay(5);
          }
        }
      }
    }
  }
}

//--------------------------------------------------------------------
// Display Opening animation, instructions and "Press button to start" message
void displayImpactInitialScreens()
{
  lcd.clear();
  for (uint8_t y=0; y<2; y++) 
  {
    for (uint8_t x=0; x<16; x++) 
    {
      lcd.setCursor(x, y);
      lcd.print(char(1));
      delay(100);
      lcd.setCursor(x, y);
      if (y==0) 
      {
        lcd.print("    SPACE       "[x]);
      } 
      else 
      {
        lcd.print("      IMPACT    "[x]);
      }
    }
  }
  delay(1000);
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("3 -> LIFE POINTS");
  lcd.setCursor(0,1);
  lcd.print("9 -> WEAPON LOAD");
  delay(3000);
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("  PRESS BUTTON  ");
  lcd.setCursor(0,1);
  lcd.print("    TO START    ");
}

//---------------------------------------------------------------
//Handle game over
void impactGameOver()
{
  //Kill interrupts
  impactShutDown();
  lcd.clear();
  lcd.setCursor(6, 0);
  lcd.print("GAME");
  lcd.setCursor(6, 1);
  lcd.print("OVER");
  playLoseSound();
  for (uint8_t y=0; y<2; y++) 
  {
    for (uint8_t x=0; x<16; x++) 
    {
      lcd.setCursor(x, y);
      lcd.print(char(1));
      delay(100);
      lcd.setCursor(x, y);
      lcd.print(" ");
    }
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SCORE:");
  lcd.setCursor(7, 0);
  lcd.print(score);
  lcd.setCursor(0, 1);
  lcd.print("BEST:");
  lcd.setCursor(7, 1);
  lcd.print(EepromData.impactHighScore);
  if (score > EepromData.impactHighScore)
  {
    EepromData.impactHighScore = score;
    writeEepromData();
    playWinSound();
  }
  delay(1000);
  for (uint8_t y=0; y<2; y++) 
  {
    for (uint8_t x=0; x<16; x++) 
    {
      lcd.setCursor(x, y);
      lcd.print(char(1));
      delay(100);
      lcd.setCursor(x, y);
      lcd.print(" ");
    }
  }
  delay(1000);
}
