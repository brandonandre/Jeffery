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
#define SEND_IMAGE 0
#define FRONT_FACE "data/haarcascades/haarcascade_frontalface_alt.xml"
#define FRONT_FACE_1 "data/haarcascades/haarcascade_profileface.xml"
#define FRONT_FACE_2 "data/haarcascades/haarcascade_frontalface_default.xml"
#define PROFILE_FACE "data/haarcascades/haarcascade_profileface.xml"

/* Function Prototypes */
vector<Rect> scanForPeople(Mat&);
void sendClienvoidtImage(Mat&);
int turnTowardsPerson(vector<Rect> facesFound);
long rangeMap(long, long, long, long, long);
void sendClientImage(Mat&);
int getArea(Rect face);

/* Global Variables */
ImageSender imageSender;
int framesToSend = 0;
int currentCascade = 0;

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
		printf("Video capturing has been started...\n");

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
			// Display image to screen.

			capture >> frame;
			if (frame.empty())
				break;

			// Frame for processing...
			Mat grayCameraImage = frame.clone();
			cvtColor(grayCameraImage, grayCameraImage, COLOR_BGR2GRAY);
			equalizeHist(grayCameraImage, grayCameraImage);

			//sendClientImage(frame);

			// Scan for people...
			vector<Rect> facesFound = scanForPeople(grayCameraImage);
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
				if (turnTowardsPerson(facesFound)) {
					wagTail();
					moveSeconds(2, FORWARDS);
				}
			}
			
		}
	} else {
		printf("Could not connect to the camera.");
	}
	
	return 0;
}

int turnTowardsPerson(vector<Rect> facesFound) {
	Rect faceToFollow = facesFound[0];
	for (size_t i = 1; i < facesFound.size(); i++) {
		if (getArea(facesFound[i]) > getArea(faceToFollow)) {
			printf("Bigger face selected.\n");
			faceToFollow = facesFound[i];
		}
	}

	printf("X: %d\n", (faceToFollow.x + (faceToFollow.width / 2)));

	int faceX = (faceToFollow.x + (faceToFollow.width / 2));
	long turnRadius = 0;
	if (currentCascade == 2) {
		turnRadius = rangeMap(faceX, 0, 640, 35, -35);
	} else {
		turnRadius = rangeMap(faceX, 80, 640, -35, 35);
	}

	// Check if directly facing the person.
	if (turnRadius > -15 && turnRadius < 15) {
		printf("Head on!\n");
		return 0;
	}

	printf("Turn Radius: %ld - X: %d\n", turnRadius, faceX);

	tightTurn(turnRadius);

	return 1;
}

int getArea(Rect face) {
	return face.height * face.width;
}

/*
 * Range mapping. Used to convert camera x to a radius. 
 */
long rangeMap(long x, long in_min, long in_max, long out_min, long out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void sendClientImage(Mat& cameraImage) {
	#if SEND_IMAGE
		imageSender.send(&cameraImage);	
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

	switch(currentCascade) {
		case 0:
			faceDetection.load(FRONT_FACE);
			printf("FRONT SCAN (1)\n");
			currentCascade++;
			break;
		case 1:
			faceDetection.load(FRONT_FACE_1);
			flip(cameraImage, cameraImage, 1);
			printf("PROFILE SCAN FLIPPED (2)\n");
			currentCascade++;
			break;
		case 2:
			faceDetection.load(FRONT_FACE_2);
			printf("FRONT SCAN (3)\n");
			currentCascade++;
			break;
		case 3:
			faceDetection.load(PROFILE_FACE);
			printf("PROFILE SCAN (4)\n");
			currentCascade = 0;
			break;
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
