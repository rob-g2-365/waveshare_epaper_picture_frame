import sys
import os
import os.path
import re
import process_image
import random


def get_image_files_list(source_dir):
    #file_list = [f for f in os.listdir(source_dir) if os.path.isfile(os.path.join(source_dir, f))]

    #expression = "([a-zA-Z0-9\\s_\\\\.\\-\\(\\):])+(.bmp|.png|.jpg|.jpeg)$"
    #img_file_list = []
    #for f in file_list:
    #    if re.match(expression, f):
    #        img_file_list.append(os.path.join(source_dir, f))
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
        print("Format:  source_dir dest_dir")
        sys.exit(0)

    source_dir = sys.argv[1]
    dest_dir = sys.argv[2]

    image_file_list = get_image_files_list(source_dir)
    random.shuffle(image_file_list)
    process_images(image_file_list, dest_dir)


    #file = "C:\\work\\arduino\\pictures\\ConverterTo7color-2\\amber_and_dad.png"
    #file2 = "C:\\work\\arduino\\pictures\\ConverterTo7color-2\\amber_and_dad_7color2.bmp"
    #img1 = open_file(file)
    #img2 = crop_image_to_proper_ratio(img1)
    #img3 = resize_image(img2)
    #img4 = saturation(img3)
    #img5 = convert_image(img4)

    # img5.save(file2)
    # print(img5)
    # img5.show()


