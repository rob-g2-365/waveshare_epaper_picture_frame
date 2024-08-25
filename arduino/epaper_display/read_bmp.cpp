/******************************************************************************
File: readbmp.cpp

Class to parse a bitmap file.
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

#include "read_bmp.h"
#include "debug.h"

/******************************************************************************
  Constructor

  @param file       File handle for the bitmap file.
******************************************************************************/

ReadBMP::ReadBMP(File *file) {
  this->m_file = file;
}

/******************************************************************************
  Read one half raster line.  This is 4 bits per pixel.

  @param buffer     Pointer to the line buffer.  Must allocate enough for 
                    a line to be 8 bits per pixel.

  @retval SUCCESS
  @retval FAILURE
******************************************************************************/
int ReadBMP::readQuarterLine(char * buffer) {

  int quarterLineSize = (m_width /4)  * m_bits_per_pixel / 8;
  if(quarterLineSize != m_file->read(buffer, quarterLineSize)) {
    Debug::error(ERRORCODE_BMP_FAILED_TO_READ_LINE);
    Debug::printProgMem(PSTR("Failed to read bmp line.\r\n"));
    return FAILURE;
  }

  // If 8 bits per pixel,convert to 4 bits per pixel.
  if(m_bits_per_pixel == 8) {
    for(int i = 0 ; i < quarterLineSize; i+=2) {
      int b0 = buffer[i];
      int b1 = buffer[i + 1];
      buffer[i>>1] =(b0 << 4) | b1;
    }
  }

  return SUCCESS;
}

/******************************************************************************
  Does the following:
     1.  Parses the header
     2.  Verifies that the bmp is in the correct format.
     3.  Saves basic format information about the bmp.

  @retval SUCCESS
  @retval FAILURE
******************************************************************************/
int ReadBMP::parseHeader() {
  int header_size;
  BmpHeader header;

  header_size = m_file->read(&header, sizeof(BmpHeader));
  if(header_size !=sizeof(BmpHeader)) {
    Debug::error(ERRORCODE_BMP_FAILED_TO_READ_HEADER);
    Debug::printProgMem(PSTR("Failed to read bmp header. header size = "));
    Serial.print(header_size, HEX);
    Serial.print("\r\n");
    return FAILURE;
  }

  if(header.header_field != BMP_HEADER_FIELD_WIN) {
    Debug::error(ERRORCODE_BMP_HEADER_FIELD_PROB);
    Debug::printProgMem(PSTR("Header field not correct.\r\n"));
    return FAILURE;
  }

  if( header.bits_per_pixel != 4 && header.bits_per_pixel != 8) {
    Debug::error(ERRORCODE_BMP_INVALID_BITS_PER_PIXEL);
    Debug::printProgMem(PSTR("Invalid number of bits per pixel.\r\n"));
    return FAILURE;
  }

  m_width = header.width_in_pixels;
  m_height = header.height_in_pixels;
  m_bits_per_pixel = header.bits_per_pixel;

  if(!m_file->seek(header.offset)) {
    Debug::error(ERRORCODE_SD_FAILED_TO_SEEK);
    Debug::printProgMem(PSTR("Failed to seek\r\n"));
    return FAILURE;
  }

  return SUCCESS;
}