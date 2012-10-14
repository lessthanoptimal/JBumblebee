Java code for grabbing stereo images from a Bumblebee 2 stereo camera by PointGrey.  This code does not use the official 1394 linux drivers provided by PointGrey.  Historically they have been a bit flaky when it comes to linux and the drivery is very light weight without it.


Requirements: Linux, libdc1394-2

-------------------------------------------
Build Instructions:

1) Modify Makefile so that JDK points to the location of your JDK
2) run 'make'
3) If no errors, then libjbumblebee.so and testdriver are now in the same directory
4) Make sure the camera is plugged in an turned on
5) Test source code by running ./testdriver
6) I'll add a nice ant script later on.  For now you're on your own!

To collect a sequence of images:

1) Follow instructions above
2) type 'ant sequence'
3) type ./sequence.sh
4) To stop data collection hit control-c
