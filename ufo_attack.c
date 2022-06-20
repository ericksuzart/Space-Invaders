/*UFO ATTACK! - Written by Anthony Clarke. */
/*Some specific lines of code were lifted from open source example programs. These are labled in comments.*/

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

const int pinCS = 10; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
const int numberOfHorizontalDisplays = 4;
const int numberOfVerticalDisplays = 1;

Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

// 'ship', 8x8px
const unsigned char ship [] PROGMEM = {
  0x80, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'UFO', 8x8px
const unsigned char ufo [] PROGMEM = {
  0x40, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'explosion', 8x8px
const unsigned char explosion [] PROGMEM = {
  0xe0, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'Bouncer', 8x8px
const unsigned char bouncer [] PROGMEM = {
  0x60, 0xc0, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'Boss1', 8x8px
const unsigned char boss [] PROGMEM = {
  0x18, 0x24, 0x42, 0xff, 0x24, 0xff, 0x7e, 0x3c
};
// 'Faller', 8x8px
const unsigned char faller [] PROGMEM = {
  0x80, 0xc0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'Riser', 8x8px
const unsigned char riser [] PROGMEM = {
  0xe0, 0xc0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'LT', 8x8px
const unsigned char LT [] PROGMEM = {
  0x78, 0xfc, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'UFO laser', 8x8px
const unsigned char ufoLaser [] PROGMEM = {
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'BossBoom', 8x8px
const unsigned char bossBoom1 [] PROGMEM = {
  0xaa, 0x55, 0xa2, 0x51, 0x8a, 0x45, 0xaa, 0x55
};
// 'BossBoom2', 8x8px
const unsigned char bossBoom2 [] PROGMEM = {
  0x55, 0xaa, 0x45, 0x8a, 0x51, 0xa2, 0x55, 0xaa
};



//Game started variable
bool gameStart = false;

// Defining joystick control pins
int up = 2;
int down = 3;
int left = 4;
int right = 5;
int trigger = 6;

//Title ticker variables..Shamlessly stolen from the ticker example :P
String tape = "Press fire to start";

int wait = 40; // In milliseconds
int spacer = 1;
int width = 5 + spacer; // The font width is 5 pixels

//Ship and laser position variables
int shipx = 0;
int shipy = 0;
int lasx;
int lasy;
//Lives counter
int lives = 5;
//laser existense variable
bool lasExist;
//UFO laser position variables
int ufoLas1x;
int ufoLas1y;
int ufoLas2x;
int ufoLas2y;
int ufoLas3x;
int ufoLas3y;
int ufoLas4x;
int ufoLas4y;
//UFO laser existence variable
bool ufoLasExist1 = false;
bool ufoLasExist2 = false;
bool ufoLasExist3 = false;
bool ufoLasExist4 = false;
//UFO laser selection variable
int selection;
int lastSelection;
//Boss Minion selection variable
int minionSelection;
//UFO position variables
int ufo1x;
int ufo1y;
int ufo2x;
int ufo2y;
int ufo3x;
int ufo3y;
//Bouncer position variables
int bouncer1x;
int bouncer1y;
int bouncer2x;
int bouncer2y;
int bouncer3x;
int bouncer3y;
int bouncer4x;
int bouncer4y;
//Faller position variables
int faller1x;
int faller1y;
int faller2x;
int faller2y;
int faller3x;
int faller3y;
int faller4x;
int faller4y;
//Faller speed variables
int faller1Speedx;
int faller1Speedy;
int faller2Speedx;
int faller2Speedy;
int faller3Speedx;
int faller3Speedy;
int faller4Speedx;
int faller4Speedy;
int fallerxSpeedTarget = 2;
int fallerySpeedTarget = 2;
//Riser position variables
int riser1x;
int riser1y;
int riser2x;
int riser2y;
int riser3x;
int riser3y;
int riser4x;
int riser4y;
//Riser speed variables
int riser1Speedx;
int riser1Speedy;
int riser2Speedx;
int riser2Speedy;
int riser3Speedx;
int riser3Speedy;
int riser4Speedx;
int riser4Speedy;
//LT position variable
int ltx;
int lty;
//LT health variable
int ltHealth = 3;
//LT speed variable
int ltxSpeed = 0;
int ltxSpeedTarget = 2;
//Boss position variable
int bossx = 34;
int bossy = 0;
//Boss movement speed variables
int bossxSpeed = 0;
int bossySpeed = 0;
int bossxSpeedTarget = 3;
int bossySpeedTarget = 1; 
//UFO existence variables
bool ufo1Exist = false;
bool ufo2Exist = false;
bool ufo3Exist = false;
//Bouncer existence variables
bool bouncer1Exist = false;
bool bouncer2Exist = false;
bool bouncer3Exist = false;
bool bouncer4Exist = false;
//Faller existence variables
bool faller1Exist = false;
bool faller2Exist = false;
bool faller3Exist = false;
bool faller4Exist = false;
//Riser existence variables
bool riser1Exist = false;
bool riser2Exist = false;
bool riser3Exist = false;
bool riser4Exist = false;
//LT existence variable
bool ltExist = false;
//Boss1 Existence variable
bool boss1Exist = false;
//Boss health variable
int bossHealth;


//Bouncer top or bottom variable
int bouncerOrigin;
//Bouncer gap space variable
int bouncerGap = 6;
//Bouncer direction variables
bool bouncer1Direction;
bool bouncer2Direction;
bool bouncer3Direction;
bool bouncer4Direction;
int bouncer1xSpeed;
int bouncer2xSpeed;
int bouncer3xSpeed;
int bouncer4xSpeed;
int bouncer1ySpeed;
int bouncer2ySpeed;
int bouncer3ySpeed;
int bouncer4ySpeed;
int ySpeed = 1;
int xSpeed = 2;


//spawn timer and spawn time variables for UFO enemies
unsigned long startTime;//setting up timer variables
unsigned long currentTime;
unsigned long spawnTime = 3000;   //Spawn timer for UFO's
//spawn timer and spawn time for bouncer enemies
unsigned long startTime2;
unsigned long currentTime2;
unsigned long spawnTime2 = 10000; //Spawn timer for bouncers
//Spawn timer for LT mini boss
unsigned long startTime3;
unsigned long currentTime3;
unsigned long spawnTime3;     //Spawn timer for LT
//spawn timer for UFO lasers
unsigned long bossStartTime;
unsigned long bossCurrentTime;
unsigned long bossStartTime2;
unsigned long bossCurrentTime2;
unsigned long bossLaserTimer = 1000;
unsigned long bossMinionTimer = 6000;
//Spawn enabler variable, needed  to temporarily stop spawning for boss battle
bool spawnEnabled;

//Level variable
int levelNumber = 1;
//Score variable
int score = 0;
//Level progression variables
int kills;
int killsTarget;
//Boss battle engaged variable
bool bossBattleStarted = false;

//Player ship collision detection variable
bool collisionDetection = true;
//Coin flip for UFOLaser 4 selection
int coinFlip;

//Sound variables
int buzzerPin = 9;
//Boss siren variables
float sinVal;             // Define a variable to save sine value 
int toneVal;              // Define a variable to save sound frequency 



void setup() 
{
  //Serial.begin(9600);
  //Setting up sound pin
  pinMode(buzzerPin, OUTPUT);   // Set the buzzer pin to output mode
  //setting up spawntimers
 
  
  //setting pin modes for joystick operation
  pinMode(up, INPUT_PULLUP);    //UP pin
  pinMode(down, INPUT_PULLUP);    //DOWN pin
  pinMode(left, INPUT_PULLUP);    //LEFT pin
  pinMode(right, INPUT_PULLUP);    //RIGHT pin
  pinMode(trigger, INPUT_PULLUP);    //FIRE pin

  

  //setting led intensity
  matrix.setIntensity(0);

// Adjust to your own needs
  matrix.setPosition(0, 3, 0); // The first display is at <0, 0>
  matrix.setPosition(1, 2, 0); // The second display is at <1, 0>
  matrix.setPosition(2, 1, 0); // The third display is at <2, 0>
  matrix.setPosition(3, 0, 0); // And the last display is at <3, 0>
//  ...
  matrix.setRotation(0, 3);    // The first display is position upside down
  matrix.setRotation(1, 3);
  matrix.setRotation(2, 3);
  matrix.setRotation(3, 3);    // The same hold for the last display
  matrix.fillScreen(LOW);


}
void loop() 
{
  if (gameStart == false)
  {
    Ticker(true);
  }
  
  SpawnTimer(); 
  JoystickControl();
  UfoControl();
  BouncerControl();
  LaserControl();
  ScreenLimit();
  BossBattle();
  BossControl();
  UFOLaserControl();
  LTControl();
  FallerControl();
  RiserControl();
  ShipCollision();
  //Serial.println(ufoLas1x);
  
  

  
  matrix.write();//draw the screen
  delay(50);//speed delay
  matrix.fillScreen(LOW);//clear screen, must be done every frame.
  

}

void Level(int level)
{
  if (level == 1)
  {
    spawnEnabled = true; //Enable spawning
    spawnTime = 3000;   //Spawn timer for UFO'a
    spawnTime2 = 10000000; //Spawn timer for bouncers set so high they'll never appear
    spawnTime3 = 10000000; //Spawn timer for LT set so high so they'll never appear
    killsTarget = 10;   //Sets kill target. When target reached boss should be spawned
    bossHealth = 10;
    bossLaserTimer = 1000;
  }
  if (level == 2)
  {
    spawnEnabled = true;
    spawnTime = 3000;
    spawnTime2 = 8000;
    spawnTime3 = 10000000;
    killsTarget = 20;
    bossHealth = 15;
    bossLaserTimer = 900;
  }
  if (level == 3)
  {
    spawnEnabled = true;
    spawnTime = 2000;
    spawnTime2 = 6000;
    spawnTime3 = 8000;
    killsTarget = 30;
    bossHealth = 20;
    bossLaserTimer = 800;
  }
  if (level == 4)
  {
    spawnEnabled = true;
    spawnTime = 2000;
    spawnTime2 = 5000;
    spawnTime3 = 7000;
    killsTarget = 40;
    bossHealth = 25;
    bossLaserTimer = 700;
    bossMinionTimer = 6000;
  }
  if (level == 5)
  {
    spawnEnabled = true;
    spawnTime = 1800;
    spawnTime2 = 4500;
    spawnTime3 = 6000;
    killsTarget = 50;
    bossHealth = 30;
    bossLaserTimer = 600;
    bossMinionTimer = 5500;
  }
  if (level == 6)
  {
    spawnEnabled = true;
    spawnTime = 2000;
    spawnTime2 = 5000;
    spawnTime3 = 7000;
    killsTarget = 60;
    bossHealth = 35;
    bossLaserTimer = 500;
    bossMinionTimer = 5000;
  }
  if (level == 7)
  {
    spawnEnabled = true;
    spawnTime = 2000;
    spawnTime2 = 5000;
    spawnTime3 = 7000;
    killsTarget = 70;
    bossHealth = 40;
    bossLaserTimer = 400;
    bossMinionTimer = 4000;
  }
  if (level == 8)
  {
    spawnEnabled = true;
    spawnTime = 2000;
    spawnTime2 = 5000;
    spawnTime3 = 7000;
    killsTarget = 80;
    bossHealth = 45;
    bossLaserTimer = 300;
    bossMinionTimer = 3000;
  }
  if (level == 9)
  {
    spawnEnabled = true;
    spawnTime = 2000;
    spawnTime2 = 5000;
    spawnTime3 = 7000;
    killsTarget = 90;
    bossHealth = 50;
    bossLaserTimer = 200;
    bossMinionTimer = 2000;
  }
  if (level == 10)
  {
    spawnEnabled = true;
    spawnTime = 2000;
    spawnTime2 = 5000;
    spawnTime3 = 7000;
    killsTarget = 100;
    bossHealth = 55;
    bossLaserTimer = 100;
    bossMinionTimer = 1000;
  }
  
  
  //Need to add congratulation message for getting past level 10
}



void Ticker(bool resetKills)        //Ticker code shamlessly stolen from the Ticker example. Used for Title and score and gameover
{
  while (gameStart == false)
  {
    

    for ( int i = 0 ; i < width * tape.length() + matrix.width() - 1 - spacer; i++ ) 
    {
      
      matrix.fillScreen(LOW);
  
      int letter = i / width;
      int x = (matrix.width() - 1) - i % width;
      int y = (matrix.height() - 8) / 2; // center the text vertically
  
      while ( x + width - spacer >= 0 && letter >= 0 ) 
      {
        
        if ( letter < tape.length() ) {
          matrix.drawChar(x, y, tape[letter], HIGH, LOW, 1);
          if (digitalRead(trigger)== 0)
          {
            gameStart = true;
          }
        }
        
        letter--;
        x -= width;
      }
      if (digitalRead(trigger)== 0)       //If trigger pressed then start the game
          {
             bossBattleStarted = false;
             gameStart = true;
             startTime = millis();  //Setting up spawn timer for UFO
             startTime2 = millis(); //setting up spawn timer for Bouncer
             startTime3 = millis(); //Setting up spawn timer for LT
             currentTime = startTime;//Setting up start timers for SpawnTimer function
             currentTime2 = startTime2;
             currentTime3 = startTime3;
             spawnEnabled = true;//Enabling spawnTimer
             Level(levelNumber);//Calls level function to set level variables
             shipx = 0;
             shipy = 3;
             if (resetKills == true)
             {
              kills = 0;
             }
             else if (resetKills == false)
             {
              kills = kills;
             }
             delay(500);
            return;
          }
      matrix.write(); // Send bitmap to display
  
      delay(wait);
    }

      
  }
}


void JoystickControl()
{
   //draw ship at ship coords
   matrix.drawBitmap(shipx,shipy,ship,3,2,1);
  
   if (digitalRead(up)== 0)//if joystick up then move ship coords up
  {
    shipy--;
  }
  if (digitalRead(down)== 0)//if joystick down then move ship coords down
  {
    shipy++;
  }
  if (digitalRead(left)== 0)//if joystick left then move ship coords left
  {
    shipx--;
  }
  if (digitalRead(right)== 0)//if joystick right then move ship coords right
  {
    shipx++;
  }
  if (digitalRead(trigger)== 0 && lasExist == false)//if trigger pressed then draw laser and mark it as existing
  {
    ShootLaser();
  }
  
}

void ShootLaser()
{
  if (lasExist == false)
  {
    lasx=shipx+3;
    lasy=shipy+1;
    lasExist=true;
    tone(buzzerPin, 400,75);//play laser sound
    
  }  
}

void LaserControl()       //Contains control code for the player laser and collision detection routines
{
  //If the laser exists then move the laser dot across the screen
  if (lasExist == true)
  {
    matrix.drawPixel(lasx,lasy,1);
    lasx++;
    //collision detection for UFO's
    //Each IF statement covers every pixel with an extra one shadowing the top pixel because the laser can sometimes go through.
    if (ufo1Exist == true)
    {
      
      for (int a = 0; a < 2; a++)
      {
          if (lasx == ufo1x+a && lasy == ufo1y+1)
        {
          ufo1Exist = false;
          lasExist = false;
          Boom(ufo1x,ufo1y,true,1);
          ufo1x = 34;
        }
      }     
      for (int a = 1; a < 3; a++)
      {
        if (lasx == ufo1x+a && lasy == ufo1y)
        {
          ufo1Exist = false;
          lasExist = false;
          Boom(ufo1x,ufo1y,true,1);
          ufo1x = 34;
        }
      }     
    }    
    //collision detection for UFO2
    if (ufo2Exist == true)
    {
      
      for (int a = 0; a < 3; a++)
      {
          if (lasx == ufo2x+a && lasy == ufo2y+1)
        {
          ufo2Exist = false;
          lasExist = false;
          Boom(ufo2x,ufo2y,true,1);
          ufo2x = 34;
        }
      }     
      for (int a = 1; a < 3; a++)
      {
        if (lasx == ufo2x+a && lasy == ufo2y)
        {
          ufo2Exist = false;
          lasExist = false;
          Boom(ufo2x,ufo2y,true,1);
          ufo2x = 34;
        }
      }     
    }
    
    //Collision detection for UFO3
    if (ufo3Exist == true)
    {
      
      for (int a = 0; a < 3; a++)
      {
          if (lasx == ufo3x+a && lasy == ufo3y+1)
        {
          ufo3Exist = false;
          lasExist = false;
          Boom(ufo3x,ufo3y,true,1);
          ufo3x = 34;
        }
      }     
      for (int a = 1; a < 3; a++)
      {
        if (lasx == ufo3x+a && lasy == ufo3y)
        {
          ufo3Exist = false;
          lasExist = false;
          Boom(ufo3x,ufo3y,true,1);
          ufo3x = 34;
        }
      }     
    }
   

    //Collision detection for Bouncer1
    if (bouncer1Exist == true)
    {
      for (int a = 1; a < 2; a++)
      {
        if (lasx == bouncer1x+a && lasy == bouncer1y)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
          bouncer1Exist = false;
          lasExist=false;
          Boom(bouncer1x,bouncer1y,true,2);
          bouncer1x = 34;
        }
      }
      for (int a = 0; a < 2; a++)
      {
        if (lasx == bouncer1x+a && lasy == bouncer1y+1)
        {
          bouncer1Exist = false;
          lasExist=false;
          Boom(bouncer1x,bouncer1y,true,2);
          bouncer1x = 34;
        }
      }
      for (int a = 1; a < 2; a++)
      {
        if (lasx == bouncer1x+a && lasy == bouncer1y+2)
        {
          bouncer1Exist = false;
          lasExist=false;
          Boom(bouncer1x,bouncer1y,true,2);
          bouncer1x = 34;
        }
      }
    }    
    //Collision detection for Bouncer 2
     if (bouncer2Exist == true)
    {
      for (int a = 1; a < 2; a++)
      {
        if (lasx == bouncer2x+a && lasy == bouncer2y)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
          bouncer2Exist = false;
          lasExist=false;
          Boom(bouncer2x,bouncer2y,true,2);
          bouncer2x = 34;
        }
      }
      for (int a = 0; a < 2; a++)
      {
        if (lasx == bouncer2x+a && lasy == bouncer2y+1)
        {
          bouncer2Exist = false;
          lasExist=false;
          Boom(bouncer2x,bouncer2y,true,2);
          bouncer2x = 34;
        }
      }
      for (int a = 1; a < 2; a++)
      {
        if (lasx == bouncer2x+a && lasy == bouncer2y+2)
        {
          bouncer2Exist = false;
          lasExist=false;
          Boom(bouncer2x,bouncer2y,true,2);
          bouncer2x = 34;
        }
      }
    }


    
    //Collision detection for Bouncer 3
     if (bouncer3Exist == true)
    {
      for (int a = 1; a < 2; a++)
      {
        if (lasx == bouncer3x+a && lasy == bouncer3y)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
          bouncer3Exist = false;
          lasExist=false;
          Boom(bouncer3x,bouncer3y,true,2);
          bouncer3x = 34;
        }
      }
      for (int a = 0; a < 2; a++)
      {
        if (lasx == bouncer3x+a && lasy == bouncer3y+1)
        {
          bouncer3Exist = false;
          lasExist=false;
          Boom(bouncer3x,bouncer3y,true,2);
          bouncer3x = 34;
        }
      }
      for (int a = 1; a < 2; a++)
      {
        if (lasx == bouncer3x+a && lasy == bouncer3y+2)
        {
          bouncer3Exist = false;
          lasExist=false;
          Boom(bouncer3x,bouncer3y,true,2);
          bouncer3x = 34;
        }
      }
    }
    
    //Collision detection for Bouncer 4
     if (bouncer4Exist == true)
    {
      for (int a = 1; a < 2; a++)
      {
        if (lasx == bouncer4x+a && lasy == bouncer4y)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
          bouncer4Exist = false;
          lasExist=false;
          Boom(bouncer4x,bouncer4y,true,2);
          bouncer4x = 34;
        }
      }
      for (int a = 0; a < 2; a++)
      {
        if (lasx == bouncer4x+a && lasy == bouncer4y+1)
        {
          bouncer4Exist = false;
          lasExist=false;
          Boom(bouncer4x,bouncer4y,true,2);
          bouncer4x = 34;
        }
      }
      for (int a = 1; a < 2; a++)
      {
        if (lasx == bouncer4x+a && lasy == bouncer4y+2)
        {
          bouncer4Exist = false;
          lasExist=false;
          Boom(bouncer4x,bouncer4y,true,2);
          bouncer4x = 34;
        }
      }
    }
    
    //Collision detection for Boss
    if (boss1Exist == true)
    {
      for (int a = 3; a < 5; a++ )       //Top line 0
      {
        if (lasx == bossx+a && lasy == bossy)
        {
          bossHealth--;
          lasExist=false;
          Boom(lasx,lasy,false,0);
        }
      }
      for (int a = 2; a < 5; a++)         //Line 1
      {
        if (lasx == bossx+a && lasy == bossy+1)
        {
          bossHealth--;
          lasExist=false;
          Boom(lasx,lasy,false,0);
        }
      }
      for (int a = 1; a < 6; a++)          //Line 2
      {
        if (lasx == bossx+a && lasy == bossy+2)
        {
          bossHealth--;
          lasExist=false;
          Boom(lasx,lasy,false,0);
        }
      }
      for (int a = 0; a < 7; a++)          //Line 3
      {
        if (lasx == bossx+a && lasy == bossy+3)
        {
          bossHealth--;
          lasExist=false;
          Boom(lasx,lasy,false,0);
        }
      }
      for (int a = 2; a < 6; a++)    //Line 4
      {
        if (lasx == bossx+a && lasy == bossy+4)
        {
          bossHealth--;
          lasExist=false;
          Boom(lasx,lasy,false,0);
        }
      }
      for (int a = 0; a < 7; a++)      //Line 5
      {
        if (lasx == bossx+a && lasy == bossy+5)
        {
          bossHealth--;
          lasExist=false;
          Boom(lasx,lasy,false,0);
        }
      }
      for (int a = 1; a < 6; a++)      //Line 6
      {
        if (lasx == bossx+a && lasy == bossy+6)
        {
          bossHealth--;
          lasExist=false;
          Boom(lasx,lasy,false,0);
        }
      }        
      for (int a = 2; a < 5; a++)      //Line 7
      {
        if (lasx == bossx+a && lasy == bossy+7)
        {
          bossHealth--;
          lasExist=false;
          Boom(lasx,lasy,false,0);
        }
      }
     
    }

    //Collision detection routines for LT
    if (ltExist == true)
    {
      for (int a = 1; a < 4; a++)    //Top line 0
      {
        if (lasx == ltx+a && lasy == lty)
        {
          ltHealth--;
          lasExist=false;
          Boom(lasx,lasy,false,0);
        }
      }
      for (int a = 0; a < 5; a++)    //Line 1
      {
        if (lasx == ltx+a && lasy == lty+1)
        {
          ltHealth--;
          lasExist=false;
          Boom(lasx,lasy,false,0);
        }
      }
      for (int a = 2; a < 4; a++)           //Line 2
      {
        if (lasx == ltx+a && lasy == lty+2)
        {
          ltHealth--;
          lasExist=false;
          Boom(lasx,lasy,false,0);
        }
      }     
    }
//Collision detection for Faller 1
    if (faller1Exist == true)
    {
      for (int a = 0; a < 2; a++)         //Top Line 0
      {
        if (lasx == faller1x+a && lasy == faller1y)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
          faller1Exist = false;
          lasExist=false;
          Boom(faller1x,faller1y,false,0);
          faller1x = 34;
        }
      }
      for (int a = 0; a < 3; a++)          //Line 1
      {
        if (lasx == faller1x+a && lasy == faller1y+1)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
          faller1Exist = false;
          lasExist=false;
          Boom(faller1x,faller1y,false,0);
          faller1x = 34;
        }
      }
      for (int a = 0; a < 3; a++)          //Line 2
      {
        if (lasx == faller1x+a && lasy == faller1y+2)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
          faller1Exist = false;
          lasExist=false;
          Boom(faller1x,faller1y,false,0);
          faller1x = 34;
        }
      }     
    }
    //Collision detection for Faller 2
   if (faller2Exist == true)
    {
      for (int a = 0; a < 2; a++)         //Top Line 0
      {
        if (lasx == faller2x+a && lasy == faller2y)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
          faller2Exist = false;
          lasExist=false;
          Boom(faller2x,faller2y,false,0);
          faller2x = 34;
        }
      }
      for (int a = 0; a < 3; a++)          //Line 1
      {
        if (lasx == faller2x+a && lasy == faller2y+1)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
          faller2Exist = false;
          lasExist=false;
          Boom(faller2x,faller2y,false,0);
          faller2x = 34;
        }
      }
      for (int a = 0; a < 3; a++)          //Line 2
      {
        if (lasx == faller2x+a && lasy == faller2y+2)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
          faller2Exist = false;
          lasExist=false;
          Boom(faller2x,faller2y,false,0);
          faller2x = 34;
        }
      }
    }
    //Collision detection for Faller 3
    if (faller3Exist == true)
    {
      for (int a = 0; a < 2; a++)         //Top Line 0
      {
        if (lasx == faller3x+a && lasy == faller3y)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
          faller3Exist = false;
          lasExist=false;
          Boom(faller3x,faller3y,false,0);
          faller3x = 34;
        }
      }
      for (int a = 0; a < 3; a++)          //Line 1
      {
        if (lasx == faller3x+a && lasy == faller3y+1)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
          faller3Exist = false;
          lasExist=false;
          Boom(faller3x,faller3y,false,0);
          faller3x = 34;
        }
      }
      for (int a = 0; a < 3; a++)          //Line 2
      {
        if (lasx == faller3x+a && lasy == faller3y+2)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
          faller3Exist = false;
          lasExist=false;
          Boom(faller3x,faller3y,false,0);
          faller3x = 34;
        }
      }
    }
    //Collision detection for Faller 4
   if (faller4Exist == true)
    {
      for (int a = 0; a < 2; a++)         //Top Line 0
      {
        if (lasx == faller4x+a && lasy == faller4y)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
          faller4Exist = false;
          lasExist=false;
          Boom(faller4x,faller4y,false,0);
          faller4x = 34;
        }
      }
      for (int a = 0; a < 3; a++)          //Line 1
      {
        if (lasx == faller4x+a && lasy == faller4y+1)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
          faller4Exist = false;
          lasExist=false;
          Boom(faller4x,faller4y,false,0);
          faller4x = 34;
        }
      }
      for (int a = 0; a < 3; a++)          //Line 2
      {
        if (lasx == faller4x+a && lasy == faller4y+2)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
          faller4Exist = false;
          lasExist=false;
          Boom(faller4x,faller4y,false,0);
          faller4x = 34;
        }
      }
    }
    
    //Collision detection for Riser 1
    if (riser1Exist == true)
    {
      for (int a = 0; a < 3; a++)          //Top Line 0
      {
        if (lasx == riser1x+a && lasy == riser1y)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
          riser1Exist = false;
          lasExist=false;
          Boom(riser1x,riser1y,false,0);
          riser1x = 34;
        }
      }      
      for (int a = 0; a < 2; a++)          //Line 1
      {
        if (lasx == riser1x+0 && lasy == riser1y+1)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
          {
            riser1Exist = false;
            lasExist=false;
            Boom(riser1x,riser1y,false,0);
            riser1x = 34;
          }
      }
      for (int a = 0; a < 1; a++)         //Line 2
      {      
        if (lasx == riser1x+a && lasy == riser1y+2)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
          riser1Exist = false;
          lasExist=false;
          Boom(riser1x,riser1y,false,0);
          riser1x = 34;
        }
      }
    }      
    

      //Collision detection for Riser 2
   if (riser2Exist == true)
    {
      for (int a = 0; a < 3; a++)          //Top Line 0
      {
        if (lasx == riser2x+a && lasy == riser2y)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
          riser2Exist = false;
          lasExist=false;
          Boom(riser2x,riser2y,false,0);
          riser2x = 34;
        }
      }      
      for (int a = 0; a < 2; a++)          //Line 1
      {
        if (lasx == riser2x+0 && lasy == riser2y+1)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
          {
            riser2Exist = false;
            lasExist=false;
            Boom(riser2x,riser2y,false,0);
            riser2x = 34;
          }
      }
      for (int a = 0; a < 1; a++)         //Line 2
      {      
        if (lasx == riser2x+a && lasy == riser2y+2)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
          riser2Exist = false;
          lasExist=false;
          Boom(riser2x,riser2y,false,0);
          riser1x = 34;
        }
      }
    }      

      //Collision detection for Riser 3
    if (riser3Exist == true)
    {
      for (int a = 0; a < 3; a++)          //Top Line 0
      {
        if (lasx == riser3x+a && lasy == riser3y)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
          riser3Exist = false;
          lasExist=false;
          Boom(riser3x,riser3y,false,0);
          riser3x = 34;
        }
      }      
      for (int a = 0; a < 2; a++)          //Line 1
      {
        if (lasx == riser3x+0 && lasy == riser3y+1)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
          {
            riser3Exist = false;
            lasExist=false;
            Boom(riser3x,riser3y,false,0);
            riser3x = 34;
          }
      }
      for (int a = 0; a < 1; a++)         //Line 2
      {      
        if (lasx == riser3x+a && lasy == riser3y+2)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
          riser3Exist = false;
          lasExist=false;
          Boom(riser3x,riser3y,false,0);
          riser3x = 34;
        }
      }
    }      
      //Collision detection for Riser 4
    if (riser4Exist == true)
    {
      for (int a = 0; a < 3; a++)          //Top Line 0
      {
        if (lasx == riser4x+a && lasy == riser4y)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
          riser4Exist = false;
          lasExist=false;
          Boom(riser4x,riser4y,false,0);
          riser4x = 34;
        }
      }      
      for (int a = 0; a < 2; a++)          //Line 1
      {
        if (lasx == riser4x+0 && lasy == riser4y+1)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
          {
            riser4Exist = false;
            lasExist=false;
            Boom(riser4x,riser4y,false,0);
            riser4x = 34;
          }
      }
      for (int a = 0; a < 1; a++)         //Line 2
      {      
        if (lasx == riser4x+a && lasy == riser4y+2)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
          riser4Exist = false;
          lasExist=false;
          Boom(riser4x,riser4y,false,0);
          riser4x = 34;
        }
      }
    }   
  }
}



void Boom(int x, int y, bool addKill, int scoreType)               //Spawns explosion bitmap at called coords. Then increments kill count
{
  matrix.drawBitmap(x,y,explosion,3,3,1); //Draw explosion
  tone(buzzerPin, 75,75);
  if (addKill == true)
  {
    kills++;
  }
  if (scoreType == 1) //Score type for UFOs
  {
    score = score+10;
  }
  if (scoreType == 2) //Score type for bouncers
  {
    score = score+20;
  }
  if (scoreType == 3) //Scoretype for LT
  {
    score = score+70;
  }
  //if (scoreType == 4) //Score type for Risers and Fallers.. Currently not using it
 // {
 //   score = score+30;
//  }
  if (scoreType == 5) //Score type for Boss
  {
    score = score+(100*levelNumber);
  }
}


void ScreenLimit()
{
  //setting boundries of the screen for ship
  if (shipx < 0)
  {
    shipx = 0;
  }
  if (shipx > 29)
  {
    shipx = 29;
  }
  
  if (shipy < 0)
  {
    shipy = 0;
  }
  if (shipy > 6)
  {
    shipy = 6;
  }
  
  if (lasExist == true) //When it goes off the screen then stop drawing it and mark it as not existing
  {
    
    if (lasx > 31)
    {
      lasExist=false;
    }
    if (lasy < 0)
    {
      lasy = 0;
    }
    if (lasy > 7)
    {
      lasy = 7;
    }
  }

    //UFO laser screen limit controls
    
      if (ufoLas1x < -2)
      {
          ufoLasExist1 = false;
      }      
    

  
      if (ufoLas2x < -2)
      {
          ufoLasExist2 = false;
      }      
    

    
      if (ufoLas3x < -2)
      {
          ufoLasExist3 = false;
      }      
    

    
      if (ufoLas4x < -2)
      {
          ufoLasExist4 = false;
      }      
    
  
    //UFO screen limit controls. Automatically respawning UFO's at a random Y coordinate on the right side if they go off the left side
    if (ufo1Exist == true && ufo1x < -3)
    {
      ufo1x = 33; //if UFO goes off the left side it gets moved to the right side again
      ufo1y = random(0,7);
      // going to need some sort of sorting code to stop ufo's overlapping  
    }
     if (ufo2Exist == true && ufo2x < -3)
    {
      ufo2x = 33; //if UFO goes off the left side it gets moved to the right side again
      ufo2y = random(0,7);
      // going to need some sort of sorting code to stop ufo's overlapping  
    }
     if (ufo3Exist == true && ufo3x < -3)
    {
      ufo3x = 33; //if UFO goes off the left side it gets moved to the right side again
      ufo3y = random(0,7);
      // going to need some sort of sorting code to stop ufo's overlapping  
    }
  
    //Bouncer screen limit controls. When bouncers hit top or bottom the direction variable changes. When off the left they are wiped from existence and respawned at a later time.
  
    if (bouncer1Exist == true && bouncer1y > 5)
    {
      bouncer1Direction = false;
    }
    else if (bouncer1Exist == true && bouncer1y < 0)
    {
      bouncer1Direction = true;
    }
  
     if (bouncer2Exist == true && bouncer2y > 5)
    {
      bouncer2Direction = false;
    }
    else if (bouncer2Exist == true && bouncer2y < 0)
    {
      bouncer2Direction = true;
    }
  
     if (bouncer3Exist == true && bouncer3y > 5)
    {
      bouncer3Direction = false;
    }
    else if (bouncer3Exist == true && bouncer3y < 0)
    {
      bouncer3Direction = true;
    }
  
     if (bouncer4Exist == true && bouncer4y > 5)
    {
      bouncer4Direction = false;
    }
    else if (bouncer4Exist == true && bouncer4y < 0)
    {
      bouncer4Direction = true;
    }
    // If bouncers go off the left side they disappear as despawned
    if (bouncer1x < 0)
    {
      bouncer1Exist = false;
    }
  
    if (bouncer2x < 0)
    {
      bouncer2Exist = false;
    }
  
    if (bouncer3x < 0)
    {
      bouncer3Exist = false;
    }
  
    if (bouncer4x < 0)
    {
      bouncer4Exist = false;
    }
    //LT screen limit control
    if (ltExist == true && ltx < -6)
    {
      ltx = 33; //if LT goes off the left side it gets moved to the right side again
      lty = random(0,6);  
    }
    //Screen limit controls for Risers
    if (faller1Exist == true && faller1y > 7)
    {
      faller1Exist = false;
    }
    if (faller2Exist == true && faller2y > 7)
    {
      faller2Exist = false;
    }
    if (faller3Exist == true && faller3y > 7)
    {
      faller3Exist = false;
    }
    if (faller4Exist == true && faller4y > 7)
    {
      faller4Exist = false;
    }
    //Screen limit controls for Risers
    if (riser1Exist == true && riser1y < -2)
    {
      riser1Exist = false;
    }
    if (riser2Exist == true && riser2y < -2)
    {
      riser2Exist = false;
    }
    if (riser3Exist == true && riser3y < -2)
    {
      riser3Exist = false;
    }
    if (riser4Exist == true && riser4y < -2)
    {
      riser4Exist = false;
    }
    
}

void SpawnTimer()
{
  if (spawnEnabled == true)
  {
    //Counting spawn timer
    currentTime = millis(); //get the current time in milliseconds
    currentTime2 = millis();//same again for second timer  
    currentTime3 = millis();//and again
    if (currentTime - startTime >= spawnTime)   //Uses the currentTime, startTime and spawnTime variables to spawn enemies on set times
    {
        //spawn UFO
        
        SpawnUFO();
        //reset timer
        startTime = currentTime;  //Saves the start time of the spawn so the if statement works
    }
  
    if (currentTime2 - startTime2 >= spawnTime2)
    {
        //spawn bouncer squad
        
        SpawnBouncer();
        //reset timer
        startTime2 = currentTime2;  //Saves the start time of the spawn so the if statement works
    }

    if (currentTime3 - startTime3 >= spawnTime3)
    {
        //spawn LT
        
        SpawnLT();
        //reset timer
        startTime3 = currentTime3;  //Saves the start time of the spawn so the if statement works
    }
  }
  
}

void SpawnUFO()
{
  if (ufo1Exist == false)
  {
    
    ufo1x = 32;           //Spawn off the right side of the screen
    ufo1y = random(0,7);   //Spawn at a random Y coord
    
    ufo1Exist = true;
    
  }
  else if (ufo2Exist == false)
  {
    ufo2x = 32;           //Spawn off the right side of the screen
    ufo2y = random(0,7);   //Spawn at a random Y coord
    
    ufo2Exist = true;
  }
  else if (ufo3Exist == false)
  {
    ufo3x = 32;           //Spawn off the right side of the screen
    ufo3y = random(0,7);   //Spawn at a random Y coord
    
    ufo3Exist = true;
  }
   
}

void UfoControl()
{
  //If the UFO exists then draw its bitmap
  if (ufo1Exist == true)
  {
    ufo1x--;
    matrix.drawBitmap(ufo1x,ufo1y,ufo,3,2,1);
  }
  if (ufo2Exist == true)
  {
    ufo2x--;
    matrix.drawBitmap(ufo2x,ufo2y,ufo,3,2,1);
  }
  if (ufo3Exist == true)
  {
    ufo3x--;
    matrix.drawBitmap(ufo3x,ufo3y,ufo,3,2,1);
  }
}


void SpawnBouncer()
{
  if (bouncer1Exist == false && bouncer2Exist == false && bouncer3Exist == false && bouncer4Exist == false)
  {
    
    bouncer1x = 32;           //Spawn off the right side of the screen
    bouncer2x = bouncer1x+bouncerGap;//spawn other bouncers spaced out on the x axis
    bouncer3x = bouncer2x+bouncerGap;
    bouncer4x = bouncer3x+bouncerGap;
    bouncerOrigin = random(0,2);
    if (bouncerOrigin = 0)
    {
      bouncer1y = 0;   //Spawn at top of screen
      bouncer2y = 1;
      bouncer3y = 2;
      bouncer4y = 3;
    }
    if (bouncerOrigin = 1)
    {
      bouncer1y = 7; //Spawn at bottom
      bouncer2y = 6;
      bouncer3y = 5;
      bouncer4y = 4;
    }
    
    
    bouncer1Exist = true;
    bouncer2Exist = true;
    bouncer3Exist = true;
    bouncer4Exist = true;
    
  }
}

void BouncerControl()
{
  //Bouncer move across to the left and up or down depending opn direction variable. That variable is changed in ScreenLimit()
  
  if (bouncer1Exist == true)    //If bouncer exists then increment bouncer x and y counters. These are used to slow the movement.
  {
    bouncer1xSpeed++;
    bouncer1ySpeed++;
    if (bouncer1xSpeed > xSpeed)    //When the counter goes above the speed value then IF statement executes
    {
      bouncer1x--;
      bouncer1xSpeed = 0;
    }
    if (bouncer1Direction == true)
    {
      if (bouncer1ySpeed > ySpeed)
      {
        bouncer1y++;
        bouncer1ySpeed = 0;
      }
    }
    else if (bouncer1Direction == false)
    {
      if (bouncer1ySpeed > ySpeed)
      {
        bouncer1y--;
        bouncer1ySpeed = 0;
      }
    }
    matrix.drawBitmap(bouncer1x,bouncer1y,bouncer,2,3,1);
  }
  if (bouncer2Exist == true)
  {
     bouncer2xSpeed++;
     bouncer2ySpeed++;
    if (bouncer2xSpeed > xSpeed)
    {
      bouncer2x--;
      bouncer2xSpeed = 0;
    }
    if (bouncer2Direction == true)
    {
      if (bouncer2ySpeed > ySpeed)
      {
        bouncer2y++;
        bouncer2ySpeed = 0;
      }
    }
    else if (bouncer2Direction == false)
    {
      if (bouncer2ySpeed > 2)
      {
        bouncer2y--;
        bouncer2ySpeed = 0;
      }
    }
    matrix.drawBitmap(bouncer2x,bouncer2y,bouncer,2,3,1);
  }
  if (bouncer3Exist == true)
  {
    bouncer3xSpeed++;
    bouncer3ySpeed++;
    if (bouncer3xSpeed > xSpeed)
    {
      bouncer3x--;
      bouncer3xSpeed = 0;
    }
    if (bouncer3Direction == true)
    {
      if (bouncer3ySpeed > ySpeed)
      {
        bouncer3y++;
        bouncer3ySpeed = 0;
      }
    }
    else if (bouncer3Direction == false)
    {
      if (bouncer3ySpeed > 2)
      {
        bouncer3y--;
        bouncer3ySpeed = 0;
      }
    }
    matrix.drawBitmap(bouncer3x,bouncer3y,bouncer,2,3,1);
  }
  if (bouncer4Exist == true)
  {
    bouncer4xSpeed++;
    bouncer4ySpeed++;
    if (bouncer4xSpeed > xSpeed)
    {
      bouncer4x--;
      bouncer4xSpeed = 0;
    }
    if (bouncer4Direction == true)
    {
      if (bouncer4ySpeed > 2)
      {
        bouncer4y++;
        bouncer4ySpeed = 0;
      }
    }
    else if (bouncer4Direction == false)
    {
      if (bouncer4ySpeed > ySpeed)
      {
        bouncer4y--;
        bouncer4ySpeed = 0;
      }
    }
    matrix.drawBitmap(bouncer4x,bouncer4y,bouncer,2,3,1);
  }
}

void SpawnFaller()
{
  if (faller1Exist == false && faller2Exist == false && faller3Exist == false && faller4Exist == false)
  {
    
    faller1x = 15;           //Spawn above the boss off the top of the screen
    faller2x = faller1x+bouncerGap;//spawn other bouncers spaced out on the x axis
    faller3x = faller2x+bouncerGap;
    faller4x = faller3x+bouncerGap;
    faller1y = -1;   //Spawn at top of screen
    faller2y = -2;
    faller3y = -3;
    faller4y = -4;
        
    faller1Exist = true;
    faller2Exist = true;
    faller3Exist = true;
    faller4Exist = true;
    
  }
}

void FallerControl()
{
  //Fallers are spawned by the Boss during battle after level 4. Timer for spawning is in the BossControl function.
  //Control routines for faller 1
  if (faller1Exist == true)    //If faller exists then increment fallerSpeed x and y counters. These are used to slow the movement.
  {
    faller1Speedx++;
    faller1Speedy++;
    if (faller1Speedx > fallerxSpeedTarget)    //When the counter goes above the speed value then IF statement executes
    {
      faller1x--;
      faller1Speedx = 0;
    }
    
    
    if (faller1Speedy > fallerySpeedTarget)
    {
      faller1y++;
      faller1Speedy = 0;
    }
    
    matrix.drawBitmap(faller1x,faller1y,faller,3,3,1);
  }
// Control routines for faller 2
    if (faller2Exist == true)    //If faller exists then increment fallerSpeed x and y counters. These are used to slow the movement.
  {
    faller2Speedx++;
    faller2Speedy++;
    if (faller2Speedx > fallerxSpeedTarget)    //When the counter goes above the speed value then IF statement executes
    {
      faller2x--;
      faller2Speedx = 0;
    }
    
    
    if (faller2Speedy > fallerySpeedTarget)
    {
      faller2y++;
      faller2Speedy = 0;
    }
    
    matrix.drawBitmap(faller2x,faller2y,faller,3,3,1);
   }

  // Control routines for faller 3
    if (faller3Exist == true)    //If faller exists then increment fallerSpeed x and y counters. These are used to slow the movement.
  {
    faller3Speedx++;
    faller3Speedy++;
    if (faller3Speedx > fallerxSpeedTarget)    //When the counter goes above the speed value then IF statement executes
    {
      faller3x--;
      faller3Speedx = 0;
    }
    
    
    if (faller3Speedy > fallerySpeedTarget)
    {
      faller3y++;
      faller3Speedy = 0;
    }
    
    matrix.drawBitmap(faller3x,faller3y,faller,3,3,1);
   }

   // Control routines for faller 4
    if (faller4Exist == true)    //If faller exists then increment fallerSpeed x and y counters. These are used to slow the movement.
  {
    faller4Speedx++;
    faller4Speedy++;
    if (faller4Speedx > fallerxSpeedTarget)    //When the counter goes above the speed value then IF statement executes
    {
      faller4x--;
      faller4Speedx = 0;
    }
    
    
    if (faller4Speedy > fallerySpeedTarget)
    {
      faller4y++;
      faller4Speedy = 0;
    }
    
    matrix.drawBitmap(faller4x,faller4y,faller,3,3,1);
   }  
}


void SpawnRiser()
{
  if (riser1Exist == false && riser2Exist == false && riser3Exist == false && riser4Exist == false)
  {
    
    riser1x = 15;           //Spawn above the boss off the top of the screen
    riser2x = riser1x+bouncerGap;//spawn other bouncers spaced out on the x axis
    riser3x = riser2x+bouncerGap;
    riser4x = riser3x+bouncerGap;
    riser1y = 7;   //Spawn at Bottom of screen
    riser2y = 8;
    riser3y = 9;
    riser4y = 10;
        
    riser1Exist = true;
    riser2Exist = true;
    riser3Exist = true;
    riser4Exist = true;
    
  }
}

void RiserControl()
{
  //Risers are spawned by the Boss during battle after level 5. Timer for spawning is in the BossControl function.
  //Control routines for riser 1
  if (riser1Exist == true)    //If riser exists then increment riserSpeed x and y counters. These are used to slow the movement.
  {
    riser1Speedx++;
    riser1Speedy++;
    if (riser1Speedx > fallerxSpeedTarget)    //When the counter goes above the speed value then IF statement executes
    {
      riser1x--;
      riser1Speedx = 0;
    }
    
    
    if (riser1Speedy > fallerySpeedTarget)
    {
      riser1y--;
      riser1Speedy = 0;
    }
    
    matrix.drawBitmap(riser1x,riser1y,riser,3,3,1);
  }
  //Control routines for riser 2
  if (riser2Exist == true)    //If riser exists then increment riserSpeed x and y counters. These are used to slow the movement.
  {
    riser2Speedx++;
    riser2Speedy++;
    if (riser2Speedx > fallerxSpeedTarget)    //When the counter goes above the speed value then IF statement executes
    {
      riser2x--;
      riser2Speedx = 0;
    }
    
    
    if (riser2Speedy > fallerySpeedTarget)
    {
      riser2y--;
      riser2Speedy = 0;
    }
    
    matrix.drawBitmap(riser2x,riser2y,riser,3,3,1);
  }
  //Control routines for riser 3
  if (riser3Exist == true)    //If riser exists then increment riserSpeed x and y counters. These are used to slow the movement.
  {
    riser3Speedx++;
    riser3Speedy++;
    if (riser3Speedx > fallerxSpeedTarget)    //When the counter goes above the speed value then IF statement executes
    {
      riser3x--;
      riser3Speedx = 0;
    }
    
    
    if (riser3Speedy > fallerySpeedTarget)
    {
      riser3y--;
      riser3Speedy = 0;
    }
    
    matrix.drawBitmap(riser3x,riser3y,riser,3,3,1);
  }
  //Control routines for riser 4
  if (riser4Exist == true)    //If riser exists then increment riserSpeed x and y counters. These are used to slow the movement.
  {
    riser4Speedx++;
    riser4Speedy++;
    if (riser4Speedx > fallerxSpeedTarget)    //When the counter goes above the speed value then IF statement executes
    {
      riser4x--;
      riser4Speedx = 0;
    }
    
    
    if (riser4Speedy > fallerySpeedTarget)
    {
      riser4y--;
      riser4Speedy = 0;
    }
    
    matrix.drawBitmap(riser4x,riser4y,riser,3,3,1);
  }

}

void SpawnLT()
{
  if (ltExist == false)
  {
    
    ltx = 32;           //Spawn off the right side of the screen
    lty = random(0,6);   //Spawn at a random Y coord
    
    ltExist = true;
   }
}

void LTControl()
{
  //If the UFO exists then draw its bitmap
  if (ltExist == true)
  {
    ltxSpeed++;
    if (ltxSpeed == ltxSpeedTarget)
    {
      ltxSpeed = 0;
      ltx--;
    }
    matrix.drawBitmap(ltx, lty, LT, 6, 3, 1);
  }
  if (ltHealth <= 0)
  {
    ltExist = false;
    Boom(ltx,lty,false,3);  //Calls Boom() function to add score. Kills are added manually below.
    kills=kills+3;
    ltHealth = 3;
  }
}



void BossBattle()//Sounds the warning siren then spawns boss. Still need to finish movement
{
    if (kills > killsTarget && bossBattleStarted == false)
    {
      bossBattleStarted = true;
      spawnEnabled = false;             //Turns off spawning for other UFO types
      for(int i = 5; i > 0; i--)          //Play alarm sound
    {
      for (int x = 0; x < 360; x++)       // X from 0 degree->360 degree 
      {       
        sinVal = sin(x * (PI / 180));            // Calculate the sine of x 
        toneVal = 2000 + sinVal * 500;       // Calculate sound frequency according to the sine of x  
        tone(buzzerPin, toneVal);             // Output sound frequency to buzzerPin 
        delay(1);
      }
    }
    tone(buzzerPin, 0, 5);  //This is to stop the tone sound. It sticks around otherwise.
    if (boss1Exist == false)
    {    
      matrix.drawBitmap(bossx, bossy, boss, 8 ,8 ,1);
      boss1Exist = true;
      bossStartTime = millis();   //Set start timer for boss weapons
      bossStartTime = millis();   //Set start timer for boss minions
    }
    
  }
}

void BossControl()
{
  if (boss1Exist == true)
  {      
      bossxSpeed++;
      if (bossxSpeed > bossxSpeedTarget && bossx > 24)    //When the counter goes above the speed value then IF statement executes
      {
        bossx--;                //Move the boss x cords to the left
        bossxSpeed = 0;
      }
    
    matrix.drawBitmap(bossx, bossy, boss, 8, 8, 1); //Draw the boss bitmap at the new coords
    if (bossx == 24)
    {
       
      //Counting shoot timer
      bossCurrentTime = millis(); //get the current time in milliseconds
      bossCurrentTime2 = millis();
     
      if (bossCurrentTime - bossStartTime >= bossLaserTimer)    //startTimer is set in BossBattle function
      {
          //shoot laser
          selection = random(1,5);//Select laser cannon at random
          while (selection == lastSelection)//While loop to ensure that the same laser isnt fired twice in a row too quick. Prevents timing gaps
          {
            selection = random(1,5);
          }
          UFOLaserShoot(selection);//Call UFOLaserShoot function with cannon selection
          //reset timer
          bossStartTime = bossCurrentTime;  //Saves the start time of the spawn so the if statement works
      }
      if (levelNumber >= 4)
      {
        if (bossCurrentTime2 - bossStartTime2 >= bossMinionTimer)  //startTimer is set in BossBattle function
        {
            //Select minions and spawn them
            minionSelection = random(1,3);
            if (minionSelection == 1)
            {
              SpawnFaller();
            }
            if (minionSelection == 2)
            {
              SpawnRiser();
            }
            
            
            //reset timer
            bossStartTime2 = bossCurrentTime2;  //Saves the start time of the spawn so the if statement works
        }
      }
    }
    //Boss health counter hits zero, trigger boss death and new level
    if (bossHealth <= 0)
    {
      boss1Exist = false;   //Boss no longer exists
      Boom(bossx,bossy,false,5);//Calls Boom() function to add score
      matrix.fillScreen(LOW); //Clear screen
      for (int i = 5; i > 0; i--)
      {      
        matrix.drawBitmap(bossx, bossy, bossBoom1 ,8 ,8, 1);    //Boss explosion first frame
        tone(buzzerPin, 100,100);             //Play sound
        delay(200);   //Wait
        //matrix.fillScreen(LOW); //Clear screen
        matrix.drawBitmap(bossx, bossy, bossBoom2 ,8 ,8, 1);    //Boss explosion second frame
        tone(buzzerPin, 200,100);         //Play second sound
        delay(200); //Wait
      }
      tone(buzzerPin, 1, 1);//Stop beep
      
      bossx=39;             //Move bossx back to 32
      matrix.fillScreen(LOW); //clear the screen
      gameStart = false;    //Mark game as stopped
      levelNumber++ ;       //increment level
      if (levelNumber > 10)
      {
        GameOver(1);
        return;
      }
      tape = "Level ";      //Prepare new tape for ticker function
      tape += levelNumber;  //Adding level number to ticker string
      tape += "  Score: ";
      tape += score;
      ufoLasExist1 = false;
      ufoLasExist2 = false;
      ufoLasExist3 = false;
      ufoLasExist4 = false;
      faller1Exist = false;
      faller2Exist = false;
      faller3Exist = false;
      faller4Exist = false;
      riser1Exist = false;
      riser2Exist = false;
      riser3Exist = false;
      riser4Exist = false;
      ufoLas1x=bossx;
      ufoLas2x=bossx;
      ufoLas3x=bossx;
      ufoLas4x=bossx;
      Ticker(true);             //Call ticker function to show score and new level
      
    }
  }
}
void UFOLaserShoot(int selection)
{
  if (selection == 1)
  {
    if (ufoLasExist1 == false)//Check to see if the UFO laser already exists
    {
      ufoLas1x=bossx+1;      //spawn laser in the middle
      ufoLas1y=bossy+4;
      ufoLasExist1=true;
      tone(buzzerPin, 200,100);//play laser sound    
    }
  }
  if (selection == 2)
  { 
    if (ufoLasExist2 == false)
    {
      ufoLas2x=bossx+2;      //spawn laser at the top side
      ufoLas2y=bossy+1;
      ufoLasExist2=true;
      tone(buzzerPin, 300,100);//play laser sound    
    }
  }
  if (selection == 3)
  {
    if (ufoLasExist3 == false)
    {
      ufoLas3x=bossx+2;    //spawn laser at the bottom side
      ufoLas3y=bossy+7;
      ufoLasExist3=true;
      tone(buzzerPin, 400,100);//play laser sound    
    }
  }
  if (selection == 4)
  {
    if (ufoLasExist4 == false)//Planning to make this one random
    {
      coinFlip = random(0,2);
      if (coinFlip == 0)
      {
        ufoLas4x=bossx+2;   
        ufoLas4y=bossy+2;
        ufoLasExist4=true;
        tone(buzzerPin, 500,100);//play laser sound
      }
      if (coinFlip == 1)
      {
        ufoLas4x=bossx+2;   
        ufoLas4y=bossy+6;
        ufoLasExist4=true;
        tone(buzzerPin, 500,100);//play laser sound
      }
    }
  }    
}

void UFOLaserControl()
{
  if (ufoLasExist1 == true)
  {
    ufoLas1x--;
    matrix.drawBitmap(ufoLas1x, ufoLas1y, ufoLaser, 2, 1, 1);
  }
  if (ufoLasExist2 == true)
  {
    ufoLas2x--;
    matrix.drawBitmap(ufoLas2x, ufoLas2y, ufoLaser, 2, 1, 1);
  }
  if (ufoLasExist3 == true)
  {
    ufoLas3x--;
    matrix.drawBitmap(ufoLas3x, ufoLas3y, ufoLaser, 2, 1, 1);
  }
  if (ufoLasExist4 == true)
  {
      ufoLas4x--;
      matrix.drawBitmap(ufoLas4x, ufoLas4y, ufoLaser, 2, 1, 1);  
  }

  
}

void ShipCollision()
{  
  if (gameStart == true && collisionDetection == true)
  {
    //Collision detection for line 0 of ship
    //Collision detection for UFO1. Dont need line 0 of UFO because line 0 of ship cannot touch it anyway.
    if (ufo1Exist == true)
    {
      
      for (int a = 0; a < 2; a++)
      {
          if (shipx == ufo1x+a && shipy == ufo1y+1) ///Line 1
        {
          ShipHit();
        }
      } 
    //Collision detection for line 1 of ship..................
    //Contains extra for loop to loop through all the ship line 1 pixels
    //Collision detection for UFO1
      for (int b = 0; b < 1; b++)   //Had b < 2 and the collision detection worked but looked wonky when moving towards ufos
      {
        for (int a = 0; a < 2; a++)
        {
            if (shipx+b == ufo1x+a && shipy+1 == ufo1y+1)     //Line 1
          {
            ShipHit();
          }
        }
           
        for (int a = 1; a < 3; a++)
        {
          if (shipx+b == ufo1x+a && shipy+1 == ufo1y)         //Line 0
          {
            ShipHit();
          }
        }
      }     
    }
    //Collision detection for line 0 of ship    
    //collision detection for UFO2
    if (ufo2Exist == true)
    {
      
      for (int a = 0; a < 2; a++)
      {
          if (shipx == ufo2x+a && shipy == ufo2y+1) ///Line 1
        {
          ShipHit();
        }
      }
    //Collision detection for line 1 of ship
    //Collision detection for UFO2 
      for (int b = 0; b < 1; b++)
      {
        for (int a = 0; a < 2; a++)
        {
            if (shipx+b == ufo2x+a && shipy+1 == ufo2y+1)     //Line 1
          {
            ShipHit();
          }
        }
           
        for (int a = 1; a < 3; a++)
        {
          if (shipx+b == ufo2x+a && shipy+1 == ufo2y)         //Line 0
          {
            ShipHit();
          }
        }
      }     
    }    
    //Collision detection for line 0 of ship
    //Collision detection for UFO3
    if (ufo3Exist == true)
    {
      
      for (int a = 0; a < 2; a++)
      {
          if (shipx == ufo3x+a && shipy == ufo3y+1) ///Line 1
        {
          ShipHit();
        }
      } 

      //Contains extra for loop to loop through all the ship line 1 pixels
      //Collision detection for UFO3
      for (int b = 0; b < 1; b++)
      {
        for (int a = 0; a < 2; a++)
        {
            if (shipx+b == ufo3x+a && shipy+1 == ufo3y+1)     //Line 1
          {
            ShipHit();
          }
        }
           
        for (int a = 1; a < 3; a++)
        {
          if (shipx+b == ufo3x+a && shipy+1 == ufo3y)         //Line 0
          {
            ShipHit();
          }
        }
      }     
    }
    //Collision detection for line 0 of ship    
    //Collision detection for Bouncer1. Don't need line 0 and 1 becuase line 0 and 1 of bouncers and line 0 of ship never meet
    if (bouncer1Exist == true)
    {
      for (int a = 1; a < 2; a++)
      {
        if (shipx == bouncer1x+a && shipy == bouncer1y+2) //Line 2
        {
          ShipHit();
        }
      }

      //Collision detection for Bouncer1 and Line 1 of ship
      for (int b = 0; b < 1; b++)
      {
        for (int a = 1; a < 2; a++)
        {
          if (shipx+b == bouncer1x+a && shipy+1 == bouncer1y)   //Line 0
          {
              ShipHit();
          }
        }
        for (int a = 0; a < 2; a++)
        {
          if (shipx+b == bouncer1x+a && shipy+1 == bouncer1y+1)       //Line 1
          {
              ShipHit();
          }
        }
        for (int a = 1; a < 2; a++)
        {
          if (shipx+b == bouncer1x+a && shipy+1 == bouncer1y+2)       //Line 2
          {
              ShipHit();
          }
        }
      }
    }    
    //Collision detection for line 0 of ship    
    //Collision detection for Bouncer2. Don't need line 0 and 1 because line 0 and 1 of bouncers and line 0 of ship never meet
    if (bouncer2Exist == true)
    {
     
      for (int a = 1; a < 2; a++)
      {
        if (shipx == bouncer2x+a && shipy == bouncer2y+2) //Line 2
        {
          ShipHit();
        }
      }

       //Collision detection for Bouncer2 and Line 1 of ship
          
      for (int b = 0; b < 1; b++)
      {
        for (int a = 1; a < 2; a++)
        {
          if (shipx+b == bouncer2x+a && shipy+1 == bouncer2y)   //Line 0
          {
              ShipHit();
          }
        }
        for (int a = 0; a < 2; a++)
        {
          if (shipx+b == bouncer2x+a && shipy+1 == bouncer2y+1)       //Line 1
          {
              ShipHit();
          }
        }
        for (int a = 1; a < 2; a++)
        {
          if (shipx+b == bouncer2x+a && shipy+1 == bouncer2y+2)       //Line 2
          {
              ShipHit();
          }
        }
      }
    }    


    
    //Collision detection for line 0 of ship    
    //Collision detection for Bouncer3. Don't need line 0 and 1 becuase line 0 and 1 of bouncers and line 0 of ship never meet
    if (bouncer3Exist == true)
    {
     
      for (int a = 1; a < 2; a++)
      {
        if (shipx == bouncer3x+a && shipy == bouncer3y+2) //Line 2
        {
          ShipHit();
        }
      }

    //Collision detection for Bouncer3 and Line 1 of ship    
      for (int b = 0; b < 1; b++)
      {
        for (int a = 1; a < 2; a++)
        {
          if (shipx+b == bouncer3x+a && shipy+1 == bouncer3y)   //Line 0
          {
              ShipHit();
          }
        }
        for (int a = 0; a < 2; a++)
        {
          if (shipx+b == bouncer3x+a && shipy+1 == bouncer3y+1)       //Line 1
          {
              ShipHit();
          }
        }
        for (int a = 1; a < 2; a++)
        {
          if (shipx+b == bouncer3x+a && shipy+1 == bouncer3y+2)       //Line 2
          {
              ShipHit();
          }
        }
      }      
    }    
    
    //Collision detection for line 0 of ship    
    //Collision detection for Bouncer4. Don't need line 0 and 1 becuase line 0 and 1 of bouncers and line 0 of ship never meet
    if (bouncer4Exist == true)
    {
 
      for (int a = 1; a < 2; a++)
      {
        if (shipx == bouncer4x+a && shipy == bouncer4y+2) //Line 2
        {
          ShipHit();
        }
      }

     //Collision detection for Bouncer4 and Line 1 of ship
      for (int b = 0; b < 1; b++)
      {
        for (int a = 1; a < 2; a++)
        {
          if (shipx+b == bouncer4x+a && shipy+1 == bouncer4y)   //Line 0
          {
              ShipHit();
          }
        }
        for (int a = 0; a < 2; a++)
        {
          if (shipx+b == bouncer4x+a && shipy+1 == bouncer4y+1)       //Line 1
          {
              ShipHit();
          }
        }
        for (int a = 1; a < 2; a++)
        {
          if (shipx+b == bouncer4x+a && shipy+1 == bouncer4y+2)       //Line 2
          {
              ShipHit();
          }
        }
      }      
    }    



//Dont need collision detection for lines 0 and 1 of LT and line 0 of ship because they cant touch anyway
//Collision detection routines for LT
    if (ltExist == true)
    {
      
      for (int a = 2; a < 4; a++)           //Line 2
      {
        if (shipx == ltx+a && shipy == lty+2)
        {
          ShipHit();
        }
      }

    //Collision detection routines for LT and Ship line 1
      for (int b = 0; b < 2; b++)
      {
        for (int a = 1; a < 4; a++)           //Top line 0
        {
          if (shipx+b == ltx+a && shipy+1 == lty)
          {
            ShipHit();
          }
        }
        for (int a = 0; a < 5; a++)           //Line 1
        {
          if (shipx+b == ltx+a && shipy+1 == lty+1)
          {
            ShipHit();
          }
        }
        for (int a = 2; a < 4; a++)           //Line 2
        {
          if (shipx+b == ltx+a && shipy+1 == lty+2)
          {
            ShipHit();
          }
        }
      }           
    }
    
//Dont need collision detection between line 0 of ship and line 0 and 1 of fallers because they will never touch
//Collision detection for Faller 1
    if (faller1Exist == true)
    {
      
      for (int a = 0; a < 3; a++)          //Line 2
      {
        if (shipx == faller1x+a && shipy == faller1y+2)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
          ShipHit();
        }
      }

      //Collision detection for Faller 1 and ship line 1
      for (int b = 0; b < 1; b++)
      {
        for (int a = 0; a < 2; a++)         //Top Line 0
        {
          if (shipx+b == faller1x+a && shipy+1 == faller1y)  
          {
            ShipHit();
          }
        }
        for (int a = 0; a < 3; a++)          //Line 1
        {
          if (shipx+b == faller1x+a && shipy+1 == faller1y+1)    
          {
            ShipHit();
          }
        }
        for (int a = 0; a < 3; a++)          //Line 2
        {
          if (shipx+b == faller1x+a && shipy+1 == faller1y+2)  
          {
            ShipHit();
          }
        }
      }           
    }
    //Dont need collision detection between line 0 of ship and line 0 and 1 of fallers because they will never touch
    //Collision detection for Faller 2
   if (faller2Exist == true)
    {
      
      for (int a = 0; a < 3; a++)          //Line 2
      {
        if (shipx == faller2x+a && shipy == faller2y+2)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
          ShipHit();
        }
      }

      //Collision detection for Faller 2 and ship line 1
      for (int b = 0; b < 1; b++)
      {
        for (int a = 0; a < 2; a++)         //Top Line 0
        {
          if (shipx+b == faller2x+a && shipy+1 == faller2y)  
          {
            ShipHit();
          }
        }
        for (int a = 0; a < 3; a++)          //Line 1
        {
          if (shipx+b == faller2x+a && shipy+1 == faller2y+1)    
          {
            ShipHit();
          }
        }
        for (int a = 0; a < 3; a++)          //Line 2
        {
          if (shipx+b == faller2x+a && shipy+1 == faller2y+2)  
          {
            ShipHit();
          }
        }
      }           
    }
    //Dont need collision detection between line 0 of ship and line 0 and 1 of fallers because they will never touch
    //Collision detection for Faller 3
    if (faller3Exist == true)
    {
      
      for (int a = 0; a < 3; a++)          //Line 2
      {
        if (shipx == faller3x+a && shipy == faller3y+2)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
          ShipHit();
        }
      }

      //Collision detection for Faller 3 and ship line 1
      for (int b = 0; b < 1; b++)
      {
        for (int a = 0; a < 2; a++)         //Top Line 0
        {
          if (shipx+b == faller3x+a && shipy+1 == faller3y)  
          {
            ShipHit();
          }
        }
        for (int a = 0; a < 3; a++)          //Line 1
        {
          if (shipx+b == faller3x+a && shipy+1 == faller3y+1)    
          {
            ShipHit();
          }
        }
        for (int a = 0; a < 3; a++)          //Line 2
        {
          if (shipx+b == faller3x+a && shipy+1 == faller3y+2)  
          {
            ShipHit();
          }
        }
      }      
    }
    //Dont need collision detection between line 0 of ship and line 0 and 1 of fallers because they will never touch
    //Collision detection for Faller 4
   if (faller4Exist == true)
    {
      
      for (int a = 0; a < 3; a++)          //Line 2
      {
        if (shipx == faller4x+a && shipy == faller4y+2)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
          ShipHit();
        }
      }

      //Collision detection for Faller 4 and ship line 1
      for (int b = 0; b < 1; b++)
      {
        for (int a = 0; a < 2; a++)         //Top Line 0
        {
          if (shipx+b == faller4x+a && shipy+1 == faller4y)  
          {
            ShipHit();
          }
        }
        for (int a = 0; a < 3; a++)          //Line 1
        {
          if (shipx+b == faller4x+a && shipy+1 == faller4y+1)    
          {
            ShipHit();
          }
        }
        for (int a = 0; a < 3; a++)          //Line 2
        {
          if (shipx+b == faller4x+a && shipy+1 == faller4y+2)  
          {
            ShipHit();
          }
        }
      }      
    }
    //Dont need collision detection between line 0 of ship and lines 0 and 1 of  risers because they'll never touch
    //Collision detection for Riser 1
    if (riser1Exist == true)
    {
     
      for (int a = 0; a < 1; a++)         //Line 2
      {      
        if (shipx == riser1x+a && shipy == riser1y+2)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
           ShipHit();
        }
      }

      //Collision detection for Riser 1 and Ship line 1
      for (int b = 0; b < 1; b++)
      {
        for (int a = 0; a < 3; a++)          //Top Line 0
        {
          if (shipx+b == riser1x+a && shipy+1 == riser1y)   
          {
            ShipHit();
          }
        }      
        for (int a = 0; a < 2; a++)          //Line 1
        {
          if (shipx+b == riser1x+0 && shipy+1 == riser1y+1)   
          {
            ShipHit();
          }
        }
        for (int a = 0; a < 1; a++)         //Line 2
        {      
          if (shipx+b == riser1x+a && shipy+1 == riser1y+2)   
          {
            ShipHit();
          }
        }
      }      
    }      
    
   //Dont need collision detection between line 0 of ship and lines 0 and 1 of  risers because they'll never touch
   //Collision detection for Riser 2
   if (riser2Exist == true)
    {
      
      for (int a = 0; a < 1; a++)         //Line 2
      {      
        if (shipx == riser2x+a && shipy == riser2y+2)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
          ShipHit();
        }
      }

      //Collision detection for Riser 2 and Ship line 1
      for (int b = 0; b < 1; b++)
      {
        for (int a = 0; a < 3; a++)          //Top Line 0
        {
          if (shipx+b == riser2x+a && shipy+1 == riser2y)   
          {
            ShipHit();
          }
        }      
        for (int a = 0; a < 2; a++)          //Line 1
        {
          if (shipx+b == riser2x+0 && shipy+1 == riser2y+1)   
          {
            ShipHit();
          }
        }
        for (int a = 0; a < 1; a++)         //Line 2
        {      
          if (shipx+b == riser2x+a && shipy+1 == riser2y+2)   
          {
            ShipHit();
          }
        }
      }      
    }      
//Dont need collision detection between line 0 of ship and lines 0 and 1 of  risers because they'll never touch
      //Collision detection for Riser 3
    if (riser3Exist == true)
    {
      
      for (int a = 0; a < 1; a++)         //Line 2
      {      
        if (shipx == riser3x+a && shipy == riser3y+2)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
          ShipHit();
        }
      }

      //Collision detection for Riser 3 and Ship line 1
      for (int b = 0; b < 1; b++)
      {
        for (int a = 0; a < 3; a++)          //Top Line 0
        {
          if (shipx+b == riser3x+a && shipy+1 == riser3y)   
          {
            ShipHit();
          }
        }      
        for (int a = 0; a < 2; a++)          //Line 1
        {
          if (shipx+b == riser3x+0 && shipy+1 == riser3y+1)   
          {
            ShipHit();
          }
        }
        for (int a = 0; a < 1; a++)         //Line 2
        {      
          if (shipx+b == riser3x+a && shipy+1 == riser3y+2)   
          {
            ShipHit();
          }
        }
      }      
    }  
    //Dont need collision detection between line 0 of ship and lines 0 and 1 of  risers because they'll never touch    
      //Collision detection for Riser 4
    if (riser4Exist == true)
    {
      
      for (int a = 0; a < 1; a++)         //Line 2
      {      
        if (shipx == riser4x+a && shipy == riser4y+2)   //Checks to see if laser coords match bouncer bitmap coords, including the pixels just behind them.
        {
          ShipHit();
        }
      }

       //Collision detection for Riser 4 and Ship line 1
      for (int b = 0; b < 1; b++)
      {
        for (int a = 0; a < 3; a++)          //Top Line 0
        {
          if (shipx+b == riser4x+a && shipy+1 == riser4y)   
          {
            ShipHit();
          }
        }      
        for (int a = 0; a < 2; a++)          //Line 1
        {
          if (shipx+b == riser4x+0 && shipy+1 == riser4y+1)   
          {
            ShipHit();
          }
        }
        for (int a = 0; a < 1; a++)         //Line 2
        {      
          if (shipx+b == riser4x+a && shipy+1 == riser4y+2)   
          {
            ShipHit();
          }
        }
      }      
    }

    //No collision detection for line 0 of ship and boss because they'll never touch anyway.
    //Collision detection for Boss abd Ship line 1
    if (boss1Exist == true)
    {
      for (int b = 0; b < 1; b++)
      {
        for (int a = 3; a < 5; a++ )       //Top line 0
        {
          if (shipx+b == bossx+a && shipy+1 == bossy)
          {
            ShipHit();
          }
        }
        for (int a = 2; a < 5; a++)         //Line 1
        {
          if (shipx+b == bossx+a && shipy+1 == bossy+1)
          {
            ShipHit();
          }
        }
        for (int a = 1; a < 6; a++)          //Line 2
        {
          if (shipx+b == bossx+a && shipy+1 == bossy+2)
          {
            ShipHit();
          }
        }
        for (int a = 0; a < 7; a++)          //Line 3
        {
          if (shipx+b == bossx+a && shipy+1 == bossy+3)
          {
            ShipHit();
          }
        }
        for (int a = 2; a < 6; a++)           //Line 4
        {
          if (shipx+b == bossx+a && shipy+1 == bossy+4)
          {
            ShipHit();
          }
        }
        for (int a = 0; a < 7; a++)           //Line 5
        {
          if (shipx+b == bossx+a && shipy+1 == bossy+5)
          {
            ShipHit();
          }
        }
        for (int a = 1; a < 6; a++)            //Line 6
        {
          if (shipx+b == bossx+a && shipy+1 == bossy+6)
          {
            ShipHit();
          }
        }        
        for (int a = 2; a < 5; a++)            //Line 7
        {
          if (shipx+b == bossx+a && shipy+1 == bossy+7)
          {
            ShipHit();
          }
        }
      }     
    }
    
    //Collision detection for UFO Lasers. They only have line 0
    if (ufoLasExist1 == true)
    {
      
      for (int a = 0; a < 1; a++)
      {
          if (shipx == ufoLas1x+a && shipy == ufoLas1y) ///Line 0
        {
          ShipHit();
        }
      }
      for (int b = 0; b < 1; b++)
      {
        for (int a = 0; a < 1; a++)
        {
            if (shipx+b == ufoLas1x+a && shipy+1 == ufoLas1y) ///Line 1
          {
            ShipHit();
          }
        }
      }            
    }
    //Collision detection for UFO Lasers. They only have line 0
    if (ufoLasExist2 == true)
    {
      
      for (int a = 0; a < 1; a++)
      {
          if (shipx == ufoLas2x+a && shipy == ufoLas2y) ///Line 0
        {
          ShipHit();
        }
      }
      for (int b = 0; b < 1; b++)
      {
        for (int a = 0; a < 1; a++)
        {
            if (shipx+b == ufoLas2x+a && shipy+1 == ufoLas2y) ///Line 1
          {
            ShipHit();
          }
        }
      }            
    } 
    //Collision detection for UFO Lasers. They only have line 0
    if (ufoLasExist3 == true)
    {
      
      for (int a = 0; a < 1; a++)
      {
          if (shipx == ufoLas3x+a && shipy == ufoLas3y) ///Line 0
        {
          ShipHit();
        }
      }
      for (int b = 0; b < 1; b++)
      {
        for (int a = 0; a < 1; a++)
        {
            if (shipx+b == ufoLas3x+a && shipy+1 == ufoLas3y) ///Line 1
          {
            ShipHit();
          }
        }
      }            
    }
    //Collision detection for UFO Lasers. They only have line 0
    if (ufoLasExist4 == true)
    {
      
      for (int a = 0; a < 1; a++)
      {
          if (shipx == ufoLas4x+a && shipy == ufoLas4y) ///Line 0
        {
          ShipHit();
        }
      }
      for (int b = 0; b < 1; b++)
      {
        for (int a = 0; a < 1; a++)
        {
            if (shipx+b == ufoLas4x+a && shipy+1 == ufoLas4y) ///Line 1
          {
            ShipHit();
          }
        }
      }            
    }
  }
}
void GameOver(int gameStatus)
{
  spawnEnabled = false;   
  if (gameStatus == 0)   //Game over due to death
  {
    tape = "Game Over :( Your score was ";
    tape += score;
   // Ticker(true);
    tape = "UFO ATTACK!   - Press fire to start ";
    lives = 5;
    score = 0;
    levelNumber = 1;
  }
  else if (gameStatus == 1)//Game over due to winning level 10
  {
    tape = "Congratulations! You defeated the alien invasion :) Your total score was ";
    tape += score;
    //Ticker(true);
    lives = 5;
    score = 0;
    levelNumber = 1;
  }
  else if (gameStatus == 2)//Life lost
  {
    tape = "Ship destroyed!  ";
    tape += "Lives left: ";
    tape += lives;   
  }
  

  ufo1Exist = false;
  ufo2Exist = false;
  ufo3Exist = false;
  bouncer1Exist = false;
  bouncer2Exist = false;
  bouncer3Exist = false;
  bouncer4Exist = false;
  faller1Exist = false;
  faller2Exist = false;
  faller3Exist = false;
  faller4Exist = false;
  riser1Exist = false;
  riser2Exist = false;
  riser3Exist = false;
  riser4Exist = false;
  ltExist = false;
  boss1Exist = false;
  gameStart = false;
  ufoLasExist1 = false;
  ufoLasExist2 = false;
  ufoLasExist3 = false;
  ufoLasExist4 = false;
  ufoLas1x = 34;
  ufoLas1y = 34;
  ufoLas2x = 34;
  ufoLas2y = 34;
  ufoLas3x = 34;
  ufoLas3y = 34;
  ufoLas4x = 34;
  ufoLas4y = 34;
  ufo1x = 34;
  ufo2x = 34;
  ufo3x = 34;
  bouncer1x = 34;
  bouncer2x = 34;
  bouncer3x = 34;
  bouncer4x = 34;
  faller1x = 34;
  faller2x = 34;
  faller3x = 34;
  faller4x = 34;
  riser1x = 34;
  riser2x = 34;
  riser3x = 34;
  riser4x = 34;
  ltx = 34;
  bossx = 34;
  shipx = 0;
  shipy = 3;
  lasExist = false;
  
if (gameStatus == 2)
{
  Ticker(false);
}
else if (gameStatus == 0 || gameStatus == 1)
{
  Ticker(true);
}
}


void ShipHit()
{  
  lives--;
  for (int i = 5000; i > 1; i = i-5)
  {
    tone(buzzerPin, i, 1);
    delay(1);
  }
  if (lives > 0)
  {
    GameOver(2);
  }
  else if (lives <= 0)
  {
    GameOver(0);
  }
}