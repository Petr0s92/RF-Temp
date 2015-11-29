#include <OneWire.h>
#include <DallasTemperature.h>
#include <VirtualWire.h>
#include <EasyTransferVirtualWire.h>
#define ONE_WIRE_BUS 3

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;

// Probes ID
DeviceAddress Probe01 = {0x28, 0x39, 0xD7, 0xB7, 0x01, 0x00, 0x00, 0xD6};
DeviceAddress Probe02 = {0x28, 0xB0, 0x02, 0xB8, 0x01, 0x00, 0x00, 0xBC};
float solar_temp;
float boiler_temp;
float boiler_status;
char charnum[10];
int led = 13;
int relay = 4;
int test = 0;
// create object
EasyTransferVirtualWire Temps;

struct SEND_DATA_STRUCTURE {
  // put your variable definitions here for the data you want to send
  // THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  // Struct can'e be bigger then 26 bytes for VirtualWire version
  float solar_temprx;
  float boiler_temprx;
  int boiler_statusrx;
};
SEND_DATA_STRUCTURE tempdata;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  pinMode(relay, OUTPUT);
  sensors.begin();
  // start the library, pass in the data details
  Temps.begin(details(tempdata));
  if (!sensors.getAddress(insideThermometer, 0))
    Serial.print("Unable to find address for Device 0");
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.print(" devices.");
  Serial.print("Address:");
  printAddress(insideThermometer);
  Serial.println();
  // set the resolution to 9 bit (Each Dallas/Maxim device is capable of
  // several different resolutions)
  sensors.setResolution(Probe01, 11);
  sensors.setResolution(Probe02, 11);
  // Init RF
  Serial.println("Init RF Transmission");

  // Initialise the IO and ISR
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(2000);            // Bits per sec
  randomSeed(analogRead(0));
}
void printTemperature(DeviceAddress deviceAddress) {

  // method 2 - faster

  solar_temp = sensors.getTempC(Probe01);
  boiler_temp = sensors.getTempC(Probe02);
}

void loop() {
  // put your main code here, to run repeatedly:
  sensors.requestTemperatures(); // Send the command to get temperatures
  printTemperature(insideThermometer);
  printTemperature(Probe01);
  printTemperature(Probe02);

  // this is how you access the variables. [name of the group].[variable name]
  tempdata.solar_temprx = solar_temp;
  tempdata.boiler_temprx = boiler_temp;
  // send the data
  digitalWrite(led, LOW);
  Serial.print("temperature:");
  // Serial.print(solar_temp);
  Serial.print(",");
  Serial.print(boiler_temp);
  Serial.println();
  // Serial.print(tempdata.solar_temprx);
  Serial.print(",");
  Serial.print(tempdata.boiler_temprx);
  Serial.print(",");
  Serial.print(test);
  
  delay(700);
  // You can have more than one IC on the same bus.
  // 0 refers to the first IC on the wire
  if (tempdata.boiler_temprx >= 22.75) {
    digitalWrite(relay, HIGH);
    tempdata.boiler_statusrx = 1;

  } else {
    digitalWrite(relay, LOW);
    tempdata.boiler_statusrx = 0;
  }
  Temps.sendData();
  digitalWrite(led, HIGH);
}
void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++) {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
