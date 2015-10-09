#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <VirtualWire.h>
#include "CustomChars.h"
#undef int
#undef abs
#undef double
#undef float
#undef round
char myArray[32];// array needed to store data for string
byte myCounter; // counter for storing data
int myValue;  // value to store incoming data
int con = 0;

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

void setup()
{
  Serial.begin(9600);	// Debugging only
  Serial.println("setup");
  pinMode(13, OUTPUT);
  // Initialise the IO and ISR
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(2000);	 // Bits per sec
  vw_rx_start();       // Start the receiver PLL running
  lcd.begin(20, 4);                     // initialize the lcd
  lcd.createChar(1, thermometer);
  lcd.createChar(2, droplet);
  lcd.createChar(3, hi);
  lcd.clear();
  con = 0;
  //  lcd.backlight();

}

void loop()
{
  if (con>50) { con=0; }
  //CPU Count debug
  //  Serial.println(cpu);
  uint8_t buflen = VW_MAX_MESSAGE_LEN; //Maximum length of the message
  uint8_t buf[buflen]; // The buffer that holds the message
  if (vw_have_message()) {
    if (vw_get_message(buf, &buflen))
    {
      con = 0;
      buf[buflen] = '\0';
      //Serial.println((char *)buf);
      String solar = getValue((char *)buf, ' ', 0);   //I love
      String boiler = getValue((char *)buf, ' ', 1);   //Noodles
      Serial.println(solar);
      Serial.println(boiler);

      digitalWrite(13, HIGH);
      lcd.createChar(0, heart);
      lcd.setCursor(17, 3);
      lcd.write((byte)0);
      lcd.setCursor(0, 0);
      lcd.print("Solar: ");
      lcd.setCursor(0, 1);
      //tempC
      lcd.write(1);
      lcd.print(solar + char(223) + "C ");
      lcd.setCursor(11, 0);
      lcd.print("Boiler: ");
      lcd.setCursor(11, 1);
      lcd.write(1);
      lcd.print(boiler + char(223) + "C ");

      if (solar.toFloat() >= 55) {
        lcd.setCursor(0, 3);
        lcd.print("Water ready!");
        delay(350);
        lcd.setCursor(0, 3);
        lcd.print("                   ");
      }
      else {
        lcd.setCursor(0, 3);
        lcd.print("                   ");
        digitalWrite(13, LOW);
      }
      if (boiler.toFloat() >= 55) {
        lcd.setCursor(0, 3);
        lcd.print("Water ready!");
        delay(350);
        lcd.setCursor(0, 3);
        lcd.print("                    ");
      }
      else {
        lcd.setCursor(0, 3);
        lcd.print("                    ");
        digitalWrite(13, LOW);
      }
    }
  } else {

    delay(1100);
    con++;
    if (con > 2) {
      checkconnection();

    }
    Serial.println(con);
  }
}
String getValue(String data, char separator, int index)
{
  int maxIndex = data.length() - 1;
  int j = 0;
  String chunkVal = "";
  for (int i = 0; i <= maxIndex && j <= index; i++)
  {
    chunkVal.concat(data[i]);
    if (data[i] == separator)
    {
      j++;
      if (j > index)
      {
        chunkVal.trim();
        return chunkVal;
      }
      chunkVal = "";
    }
  }
}
void checkconnection() {
  String solar = "--.--";
  String boiler = "--.--";
  lcd.setCursor(0, 0);
  lcd.print("Solar: ");
  lcd.setCursor(0, 1);
  //tempC
  lcd.write(1);
  lcd.print(solar + char(223) + "C ");
  lcd.setCursor(11, 0);
  lcd.print("Boiler: ");
  lcd.setCursor(11, 1);
  lcd.write(1);
  lcd.print(boiler + char(223) + "C ");
}
