/**************************************************************************
 Space Invader Game

 Author: John Bradnam (jbrad2089@gmail.com)
 
 Modified code from LCD Invaders by arduinocelentano
 (https://www.instructables.com/LCD-Invaders-a-Space-Invaders-Like-Game-on-16x2-LC/)
 
 2021-07-14
  - Updated program for ATtiny1614 and X-Pad
  - Halved bullet vertical speed to make bullets more visible
  - Add sound effects
  - Made LCD backlight and power enabled via code
  - Added processor sleep mode for battery powered systems
  - Added EEPROM functions to store high score 
 
 **************************************************************************/

//game field size
#define WIDTH 16
#define HEIGHT 4

//custom sprites
#define SHIP 0
#define BULLET_UP 1
#define BULLET_DOWN 2
#define SHIP_BULLET 3
#define ALIEN1 4
#define ALIEN2 5
#define ALIEN1BULLET 6
#define ALIEN2BULLET 7

#define GAME_STEP 100 //Delay (ms) between game steps
#define ALIENS_NUM 8 //Number of aliens

byte animationStep; //Number of game step
char screenBuffer[HEIGHT/2][WIDTH+1]; //Characters to be displayed on the screen
byte alienStep = 5; //The number of game steps between alien's movements
byte fireProbability = 20; //Probability of alien to shoot
byte level = 1; //Game level
byte aliensLeft = 0; //Number of aliens left on current level

//Define custom characters for game sprites
byte ship_sprite[] = {
B00000,
B00000,
B00000,
B00000,
B00000,
B00100,
B01110,
B11011
};

byte ship_bullet_sprite[] = {
B00000,
B00100,
B00100,
B00000,
B00000,
B00100,
B01110,
B11011
};

byte bullet_down_sprite[] = {
B00000,
B00000,
B00000,
B00000,
B00000,
B00100,
B00100,
B00000
};

byte bullet_up_sprite[] = {
B00000,
B00100,
B00100,
B00000,
B00000,
B00000,
B00000,
B00000
};

byte alien1_1_sprite[] = {
B01010,
B10101,
B01110,
B10001,
B00000,
B00000,
B00000,
B00000
};

byte alien1_2_sprite[] = {
B01010,
B10101,
B01110,
B01010,
B00000,
B00000,
B00000,
B00000
};

byte alien1_1_bullet_sprite[] = {
B01010,
B10101,
B01110,
B10001,
B00000,
B00100,
B00100,
B00000
};

byte alien1_2_bullet_sprite[] = {
B01010,
B10101,
B01110,
B01010,
B00000,
B00100,
B00100,
B00000
};

//-------------------------------------------------------------------------
//Base class for game objects
class GameObject
{
  //Object's coordinates and speed
  protected:
    int8_t _x;
    int8_t _y;
    int8_t _speed;
    
  public:
    GameObject():_x(0),_y(0),_speed(0){}
    GameObject(int8_t x, int8_t y): _x(x), _y(y), _speed(0){}
    GameObject(int8_t x, int8_t y, int8_t speed): _x(x), _y(y), _speed(speed){}
    
    //Getters and setters
    int8_t x() const
    {
      return _x;
    }
    
    int8_t y() const
    {
      return _y;
    }
    
    int8_t speed() const
    {
      return _speed;
    
    }
    
    bool setX(int8_t x)
    {
      if (x<0||x>WIDTH)
      {
        return false;
      }
      _x = x;
      return true;
    }
    
    bool setY(int8_t y)
    {
      if (y<0||y>HEIGHT)
      {
        return false;
      }
      _y = y;
      return true;
    }
    
    bool setSpeed(int8_t speed)
    {
      _speed = speed;
      return true;
    }
    
    //Collision detection
    bool collides(const GameObject& o)
    {
      return (_x==o.x() && _y==o.y()) ? true : false;
    }
};

//-------------------------------------------------------------------------
//Bullet class
class Bullet:public GameObject
{
  
  #define BULLET_DELAY 3 //This slows the bullets down so that are more visible on the LCD screen
  
  private:
    bool _active; //Bullet is active while it is within game field
    bool _half;   //Used to half the speed of the bullet due to limitations of LCD
    int _delay;
    
  public:
    Bullet():GameObject(), _active(false), _half(false), _delay(0) {}

    void setActive(bool active)
    {
      _active = active;
    }
  
    bool active()
    {
      return _active;
    }
    
    // Moving bullet. Returns true if successful
    bool move()
    {
      _delay++;
      if (_delay == BULLET_DELAY)
      {
        _delay = 0;
        _y+=_speed;//for bullets speed is always vertical
        if (_y<0||_y>=HEIGHT) //if bullet leaves the field
        {
          if (_y<0)
          {
            playMissTone();
          }
          _y-=_speed;
          _active = false;
          return false;
        }
        else
        {
          return true;
        }
      }
      return true;
    }
} shipBullet, alienBullets[ALIENS_NUM]; //bullet objects for ship and aliens

//-------------------------------------------------------------------------
// Ship class
class Ship: public GameObject
{
  public:
    //Moving right. Returns true if successfull
    bool moveRight()
    {
      _x++;
      if (_x>=WIDTH)
      {
        _x--;
        return false;
      }
      else 
      {
        return true;
      }
    }
    
    //Moving left. Returns true if successfull
    bool moveLeft()
    {
      _x--;
      if (_x<0)
      {
        _x++;
        return false;
      }
      else 
      {
        return true;
      }
    }
} ship;

//-------------------------------------------------------------------------
// Alien class
class Alien: public GameObject
{
  private:
    bool _alive;//shows wether alien is alive
    bool _state;//alien's current state for animation purpose
    
  public:
    Alien():GameObject(), _alive(false), _state(false){}

    void setAlive(bool alive)
    {
      _alive = alive;
    }

    bool alive()
    {
      return _alive;
    }

    void setState(bool state)
    {
      _state = state;
    }

    bool state()
    {
      return _state;
    }
    
    //Moving alien. Returns true if successfull
    bool move()
    {
      _x+=_speed;
      _state = !_state;
      if (_x<0||_x>=WIDTH)
      {
        _x-=_speed;
        return false;
      }
      else 
      {
        return true;
      }
    }
} aliens[8];

//-------------------------------------------------------------------------
// Update LCD screen
// First drawing in a character buffer, then print it to the screen to avoid flickering.
// Note: we have to draw ship separately since it has char code 0 and lcd.print() processes it like EOL*/
void updateScreen()
{
  bool shipDisplayed = false; //shows whether ship have been displayed with SHIP_BULLET sprite
  
  //Clearing the buffer
  for (byte i = 0; i < HEIGHT/2; i++)
  {
    for (byte j = 0; j < WIDTH; j++)
    {
      screenBuffer[i][j] = ' ';
    }
    screenBuffer[i][WIDTH] = '\0';
  }
  
  //Drawing ship's bullet
  if (shipBullet.active())
  {
    if ((ship.x()==shipBullet.x()) && (shipBullet.y()==2))
    {
      screenBuffer[shipBullet.y()/2][shipBullet.x()] = SHIP_BULLET;
      shipDisplayed = true;
    }
    else
    {
      screenBuffer[shipBullet.y()/2][shipBullet.x()] = shipBullet.y()%2 ? BULLET_DOWN : BULLET_UP;
    }
  }
  
  //Drawing aliens
  for (byte i = 0; i<ALIENS_NUM; i++)
  {
    if(aliens[i].alive())
    {
      screenBuffer[aliens[i].y()/2][aliens[i].x()] = aliens[i].state() ? ALIEN1 : ALIEN2;
    }
  }
  
  //Drawing aliens and bullets
  bool bulletDisplayed = false;
  for (byte i = 0; i < ALIENS_NUM; i++)
  {
    if(alienBullets[i].active())
    {
      bulletDisplayed = false;
      for (int j = 0; j < ALIENS_NUM; j++)
      {
        if ((aliens[j].x()==alienBullets[i].x()) && (alienBullets[i].y()==1) && (aliens[i].alive()))
        {
          screenBuffer[alienBullets[i].y()/2][alienBullets[i].x()] = aliens[i].state() ? ALIEN1BULLET : ALIEN2BULLET;
          bulletDisplayed = true;
        }
      }
      if (!bulletDisplayed)
      {
        if ((ship.x()==alienBullets[i].x()) && (alienBullets[i].y()==2))
        {
          screenBuffer[alienBullets[i].y()/2][alienBullets[i].x()] = SHIP_BULLET;
          shipDisplayed = true;
        }
        else
        {
          screenBuffer[alienBullets[i].y()/2][alienBullets[i].x()] = alienBullets[i].y()%2 ? BULLET_DOWN : BULLET_UP;
        }
      }
    }
  }
  
  //Sending the buffer to the screen
  for (byte i = 0; i < HEIGHT/2; i++)
  {
    lcd.setCursor(0,i);
    lcd.print(screenBuffer[i]);
  }
  
  //After all, displaying the ship
  if (!shipDisplayed)
  {
    lcd.setCursor(ship.x(), ship.y()/2);
    lcd.write(byte(SHIP));
  }
}

//-------------------------------------------------------------------------
// Reset all the objects before easch level
void initLevel(byte l)
{
  level = l;
  if (level>42)//Easter egg: 42 is the ultimate level :)
  {
    level = 42;
  }
  
  //Reset ship object
  ship.setX(WIDTH/2);
  ship.setY(3);
  shipBullet.setX(WIDTH/2);
  shipBullet.setY(3);
  shipBullet.setActive(false);
  
  //Reset aliens objects
  for (byte i = 0; i<ALIENS_NUM; i++)
  {
     aliens[i].setX(i);
     aliens[i].setY(0);
     aliens[i].setSpeed(1);
     aliens[i].setAlive(true);
     aliens[i].setState(false);
     alienBullets[i].setActive(false);
  }
  
  //Reset the rest of the game variables
  animationStep = 0;
  alienStep = 6-level/2;//alien's speed depends on a level
  if (alienStep < 1)
  {
    alienStep = 1;
  }
  fireProbability = 110-level*10; //alien's shoot probability depends on a level
  if (fireProbability < 10)
  {
    fireProbability = 10;
  }
  aliensLeft = ALIENS_NUM;
  
  //Displaying a number of level
  lcd.clear();
  lcd.print("Level ");
  lcd.setCursor(8,0);
  lcd.print(level);
  delay(1000);
  lcd.clear();
}

//-------------------------------------------------------------------------
// Initialise Hardware for space invaders
void invaderSetup()
{
  //Define custom characters
  lcd.createChar(SHIP, ship_sprite);
  lcd.createChar(BULLET_UP, bullet_up_sprite);
  lcd.createChar(BULLET_DOWN, bullet_down_sprite);
  lcd.createChar(SHIP_BULLET, ship_bullet_sprite);
  lcd.createChar(ALIEN1, alien1_1_sprite);
  lcd.createChar(ALIEN2, alien1_2_sprite);
  lcd.createChar(ALIEN1BULLET, alien1_1_bullet_sprite);
  lcd.createChar(ALIEN2BULLET, alien1_2_bullet_sprite);

  invaderInitialise();
}

//-------------------------------------------------------------------------
// Initialise Hardware for space invaders
void invaderInitialise()
{
  score = 0;
  initLevel(1);
}

//-------------------------------------------------------------------------
// Release Hardware used for space invaders
void invaderShutDown()
{
}

//-------------------------------------------------------------------------
// Main program loop
bool invaderLoop()
{
  //Processing the buttons
  if (rightButton->State() == HIGH)
  {
    ship.moveRight();
  }
  else if (leftButton->State() == HIGH)
  {
    ship.moveLeft();
  }
  else if (downButton->State() == HIGH)
  {
     //Game pause
    while (downButton->State() == HIGH);
    while (downButton->State() == LOW);
    while (downButton->State() == HIGH);
  }
  else if (fireButton->State() == LOW && !shipBullet.active())
  {
    shipBullet.setX(ship.x());
    shipBullet.setY(ship.y());
    shipBullet.setSpeed(-1);
    shipBullet.setActive(true);
  }
  
  //Moving all the objects
  if(shipBullet.active()) //Moving the ship bullet
  {
    shipBullet.move();
  }
  
  //Moving the aliens and their bullets
  for (byte i = 0; i<ALIENS_NUM; i++)
  {
    if (alienBullets[i].active())
    {
      alienBullets[i].move();
      if (alienBullets[i].collides(ship)) //Ship destruction
      {
        invaderGameOver();
        return true;
      }
    }
    if (!(animationStep % alienStep))
    {
      aliens[i].move();
    }
    if (aliens[i].collides(shipBullet) && shipBullet.active() && aliens[i].alive()) //Alien dies
    {
      aliens[i].setAlive(false);
      score += 10*level;
      aliensLeft--;
      playHitTone();
      shipBullet.setActive(false);
    }
    if (!random(fireProbability) && !alienBullets[i].active() && aliens[i].alive()) //Alien shoots
    {
      alienBullets[i].setX(aliens[i].x());
      alienBullets[i].setY(aliens[i].y()+1);
      alienBullets[i].setSpeed(1);
      alienBullets[i].setActive(true);
    }
  }
  if ( !(animationStep % alienStep) && (aliens[0].x()==0 || aliens[ALIENS_NUM-1].x() == WIDTH-1)) //Changing the aliens'move direction 
  {
    for (byte i = 0; i<ALIENS_NUM; i++)
    {
      aliens[i].setSpeed(-aliens[i].speed());
    }
  }
  
  //Refresh screen
  updateScreen();
  animationStep++;
  delay (GAME_STEP);
  
  //If no aliens left, starting next level
  if (!aliensLeft)
  {
    initLevel(level+1);
  }
  return false;
}
//--------------------------------------------------------------------
// Display Opening animation, instructions and "Press button to start" message
void displayInvaderInitialScreens()
{
  lcd.clear();
  for (uint8_t y=0; y<2; y++) 
  {
    for (uint8_t x=0; x<16; x++) 
    {
      lcd.setCursor(x, y);
      lcd.print(char(ALIEN1 + (x & 1)));
      delay(100);
      lcd.setCursor(x, y);
      if (y==0) 
      {
        lcd.print("    SPACE       "[x]);
      } 
      else 
      {
        lcd.print("      INVADERS  "[x]);
      }
    }
  }
  delay(1000);
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("  PRESS BUTTON  ");
  lcd.setCursor(0,1);
  lcd.print("    TO START    ");
}

//---------------------------------------------------------------
//Handle game over
void invaderGameOver()
{
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
      lcd.print(char(ALIEN1 + (x & 1)));
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
  lcd.print(EepromData.invaderHighScore);
  if (score > EepromData.invaderHighScore)
  {
    EepromData.invaderHighScore = score;
    writeEepromData();
    playWinSound();
  }
  delay(1000);
  for (uint8_t y=0; y<2; y++) 
  {
    for (uint8_t x=0; x<16; x++) 
    {
      lcd.setCursor(x, y);
      lcd.print(char(ALIEN1 + (x & 1)));
      delay(100);
      lcd.setCursor(x, y);
      lcd.print(" ");
    }
  }
  delay(1000);
}
