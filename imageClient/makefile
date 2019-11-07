OBJS	= client.o
SOURCE	= client.cpp
HEADER	= 
OUT		= client.out
CC		= g++ 
CFLAGS	= -std=c++11 -pthread
INCLUDE = -I/usr/local/include/opencv4/
FLAGS	= -g -c -Wall -O0 -g3 -std=gnu++11 -fmessage-length=0
LFLAGS 	= -L/home/robot/build/lib/ -L/usr/local/lib/ -ldl -lopencv_calib3d -lopencv_core -lopencv_dnn -lopencv_features2d -lopencv_flann -lopencv_gapi -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_stitching -lopencv_video -lopencv_videoio

all: $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) -g $(OBJS) -o $(OUT)  $(LFLAGS)

client.o: client.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(FLAGS) client.cpp

clean:
	rm -f $(OBJS) $(OUT)

