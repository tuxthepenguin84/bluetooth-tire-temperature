/* Example 49.1
 Testing Arduino and KTM-S1201 LCD module
 Based on code by Jeff Albertson Ver 1.0
 Modifications and code normalisation by Robert W. Mech rob@mechsoftware.com
 Modified by John Boxall 11/March/2013  */

/////////////////////////////////To use the LCD you need all the lines from here////////////////////////////////////////////////////////////////////////////
#include "KTMS1201.h" // necessary header file
#include <i2cmaster.h>

// Constants for functions to ease use of display printing
static int anCount=39; // one less than actual since 0 based indexes.
static byte anConst[39]= {
  _A,_B,_C,_D,_E,_F,_G,_H,_I,_J,_K,_L,_M,_N,_O,_P,_Q,_R,_S,_T,_U,_V,_W,_X,_Y,_Z,_1,_2,_3,_4,_5,_6,_7,_8,_9,_0,_,_DASH,_DEGREE};
static byte anHumanConst[39]= {
  'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','1','2','3','4','5','6','7','8','9','0',' ','-','*'};

void ktmPrnNumb(int n, byte p,byte d,byte l)
/*
  ktmPrnNumb
 This function demonstrates how to output data and place decimal
 points in a specific place. 
 */
{
  // n is the number to be printed 
  // p is the position for lsb from right starting at 0 and 
  // d in the number of digits to the right of the decimal point
  // l is length number of digits in n
  int nt;
  int i;
  int j=1;
  int r=0;
  digitalWrite(LcdCnD, HIGH); //Put in command mode
  digitalWrite(LcdnCs, LOW); //Select LCD
  delay(1);
  ktmshiftOut(_Decode);
  ktmshiftOut(_LoadPtr+p*2);
  digitalWrite(LcdCnD, LOW);  	//Put in data mode
  digitalWrite(LcdnCs, LOW); 
  delay(1) ;
  nt = n;
  for (i=0;i<l;i++)//display numbers (reverse order)
  {
    ktmshiftOut(nt%10);
    nt=nt/10;
  }
  j=p*2+d*2;
  if (d>0)  //   Set Decimal point
  {
    digitalWrite(LcdCnD, HIGH); //Put in command mode
    digitalWrite(LcdnCs, LOW); //Select LCD
    delay(1);
    ktmshiftOut(_NoDecode);
    ktmshiftOut(_LoadPtr+j);
    ktmshiftOut(_SetDp);
    delay(1);
  }
  digitalWrite(LcdnCs, HIGH); //deselect LCD to display data
  delay(1);
}

void ktmInit()
/*
   ktmInit
 This function sets the display up for commands. 
 */
{
  pinMode(LcdSck,OUTPUT);
  pinMode(LcdSi,OUTPUT);
  pinMode(LcdCnD,OUTPUT);
  pinMode(LcdnRes,OUTPUT);
  pinMode(LcdnBus,INPUT);
  pinMode(LcdnCs,OUTPUT);
  // run these once at startup/reset to initialise the LCD module
  digitalWrite(LcdCnD, HIGH); // Put in copmmand mode
  digitalWrite(LcdnCs, HIGH); // deselect KTM-S1201
  digitalWrite(LcdSck, HIGH);
  digitalWrite(LcdSi,  HIGH);
  digitalWrite(LcdnRes, LOW); // reset lcd
  delay(10);
  digitalWrite(LcdnRes, HIGH);
  delay(10);
  // Setup command mode
  ktmCommand(_Mode);
  ktmCommand(_USync);
}

void ktmWriteString(String ktmMessage)
/*
    ktmWriteString(<Message>)
 This function will take a text string and convert it into a message for 
 the display.  You only need to send a message it will space out all other
 characters.
 */
{
  byte TextString[12]={
    _,_,_,_,_,_,_,_,_,_,_,_    }; // Ultimate buffer of characters to write.
  String message=ktmMessage;
  message+="            "; // pad to 12
  message=message.substring(0,12); // Make sure we only have 12 characters
  message.toUpperCase();
  // Iterate through human readable array and stuff TextString with hex values. 
  for(int i=0;i<12;i++){ // Outer loop is the message String
    for(int j=0;j<anCount;j++){ // Inner loop is maximum values of hex arrays.
      if(message[i]==anHumanConst[j]){ // If we match the human readable character populate the text string with constant. 
        TextString[i]=anConst[j];
      }
    }
  }
  digitalWrite(LcdCnD, LOW);  	//Put in data mode
  digitalWrite(LcdnCs, LOW); 
  delay(1);
  for (int i = 11; i >= 0; i --)//Put in KTM-S1201 Right Character First
  {
    ktmshiftOut(TextString[i]);
  }
  digitalWrite(LcdnCs, HIGH); //deselect LCD to display data
  delay(1);                    //always delay after LcdnCs change
}

void ktmCommand(byte _cmd)
// necessary for other LCD functions
{
  digitalWrite(LcdCnD, HIGH);
  delay(1);
  digitalWrite(LcdnCs, LOW);
  delay(1);
  ktmshiftOut(_cmd);
  delay(1);
  digitalWrite(LcdnCs, HIGH); //deselect LCD to display data
  delay(1);
}

void ktmshiftOut(byte val)
// necessary for other LCD functions
// same as shiftout command but invert Sck
{
  int i;
  for (i=0;i<8;i++)
  {
    digitalWrite(LcdSi,!!(val & (1<< (7-i))));
    digitalWrite(LcdSck,LOW);
    delay(1);
    digitalWrite(LcdSck,HIGH);
  }
  delay(1);
}

void ktmWriteHex(int hexVal,int pos)
/*
  ktmWriteHex
 Writes any value in any position. 
 */
{
  byte TextString[12]={
    _,_,_,_,_,_,_,_,_,_,_,_    }; // Ultimate buffer of characters to write.
  // Iterate through human readable array and stuff TextString with hex values. 
  TextString[pos]=hexVal;
  digitalWrite(LcdCnD, LOW);  	//Put in data mode
  digitalWrite(LcdnCs, LOW); 
  delay(1);
  for (int i = 11; i >= 0; i --)//Put in KTM-S1201 Right Character First
  {
    ktmshiftOut(TextString[i]);
  }
  digitalWrite(LcdnCs, HIGH); //deselect LCD to display data
  delay(1);                    //always delay after LcdnCs change
}

/////////////////////////////////... to here, and the five lines in void setup() ////////////////////////////////////////////////////////////////////////////
void setup()
{
  ktmInit(); // Runs inital reset and prepares the display for commands. 
  ktmCommand(_NoBlink);  // Optional _SBlink to blink display. 
  ktmCommand(_DispOn);   // Turn on display
  ktmCommand(_NoDecode); 
  ktmCommand(_ClearDsp); 
  Serial.begin(9600);

  i2c_init(); //Initialise the i2c bus
  PORTC = (1 << PORTC4) | (1 << PORTC5);//enable pullups
}

void loop()
{
  const int temperaturePin = 0;
  float voltage, degreesC, degreesF;
  char lcdtmp[4]; //temperature value for lcd, capable of 3 digits (4th is the null value)
  char phonetmp[5]; //temperature value for phone 
  char lcdtmpUnit[] = "*F"; //temperature units
  char lcdshow1[13]; //lcd final screen
  char phoneshow1[10]; //phone final screen
  
  //This converts high and low bytes together and processes temperature, MSB is a error bit and is ignored for temps
  double tempFactor = 0.02; // 0.02 degrees per LSB (measurement resolution of the MLX90614)
  double tempData = 0x0000; // zero out the data
  int frac; // data past the decimal point

  int dev = 0x5A<<1;
  int data_low = 0;
  int data_high = 0;
  int pec = 0;
  i2c_start_wait(dev+I2C_WRITE);
  i2c_write(0x07);

  // read
  i2c_rep_start(dev+I2C_READ);
  data_low = i2c_readAck(); //Read 1 byte and then send ack
  data_high = i2c_readAck(); //Read 1 byte and then send ack
  pec = i2c_readNak();
  i2c_stop();

  // This masks off the error bit of the high byte, then moves it left 8 bits and adds the low byte.
  tempData = (double)(((data_high & 0x007F) << 8) + data_low);
  tempData = (tempData * tempFactor)-0.01;

  float celcius = tempData - 273.15;
  float fahrenheit = (celcius*1.8) + 32;

  //lcd can only output strings (not floats or doubles)
  dtostrf(fahrenheit,3,0,lcdtmp); //float to string, 3 total characters with 0 precision after decimal
  dtostrf(fahrenheit,4,0,phonetmp); //float to string, 4 total characters with 0 precision after decimal

  //start combining strings together to show on LCD. combines lcdtmp & lcdUnit into lcdshow1
  lcdshow1[0] = '\0'; //This first value has to be null, I believe this an indicator of where to start, if you leave this out stuff starts overlapping
  strcat(lcdshow1, lcdtmp); //first string value
  strcat(lcdshow1, lcdtmpUnit); //second string value
  
  //phoneshow1[0] = '\0';
  //strcat(phoneshow1, phonetmp);
  //strcat(phoneshow1, runavg);
  
  ktmWriteString(lcdshow1); //write to LCD
  Serial.println(phonetmp); //write to Serial
  
  delay(100);
}



