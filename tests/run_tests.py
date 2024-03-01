import unittest
import os
import glob
from PIL import Image
import convert_images
import process_image

class TestConvertImages(unittest.TestCase):
    IMAGE0_SIZE = (2048, 1536)
    IMAGE0_RESIZED = (800, 480)
    IMAGE0_REL_PATH = 'images\\IMG_20130702_134515.jpg'
    SCRATCH_IMAGE = "scratch_image"
    def testget_image_files_list(self):
        expected_file_names = ['IMG_20130702_134515.jpg', 'DSCN6796.JPG', 'DSCN6805.JPG']        
        file_list = convert_images.get_image_files_list("images")
        self.assertEqual(len(file_list), 3);
        filename_only_list = [os.path.basename(filename) for filename in file_list]
        self.assertEqual(expected_file_names, filename_only_list)
        
    def test_dest_pic_filename(self):
        filename = convert_images.dest_pic_filename(0)
        self.assertEqual('pic_000.bmp', filename)
    
    def test_open_file(self):
        with process_image.open_file(TestConvertImages.IMAGE0_REL_PATH) as image:
            image = process_image.open_file(TestConvertImages.IMAGE0_REL_PATH)
            self.assertEqual(TestConvertImages.IMAGE0_SIZE, image.size)
    
    def test_apply_exif_rotation_tag(self):
        with process_image.open_file(TestConvertImages.IMAGE0_REL_PATH) as image:
            exif_image = process_image.apply_exif_rotation_tag(image)
            self.assertEqual(TestConvertImages.IMAGE0_SIZE, image.size)
        
    def test_mirror(self):
        with process_image.open_file(TestConvertImages.IMAGE0_REL_PATH) as image:
            mirror_image = process_image.mirror(image)
            self.assertEqual(TestConvertImages.IMAGE0_SIZE, mirror_image.size)
            
    def test_convert_image(self):
        with process_image.open_file(TestConvertImages.IMAGE0_REL_PATH) as image:
            new_image = process_image.convert_image(image)
            self.assertEqual(TestConvertImages.IMAGE0_SIZE, new_image.size)
    
    def test_resize_image(self):
        with process_image.open_file(TestConvertImages.IMAGE0_REL_PATH) as image:
            resized_image = process_image.resize_image(image);
            self.assertEqual(TestConvertImages.IMAGE0_RESIZED, resized_image.size)
    
    def test_crop_image_to_proper_ratio(self):
        with process_image.open_file(TestConvertImages.IMAGE0_REL_PATH) as image:
            rotate_image = image.rotate(90, Image.Resampling.NEAREST, expand = 1)
            print(rotate_image.size)
            new_image = process_image.crop_image_to_proper_ratio(rotate_image)
            self.assertEqual((1536, 922), new_image.size)
    
    def test_process_images(self):
        if not os.path.exists(TestConvertImages.SCRATCH_IMAGE):
            os.makedirs(TestConvertImages.SCRATCH_IMAGE)
        file_list = convert_images.get_image_files_list('images')
        convert_images.process_images(file_list, TestConvertImages.SCRATCH_IMAGE)
        self.assertEqual(os.listdir(TestConvertImages.SCRATCH_IMAGE), ['pic_000.bmp', 'pic_001.bmp', 'pic_002.bmp'] )
        
        # Remove files in scratch directory
        if os.path.exists(TestConvertImages.SCRATCH_IMAGE):
            files = glob.glob(f"{TestConvertImages.SCRATCH_IMAGE}/*.bmp")
            for f in files:
                try:
                    os.remove(f)
                except OSError as e:
                    print("Error: %s : %s" % (f, e.strerror))

        # Remove the directory
        try:
            os.rmdir(TestConvertImages.SCRATCH_IMAGE)
        except OSError as e:
            print("Error: %s : %s" % (f, e.strerror))
           


if __name__ == '__main__':
    unittest.main()