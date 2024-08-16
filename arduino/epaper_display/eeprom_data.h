/******************************************************************************
File: eeprom_data.h

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

#ifndef _H_EEPROM_DATA
#define _H_EEPROM_DATA

#define EEPROM_ADDRESS_PIC_INDEX          0x10
#define EEPROM_ADDRESS_ERROR_INDEX        0x12
#define EEPROM_ADDRESS_ERROR_PIC_INDEX    0x20

#define MAX_ERROR_MSG_SIZE        25
class EepromData {
  public:
    static void clearEepromData();
    static void savePictureIndex();
    static int readPictureIndex();
    static void saveErrorPicIndex();
    static int readErrorPicIndex();
    static void saveErrorCode(int error_code);
    static int readErrorCode();
};

#endif // _H_EEPROM_DATA