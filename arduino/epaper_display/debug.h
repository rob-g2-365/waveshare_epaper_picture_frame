/******************************************************************************
File: debug.h

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

#ifndef _H_ERROR_CODE
#define _H_ERROR_CODE

#define SUCCESS 0
#define FAILURE 1


#define ERRORCODE_TEST                              0
#define ERRORCODE_EPAPER_INIT_FAILED                0x101
#define ERRORCODE_EPAPER_FAILED_TURN_ON_DISPLAY     0x102
#define ERRORCODE_SD_FAILED_TO_OPEN_FILE            0x201
#define ERRORCODE_SD_FAILED_TO_OPEN_ANY_FILE        0x202
#define ERRORCODE_SD_FAILED_TO_SEEK                 0x203
#define ERRORCODE_BMP_FAILED_TO_READ_LINE           0x301
#define ERRORCODE_BMP_FAILED_TO_READ_HEADER         0x302
#define ERRORCODE_BMP_HEADER_FIELD_PROB             0x303
#define ERRORCODE_BMP_INVALID_BITS_PER_PIXEL        0x304
#define ERRORCODE_BMP_INVALID_IMAGE_SIZE            0x305
#define ERRORCODE_NONE                              -1

// Include this flag to turn on serial logging.
// #define SERIAL_DEBUG
#define EEPROM_DEBUG

class Debug {
  public:
    static void print(const char * msg );
    static void printProgMem(const PROGMEM char *msg);
    static void serialBegin(int baud_rate) ;
    static void serialFlush();
    static void error(int errorCode);
    static void printEepromErrorInfo();
  private:
    static void printErrorAndPicIndex(int errorCode, int pictureIndex);
};
#endif // _H_ERROR_CODE
