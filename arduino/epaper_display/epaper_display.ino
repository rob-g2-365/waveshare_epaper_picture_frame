/******************************************************************************
File: epaper_display.ini
******************************************************************************
MIT License

Copyright (c) 2023 Robert L Gorsegner II

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/
#include <Arduino.h>
#include <SD.h>
#include <LowPower.h>
#include <SPI.h>
#include <EEPROM.h>
#include "epd7in3f.h"
#include "epdif.h"
#include "read_bmp.h"
#include "eeprom_data.h"
#include "debug.h"
#include "picture_index.h"


// Definitions
#define PIN_POWER_SELECT    5
#define PIN_SD_CHIP_SELECT  6
#define PIN_MOSI            11

#define SECONDS_IN_A_DAY  86400
#define NUMBER_OF_SLEEP_LOOPS 10800  // Seconds in a day divided by 8s

// Function declarations.
int get_next_file_handle(SDFile &sdFile);
inline void beginSpiTransaction() __attribute__((always_inline));
inline void endSpiTransaction() __attribute__((always_inline));

// Global variables
int sleep_loops = 0;

/******************************************************************************
function : 
      Setup
******************************************************************************/
void setup() {
  // put your setup code here, to run once:
  // Set up serial port baud rate for debug
  Debug::serialBegin(9600);

  Debug::printEepromErrorInfo();

  int pictureIndex = EepromData::readPictureIndex();

  #ifdef SERIAL_DEBUG
  Serial.print("Starting off from picture index ");
  Serial.print(pictureIndex, DEC);
  Serial.print(".\r\n");
  #endif // SERIAL_DEBUG

  PictureIndex::setPictureIndex(pictureIndex);
}

/******************************************************************************
function : 
      Main loop
******************************************************************************/
void loop() {

  powerUpPeripherals();

  updateDisplayWithNewPicture();

  powerDownPeripherals();

  while(sleep_loops < NUMBER_OF_SLEEP_LOOPS) {
    Debug::print("Sleeping\r\n");

    Debug::serialFlush();
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
    sleep_loops++;

    Debug::print("Waking\r\n");
  }
  sleep_loops = 0;
}

/******************************************************************************
  Update the display with a new picture
******************************************************************************/
void updateDisplayWithNewPicture() {
  Epd epd;
  SDFile bmpFile;

  Debug::print("epd.Init()\r\n");
  if (epd.Init() != 0) {
    Debug::print("e-Paper init failed\r\n");
    Debug::error(ERRORCODE_EPAPER_INIT_FAILED);
    return;
  }

  // Clear the display.
  Debug::print("e-Paper Clear\r\n");
  beginSpiTransaction();
  epd.Clear(EPD_7IN3F_WHITE);
  endSpiTransaction();

  SD.begin(SPI_QUARTER_SPEED, PIN_SD_CHIP_SELECT);
  if( getNextFileHandle(bmpFile) == FAILURE) {
    goto end_serial;
  }

  Debug::print("Display Image\r\n");
  displayImage(&epd, bmpFile);
  // epd.EPD_7IN3F_Show7Block();

  // Delay 3 seconds while the image shows on the screen.
  delay(3000);

  Debug::print("Goto Sleep...\r\n");
  beginSpiTransaction();
  epd.Sleep();
  endSpiTransaction();

  // Close the file handle.
  bmpFile.close();


end_serial:  
  SD.end();
  PictureIndex::updateInUsePictureIndex();
}

/******************************************************************************
  Power up the SDCARD and E-Hat
******************************************************************************/
void powerUpPeripherals() {
  // Set MOSI as an output.  When powered down it is set to an input to 
  // conserve power.
  pinMode(PIN_MOSI, OUTPUT);

  // Set the CS pin high so the peripherals don't start reading the SPI bus
  // when powered up.
  digitalWrite(PIN_SD_CHIP_SELECT, HIGH);

  // Turn on the peripheral power by setting power select low.
  pinMode(PIN_POWER_SELECT, OUTPUT);
  digitalWrite(PIN_POWER_SELECT, LOW);

  // Allow a small delay for the peripherals to get power.
  delay(500);
}

/******************************************************************************
  Power down the SDCARD and E-Hat
******************************************************************************/
void powerDownPeripherals() {
  // Turn off the peripheral power by setting the power select GPIO high.  
  // The power will remain off while sleeping.
  digitalWrite(PIN_POWER_SELECT, HIGH);
  
  // If MOSI or the CS lines are high, the SD Card board will draw power on some
  // SD Card boards.  This can be prevented by setting MOSI as an input and 
  // SD CS low.
  pinMode(PIN_MOSI, INPUT);
  digitalWrite(PIN_SD_CHIP_SELECT, LOW);
}

/******************************************************************************
  Get next file.  Remember to close the file handle after use.

  @param[out] sdFile
         Pointer to the display object.

  @retval SUCCESS
  @retval FAILURE
******************************************************************************/
int getNextFileHandle(SDFile &sdFile) {
  char filename[MAX_8_3_FILENAME_SIZE];

  // Get the next filename.
  Debug::print("Get the next filename\r\n");
  PictureIndex::getBmpFilename(filename);

  Debug::print("Opening file ");
  Debug::print(filename);
  Debug::print("\r\n");
  sdFile = SD.open(filename, FILE_READ);
  if(sdFile) {
    Debug::print("File opened.\r\n");
    PictureIndex::incrementPictureIndex();
    EepromData::savePictureIndex();
    return SUCCESS;
  }

  if(PictureIndex::getPictureIndex()==0) {
    // Do not log the first picture to the eeprom because this is easy to reproduce.
    Debug::print("Failed to open file pic_000.bmp.\r\n");
    return FAILURE;
  } 

  Debug::print("Failed to open file.\r\n");
  sdFile.close();

  // Sent the picture index to the original.
  PictureIndex::clearPictureIndex();
  PictureIndex::updateInUsePictureIndex();
  PictureIndex::getBmpFilename(filename);

  Debug::print("Opening file ");
  Debug::print(filename);
  Debug::print("\r\n");
  sdFile = SD.open(filename, FILE_READ);
  if(sdFile) {
    Debug::print("File opened.");
    PictureIndex::incrementPictureIndex();
    EepromData::savePictureIndex();
    return SUCCESS;
  }

  Debug::print("Failed to open any bmp file.\r\n");
  return FAILURE;
}

/******************************************************************************
  Displays the image.

  @param pEpd
         Pointer to the display object.
  @param filename
         Filename of the bitmap.

  @retval SUCCESS
  @retval FAILURE
******************************************************************************/
int displayImage(Epd * pEpd, SDFile & bmpFile) {

  // Send the command to start writing the data buffer.
  Debug::print("pEpd->SendCommand(0x10)\r\n");
  beginSpiTransaction();
  pEpd->SendCommand(0x10);
  endSpiTransaction();

  // Create the ReadBMP class
  ReadBMP readbmp(&bmpFile);

  if (readbmp.parseHeader() != SUCCESS) {
    // Close the data file
    bmpFile.close();
    return FAILURE;
  }

  if(readbmp.width() != EPD_WIDTH || readbmp.height() != EPD_HEIGHT ) {
    Debug::print("Invalid image size.\r\n");
    Debug::error(ERRORCODE_BMP_INVALID_IMAGE_SIZE);

    // Close the data file
    bmpFile.close();
    return FAILURE;
  }

  {
    char quarterLineBuffer[EPD_WIDTH/4];
    for(int row = 0; row < EPD_HEIGHT; row++) {
      for(int quarter = 0; quarter < 4 ; quarter++) {
        // Read in one half row of data. Callee handles error message.
        if(readbmp.readQuarterLine(quarterLineBuffer) == FAILURE) {
          return FAILURE;
        }

        beginSpiTransaction();
        // Write out one half row to the screen.
        sendQuarterRow(pEpd, quarterLineBuffer);
        endSpiTransaction();
      }
    }
  }

  // Debug::print("pEpd->TurnOnDisplay()\r\n");
  beginSpiTransaction();
  pEpd->TurnOnDisplay();
  endSpiTransaction();

  // Close the data file
  bmpFile.close();

  // Show the image.
  return SUCCESS;
}

/******************************************************************************
  Begin a spi transaction for the e-paper screen
******************************************************************************/
void beginSpiTransaction() {
  SPI.beginTransaction(SPISettings(EPD_SPI_SPEED, MSBFIRST, SPI_MODE0));
}

/******************************************************************************
  End a spi transaction for the e-paper screen
******************************************************************************/
void endSpiTransaction() {
  SPI.endTransaction();
}

/******************************************************************************
  Send a half row.  A full row takes up to much memory on the arduino.

  @param buffer A buffer for the half row.

******************************************************************************/
void printHalfRow(char *buffer) {
  #ifdef SERIAL_DEBUG
  for(int col = 0; col< EPD_WIDTH /4 ; col++ ) {
    Serial.print(buffer[col], HEX);
    Serial.print(",");
  }
  Serial.print("\r\n");
  #else
  // Gets rid of the warning for an unused variable.
  (void *)buffer;
  #endif // SERIAL_DEBUG
}

/******************************************************************************
  Sends a quarter row of data to the device.

  @param pEpd
         Pointer to the display object.
  @param buffer
         The buffer for one half line.          
******************************************************************************/
void sendQuarterRow(Epd *pEpd, char *buffer) {
  for(int col = 0; col< EPD_WIDTH /8 ; col++ ) {
    pEpd->SendData(buffer[col]);
  }
}

