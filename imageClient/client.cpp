#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

int main(int argc, char *argv[]) {

	int sockfd = 0;
	struct sockaddr_in serv_addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if( sockfd < 0 )
	{
		printf("\n Error : Could not create socket \n");
		return 1;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(5000);

	if (inet_pton(AF_INET, "172.16.0.1", &serv_addr.sin_addr) <= 0)
	{
		printf("\n inet_pton error occured\n");
		return 1;
	}

	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("\n Error : Connect Failed \n");
		return 1;
	}

	Mat decodedMat;
	int byteReceived = 0;
	vector<uchar> buffer;
	for (;;)
	{

		int numOfByteToReceive;
		byteReceived = read(sockfd, &numOfByteToReceive, 4);

		if(numOfByteToReceive == -1)
			break;

		buffer = vector<uchar>(numOfByteToReceive);

		byteReceived = 0;
		while (byteReceived < numOfByteToReceive)
		{
			byteReceived += read(sockfd, &buffer[0 + byteReceived], numOfByteToReceive - byteReceived);
		}

		decodedMat = imdecode(Mat(buffer), 1);

		Size size(1920, 1080);
		resize(decodedMat, decodedMat, size);

		imshow("Client", decodedMat);
        if(waitKey(1) == 27) break;

	}

	close(sockfd);

	return 0;
}
