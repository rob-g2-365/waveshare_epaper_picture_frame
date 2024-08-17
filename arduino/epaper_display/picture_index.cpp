/******************************************************************************
File: picture_index.cpp

Class to keep track ofthe picture index.
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
#include "picture_index.h"

int PictureIndex::pictureIndex = 0;
int PictureIndex::inUsePictureIndex = 0;

/******************************************************************************
  Get next filename

  @param[out] filename
              Filename of the bitmap.
******************************************************************************/
void PictureIndex::getBmpFilename(char * filename) {
  filename[0] = 'p';
  filename[1] = 'i';
  filename[2] = 'c';
  filename[3] = '_';
  filename[4] = '0' + ((pictureIndex / 100) % 10);
  filename[5] = '0' + ((pictureIndex / 10) % 10);
  filename[6] = '0' + (pictureIndex % 10);
  filename[7] = '.';
  filename[8] = 'b';
  filename[9] = 'm';
  filename[10] = 'p';
  filename[11] = '\0';
}
