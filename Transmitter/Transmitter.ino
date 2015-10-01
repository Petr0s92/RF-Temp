#include <OneWire.h>
#include <DallasTemperature.h>
#include <VirtualWire.h>
#define ONE_WIRE_BUS 3
#undef float


OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;

//Probes ID
DeviceAddress Probe01 = { 0x28, 0x39, 0xD7, 0xB7, 0x01, 0x00, 0x00, 0xD6 };
DeviceAddress Probe02 = { 0x28, 0xB0, 0x02, 0xB8, 0x01, 0x00, 0x00, 0xBC };
float tempC;
float tempC2;
char charnum[10];
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  sensors.begin();
  if (!sensors.getAddress(insideThermometer, 0)) Serial.print("Unable to find address for Device 0");
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.print(" devices.");
  Serial.print("Address:");
  printAddress(insideThermometer);
  Serial.println();
  // set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
  sensors.setResolution(Probe01, 11);
  sensors.setResolution(Probe02, 11);
  //Init RF
  Serial.println("Init RF Transmission");

  // Initialise the IO and ISR
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(2000);   // Bits per sec
}
void printTemperature(DeviceAddress deviceAddress)
{

  // method 2 - faster

  tempC = sensors.getTempC(Probe01);
  tempC2 = sensors.getTempC(Probe02);
}

void loop() {
  // put your main code here, to run repeatedly:
  sensors.requestTemperatures(); // Send the command to get temperatures
  printTemperature(insideThermometer);
  printTemperature(Probe01);
  printTemperature(Probe02);

  //  Serial.println(tempC2);
  //  vw_send((uint8_t *)&tempC2, sizeof(float));
  //  vw_wait_tx(); // Wait until the whole message is gone
  //Temp (Baro Pressure)
  float temperature = tempC; //MUST be called first
  float temperature2 = tempC2; //MUST be called first
  //Send Data #C is for Celcious
  SendData((String(temperature, 2))+" "+(String(temperature2, 2)));

  delay(700);
  // You can have more than one IC on the same bus.
  // 0 refers to the first IC on the wire
}
void SendData(String Data)
{
  //Debug
  //Serial.println("Sensor:" + tempC2 + " Transmit Data:");

  //Making char Array of String
  const char* rawdata = Data.c_str();

  digitalWrite(13, true); // Flash a light to show transmitting
  vw_send((uint8_t *)rawdata, strlen(rawdata)); //Send Data
  vw_wait_tx(); // Wait until the whole message is gone
  digitalWrite(13, false);
}
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
