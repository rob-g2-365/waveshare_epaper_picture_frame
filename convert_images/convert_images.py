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
import argparse
import sys
import os
import os.path
import re
import random
import process_image
from test_image import create_test_image


def get_image_files_list(source_dir: str) -> list:
    """
    Returns a list of the full paths to the source files.
    :param source_dir:
    :return:
    """
    img_file_list = []
    expression = "([a-z0-9\\s_\\\\.\\-\\(\\):])+(.bmp|.png|.jpg|.jpeg)$"
    source_dir_abs = os.path.abspath(source_dir)
    for root, subFolders, files in os.walk(source_dir_abs):
        for file in files:
            file_path = os.path.join(root, file)
            if re.match(expression, file, re.IGNORECASE):
                img_file_list.append(os.path.join(root, file_path))
    return img_file_list


def dest_pic_filename(num: int) -> str:
    """
    Returns the destination file name based on the integer number.

    :param num:
    :return:
    """
    return "pic_%03d.bmp" % num


def process_images( file_list: list, dest_dir: str):
    """
    Process all the images in the file list.

    :param file_list:
    :param dest_dir:
    :return:
    """
    for i, srcImageFile in enumerate(file_list):
        print(srcImageFile)
        with process_image.open_file(srcImageFile) as img1:
            img2 = process_image.apply_exif_rotation_tag(img1)
            img3 = process_image.mirror(img2)
            img4 = process_image.auto_contrast(img3)
            img5 = process_image.saturation(img4)
            img6 = process_image.crop_image_to_proper_ratio(img5)
            img7 = process_image.resize_image(img6)
            img8 = process_image.convert_image(img7)
            dest_file = os.path.join(dest_dir, dest_pic_filename(i))
            img8.save(dest_file)


def convert_images(source_dir, dest_dir):
    """
    Recursively Converts images from the source directory to destination
    :param source_dir:
            Source directory

    :param dest_dir:
            Destination directory

    :return:
            None
    """
    image_file_list = get_image_files_list(source_dir)
    random.shuffle(image_file_list)
    process_images(image_file_list, dest_dir)
    return

def create_test_images(dest_dir: str, number_of_images:int):
    """
    Create a series of test images labeled with the image number and random color.

    :param dest_dir:
    :param number_of_images:
    :return:
    """
    for i in range(number_of_images):
        img1 = create_test_image(dest_pic_filename(i))
        img2 = process_image.mirror(img1)
        img3 = process_image.convert_image(img2)
        dest_file = os.path.join(dest_dir, dest_pic_filename(i))
        img3.save(dest_file)

if __name__ == '__main__':

    parser = argparse.ArgumentParser(
        prog='convert_images',
        description='Converts images so they are in the proper format to be displayed in the e-picture frame.'
    )

    parser.add_argument('-c', '--convert', type=str, nargs=2, metavar=('SOURCE_DIR', 'DEST_DIR'),
                        help='Convert all images from the source directory and subdirectories '
                             'to the destination directory.')
    parser.add_argument('-t', '--test', type=str,  metavar='DEST_DIR',
                        help='Create a number of test images to be put in the destination directory.')
    parser.add_argument('-n', '--number', type=int,  default=600,
                        help='Number of test images to generate. Only valid with the test option.')
    args = parser.parse_args()

    if args.convert is not None and args.test is not None:
        print("  ** You cannot use both the --convert and --test options.  **")
        exit(1)

    if args.convert is None and args.test is None:
        print("  ** You must use either the --convert or --test option.")
        print("\r\n")
        parser.print_help()
        exit(1)

    if args.convert is not None:
        convert_images(args.convert[0], args.convert[1])
        exit(0)

    if args.test is not None:
        create_test_images(args.test, args.number)
        exit(0)



