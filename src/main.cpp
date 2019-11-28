/**
 * Main Robotic Code
 */
#include "robot.h"
#include "ImageSender.h"

#include "opencv2/opencv.hpp"
#include <iostream>
#include <math.h>
#include <fstream>

using namespace std;
using namespace cv;

/* Options */
#define SEND_IMAGE 1
#define CASCADE_TO_APPLY "data/haarcascades/haarcascade_frontalface_alt.xml"

/* Function Prototypes */
vector<Rect> scanForPeople(Mat&);
void sendClienvoidtImage(Mat&);
void turnTowardsPerson(vector<Rect> facesFound);
long rangeMap(long, long, long, long, long);
void sendClientImage(Mat&);

/* Global Variables */
ImageSender imageSender;
int framesToSend = 0;

/*
 * Start the camera and begin the robotic flow.
 */
int main(int argc, const char** argv) {
	VideoCapture capture(0);
	Mat frame, image;

	// Setup the motor controller.
	printf("Jeffrey starting up.\n");
    begin();

	// Start capturing video from the webcam.
	if (capture.isOpened()) {
		printf("Video capturing has been started ...\n");

		// Setup the image sender.
		#if SEND_IMAGE
			printf("Starting server to send images...\n");
			if(imageSender.init() < 0) {
				printf("Image sender could not be setup. Contining without it.\n");
			} else {
				printf("Image sending setup.\n");
			}
		#endif

		int framesCounted = 0;

		while(true) {
			capture >> frame;
			if (frame.empty())
				break;

			// Frame for viewing...
			Mat frame1 = frame.clone();
			Mat grayCameraImage;
			cvtColor(frame1, grayCameraImage, COLOR_BGR2GRAY);
			equalizeHist(grayCameraImage, grayCameraImage);
			resize(grayCameraImage, grayCameraImage, cv::Size(), INTER_LINEAR)
			
			// Send the image out to the client program for viewing.
			sendClientImage(grayCameraImage);

			// Scan for people...
			vector<Rect> facesFound = scanForPeople(frame1);
			if (facesFound.empty()) {
				framesCounted++;
				// Check if alone for a long time.
				if (framesCounted == 10) {
					printf("Alone mode!\n");
					framesCounted = 0;
				}
			} else {
				// Face(s) found! Turn towards the person.
				framesCounted = 0;
				printf("Turn towards the person.\n");
				turnTowardsPerson(facesFound);
			}
			
		}
	} else {
		printf("Could not connect to the camera.");
	}
	
	return 0;
}

void turnTowardsPerson(vector<Rect> facesFound) {
	Rect faceToFollow = facesFound[0];
	long turnRadius = rangeMap((faceToFollow.x + (faceToFollow.width / 2)), 0, 1000, -50, 50);
	printf("Turn Radius: %ld - X: %d\n", turnRadius, (faceToFollow.x + (faceToFollow.width / 2)));
	tightTurn(turnRadius);
}

/*
 * Range mapping. Used to convert camera x to a radius. 
 */
long rangeMap(long x, long in_min, long in_max, long out_min, long out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void sendClientImage(Mat& cameraImage) {
	#if SEND_IMAGE
		framesToSend++;

		if (framesToSend == 3) {
			framesToSend = 0;
			imageSender.send(&cameraImage);	
		}
	#endif	
}

/**
 * Search for a person to hit the HAAR classifier that matches a person. 
 *
 */
vector<Rect> scanForPeople(Mat& cameraImage) {

	// Array of rectangles to display on the image.
	vector<Rect> faces;

	// Setup the face detection classifier.
	CascadeClassifier faceDetection;
	if (!faceDetection.load(CASCADE_TO_APPLY)) {
		printf("Error - Could not load the classifer file.");
	}

	// Run the detection.
	faceDetection.detectMultiScale(
		cameraImage,
		faces,
		1.1,
		3,
		0 | CASCADE_SCALE_IMAGE,
		Size(30, 30)
	);

	// Render the faces onto the screen.
	for (size_t i = 0; i < faces.size(); i++) {
		// Specific face to render a box around.
		Rect face = faces[i];
		rectangle(cameraImage, 
				  Point(cvRound(face.x), cvRound(face.y)),
				  Point(cvRound((face.x + face.width - 1)), cvRound((face.y + face.height - 1))),
				  Scalar(255,0,255), 
				  3, 
				  8, 
				  0
		);
	}

	// Return the rectangle for additional processing.
	return faces;
}
