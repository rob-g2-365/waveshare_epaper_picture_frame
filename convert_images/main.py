##########################################################################
# MIT License
#
# Copyright (c) 2023 Robert L Gorsegner II
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
##########################################################################

import sys
import os
import os.path
import re
import process_image
import random


def get_image_files_list(source_dir):
    #return img_file_list
    img_file_list = []
    expression = "([a-zA-Z0-9\\s_\\\\.\\-\\(\\):])+(.bmp|.png|.jpg|.jpeg)$"
    for root, subFolders, files in os.walk(source_dir):
        for file in files:
            file_path = os.path.join(root, file)
            if re.match(expression, file):
                img_file_list.append(os.path.join(root, file_path))
    return img_file_list


def dest_pic_filename(num):
    return "pic_%03d.bmp" % num


def process_images( file_list, dest_dir):
    for i, srcImageFile in enumerate(file_list):
        print(srcImageFile)
        img1 = process_image.open_file(srcImageFile)
        img2 = process_image.mirror(img1)
        img3 = process_image.auto_contrast(img2)
        img4 = process_image.saturation(img3)
        img5 = process_image.crop_image_to_proper_ratio(img4)
        img6 = process_image.resize_image(img5)
        img7 = process_image.convert_image(img6)
        dest_file = os.path.join(dest_dir, dest_pic_filename(i))
        img7.save(dest_file)


if __name__ == '__main__':
    numArgs = len(sys.argv)
    if len(sys.argv) != 3:
        print("Format: convert_images.py <source_dir> <dest_dir>")
        sys.exit(0)

    source_dir = sys.argv[1]
    dest_dir = sys.argv[2]

    image_file_list = get_image_files_list(source_dir)
    random.shuffle(image_file_list)
    process_images(image_file_list, dest_dir)



