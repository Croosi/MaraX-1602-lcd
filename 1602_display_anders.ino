// include the library code:
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>


//Defines
#define BUFFER_SIZE 32

// Creates an LCD object. Parameters: (rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(14, 15, 5, 4, 3, 2);

//Pins
int d0 = 0; //orange PIN 4 Mara TX to Arduino RX D5
int d1 = 1; //black  PIN 3 Mara RX to Arduino TX D6

//Internals
long lastMillis;
int seconds = 0;
int lastTimer = 0;
long serialTimeout = 0;
char buffer[BUFFER_SIZE];
int index = 0;
char line0[16]; 
char line1[16];
int laMaDa3;
int laMaDa1;
char Pump;
int last;
String Mode;
char Heat;
int actual;



//Forms
//byte Pump[8];
//byte Heat[8];

/*byte CircleEmpty[8] =
  {
    0b00000,
    0b00000,
    0b01110,
    0b10001,
    0b10001,
    0b01110,
    0b00000,
    0b00000
  };

  byte CircleFull[8] =
  {
    0b00000,
    0b00000,
    0b01110,
    0b11111,
    0b11111,
    0b01110,
    0b00000,
    0b00000
  };

  byte QuaderEmpty[8] =
  {
    0b00000,
    0b00000,
    0b11111,
    0b10001,
    0b10001,
    0b11111,
    0b00000,
    0b00000
  };

  byte QuaderFull[8] =
  {
    0b00000,
    0b00000,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b00000,
    0b00000
  };
*/

//Mara Data
String maraData[7];
String* lastMaraData;

//Instances
SoftwareSerial mySerial(d0, d1);

void setup() 
{
  lcd.begin(16, 2);
  lcd.clear();
  
  mySerial.begin(9600);
  memset(buffer, 0, BUFFER_SIZE);
  mySerial.write(0x11);
/*lcd.createChar(0, CircleEmpty);
lcd.createChar(1, CircleFull);
lcd.createChar(2, QuaderEmpty);
lcd.createChar(3, QuaderFull);
*/

// Print a message to the LCD.
  lcd.setCursor(16, 0);
  lcd.print("  Lelit Mara X");
    // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(16, 1);
  // Print a message to the LCD.
  lcd.print("     V2.0");
  
    // scroll 13 positions (string length) to the left to move it offscreen left:
  //for (int positionCounter = 0; positionCounter < 13; positionCounter++) {
    //lcd.scrollDisplayLeft(); // scroll one position left:
    //delay(150);              // wait a bit
  //}

  // scroll 29 positions (string length + display length) to the right to move it offscreen right:
  //for (int positionCounter = 0; positionCounter < 29; positionCounter++) {
    //lcd.scrollDisplayRight(); // scroll one position right
    //delay(150);               // wait a bit
  //}

  // scroll 16 positions (display length + string length) to the left to move it back to center:
  for (int positionCounter = 0; positionCounter < 16; positionCounter++) {
    lcd.scrollDisplayLeft(); // scroll one position left
    delay(150); // wait a bit
  }
  
  delay (2500);
  lcd.clear();
  delay (1000);

  
  
}



void getMaraData()
{
  /*
    Example Data: C1.06,116,124,093,0840,1,0\n every ~400-500ms
    Length: 26
    [Pos] [Data] [Describtion]
    0)      C     Coffee Mode (C) or SteamMode (V)
    -        1.06  Software Version
    1)      116   current steam temperature (Celsisus)
    2)      124   target steam temperature (Celsisus)
    3)      093   current hx temperature (Celsisus)
    4)      0840  countdown for 'boost-mode'
    5)      1     heating element on or off
    6)      0     pump on or off
  */

  while (mySerial.available())
  {
    serialTimeout = millis();
    char rcv = mySerial.read();
    if (rcv != '\n')
      buffer[index++] = rcv;
    else {
      index = 0;
      Serial.println(buffer);
      char* ptr = strtok(buffer, ",");
      int idx = 0;
      while (ptr != NULL)
      {
        maraData[idx++] = String(ptr);
        ptr = strtok(NULL, ",");
      }
      lastMaraData = maraData;
    }
  }
  if (millis() - serialTimeout > 6000)
  {
    serialTimeout = millis();
    mySerial.write(0x11);
  }
}

void updateView()
{
  lcd.clear();
  
  //HX
  //lcd.setCursor(0, 1);
  int laMaDa3 = lastMaraData[3].toInt();
  Serial.print(laMaDa3);
  //Pump
  //lcd.setCursor(0, 7);
  if (lastMaraData[5].toInt() == 0)
    char Pump = " ";
  else
    char Pump = "X";
  
  //Steam
  //lcd.setCursor(1, 1);
  int laMaDa1 = lastMaraData[1].toInt();
  Serial.print(laMaDa1);
  //Timer
  //lcd.setCursor(0, 10);
    if (lastTimer >3)
    {
    String last = String(lastTimer);
    if (last.length()< 2)
      last = "0" + last;
    }
  else
  {
    char last = "00";
  }

  //Mode
  //lcd.setCursor(0, 14);
  Mode = (lastMaraData[0].substring(0, 1));
  Serial.print(lastMaraData[0]);
  
  //Heat
  //lcd.setCursor(1, 7);
  if (lastMaraData[6].toInt() == 0)
    char Heat = " ";
  else
    char Heat = "X";
 
  //lcd.setCursor(1, 10);
  String actual = String(seconds);
  actual = "0" + actual;
  
    
  
 }



void loop()
{


  
  getMaraData();
  int pumpState = lastMaraData[6].toInt();
  if (pumpState == 1)
  {
    if ( millis() - lastMillis >= 1000)
    {
      lastMillis = millis();
      ++seconds;
      if (seconds > 99)
        seconds = 0;
    }
  }
  else
  {
    if (seconds != 0)
      lastTimer = seconds;
    seconds = 0;
  }
  updateView();
lcd.clear();

lcd.setCursor(0,0);
   sprintf(line0,"  %d%cC  P%s   %ds  %c", laMaDa3, (char)223,  Pump, last, Mode);
   lcd.print(line0);
lcd.setCursor(0,1);   
   sprintf(line1,"  %d%cC  H%s   %ds", laMaDa1, (char)223,  Heat, actual);
   lcd.print(line1);
delay(400);

  
}
