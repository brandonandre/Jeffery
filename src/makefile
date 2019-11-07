OBJS	= main.o movement.o
SOURCE	= main.cpp movement.cpp
OUT		= main.out
SUBDIR  = myrio
SENDER  = imageSender
CC		= /usr/local/oecore-x86_64/sysroots/x86_64-nilrtsdk-linux/usr/bin/arm-nilrt-linux-gnueabi/arm-nilrt-linux-gnueabi-g++ 
CFLAGS	= --sysroot=/usr/local/oecore-x86_64/sysroots/cortexa9-vfpv3-nilrt-linux-gnueabi -std=c++11 -pthread
INCLUDE = -I"/opencvLb/include/opencv4/opencv2" -Imyrio -Imyrio/csupport -I"/usr/local/oecore-x86_64/sysroots/cortexa9-vfpv3-nilrt-linux-gnueabi/usr/include" -I"/usr/local/oecore-x86_64/sysroots/cortexa9-vfpv3-nilrt-linux-gnueabi/usr/include/c++/4.9.2/arm-nilrt-linux-gnueabi" 
FLAGS	= -g -c -Wall -DMyRio_1900 -O0 -g3 -Wall -std=gnu++11 -fmessage-length=0 -mfpu=vfpv3 -mfloat-abi=softfp
LFLAGS 	= -L/opencvLib/lib/opencv -Lmyrio -Lmyrio/csupport -lcsupport -lmyrio -ldl
# why is there a -limagesender there's no library for that... it needs the opencv stuff..

all: $(OBJS)
	cd $(SUBDIR) && make
	cd $(SENDER) && make
	$(CC) $(CFLAGS) $(INCLUDE) -g $(OBJS) -o $(OUT)  $(LFLAGS)

main.o: main.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(FLAGS) main.cpp

movement.o: movement.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(FLAGS) movement.cpp

clean:
	cd $(SUBDIR) && make clean
	rm -f $(OBJS) $(OUT)
