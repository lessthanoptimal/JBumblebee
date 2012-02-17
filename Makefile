SRC = src/BumblebeeDriver.c src/boofcv_drivers_stereo_CameraBumblebee2.c test/test_driver.c
TARG = src/BumblebeeDriver.o src/boofcv_drivers_stereo_CameraBumblebee2.o test/test_driver.o libjbumblebee testdriver

JDK=/opt/jdk/latest
CC = gcc

CFLAGS = -O3 -fPIC -Wall -Wno-unused-function -Isrc/ -I$(JDK)/include -I$(JDK)/include/linux  

LDFLAGS = -lc -ldc1394

OBJ = $(SRC:.c=.o)

all: $(TARG)

libjbumblebee: src/BumblebeeDriver.o src/boofcv_drivers_stereo_CameraBumblebee2.o
	$(CC) -shared -Wl,-soname,libjbumblebee.so -o libjbumblebee.so src/BumblebeeDriver.o src/boofcv_drivers_stereo_CameraBumblebee2.o $(LDFLAGS)

testdriver: src/BumblebeeDriver.o test/test_driver.o
	$(CC) -o testdriver src/BumblebeeDriver.o test/test_driver.o $(LDFLAGS)

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@


clean: 
	rm -f *~ *.o *.so src/*.o test/*.o testdriver
