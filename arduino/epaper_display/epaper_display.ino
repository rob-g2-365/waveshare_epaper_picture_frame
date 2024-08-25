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
#include <avr/pgmspace.h>
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
#define PIN_SCLK            13

#define SECONDS_IN_A_DAY  86400
#define SECONDS_TO_DISPLAY 135
#define NUMBER_OF_SLEEP_LOOPS   ((SECONDS_IN_A_DAY - SECONDS_TO_DISPLAY)/8)

// Function declarations.
int get_next_file_handle(SDFile &sdFile);
inline void beginSpiTransaction() __attribute__((always_inline));
inline void endSpiTransaction() __attribute__((always_inline));
void sendQuarterRow(Epd *pEpd, char *buffer);

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
  Debug::printProgMem(PSTR("Starting off from picture index "));
  Serial.print(pictureIndex, DEC);
  Serial.print("\r\n");
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
    Debug::printProgMem(PSTR("Sleeping\r\n"));

    Debug::serialFlush();
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
    sleep_loops++;

    Debug::printProgMem(PSTR("Waking\r\n"));
  }
  sleep_loops = 0;
}

/******************************************************************************
  Update the display with a new picture
******************************************************************************/
void updateDisplayWithNewPicture() {
  bool bmpFileInit = false;
  Epd epd;
  SDFile bmpFile;

  Debug::printProgMem(PSTR("epd.Init()\r\n"));
  if (epd.Init() != SUCCESS) {
    Debug::printProgMem(PSTR("e-Paper init failed.\r\n"));
    Debug::error(ERRORCODE_EPAPER_INIT_FAILED);
    return;
  }

  // Clear the display.
  Debug::printProgMem(PSTR("e-Paper Clear\r\n"));
  beginSpiTransaction();
  epd.Clear(EPD_7IN3F_WHITE);
  endSpiTransaction();

  SD.begin(SPI_QUARTER_SPEED, PIN_SD_CHIP_SELECT);
  // getNextFileHandle has the error handling code in it.
  if( getNextFileHandle(bmpFile) == FAILURE) {
    goto cleanup;
  }
  bmpFileInit = true;

  Debug::printProgMem(PSTR("Display Image\r\n"));
  displayImage(&epd, bmpFile);
  // epd.EPD_7IN3F_Show7Block();

  // Delay 3 seconds while the image shows on the screen.
  delay(3000);

cleanup:  
  Debug::printProgMem(PSTR("EPaper Sleep...\r\n"));
  beginSpiTransaction();
  epd.Sleep();
  endSpiTransaction();

  // Close the file handle.
  if(bmpFileInit) {
    bmpFile.close();
  }

  // Close the SD Card.
  SD.end();
  PictureIndex::updateInUsePictureIndex();
}

/******************************************************************************
  Power up the SDCARD and E-Hat
******************************************************************************/
void powerUpPeripherals() {
  Debug::printProgMem(PSTR("powerUpPeripherals\r\n"));

  // Set the SDCARD CS, SCLK and MOSI pins high so the SD CARD doesn't start 
  // reading the SPI bus when powered up. Flush every debug serial port 
  // transaction so that if there is a problem the last message is logged.
  Debug::printProgMem(PSTR("pinMode(PIN_SD_CHIP_SELECT, OUTPUT)\r\n"));
  Serial.flush();
  pinMode(PIN_SD_CHIP_SELECT, OUTPUT);

  Debug::printProgMem(PSTR("digitalWrite(PIN_SD_CHIP_SELECT, HIGH)\r\n"));
  Serial.flush();
  digitalWrite(PIN_SD_CHIP_SELECT, HIGH);

  Debug::printProgMem(PSTR("pinMode(PIN_SCLK, OUTPUT)\r\n"));
  Serial.flush();
  pinMode(PIN_SCLK, OUTPUT);

  Debug::printProgMem(PSTR("digitalWrite(PIN_SCLK, HIGH)\r\n"));
  Serial.flush();
  digitalWrite(PIN_SCLK, HIGH);
  
  Debug::printProgMem(PSTR("pinMode(PIN_MOSI, OUTPUT)\r\n"));
  Serial.flush();
  pinMode(PIN_MOSI, OUTPUT);

  Debug::printProgMem(PSTR("digitalWrite(PIN_MOSI, HIGH)\r\n"));
  Serial.flush();
  digitalWrite(PIN_MOSI, HIGH);


  // Turn on the peripheral power by setting power select low.
  Debug::printProgMem(PSTR("pinMode(PIN_POWER_SELECT, OUTPUT)\r\n"));
  Serial.flush();
  pinMode(PIN_POWER_SELECT, OUTPUT);
  digitalWrite(PIN_POWER_SELECT, LOW);

  // Allow a small delay for the peripherals to get power.
  Debug::printProgMem(PSTR("delay(2)\r\n"));
  delay(2);

  Debug::printProgMem(PSTR("-powerUpPeripherals\r\n"));
}

/******************************************************************************
  Power down the SDCARD and E-Hat
******************************************************************************/
void powerDownPeripherals() {
  Debug::printProgMem(PSTR("powerDownPeripherals\r\n"));
  // Turn off the peripheral power by setting the power select GPIO high.  
  // The power will remain off while sleeping.
  digitalWrite(PIN_POWER_SELECT, HIGH);

  
  // If SCLK, MOSI or the CS lines are high, the SD Card board will draw power on some
  // SD Card boards.  This can be prevented by setting these lines as inputs.
  pinMode(PIN_SCLK, INPUT);
  pinMode(PIN_MOSI, INPUT);
  pinMode(PIN_SD_CHIP_SELECT, INPUT);
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
  Debug::printProgMem(PSTR("Get the next filename\r\n"));
  PictureIndex::getBmpFilename(filename);

  Debug::printProgMem(PSTR("Opening file "));
  Debug::print(filename);
  Debug::print("\r\n");
  sdFile = SD.open(filename, FILE_READ);
  if(sdFile) {
    Debug::printProgMem(PSTR("File opened.\r\n"));
    PictureIndex::incrementPictureIndex();
    EepromData::savePictureIndex();
    return SUCCESS;
  }

  if(PictureIndex::getPictureIndex()==0) {
    // Do not log the first picture to the eeprom because this is easy to reproduce.
    Debug::printProgMem(PSTR("Failed to open file pic_000.bmp.\r\n"));
    return FAILURE;
  } 

  Debug::printProgMem(PSTR("Failed to open file.\r\n"));
  sdFile.close();

  // Sent the picture index to the original.
  PictureIndex::clearPictureIndex();
  PictureIndex::updateInUsePictureIndex();
  PictureIndex::getBmpFilename(filename);

  Debug::printProgMem(PSTR("Opening file "));
  Debug::print(filename);
  Debug::printProgMem(PSTR("\r\n"));
  sdFile = SD.open(filename, FILE_READ);
  if(sdFile) {
    Debug::printProgMem(PSTR("File opened."));
    PictureIndex::incrementPictureIndex();
    EepromData::savePictureIndex();
    return SUCCESS;
  }

  Debug::printProgMem(PSTR("Failed to open any bmp file.\r\n"));
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
  int ret = SUCCESS;
  // Send the command to start writing the data buffer.
  Debug::printProgMem(PSTR("pEpd->SendCommand(0x10)\r\n"));
  beginSpiTransaction();
  pEpd->SendCommand(0x10);
  endSpiTransaction();

  // Create the ReadBMP class
  ReadBMP readbmp(&bmpFile);

  if (readbmp.parseHeader() != SUCCESS) {
    // Close the data file
    ret = FAILURE;
    goto display_image_cleanup;
  }

  if(readbmp.width() != EPD_WIDTH || readbmp.height() != EPD_HEIGHT ) {
    Debug::printProgMem(("Invalid image size.\r\n"));
    Debug::error(ERRORCODE_BMP_INVALID_IMAGE_SIZE);
    ret = FAILURE;
    goto display_image_cleanup;
  }

  {
    char quarterLineBuffer[EPD_WIDTH/4];
    for(int row = 0; row < EPD_HEIGHT; row++) {
      for(int quarter = 0; quarter < 4 ; quarter++) {
        // Read in one half row of data. Callee handles error message.
        if(readbmp.readQuarterLine(quarterLineBuffer) == FAILURE) {
          ret = FAILURE;
          goto display_image_cleanup;
        }

        beginSpiTransaction();
        // Write out one half row to the screen.
        sendQuarterRow(pEpd, quarterLineBuffer);
        endSpiTransaction();
      }
    }
  }

  Debug::printProgMem(PSTR("pEpd->TurnOnDisplay()\r\n"));
  beginSpiTransaction();
  ret = pEpd->TurnOnDisplay();
  if( ret== FAILURE) {
    Debug::printProgMem(PSTR("TurnOnDisplay Failed\r\n."));
    Debug::error(ERRORCODE_EPAPER_FAILED_TURN_ON_DISPLAY);
  }

  endSpiTransaction();

display_image_cleanup:
  // Close the data file
  bmpFile.close();

  // Show the image.
  return ret;
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

