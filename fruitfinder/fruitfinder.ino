/*********************************************************************
 This is an example for our nRF51822 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

#include <Arduino.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
  #include <SoftwareSerial.h>
#endif

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"
#include "fruit.h"

/*=========================================================================
    APPLICATION SETTINGS

    FACTORYRESET_ENABLE     Perform a factory reset when running this sketch
   
                            Enabling this will put your Bluefruit LE module
                            in a 'known good' state and clear any config
                            data set in previous sketches or projects, so
                            running this at least once is a good idea.
   
                            When deploying your project, however, you will
                            want to disable factory reset by setting this
                            value to 0.  If you are making changes to your
                            Bluefruit LE device via AT commands, and those
                            changes aren't persisting across resets, this
                            is the reason why.  Factory reset will erase
                            the non-volatile memory where config data is
                            stored, setting it back to factory default
                            values.
       
                            Some sketches that require you to bond to a
                            central device (HID mouse, keyboard, etc.)
                            won't work at all with this feature enabled
                            since the factory reset will clear all of the
                            bonding data stored on the chip, meaning the
                            central device won't be able to reconnect.
    -----------------------------------------------------------------------*/
    #define FACTORYRESET_ENABLE      1
/*=========================================================================*/


// Create the bluefruit object, either software serial...uncomment these lines
int const NUM_FRUITS{ 3 };
fruit* fruits[NUM_FRUITS];


// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
//  while (1);
}

/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module (this function is called
            automatically on startup)
*/
/**************************************************************************/
void setup(void)
{
  fruits[0] = new fruit(69, 2, 8, 3, -1);
  fruits[1] = new fruit(68, 4, 9, 5, -1);
  fruits[2] = new fruit(67, 6, 10, 7, -1);

  while (!Serial);  // required for Flora & Micro
  delay(500);

  Serial.begin(115200);
  Serial.println(F("Adafruit Bluefruit AT Command Example"));
  Serial.println(F("-------------------------------------"));

  /* Initialise the module */
  Serial.print(F("Initialising the fruits!: "));

  for (int i=0; i<NUM_FRUITS; ++i) {

    Adafruit_BluefruitLE_UART *ble = fruits[i]->uart();
    if ( !ble->begin(VERBOSE_MODE) )
    {
      error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
      fruits[i]->is_init(false);
      continue;
    }
    Serial.println( F("OK!") );

    if ( FACTORYRESET_ENABLE )
    {
      /* Perform a factory reset to make sure everything is in a known state */
      Serial.println(F("Performing a factory reset: "));
      if ( ! ble->factoryReset() ){
        error(F("Couldn't factory reset"));
      }
    }

    /* Disable command echo from Bluefruit */
    ble->echo(false);
    ble->verbose(false);
    Serial.println("Requesting Bluefruit info:");
    /* Print Bluefruit information */
    ble->info();
    fruits[i]->is_init(true);
  } // for

}

/**************************************************************************/
/*!
    @brief  Constantly poll for new command or response data
*/
/**************************************************************************/
void loop(void)
{
  // Display command prompt
  Serial.print(F("AT > "));

  // Check for user input and echo it back if anything was found
  //char command[BUFSIZE+1];
  //getUserInput(command, BUFSIZE);
  char const *command = "AT+BLEGETRSSI";
  while (true) {
    for (int i=NUM_FRUITS-1; i >= 0; --i) {

      if (! fruits[i]->is_init()) {
          continue;
      }
      Adafruit_BluefruitLE_UART *ble = fruits[i]->uart();
      fruits[i]->listen();

      // Send command
      Serial.print(F("Device: "));
      Serial.print(i);
      Serial.print(": ");
      ble->println(command);

      // Check response status
      int strength = ble->readline_parseInt();
      Serial.println(strength);
      ble->waitForOK();
      delay(500);

    }
  }
}

/**************************************************************************/
/*!
    @brief  Checks for user input (via the Serial Monitor)
*/
/**************************************************************************/
void getUserInput(char buffer[], uint8_t maxSize)
{
  memset(buffer, 0, maxSize);
  while( Serial.available() == 0 ) {
    delay(1);
  }

  uint8_t count=0;

  do
  {
    count += Serial.readBytes(buffer+count, maxSize);
    delay(2);
  } while( (count < maxSize) && !(Serial.available() == 0) );
}
