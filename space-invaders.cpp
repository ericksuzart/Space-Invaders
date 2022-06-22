// #include <Arduino.h>
// #include <LiquidCrystal.h>
#include <LiquidCrystal.h>

const int rs = 13, enable = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8;
LiquidCrystal lcd(rs, enable, d4, d5, d6, d7);

// the pins used by the LCD panel
// LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// define button constants
// #define btnUP     1
// #define btnDOWN   2
#define btnLEFT   3
#define btnRIGHT  4
#define btnFIRE 5
#define btnNONE   6

//game field size
#define WIDTH 16
#define HEIGHT 4  // subdivide the 2 rows display into 4 rows for more detailed control

// custom models for the game (assets)
// max number of custom chars (models) is 8
#define SHIP 0
#define BULLET_UP 1
#define BULLET_DOWN 2
#define SHIP_BULLET 3
#define ALIEN1 4
#define ALIEN2 5
#define ALIEN1BULLET 6
#define ALIEN2BULLET 7

#define GAME_STEP 100  // Delay (ms) between game steps
#define ALIENS_NUM 8  // Number of aliens

byte animationStep;  // Number of game step
char screenBuffer[HEIGHT / 2][WIDTH + 1];  // Characters to be displayed on the screen at certain time
byte alienStep = 5;  // The number of game steps between alien's movements
byte fireProbability = 20; // Probability of alien to shoot
int score = 0;  // Player's score
byte level = 1;  // Game level
byte aliensLeft = 0;  // Number of aliens left on current level

// change this to make the song slower or faster
int tempo = 225;

// change this to whichever pin you want to use
int buzzer = 2;

// Define custom characters for game sprites
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


// Base class for game objects
class GameObject {
 // coordinates and speed
 protected:
  int8_t _x;
  int8_t _y;
  int8_t _speed;

 public:
  GameObject():_x(0),_y(0),_speed(0){}
  GameObject(int8_t x, int8_t y): _x(x), _y(y), _speed(0){}
  GameObject(int8_t x, int8_t y, int8_t speed): _x(x), _y(y), _speed(speed){}

 //Getters and setters
  int8_t x() const { return _x; }
  int8_t y() const { return _y; }
  int8_t speed() const { return _speed; }

  bool setX(int8_t x)
  {
    if ( (x < 0) || (x > WIDTH) )
      return false;

    _x = x;
    return true;
  }

  bool setY(int8_t y)
  {
    if ( (y < 0) || (y > HEIGHT) )
      return false;

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
    return ( (_x == o.x()) && (_y == o.y()) ) ? true : false;
  }
};


// Bullet class, inherited from GameObject
class Bullet:public GameObject
{
 private:
  bool _active; //Bullet is active while it is within game field

 public:
  Bullet():GameObject(), _active(false){}

  void setActive(bool active) { _active = active; }

  bool active() { return _active; }

  //  Move bullet
  bool move(){
    _y += _speed; // for bullets, speed is always vertical

    if ( (_y < 0) || (_y >= HEIGHT) ) // if bullet leaves the field of play, it is inactive
    {
      _y -= _speed; // return the bullet to its original position
      _active = false;
      return false;
    }

    return true;
  }
} shipBullet, alienBullets[ALIENS_NUM]; // bullet objects for ship and aliens


// Ship class (player), inherited from GameObject
class Ship:public GameObject
{
 public:
  bool moveRight()
  {
    if (++_x >= WIDTH)
    {
      _x--; //  return the ship to its original position
      return false;
    }

    return true;
  }


  bool moveLeft()
  {
    if (--_x < 0)
    {
      _x++; //  return the ship to its original position
      return false;
    }

    return true;
  }
} ship; // ship object

// Alien class (enemy), inherited from GameObject
class Alien: public GameObject
{
 private:
  bool _alive; //  shows wether alien is alive
  bool _state; //  alien's current state for animation purpose (flip between two sprites)

 public:
  Alien():GameObject(), _alive(false), _state(false){}

  void setAlive(bool alive) { _alive = alive; }

  bool alive() { return _alive; }

  void setState(bool state) { _state = state; }

  bool state() { return _state; }

  bool move()
  {
    _x += _speed;
    _state = !_state; //  flip between two sprites

    if ( (_x < 0) || (_x >= WIDTH))
    {
      _x -= _speed; //  return the alien to its original position
      return false;
    }
    return true;
  }

} aliens[8]; //  array of aliens objects

// Processing buttons state
byte buttonPressed()
{
  int adc_key_in = analogRead(A0);      // read analogue value
  Serial.println(adc_key_in);
  if (adc_key_in <= 20)  return btnLEFT;
  if (adc_key_in <= 100)  return btnRIGHT;
  if (adc_key_in <= 200)  return btnFIRE;

  return btnNONE;
}

/**
 * @brief Update LCD screen First drawing in a character buffer, then print it
 * to the screen to avoid flickering.
 *
 * Note: we have to draw ship separately since it has char code 0 and
 * lcd.print() processes it like EOL (end of line)
 */
void updateDisplay()
{
  bool shipDisplayed = false; //shows whether ship have been displayed with SHIP_BULLET sprite

  // Clearing the buffer
  for (byte i = 0; i < HEIGHT / 2; i++)
  {
    for (byte j = 0; j < WIDTH; j++)
      screenBuffer[i][j] = ' ';

    screenBuffer[i][WIDTH] = '\0'; //  end of line
  }

  // Drawing ship's bullet
  if ( shipBullet.active() )
  {
    if ( (ship.x() == shipBullet.x()) && (shipBullet.y() == 2))
    {
      screenBuffer[shipBullet.y() / 2][shipBullet.x()] = SHIP_BULLET; // asset with ship and bullet
      shipDisplayed = true;
    }
    else
      // if bullet is not on ship, draw it as normal bullet
      screenBuffer[shipBullet.y()/2][shipBullet.x()] = (shipBullet.y() % 2)? BULLET_DOWN : BULLET_UP;
  }

  // Drawing aliens
  for (byte i = 0; i < ALIENS_NUM; i++)
  {
    // if alien is alive, draw it
    if ( aliens[i].alive() )
      // in function of alien's state, draw it with different asset
      screenBuffer[aliens[i].y() / 2][aliens[i].x()] = (aliens[i].state())? ALIEN1 : ALIEN2;
  }

  bool bulletDisplayed = false;

  // Drawing aliens and bullets
  for (byte i = 0; i < ALIENS_NUM; i++)
  {
    if( alienBullets[i].active() )
    {
      bulletDisplayed = false;
      for (int j = 0; j < ALIENS_NUM; j++)
      {
        if ( (aliens[j].x() == alienBullets[i].x()) && (alienBullets[i].y() == 1) && (aliens[i].alive()) )
        {
          // if bullet is on alien, draw it with correspondent asset
          screenBuffer[alienBullets[i].y()/2][alienBullets[i].x()] = (aliens[i].state()) ? ALIEN1BULLET : ALIEN2BULLET;
          bulletDisplayed = true;
        }
      }
      if (!bulletDisplayed)
      {
        if ( (ship.x() == alienBullets[i].x()) && (alienBullets[i].y() == 2) )
        {
          // bullet is on ship, draw it with correspondent asset
          screenBuffer[alienBullets[i].y()/2][alienBullets[i].x()] = SHIP_BULLET;
          shipDisplayed = true;
        }
        else
          // bullet is not on ship, draw it as normal bullet
          screenBuffer[alienBullets[i].y()/2][alienBullets[i].x()] = (alienBullets[i].y() % 2) ? BULLET_DOWN : BULLET_UP;
      }
    }
  }

  //Sending the buffer to the display
  for (byte i = 0; i < HEIGHT/2; i++)
  {
    lcd.setCursor(0,i);
    lcd.print(screenBuffer[i]);
  }

  //After all, displaying the ship if it is not displayed yet
  if (!shipDisplayed)
  {
    lcd.setCursor(ship.x(), ship.y()/2);
    lcd.write(byte(SHIP));
  }
}

// Reset all the objects before each level
void initLevel(byte l)
{
  level = l;

  if (level>42) // Easter egg: 42 is the ultimate level
    level = 42;

  // Reset ship object
  ship.setX(WIDTH/2);
  ship.setY(3);
  shipBullet.setX(WIDTH/2);
  shipBullet.setY(3);
  shipBullet.setActive(false);

  // Reset aliens objects
  for (byte i = 0; i < ALIENS_NUM; i++)
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
  alienStep = 6 - level/2; // alien's speed depends on a level

  if (alienStep < 1)
    alienStep = 1;

  fireProbability = 110 - level*10; // alien's shoot probability depends on a level

  if (fireProbability < 10)
    fireProbability = 10;

  aliensLeft = ALIENS_NUM;

  // Displaying the level number
  lcd.clear();
  lcd.print("Level ");
  lcd.setCursor(8,0);
  lcd.print(level);
  delay(1000);
  lcd.clear();
}

// Displaying the final score
void gameOver()
{
  // TODO: add gameover sound
  lcd.setCursor(0,0);
  lcd.print("G a m e  o v e r");
  lcd.setCursor(0,1);
  lcd.print("Score:  ");
  lcd.setCursor(8,1);
  lcd.print(score);
  while(1);
}

// Init: starting LCD and create the custom characters (assets)
void setup()
{
  lcd.begin(16, 2);
  lcd.createChar(SHIP, ship_sprite);
  lcd.createChar(BULLET_UP, bullet_up_sprite);
  lcd.createChar(BULLET_DOWN, bullet_down_sprite);
  lcd.createChar(SHIP_BULLET, ship_bullet_sprite);
  lcd.createChar(ALIEN1, alien1_1_sprite);
  lcd.createChar(ALIEN2, alien1_2_sprite);
  lcd.createChar(ALIEN1BULLET, alien1_1_bullet_sprite);
  lcd.createChar(ALIEN2BULLET, alien1_2_bullet_sprite);
  score = 0;
  randomSeed(analogRead(1));
  initLevel (1);
}

// Game main loop
void loop()
{
  // Process buttons
  switch(buttonPressed())
  {
    case btnRIGHT: {
      ship.moveRight();
      Serial.println("Right");
      break;
    }
    case btnLEFT:{
      ship.moveLeft();
      Serial.println("Left");
      break;
    }
    // Shoot
    case btnFIRE:{
      Serial.println("FIRE");
      if(!shipBullet.active()){
        // TODO: add shoot sound
        shipBullet.setX(ship.x());
        shipBullet.setY(ship.y());
        shipBullet.setSpeed(-1);
        shipBullet.setActive(true);
      }
      break;
    }
    default:
      Serial.println("NONE");
      break;
  }

  // Move all the objects
  if(shipBullet.active()) //Moving the ship bullet
    shipBullet.move();

  // Move the aliens and their bullets
  for (byte i = 0; i < ALIENS_NUM; i++)
  {
    if ( alienBullets[i].active() )
    {
      alienBullets[i].move();

      if (alienBullets[i].collides(ship)) //Ship destruction
        // TODO: add explosion sound
        gameOver();
    }

    // Aliens don't move if the steps didn't match
    if (!( animationStep % alienStep ))
      aliens[i].move();

    // Alien dies
    if ( (aliens[i].collides(shipBullet)) && (shipBullet.active()) && (aliens[i].alive()) )
    {
      // TODO: add alien death sound
      aliens[i].setAlive(false);
      score += 10 * level;
      aliensLeft--;
    }

    // Alien shoots
    if ( (!random(fireProbability)) && (!alienBullets[i].active()) && (aliens[i].alive()))
    {
      alienBullets[i].setX(aliens[i].x());
      alienBullets[i].setY(aliens[i].y() + 1);
      alienBullets[i].setSpeed(1);
      alienBullets[i].setActive(true);
    }
  }

  // Change the aliens'move direction if they are going out of the screen
  if ( (!(animationStep % alienStep)) && ( (aliens[0].x() == 0) || (aliens[ALIENS_NUM - 1].x() == WIDTH - 1) ) )
    for (byte i = 0; i < ALIENS_NUM; i++)
      aliens[i].setSpeed( -aliens[i].speed() );

  // Refresh screen
  updateDisplay();
  animationStep++;
  delay (GAME_STEP);

  // If no aliens left, start next level
  if(!aliensLeft)
    initLevel( level + 1 );
}
