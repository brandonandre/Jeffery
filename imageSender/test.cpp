#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <math.h>

#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include <opencv2/highgui/highgui.hpp>
#include "ImageSender.h"

using namespace std;
using namespace cv;



//Whether you want to receive the image
#define ENABLE_SERVER 1

int main(int argc, char *argv[]) {

//init image server
#if  ENABLE_SERVER
	ImageSender imageSender;

	if(imageSender.init() < 0)
	{
		return 1;
	}
#endif

	//open camera
	VideoCapture cap(0);
	if(!cap.isOpened())
		return -1;

    // will automatically be 1280, 720
    cap.set(CAP_PROP_FRAME_WIDTH, 320);
    cap.set(CAP_PROP_FRAME_HEIGHT, 200);

    Mat edges; //the processed image
    clock_t current_ticks, delta_ticks; //used to calculate fps
    clock_t fps = 0; //current fps
    for(;;)
    {
        current_ticks = clock();

        Mat frame; // the captured image
        cap >> frame; // get a new image from camera

        //process the image
        cvtColor(frame, edges, COLOR_BGR2GRAY);
        GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        Canny(edges, edges, 0, 30, 3);

    	//send the image
	#if  ENABLE_SERVER
        imageSender.send(&edges);
	    //imageSender.send(&edges);
	#endif

	    //calculate fps
        delta_ticks = clock() - current_ticks;
        if(delta_ticks > 0)
            fps = CLOCKS_PER_SEC / delta_ticks;
        cout << fps << endl;
	}

	return 0;
}