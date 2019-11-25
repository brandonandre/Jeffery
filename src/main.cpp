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
#define SEND_IMAGE 1;

/* Function Prototypes */
void detectAndDraw(
	Mat& img, 
	CascadeClassifier& cascade,
	CascadeClassifier& nestedCascade,
	CascadeClassifier& mouthCascade,
	double scale, 
	bool 
);

string cascadeName;
string nestedCascadeName;
string mouthCascadeName;
long double lowest_right = 0, lowest_left = 0, lowest_mouth = 0;
long double highest_left = 0, highest_right = 0, highest_mouth = 0;
ImageSender imageSender;

int main(int argc, const char** argv) {
	VideoCapture capture;
	Mat frame, image;
	string inputName;
	CascadeClassifier cascade, nestedCascade, mouthCascade;
	double scale;

	// Setup the motor controller.
    begin();

	// Start capturing video from the webcam.
	if (capture.isOpened()) {
		cout << "Video capturing has been started ..." << endl;

		// Setup the image sender.
		if(imageSender.init() < 0) {
			cout << "Image sender could not be setup. Contining without it." << endl;
		}

		while(true) {
			capture >> frame;
			if (frame.empty())
				break;

			Mat frame1 = frame.clone();
			detectAndDraw(frame1, cascade, nestedCascade, mouthCascade, scale);
		}
	}
	
	return 0;
}

/**
 * 
 * Detect and Draw on top of image.
 * 
 * @param img Webcam capture at a particular moment to analyze.
 * 
 */
void detectAndDraw(Mat& img, CascadeClassifier& cascade, CascadeClassifier& nestedCascade,
	CascadeClassifier& mouthCascade, double scale) {

	double t = 0;
	vector<Rect> faces, faces2;
	const static Scalar colors[] =
	{
		Scalar(255,0,0),
		Scalar(255,128,0),
		Scalar(255,255,0),
		Scalar(0,255,0),
		Scalar(0,128,255),
		Scalar(0,255,255),
		Scalar(0,0,255),
		Scalar(255,0,255)
	};
	Mat gray, smallImg;

	cvtColor(img, gray, COLOR_BGR2GRAY);
	double fx = 1 / scale;
	resize(gray, smallImg, Size(), fx, fx, INTER_LINEAR_EXACT);
	equalizeHist(smallImg, smallImg);

	t = (double)getTickCount();
	cascade.detectMultiScale(smallImg, faces,
		1.1, 2, 0
		//|CASCADE_FIND_BIGGEST_OBJECT
		//|CASCADE_DO_ROUGH_SEARCH
		| CASCADE_SCALE_IMAGE,
		Size(30, 30));

	//printf("detection time = %g ms\n", t * 1000 / getTickFrequency());
	for (size_t i = 0; i < faces.size(); i++)
	{
		Rect r = faces[i];
		Mat smallImgROI;
		vector<Rect> nestedObjects, mouths;
		Point center;
		Scalar color = colors[i % 8];
		int radius;

		double aspect_ratio = (double)r.width / r.height;
		if (0.75 < aspect_ratio && aspect_ratio < 1.3)
		{
			center.x = cvRound((r.x + r.width * 0.5) * scale);
			center.y = cvRound((r.y + r.height * 0.5) * scale);
			radius = cvRound((r.width + r.height) * 0.25 * scale);
			circle(img, center, radius, color, 3, 8, 0);
		}
		else
			rectangle(img, Point(cvRound(r.x * scale), cvRound(r.y * scale)),
				Point(cvRound((r.x + r.width - 1) * scale), cvRound((r.y + r.height - 1) * scale)),
				color, 3, 8, 0);
		if (nestedCascade.empty())
			continue;
		if (mouthCascade.empty())
			continue;
		smallImgROI = smallImg(r);
		nestedCascade.detectMultiScale(smallImgROI, nestedObjects,
			1.1, 3, 0
			//|CASCADE_FIND_BIGGEST_OBJECT
			//|CASCADE_DO_ROUGH_SEARCH
			//|CASCADE_DO_CANNY_PRUNING
			| CASCADE_SCALE_IMAGE,
			Size(30, 30));
		/*I wrote this part*/
		mouthCascade.detectMultiScale(smallImgROI, mouths,
			1.1, 25, 0
			//|CASCADE_FIND_BIGGEST_OBJECT
			//|CASCADE_DO_ROUGH_SEARCH
			//|CASCADE_DO_CANNY_PRUNING
			| CASCADE_SCALE_IMAGE,
			Size(30, 30));
		for (size_t j = 0; j < mouths.size(); j++)
		{
			Rect nr1 = mouths[j];
			center.x = cvRound((r.x + nr1.x + nr1.width * 0.5) * scale);
			center.y = cvRound((r.y + nr1.y + nr1.height * 0.5) * scale);
			radius = cvRound((nr1.width + nr1.height) * 0.25 * scale);
			circle(img, center, radius, colors[1], 3, 8, 0);
		}
		if (nestedObjects.size() % 2 == 0 && nestedObjects.size() >= 2 && mouths.size() >= 1) {
			/*Get left eye and right eye*/
			Rect left_eye = nestedObjects[0];
			Rect right_eye = nestedObjects[1];

			Rect mouth = mouths[0];

			Point center_right, center_left, face_center, mouth_center;

			/*Get x and y of center of right eye*/
			center_right.x = cvRound((r.x + right_eye.x + right_eye.width * 0.5) * scale);
			center_right.y = cvRound((r.y + right_eye.y + right_eye.height * 0.5) * scale);

			/*Get x and y of center of left eye*/
			center_left.x = cvRound((r.x + left_eye.x + left_eye.width * 0.5) * scale);
			center_left.y = cvRound((r.y + left_eye.y + left_eye.height * 0.5) * scale);

			/*Get center of mouth*/
			mouth_center.x = cvRound((r.x + mouth.x + mouth.width * 0.5) * scale);
			mouth_center.y = cvRound((r.y + mouth.y + mouth.height * 0.5) * scale);

			/*compute distance between right eye center and left eye center*/
			long double distance_eyes = sqrt(pow(abs(center_left.x - center_right.x), 2) + pow(abs(center_left.y - center_right.y), 2));

			/*get center of face*/
			face_center.x = cvRound((r.x + r.width * 0.5) * scale);
			face_center.y = cvRound((r.y + r.height * 0.5) * scale);

			long double left_to_center = sqrt(pow(abs(center_left.x - face_center.x), 2) + pow(abs(center_left.y - face_center.y), 2));
			long double right_to_center = sqrt(pow(abs(center_right.x - face_center.x), 2) + pow(abs(center_right.y - face_center.y), 2));
			long double mouth_to_center = sqrt(pow(abs(mouth_center.x - face_center.x), 2) + pow(abs(mouth_center.y - face_center.y), 2));

			long double left_distance = distance_eyes / left_to_center;
			long double right_distance = distance_eyes / right_to_center;
			long double mouth_distance = distance_eyes / mouth_to_center;

			if ((left_distance >= 1.629754 && left_distance <= 1.761912) && (right_distance >= 1.586607 && right_distance <= 1.801996) && (mouth_distance >= 1.352993 && mouth_distance <= 1.584232)) {
				printf("HI MICHAEL!\n");
                moveDistance(300, SPEED, FORWARDS);
			}
			else {
				printf("Left eye to face center: %lf Right eye to face center: %lf Mouth by eye distance: %lf\n", left_distance, right_distance, mouth_distance);
                wagTail();
				shake();
            }
			
		}
		/*End of stuff I wrote*/
		for (size_t j = 0; j < nestedObjects.size(); j++)
		{
			Rect nr = nestedObjects[j];
			center.x = cvRound((r.x + nr.x + nr.width * 0.5) * scale);
			center.y = cvRound((r.y + nr.y + nr.height * 0.5) * scale);
			radius = cvRound((nr.width + nr.height) * 0.25 * scale);
			circle(img, center, radius, color, 3, 8, 0);
		}
	}

	/* Image Sending */
	#if SEND_IMAGE
	    imageSender.send(&img);
	#endif
}
