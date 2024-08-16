/******************************************************************************
File: picture_index.h

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
#ifndef _H_PICTURE_INDEX
#define _H_PICTURE_INDEX

#define MAX_8_3_FILENAME_SIZE 13
#define MAX_PICTURE_INDEX     999

class PictureIndex {
  public:
    static void getBmpFilename(char * filename);
    static inline void clearPictureIndex() {
      pictureIndex = 0;
    }
    static inline void incrementPictureIndex() {
      pictureIndex = inRange(pictureIndex + 1);
    }

    static inline void updateInUsePictureIndex() {
      inUsePictureIndex = pictureIndex;
    }

    static inline int getPictureIndex() {
      return pictureIndex;
    }

    static inline void setPictureIndex(int pictureIndex) {
      PictureIndex::pictureIndex = inRange(pictureIndex);
    }

    static inline int getInUsePictureIndex() {
      return inUsePictureIndex;
    }
  private:
    static inline int inRange(int index) {
      if(index > MAX_PICTURE_INDEX || index < 0) {
        index = 0;
      }
      return index;
    }
    static int pictureIndex;
    static int inUsePictureIndex;
};

#endif // _H_PICTURE_INDEX