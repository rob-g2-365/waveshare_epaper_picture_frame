##########################################################################
# MIT License
#
# Copyright (c) 2024 Robert L Gorsegner II
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
from PIL import ImageFont
from PIL import ImageDraw
from PIL.ImageColor import colormap
import random
from process_image import FRAME_HEIGHT, FRAME_WIDTH

BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)
RED = (255, 0, 0)
YELLOW = (255, 255, 0)
ORANGE = (255, 128, 0)

COLOR_COMBOS = [
    (BLACK, WHITE),
    (WHITE, BLACK),
    (BLUE, YELLOW),
    (YELLOW, BLUE),
    (RED, WHITE),
    (WHITE, RED),
    (ORANGE, BLACK),
    (BLACK, ORANGE),
    (GREEN, BLACK),
    (GREEN, WHITE),
    (BLACK, GREEN),
    (WHITE, GREEN)
]

FONT_SIZE = 50

def get_random_color_combo() -> tuple:
    """
    Some color foreground/background color combinations may not look good.  Return a random foreground, background
    color combination that is visible.

    :return:
    """
    return random.choice(COLOR_COMBOS)


def get_random_position() -> tuple:
    """
    Return a random position that when printed is not off the screen.
    :return:
    """
    x = (FRAME_WIDTH * 1 / 2) * random.random()
    y = (FRAME_HEIGHT - 2 * FONT_SIZE) * random.random()
    return x, y


def create_test_image(text: str) -> Image:
    """
    Create a test image with the supplied text.
    :param text:
    :return:
    """
    foreground_color, background_color = get_random_color_combo()
    font = ImageFont.truetype("arial.ttf", FONT_SIZE)
    img = Image.new('RGB', (FRAME_WIDTH, FRAME_HEIGHT), background_color)
    draw = ImageDraw.Draw(img)
    pos = get_random_position()
    draw.text(pos, text, font=font, fill= foreground_color)
    return img







