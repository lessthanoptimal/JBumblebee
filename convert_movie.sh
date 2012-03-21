#/bin/sh
#cd images

# compress using a lossless codec
ffmpeg -i left%07d.ppm -vcodec ffv1 left.avi
ffmpeg -i left%07d.ppm -vcodec ffv1 right.avi

#delete original images
rm -rf left*.ppm right*.ppm
