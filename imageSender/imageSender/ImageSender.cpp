#include "ImageSender.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <vector>

#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

int ImageSender::init()
{
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(	listenfd < 0)
	{
		perror("ERROR: socket\n");
		return -1;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(5000);

	if(bind(listenfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("ERROR: bind\n");
		return -1;
	}

	if(listen(listenfd, 10))
	{
		perror("ERROR: listen\n");
		return -1;
	}

	connfd = accept(listenfd, (struct sockaddr*) NULL, NULL);
	if(connfd < 0)
	{
		perror("ERROR: accept\n");
		return -1;
	}

	return 0;
}

void ImageSender::send( Mat* img )
{
	vector<uchar> buf;

	imencode("*.jpg", *img, buf);

	int length = buf.size();

	write(connfd, &length, 4);
	write(connfd, &buf[0], length);
	//printf("SERVER SENT: %d total bytes\n\n", buf.size());&buf[0]
}

void ImageSender::closeClient()
{
	int closeFlag = -1;
	write(connfd, &closeFlag, 4);
}

ImageSender::~ImageSender()
{
	closeClient();
	sleep(1);
	close(connfd);
}
