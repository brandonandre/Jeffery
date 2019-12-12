#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <opencv2/highgui/highgui_c.h>

using namespace cv;
using namespace std;

/*

https://github.com/udit043/Hand-Recognition-using-OpenCV

Using this to get a more accurate hand guesture functionality. The problem with the one I linked, is that it uses and old version
of openCV. We are using opencv-4.1.1-vc14_vc15 , so you will need to try to convert the code from version 2.4.10 to 4.1.1.

I found this great reference, that shows a good bunch of potential methods to use and replace.

Good learning resource to get to know the methods: https://www.bogotobogo.com/cplusplus/files/OReilly%20Learning%20OpenCV.pdf

The official doc for openCV: https://docs.opencv.org/4.1.1/

NOTE: All of this has to do this that github link, we found these resources late in the game. So we didn't get the chance to implement them.

*/
int main(int argc, const char** argv)
{

	// Get the camera
	VideoCapture cam(0);
	// Check if the camera is open
	if (!cam.isOpened()) {
		cout << "ERROR not opened " << endl;
		return -1;
	}
	Mat img;
	Mat img_threshold;
	Mat img_gray;
	Mat img_roi;
	namedWindow("Original_image", CV_WINDOW_AUTOSIZE);
	//namedWindow("Gray_image", CV_WINDOW_AUTOSIZE);
	//namedWindow("Thresholded_image", CV_WINDOW_AUTOSIZE);
	namedWindow("ROI", CV_WINDOW_AUTOSIZE);

	char screenText[40];
	// finger counter
	int fingers = 0;

	while (true) {

		bool b = cam.read(img);

		if (!b) {
			cout << "ERROR : cannot read" << endl;
			return -1;
		}

		Rect roi(340, 100, 270, 270);
		img_roi = img(roi);
		cvtColor(img_roi, img_gray, CV_RGB2GRAY);

		GaussianBlur(img_gray, img_gray, Size(19, 19), 0.0, 0);
		threshold(img_gray, img_threshold, 0, 255, THRESH_BINARY_INV + THRESH_OTSU);

		vector<vector<Point> >contours;
		vector<Vec4i>hierarchy;
		findContours(img_threshold, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point());
		if (contours.size() > 0) {
			size_t indexOfBiggestContour = -1;
			size_t sizeOfBiggestContour = 0;

			for (size_t i = 0; i < contours.size(); i++) {
				if (contours[i].size() > sizeOfBiggestContour) {
					sizeOfBiggestContour = contours[i].size();
					indexOfBiggestContour = i;
				}
			}

			vector<vector<int> >hull(contours.size());
			vector<vector<Point> >hullPoint(contours.size());
			vector<vector<Vec4i> >defects(contours.size());
			vector<vector<Point> >defectPoint(contours.size());
			vector<vector<Point> >contours_poly(contours.size());
			Point2f rect_point[4];
			vector<RotatedRect>minRect(contours.size());
			vector<Rect> boundRect(contours.size());
			for (size_t i = 0; i < contours.size(); i++) {
				if (contourArea(contours[i]) > 5000) {
					convexHull(contours[i], hull[i], true);
					convexityDefects(contours[i], hull[i], defects[i]);
					if (indexOfBiggestContour == i) {
						minRect[i] = minAreaRect(contours[i]);
						for (size_t k = 0; k < hull[i].size(); k++) {
							int ind = hull[i][k];
							hullPoint[i].push_back(contours[i][ind]);
						}
						fingers = 0;

						for (size_t k = 0; k < defects[i].size(); k++) {
							if (defects[i][k][3] > 13 * 256) {
								int p_end = defects[i][k][1];
								int p_far = defects[i][k][2];
								defectPoint[i].push_back(contours[i][p_far]);
								circle(img_roi, contours[i][p_end], 3, Scalar(0, 255, 0), 2);
								// Increment when you find a finger
								fingers++;
							}

						}

						if (fingers == 1)
							strcpy(screenText, "1");
						else if (fingers == 2)
							strcpy(screenText, "2");
						else if (fingers == 3)
							strcpy(screenText, "3");
						else if (fingers == 4)
							strcpy(screenText, "4");
						else if (fingers == 5)
							strcpy(screenText, "5");
						else
							strcpy(screenText, "Welcome !!");
						// Display the text on screen
						putText(img, screenText, Point(70, 70), CV_FONT_HERSHEY_SIMPLEX, 3, Scalar(255, 0, 0), 2, 8, false);

						drawContours(img_threshold, contours, i, Scalar(255, 255, 0), 2, 8, vector<Vec4i>(), 0, Point());
						drawContours(img_threshold, hullPoint, i, Scalar(255, 255, 0), 1, 8, vector<Vec4i>(), 0, Point());
						drawContours(img_roi, hullPoint, i, Scalar(0, 0, 255), 2, 8, vector<Vec4i>(), 0, Point());
						approxPolyDP(contours[i], contours_poly[i], 3, false);
						boundRect[i] = boundingRect(contours_poly[i]);
						rectangle(img_roi, boundRect[i].tl(), boundRect[i].br(), Scalar(255, 0, 0), 2, 8, 0);
						minRect[i].points(rect_point);
						for (size_t k = 0; k < 4; k++) {
							line(img_roi, rect_point[k], rect_point[(k + 1) % 4], Scalar(0, 255, 0), 2, 8);
						}

					}
				}

			}
			imshow("Original_image", img);
			//imshow("Gray_image", img_gray);
			//imshow("Thresholded_image", img_threshold);
			imshow("ROI", img_roi);
			if (waitKey(30) == 27) {
				return -1;
			}

		}

	}

	return 0;
}

