/**
 * @file Space-InvadersI2C.cpp
 * @author Erick Suzart Souza (ericksuzart@gmail.com)
 * @brief 
 * @version 0.1
 * @date 25-06-2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <LiquidCrystal.h>

// LiquidCrystal lcd(rs, enable, d0, d1, d2, d3, d4, d5, d6, d7)
LiquidCrystal lcd(13, 12, 11, 10, 9, 8, 7, 6, 5, 4);

// define button constants
#define btnNONE   0
#define btnLEFT   1
#define btnRIGHT  2
volatile int adc_key_in = 0; // initialize to no button pressed

#define WIDTH   16
#define HEIGHT   4

// aliens sprite codes
#define SHIP          0
#define BULLET_UP     1
#define BULLET_DOWN   2
#define SHIP_BULLET   3
#define ALIEN1        4
#define ALIEN2        5
#define ALIEN1BULLET  6
#define ALIEN2BULLET  7

#define GAME_STEP   250 // Delay (ms) between game steps
#define ALIENS_NUM    8 // Number of aliens

byte animationStep; // Number of game step

char screenBuffer[HEIGHT / 2][WIDTH + 1]; // Characters to be displayed on the screen

byte alienStep       = 5;  // The number of game steps between alien's movements
int score            = 0;  // Player's score
byte level           = 1;  // Game level
byte aliensLeft      = 0;  // Number of aliens left on current level
byte fireProbability = 20; // Probability of alien to shoot

// Define custom characters for game sprites
namespace sprite
{
  static byte ship[] = {
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00100,
    B01110,
    B11011
  };

  static byte ship_bullet[] = {
    B00000,
    B00100,
    B00100,
    B00000,
    B00000,
    B00100,
    B01110,
    B11011
  };

  static byte bullet_down[] = {
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00100,
    B00100,
    B00000
  };

  static byte bullet_up[] = {
    B00000,
    B00100,
    B00100,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000
  };

  static byte alien1_1[] = {
    B01010,
    B10101,
    B01110,
    B10001,
    B00000,
    B00000,
    B00000,
    B00000
  };

  static byte alien1_2[] = {
    B01010,
    B10101,
    B01110,
    B01010,
    B00000,
    B00000,
    B00000,
    B00000
  };

  static byte alien1_1_bullet[] = {
    B01010,
    B10101,
    B01110,
    B10001,
    B00000,
    B00100,
    B00100,
    B00000
  };

  static byte alien1_2_bullet[] = {
    B01010,
    B10101,
    B01110,
    B01010,
    B00000,
    B00100,
    B00100,
    B00000
  };
}

/**
 * @brief Base class for game objects
 * 
 */
class GameObject
{
protected:
  // Object's coordinates and speed
  int _x; // X coordinate
  int _y; // Y coordinate
  int _speed; // Object's speed

public:
  // Constructors
  GameObject(): _x(0), _y(0), _speed(0) { } // default constructor
  GameObject(int x, int y): _x(x), _y(y), _speed(0) { } // constructor with coordinates
  GameObject(int x, int y, int speed): _x(x), _y(y), _speed(speed) { } // constructor with coordinates and speed


  /**
   * @brief Get object's X coordinate
   * 
   * @return int 
   */
  int x() const { return _x; }


  /**
   * @brief Get object's Y coordinate
   * 
   * @return int 
   */
  int y() const { return _y; }


  /**
   * @brief Get object's speed
   * 
   * @return int 
   */
  int speed() const { return _speed; }


  /**
   * @brief Set object's X coordinate
   * 
   * @param x 
   * @return true if X coordinate is valid
   * @return false otherwise
   */
  bool setX(int x)
  {
    if ( (x < 0) || (x > WIDTH) )
      return false;

    _x = x;
    return true;
  }


  /**
   * @brief Set object's Y coordinate
   * 
   * @param y 
   * @return true if Y coordinate is valid
   * @return false otherwise
   */
  bool setY(int y)
  {
    if ( (y < 0) || (y > HEIGHT) )
      return false;

    _y = y;
    return true;
  }


  /**
   * @brief Set object's speed
   * 
   * @param speed 
   * @return true if speed is valid
   * @return false otherwise
   */
  bool setSpeed(int speed)
  {
    _speed = speed;
    return true;
  }
} GameObject;

/**
 * @brief Class for game bullets, inherited from GameObject
 * 
 */
class Bullet: public GameObject
{
private:
  bool _active; // Bullet is active while it is within game field

public:
  // Constructor
  Bullet(): GameObject(), _active(false) { }


  /**
   * @brief Set bullet active/inactive
   * 
   * @param active 
   */
  void setActive(bool active) { _active = active; } 


  /**
   * @brief Get bullet state (active/inactive)
   * 
   * @return true if it is active
   * @return false 
   */
  bool active() const { return _active; }


  /**
   * @brief Move bullet one step up
   * 
   * @return true if bullet is still within game field
   * @return false if bullet is out of game field
   */
  bool move()
  {
    _y += _speed; // for bullets speed is always vertical

    if ( (_y < 0) || (_y >= HEIGHT) ) // if bullet leaves the field
    {
      _y -= _speed;
      _active = false;
      return false;
    }
    else return true;
  }

} shipBullet, alienBullets[ALIENS_NUM]; // bullet objects for ship and aliens


/**
 * @brief Ship class (player), inherited from GameObject
 * 
 */
class Ship: public GameObject
{
public:
  /**
   * @brief Move ship one step right
   * 
   * @return true if it moved successfully
   * @return false if it's not possible to move right (out of game field)
   */
  bool moveRight()
  {
    if (++_x >= WIDTH) // if ship will leave the game field
    {
      _x--;
      return false;
    }
    else return true;
  }


  /**
   * @brief Move ship one step left
   * 
   * @return true if it moved successfully
   * @return false if it's not possible to move left (out of game field)
   */
  bool moveLeft()
  {
    if (--_x<0)
    {
      _x++;
      return false;
    }
    else return true;
  }

} ship;


/**
 * @brief Alien class, inherited from GameObject and has its own bullets (enemy)
 * 
 */
class Alien: public GameObject
{
private:
  bool _alive; // shows wether alien is alive
  bool _state; // alien's current state for animation purpose (1 or 2)

public:
  // Constructor
  Alien(): GameObject(), _alive(false), _state(false) { }


  /**
   * @brief Set alien alive/dead
   * 
   * @param alive 
   */
  void setAlive(bool alive) { _alive = alive; }


  /**
   * @brief Get alien state (alive/dead)
   * 
   * @return true if it is alive
   */
  bool alive(){ return _alive; }


  /**
   * @brief Set alien state for animation purpose (different model)
   * 
   * @param state 
   */
  void setState(bool state){ _state = state; }


  /**
   * @brief Get alien state for animation purpose (different model)
   * 
   * @return true state default (closed wings)
   * @return false second state (open wings)
   */
  bool state(){ return _state; }


  /**
   * @brief Move alien one step to side in function of its speed
   *
   * @return true if it moved successfully (still within game field)
   * @return false 
   */
  bool move()
  {
    _x += _speed; // increment position
    _state = !_state;

    if ( (_x < 0) || (_x >= WIDTH) ) // out of boundries
    {
      _x -= _speed;
      return false;
    }
    else return true;
  }

} aliens[ALIENS_NUM];


/**
 * @brief Check what key is pressed and return it
 * 
 * @return byte 
 */
byte buttonPressed()
{
  adc_key_in = analogRead(A0);      // read analogue value

  if (adc_key_in < 10)  return btnLEFT;
  if (adc_key_in < 350)  return btnRIGHT;

  return btnNONE;
}


/**
 * @brief Clear the display buffer
 * 
 */
void clearBuffer()
{
  for (byte i = 0; i < HEIGHT / 2; i++)
  {
    for (byte j = 0; j < WIDTH; j++)
      screenBuffer[i][j] = ' '; // neutral character

    screenBuffer[i][WIDTH] = '\0'; // null terminate (jump to next line)
  }
}


/**
 * @brief Draw the ship bullet on the screenBuffer
 * 
 */
void drawShipBullet(bool& shipDisplayed)
{
  if ( shipBullet.active() )
  {
    if ( (ship.x() == shipBullet.x()) && (shipBullet.y() == 2) )
    {
      // if bullet is on the ship's position draw it with ship
      screenBuffer[shipBullet.y() / 2][shipBullet.x()] = SHIP_BULLET;
      shipDisplayed = true;
    }
    else
      // Draw bullet on the screenBuffer (without ship)
      screenBuffer[shipBullet.y() / 2][shipBullet.x()] = (shipBullet.y() % 2) ? BULLET_DOWN : BULLET_UP;
  }
}


/**
 * @brief Draw alien on screeBuffer if it is alive
 * 
 */
void drawAliens()
{
  for (byte i = 0; i < ALIENS_NUM; i++) // for each alien
    // if alien is alive draw it on the screenBuffer with correct model in
    // function of its state
    if(aliens[i].alive())
      screenBuffer[aliens[i].y() / 2][aliens[i].x()] = (aliens[i].state())? ALIEN1 : ALIEN2;
}


/**
 * @brief Draw the bullets of aliens on the screenBuffer
 * 
 */
void drawAlienBullets(bool& bulletDisplayed, bool& shipDisplayed)
{
  for (byte i = 0; i < ALIENS_NUM; i++) // for each bullet
  {
    // check if bullet is active
    if( alienBullets[i].active() )
    {
      bulletDisplayed = false;
      
      // if is active check if it is on the aliens position
      for (int j = 0; j < ALIENS_NUM; j++)
      {
        if ( (aliens[j].x() == alienBullets[i].x()) &&
             (alienBullets[i].y() == 1) &&
             (aliens[i].alive()))
        {
          screenBuffer[alienBullets[i].y()/2][alienBullets[i].x()] = (aliens[i].state()) ? ALIEN1BULLET : ALIEN2BULLET;
          bulletDisplayed = true;
        }
      }

      // if bullet is not on the aliens position draw it on the screenBuffer
      // (without alien model)
      if (!bulletDisplayed)
      {
        // if bullet is on the ship's position draw it with ship
        if ((ship.x() == alienBullets[i].x()) && (alienBullets[i].y() == 2))
        {
          screenBuffer[alienBullets[i].y()/2][alienBullets[i].x()] = SHIP_BULLET;
          shipDisplayed = true;
        }
        else
          // otherwise draw bullet on the screenBuffer (without ship model)
          screenBuffer[alienBullets[i].y()/2][alienBullets[i].x()] = (alienBullets[i].y()%2) ? BULLET_DOWN : BULLET_UP;
      }
    }
  }
}


/**
 * @brief Update LCD screen First drawing in a character buffer, then print it
 * to the screen to avoid flickering. 
 * 
 * Note: we have to draw ship separately since it has char code 0 and
 * lcd.print() processes it like EOL
 *
 */
void updateScreen(){
  bool shipDisplayed = false; //shows whether ship have been displayed with SHIP_BULLET sprite
  bool bulletDisplayed = false;

  clearBuffer();

  //Drawing ship's bullet
  drawShipBullet(shipDisplayed);

  //Drawing aliens
  drawAliens();

  //Drawing aliens and bullets
  drawAlienBullets(bulletDisplayed, shipDisplayed);

  //Sending the buffer to the screen
  for (byte i = 0; i < HEIGHT/2; i++)
  {
    lcd.setCursor(0,i);
    lcd.print(screenBuffer[i]);
  }

  //After all, displaying the ship if not displayed yet
  if (!shipDisplayed)
  {
    lcd.setCursor(ship.x(), ship.y()/2);
    lcd.write(byte(SHIP));
  }
}


/**
 * @brief Reset and configure all the objects before each level
 * 
 * @param l number of the level
 */
void initLevel(byte l)
{
  level = l;

  if (level>42)//Easter egg: 42 is the ultimate level :)
    level = 42;

  //Reset ship object
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

  // Reset the rest of game variables
  animationStep = 0;
  alienStep = 6 - level/2; // alien's speed depends on a level

  if (alienStep < 1)
    alienStep = 1;

  fireProbability = 110 - 10 * level; // alien's shoot probability depends on a level

  if (fireProbability < 10)
    fireProbability = 10;

  aliensLeft = ALIENS_NUM;

  //Displaying a number of level
  lcd.clear();
  lcd.print("Level ");
  lcd.setCursor(8,0);
  lcd.print(level);
  delay(1000);
  lcd.clear();
}


/**
 * @brief Game over screen, displaying the final score
 * 
 */
void gameOver()
{
  lcd.setCursor(0,0);
  lcd.print("G a m e  o v e r");
  lcd.setCursor(0,1);
  lcd.print("Score:  ");
  lcd.setCursor(8,1);
  lcd.print(score);
  while(1); // Game over, wait for reset
}


/**
 * @brief Start LCD and create custom characters
 * 
 */
void setup()
{
  lcd.begin(16, 2);
  lcd.createChar(SHIP, sprite::ship);
  lcd.createChar(BULLET_UP, sprite::bullet_up);
  lcd.createChar(BULLET_DOWN, sprite::bullet_down);
  lcd.createChar(SHIP_BULLET, sprite::ship_bullet);
  lcd.createChar(ALIEN1, sprite::alien1_1);
  lcd.createChar(ALIEN2, sprite::alien1_2);
  lcd.createChar(ALIEN1BULLET, sprite::alien1_1_bullet);
  lcd.createChar(ALIEN2BULLET, sprite::alien1_2_bullet);
  score = 0;
  randomSeed(analogRead(1));
  initLevel (1);
}

/**
 * @brief Check if colision happens
 * 
 * @param b alien bullet object
 * @param s ship object
 * @return true if it happens
 */
bool colide(const Bullet& b, const Ship& s)
{
  return ( (b.x() == s.x()) && (b.y() == s.y()) )? true : false;
}

/**
 * @brief Check if colision happens
 * 
 * @param b ship bullet object
 * @param a alien object
 * @return true if it happens
 */
bool colide(const Bullet& b, const Alien& a)
{
  return ( (b.x() == a.x()) && (b.y() == a.y()) )? true : false;
}


/**
 * @brief Move all the objects and check for collisions
 * 
 */
void objects_Interactions()
{
  // Move the ship
  switch(buttonPressed())
  {
    case btnRIGHT: { ship.moveRight(); break; }
    case btnLEFT: { ship.moveLeft(); break; }
    default: break;
  }

  if(shipBullet.active()) // Move ship bullet
    shipBullet.move();

  for (byte i = 0; i < ALIENS_NUM; i++) // Moving the aliens and their bullets
  {
    if ( alienBullets[i].active() )
    {
      alienBullets[i].move();

      if ( colide(alienBullets[i], ship) ) //Ship destruction
        gameOver();
    }

    if (!(animationStep % alienStep)) // move alien only if it is on pace
      aliens[i].move();

    if ((colide(shipBullet, aliens[i])) &&
        (shipBullet.active()) &&
        (aliens[i].alive()))
    {
      aliens[i].setAlive(false);
      score += 10 * level;
      aliensLeft--; // Alien dies
    }

    if ( (!random(fireProbability))   &&
         (!alienBullets[i].active())  &&
         (aliens[i].alive()) )
    { // Alien shoots
      alienBullets[i].setX(aliens[i].x());
      alienBullets[i].setY(aliens[i].y()+1);
      alienBullets[i].setSpeed(1);
      alienBullets[i].setActive(true);
    }
  }

  // Changing the aliens'move direction
  if ( (!(animationStep % alienStep)) &&
       ((aliens[0].x() == 0) || (aliens[ALIENS_NUM - 1].x() == WIDTH - 1)) )
  {
    for (byte i = 0; i < ALIENS_NUM; i++)
      aliens[i].setSpeed(- aliens[i].speed());
  }
}


/*Game loop*/
void loop()
{
  // shoot
  if(!shipBullet.active())
  {
    shipBullet.setX(ship.x());
    shipBullet.setY(ship.y());
    shipBullet.setSpeed(-1);
    shipBullet.setActive(true);
  }

  objects_Interactions();

  //Refresh screen
  updateScreen();
  animationStep++;
  delay (GAME_STEP);

  //If no aliens left, starting next level
  if(!aliensLeft)
    initLevel(level+1);
}
