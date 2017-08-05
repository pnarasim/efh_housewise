#!/bin/bash

captured_image=/home/pi/captured_image.jpg
cropped_image=/home/pi/cropped_image.jpg
converted_image=/home/pi/converted_image.tif
fswebcam  $captured_image
#display $captured_image
convert $captured_image -crop 200x250+50+50 $cropped_image
#display $cropped_image

sudo convert -density 300 $cropped_image -auto-level -compress none  $converted_image
 
#display $converted_image

tesseract $converted_image outputbase digits
cat outputbase.txt


