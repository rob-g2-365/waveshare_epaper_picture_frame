/******************************************************************************
File: readbmp.h

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
#ifndef _H_READBMP
#define _H_READBMP
#include <stdint.h>
#include <SD.h>
#include "debug.h"
struct BmpHeader {
    uint16_t header_field;        // 0x00
    uint32_t size;                // 0x02
    uint16_t reserved1;           // 0x06
    uint16_t reserved2;           // 0x08
    uint32_t offset;              // 0x0A
    uint32_t header_size;         // 0x0E
    uint32_t width_in_pixels;     // 0x12
    uint32_t height_in_pixels;    // 0x16
    uint16_t color_planes;        // 0x1A
    uint16_t bits_per_pixel;      // 0x1C
    uint32_t compression;         // 0x1E
    uint32_t horiz_resolution;
    uint32_t vert_resolution;
    uint32_t colors_in_palette;
    uint32_t important_colors;
};

class ReadBMP {
  public:
    ReadBMP(File *file);
    int readQuarterLine(char * buffer);
    void close();
    int parseHeader();
    int width() { return (int) m_width;}
    int height() { return (int) m_height; }
    int colors_in_palette() { return (int) m_colors_in_palette; }

  private:
    File *m_file;
    int m_width;
    int m_height;
    int m_bits_per_pixel;
    int m_colors_in_palette;
};

#define BMP_HEADER_FIELD_WIN 0x4D42

#endif // _H_READBMP

