OBJS	= Utils.o Motor_Controller.o I2C.o DIO.o
SOURCE	= Utils.cpp Motor_Controller.cpp I2C.c DIO.c
HEADER	= 
OUT		= libmyrio.a
SUBDIR  = csupport
CC		= /usr/local/oecore-x86_64/sysroots/x86_64-nilrtsdk-linux/usr/bin/arm-nilrt-linux-gnueabi/arm-nilrt-linux-gnueabi-g++ 
CFLAGS	= --sysroot=/usr/local/oecore-x86_64/sysroots/cortexa9-vfpv3-nilrt-linux-gnueabi -std=c++11 -pthread
INCLUDE = -Icsupport -I"/usr/local/oecore-x86_64/sysroots/cortexa9-vfpv3-nilrt-linux-gnueabi/usr/include" -I"/usr/local/oecore-x86_64/sysroots/cortexa9-vfpv3-nilrt-linux-gnueabi/usr/include/c++/4.9.2/arm-nilrt-linux-gnueabi" 
FLAGS	= -g -c -Wall -DMyRio_1900 -O0 -g3 -Wall -std=gnu++11 -fmessage-length=0 -mfpu=vfpv3 -mfloat-abi=softfp
LFLAGS 	= -Lcsupport -lcsupport -ldl


all: $(OBJS)
	ar cr $(OUT) csupport/libcsupport.a $(OBJS)

Utils.o: Utils.cpp
		$(CC) $(CFLAGS) $(INCLUDE) $(FLAGS) Utils.cpp 

Motor_Controller.o: Motor_Controller.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(FLAGS) Motor_Controller.cpp 

I2C.o: I2C.c
	$(CC) $(CFLAGS) $(INCLUDE) $(FLAGS) I2C.c

DIO.o: DIO.c
	$(CC) $(CFLAGS) $(INCLUDE) $(FLAGS) DIO.c


clean:
	rm -f $(OBJS) $(OUT)



