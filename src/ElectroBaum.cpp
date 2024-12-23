/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

#include <Wire.h>
// Include Particle Device OS APIs
#include "Particle.h"
#include "PI4IOE5V96248.h"

#define CLOCK_SPEED_1MHZ 1000000
#define SCL_PIN SCL
#define SDA_PIN SDA
#define RESET_PIN D4
#define INT_PIN D2

#define ROW_COUNT 8
#define COLUMN_COUNT 6
#define LED_COUNT 24

int expanderLEDMaxCount = ROW_COUNT * 2;

enum LEDColor
{
  RED,
  GREEN,
  BLUE,
  BLACK
};

void setPinState(int row, int column, bool state);
byte I2CSetup();
int scanAddresses(bool *addressList);
void setLEDToColor(int ledPosition, LEDColor color);

int iDeviceAddress = 0x20;
bool pinStates[COLUMN_COUNT][ROW_COUNT];
bool blnI2CInitialized = false;

byte allOff[] = {LOW, LOW, LOW, LOW, LOW, LOW};

byte result;
// Create an instance of the PI4IOE5V96248 class
PI4IOE5V96248 io_exp;

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

volatile bool dataReceived = false;
volatile byte *values;

void interruptHandler()
{
  Log.info("Interrupt received");
  Log.info("Reading data from all pins");
  dataReceived = true;
  values = io_exp.readAll();
}

// setup() runs once, when the device is first turned on
void setup()
{
  // Set reset pin high to clear the IO expander
  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, HIGH);
  pinMode(INT_PIN, INPUT_PULLUP);

  Serial.begin(115200);
  while (!Serial)
    ; // Wait for the serial connection

  delay(1000);
  Log.info("Starting up");
  Log.info("Setting up pin state array");
  for (int i = 0; i < COLUMN_COUNT; i++)
  {
    Serial.print("[");
    for (int j = 0; j < ROW_COUNT; j++)
    {
      pinStates[i][j] = false;
      Serial.print(pinStates[i][j]);
      Serial.print(",\t");
    }
    Log.info("]");
  }
  delay(3000);
  Log.info("Setting up I2C bus");

  Wire.begin();
  if (!io_exp.begin(iDeviceAddress))
  {
    Log.info("Failed to init PI4IOE5V96248 :(");
    blnI2CInitialized = false;
  }
  else
  {
    blnI2CInitialized = true;
    Log.info("PI4IOE5V96248 found! :)");
    attachInterrupt(digitalPinToInterrupt(INT_PIN), interruptHandler, FALLING);
  }

  // byte result = I2CSetup();
  // bool addressList[127];
  // int iAddressCount = scanAddresses(addressList);

  // if (iAddressCount != 0 && result == 0)
  // {
  //   blnI2CInitialized = true;
  //   Log.info("I2C communication established");
  // }
  // else
  // {
  //   Log.info("I2C communication failed");
  // }
}

  // loop() runs over and over again, as quickly as it can execute.
void loop()
{
  if (blnI2CInitialized && Wire.isEnabled())
  {
    if (dataReceived)
    {
      Log.info("Data received: %x %x %x %x %x %x", values[0], values[1], values[2], values[3], values[4], values[5]);
      dataReceived = false;
      // Read data from PI4IOE5V96248
      // Serial.println(io_exp.readPin(3, 0), HEX); // read from port 0, pin 1, will return whatever the pins were set to

      // Wire.requestFrom(iDeviceAddress, 1);
      // while (Wire.available())
      // {
      //   byte data = Wire.read();
      //   // Process the received data
      //   Serial.print("Received data: ");
      //   Serial.println(data);
      // }
    }

    // for (int row = 0; row < ROW_COUNT; row++)
    // for (int row = 0; row < 2; row++)
    // {
    //   for (int column = 0; column < COLUMN_COUNT; column++)
    //   {
    //     Log.info("Setting pin states: [ row: %d, column: %d ]", row, column);
    //     io_exp.writePin(column, row, HIGH);
    //     Log.info("Red----------------");
    //     setLEDToColor
    //     delay(250);
    //     io_exp.writePin(column, row, LOW);
    //     ++column;
    //     Log.info("Setting pin states: [ row: %d, column: %d ]", row, column);
    //     io_exp.writePin(column, row, HIGH);
    //     Log.info("Green----------------");
    //     delay(250);
    //     io_exp.writePin(column, row, LOW);
    //     ++column;
    //     Log.info("Setting pin states: [ row: %d, column: %d ]", row, column);
    //     io_exp.writePin(column, row, HIGH);
    //     Log.info("Blue----------------");
    //     delay(250);
    //     io_exp.writePin(column, row, LOW);
    //     delay(500);
    //     Log.info(".");
    //   }
    // }
    // for(int i = 0; i < LED_COUNT; i++) {
    for(int i = 0; i < 4; i++) {
      setLEDToColor(i, RED);
      setLEDToColor(i, GREEN);
      setLEDToColor(i, BLUE);
    }

    io_exp.writeAll(allOff);
    // setPinState(0, 1, !pinStates[0][0]);
  }
  else
  {
    Serial.print("*");
  }
  delay(1000);
}

void setPinState(int row, int column, bool state)
{
  pinStates[row][column] = state;
  Serial.print("Setting pin state: ");
  // Serial.print(row);
  // Serial.print(", ");
  // Serial.print(column);
  // Serial.print(" to ");
  // Log.info(state);
  Log.info("--------------------");
  byte result = 0;

  for (int i = 0; i < COLUMN_COUNT; i++)
  {
    Wire.beginTransmission(iDeviceAddress);
    if (state)
    {
      Wire.write(0xFE);
    }
    else
    {
      Wire.write(0x00);
    }

    result = Wire.endTransmission(true);
    Serial.print(i);
    Serial.print(") ");
    // Wire.write(0x02);
    // Serial.print(0x7F, HEX);
    Serial.print(" => ");
    Serial.print(result);
    Serial.print("\t ");
    delay(250);
  }

  delay(100);
  Log.info("\nDone");
}

byte I2CSetup()
{
  // Wire.setSpeed(CLOCK_SPEED_1MHZ);
  Wire.begin();
  delay(100); // give system chance to stabilize, likely not needed.
  // could use scan here to find the device, but we know the address already for the default
  Wire.beginTransmission(iDeviceAddress);
  for (int i = 0; i < COLUMN_COUNT; i++)
  {
    // Wire.write(0xFE);
    Wire.write(0x00);
  }

  byte result = Wire.endTransmission();
  Serial.print("I2C setup result: ");
  Log.info("%d",result);
  return result;
}

int scanAddresses(bool *addressList)
{
  byte error, address;
  // bool addressList[127];
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++)
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    Serial.print("Address: 0x");
    Serial.print(address, HEX);
    Serial.print(" => ");
    Serial.println(error);
    addressList[address - 1] = false;

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      addressList[address] = true;
      if (address < 16)
      {
        Serial.print("0");
      }
      Serial.print(address, HEX);
      Serial.println(" !");
      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
      {
        Serial.print("0");
      }
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
  {
    Serial.println("No I2C devices found\n");
  }
  else
  {
    Serial.println("done\n");
  }
  // delay(5000); // wait 5 seconds for next scan
  return nDevices;
}

void setLEDToColor(int ledPosition, LEDColor color) {
  int row = ledPosition / 2;
  int column = (ledPosition % 2) * 3;
  int colorIndex = 0;

  switch (color) {
    case RED:
      colorIndex = 0;
      break;
    case GREEN:
      colorIndex = 1;
      break;
    case BLUE:
      colorIndex = 2;
      break;
  }

  column = column + colorIndex;

  if (ledPosition > expanderLEDMaxCount)
  {
    //usee the 2nd board for LEDs past the first 16
    // skip out for the momement till I set that up
    return;
  } else {
    io_exp.writePin(column, row, HIGH);
    delay(250);
    io_exp.writePin(column, row, LOW);
    delay(250);
  }
}