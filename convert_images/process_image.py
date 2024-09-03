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


from PIL import Image
from PIL.Image import Dither
from PIL import ImageEnhance
from PIL import ImagePalette
from PIL import ImageOps

FRAME_WIDTH = 800
FRAME_HEIGHT = 480

my_palette = (
    0, 0, 0,  # Black
    255, 255, 255,  # White
    0, 255, 0,  # Green
    0, 0, 255,  # Blue
    255, 0, 0,  # Red
    255, 255, 0,  # Yellow
    255, 128, 0  # Orange
)


def open_file(filename):
    return Image.open(filename)


def apply_exif_rotation_tag(img):
    return ImageOps.exif_transpose(img)


def mirror(img):
    return ImageOps.mirror(img)


def convert_image(img):
    p = ImagePalette.ImagePalette("RGB", my_palette)
    pal_image = Image.new("P", (1, 1))
    pal_image.putpalette(p)
    return img.quantize(kmeans=16, dither=Dither.FLOYDSTEINBERG, palette=pal_image)  #.convert('RGB')


def crop_image_to_proper_ratio(img):
    width, height = img.size

    if (width/height) > (float(FRAME_WIDTH) / float(FRAME_HEIGHT)):
        new_width = float(FRAME_WIDTH) / float(FRAME_HEIGHT) * height
        left = (width - new_width)/2
        right = new_width + left
        top = 0
        bottom = height
        img_new = img.crop((left, top, right, bottom))
    elif (width/height) < (float(FRAME_WIDTH) / float(FRAME_HEIGHT) ):
        new_height = float(FRAME_HEIGHT) / float(FRAME_WIDTH) * width
        left = 0
        right = width
        top = (height - new_height) / 2
        bottom = top + new_height
        img_new = img.crop((left, top, right, bottom))
    else:
        img_new = img.copy()
    return img_new


def resize_image(img):
    return img.resize((FRAME_WIDTH, FRAME_HEIGHT))


def contrast(img):
    return ImageEnhance.Contrast(img).enhance(1.1)


def auto_contrast(img):
    # applying autocontrast method
    return ImageOps.autocontrast(img, cutoff=2, ignore=2)


def saturation(img):
    return ImageEnhance.Color(img).enhance(2.5)
