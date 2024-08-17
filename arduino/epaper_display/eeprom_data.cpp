/******************************************************************************
File: eeprom_data.cpp

Header file to save and read eeprom data for the picture frame in case of power 
loss.  This allows the user to see the next picture while replacing batteries
and save error information.
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


#include <EEPROM.h>
#include <string.h>
#include <Arduino.h>
#include <avr/pgmspace.h>
#include "eeprom_data.h"
#include "debug.h"
#include "picture_index.h"

/******************************************************************************
  Clears the EEPROM to the un-initialized state.

******************************************************************************/
void EepromData::clearEepromData() {
  EEPROM.put(EEPROM_ADDRESS_PIC_INDEX, -1);
  EEPROM.put(EEPROM_ADDRESS_ERROR_INDEX, -1);
}

/******************************************************************************
  Save the last picture index to the eeprom.

  @param[in]  picIndex
              The last picture index
******************************************************************************/
void EepromData::savePictureIndex() {
  EEPROM.put(EEPROM_ADDRESS_PIC_INDEX, PictureIndex::getPictureIndex());
}


/******************************************************************************
  Read the last picture index to the eeprom.
******************************************************************************/
int EepromData::readPictureIndex() {
  int value;
  EEPROM.get(EEPROM_ADDRESS_PIC_INDEX, value);
  return value;
}

/******************************************************************************
  Save the error code.

  @param[in]    errorCode
******************************************************************************/
void EepromData::saveErrorCode(int errorCode) {
  EEPROM.put(EEPROM_ADDRESS_ERROR_INDEX, errorCode);
}

/******************************************************************************
  Read the last error code.

  @return    errorCode
******************************************************************************/
int EepromData::readErrorCode() {
  int errorCode;
  EEPROM.get(EEPROM_ADDRESS_ERROR_INDEX, errorCode);
  return errorCode;
}

/******************************************************************************
  Save off the picture index that caused an error code.
******************************************************************************/
void EepromData::saveErrorPicIndex() {
  EEPROM.put(EEPROM_ADDRESS_ERROR_PIC_INDEX, PictureIndex::getInUsePictureIndex());
}

/******************************************************************************
  Read off the picture index that caused an error code.
******************************************************************************/
int EepromData::readErrorPicIndex() {
  int pictureIndexLast;
  EEPROM.get(EEPROM_ADDRESS_ERROR_PIC_INDEX, pictureIndexLast);
  return pictureIndexLast;
}

