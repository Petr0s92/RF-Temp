#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <VirtualWire.h>
#include <EasyTransferVirtualWire.h>
#include "CustomChars.h"

int con = 0;

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

EasyTransferVirtualWire ET; // create object
struct SEND_DATA_STRUCTURE {
	// put your variable definitions here for the data you want to send
	// THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
	// Struct can'e be bigger then 26 bytes for VirtualWire version
	float tempCrx;
	float tempC2rx;
};

// give a name to the group of data
SEND_DATA_STRUCTURE mydata;

void setup() {
	Serial.begin(9600);        // Debugging only
	ET.begin(details(mydata)); // start the library, pass in the data details
	Serial.println("setup");
	pinMode(13, OUTPUT);
	// Initialise the IO and ISR
	vw_set_ptt_inverted(true); // Required for DR3100
	vw_setup(2000);            // Bits per sec
	vw_rx_start();             // Start the receiver PLL running
	lcd.begin(20, 4);          // initialize the lcd
	lcd.createChar(1, thermometer);
	lcd.createChar(2, droplet);
	lcd.createChar(3, hi);
	lcd.clear();
	con = 0;
	randomSeed(analogRead(0));
	//  lcd.backlight();
}

void loop() {
	if (con > 50) { con = 0; }
	// CPU Count debug
	//  Serial.println(cpu);

	// check and see if a data packet has come in.
	if (ET.receiveData()) {
		// this is how you access the variables. [name of the group].[variable
		// name]
		// since we have data, we will blink it out.
		con = 0;

		// Serial debuggin stuff
		Serial.print(mydata.tempCrx);
		Serial.print(" ");
		Serial.print(mydata.tempC2rx);
		Serial.println("");

		// What what?
		lcd.createChar(0, heart);
		lcd.setCursor(17, 3);
		lcd.write((byte)0);
		lcd.setCursor(0, 0);
		lcd.print("Solar: ");
		lcd.setCursor(0, 1);

		// tempC ????
		lcd.setCursor(11, 0);
		lcd.print("Boiler: ");
		lcd.setCursor(11, 1);

		if (mydata.tempCrx == -127.00) {
			lcd.setCursor(1, 1);
			lcd.print("Error    ");
		} else {
			lcd.write(1);
			lcd.print(String(mydata.tempCrx, 2) + char(223) + "C ");
		}

		if (mydata.tempC2rx == -127.00) {
			lcd.setCursor(12, 1);
			lcd.print("Error    ");
		} else {
			lcd.write(1);
			lcd.print(String(mydata.tempC2rx, 2) + char(223) + "C ");
		}

		// Blink for the temps!
		digitalWrite(13, HIGH);
		delay(100);

	} else {
		digitalWrite(13, LOW);
		lcd.setCursor(17, 3);
		lcd.print(" ");
		delay(900);
		con++;
		if (con > 10) { checkconnection(); }
	}
	Serial.println(con);
}

// buf[buflen] = '\0';
//      digitalWrite(13, HIGH);

//
//      if (solar.toFloat() >= 55) {
//        lcd.setCursor(0, 3);
//        lcd.print("Water ready!");
//        delay(350);
//        lcd.setCursor(0, 3);
//        lcd.print("                   ");
//      }
//      else {
//        lcd.setCursor(0, 3);
//        lcd.print("                   ");
//        digitalWrite(13, LOW);
//      }
//      if (boiler.toFloat() >= 55) {
//        lcd.setCursor(0, 3);
//        lcd.print("Water ready!");
//        delay(350);
//        lcd.setCursor(0, 3);
//        lcd.print("                    ");
//      }
//      else {
//        lcd.setCursor(0, 3);
//        lcd.print("                    ");
//        digitalWrite(13, LOW);
//      }
//    }
//  } else {
//
//    delay(1100);
//    con++;
//    if (con > 2) {
//      checkconnection();
//
//    }
//    Serial.println(con);
//  }
//}
void checkconnection() {
	lcd.setCursor(17, 3);
	lcd.print(" ");
	digitalWrite(13, LOW);
	String solar = "--.--";
	String boiler = "--.--";
	lcd.setCursor(0, 0);
	lcd.print("Solar: ");
	lcd.setCursor(0, 1);
	// tempC
	lcd.write(1);
	lcd.print(solar + char(223) + "C ");
	lcd.setCursor(11, 0);
	lcd.print("Boiler: ");
	lcd.setCursor(11, 1);
	lcd.write(1);
	lcd.print(boiler + char(223) + "C ");
}
