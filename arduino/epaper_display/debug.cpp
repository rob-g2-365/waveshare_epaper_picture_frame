/******************************************************************************
File: debug.cpp

Helpful functions for debugging and logging.
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
#include "debug.h"
#include "eeprom_data.h"

void Debug::serialBegin(int baudRate) {
#ifdef SERIAL_DEBUG  
  Serial.begin(baudRate);
#endif // SERIAL_DEBUG
}

void Debug::serialFlush() {
#ifdef SERIAL_DEBUG
  Serial.flush();
#endif // SERIAL_DEBUG
}

void Debug::print(const char * msg) {
#ifdef SERIAL_DEBUG
  Serial.print(msg);
#endif // SERIAL_DEBUG
}

void Debug::error(int errorCode) {

#ifdef EEPROM_DEBUG
  EepromData::saveErrorPicIndex();
  EepromData::saveErrorCode(errorCode);
#endif // EEPROM_DEBUG
}

void Debug::printEepromErrorInfo() {
#if defined(SERIAL_DEBUG) && defined(EEPROM_DEBUG)
  int errorCode = EepromData::readErrorCode();
  if(errorCode == ERRORCODE_NONE) {
    Serial.print("No EEPROM Error logged.\r\n");
    return;
  }

  Serial.print("EEPROM Error Code: 0x");
  Serial.print(errorCode, HEX);
  Serial.print("\r\n");

  int errorPictureIndex = EepromData::readErrorPicIndex();
  Serial.print("EEPROM Error Pic Index: ");
  Serial.print(errorPictureIndex, DEC);
  Serial.print("\r\n");
  #endif // defined(SERIAL_DEBUG) && defined(EEPROM_DEBUG)
}