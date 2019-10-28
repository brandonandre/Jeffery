#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <math.h>
#include <fstream>

#define _CRT_SECURE_NO_WARNINGS

#define LEARNING

using namespace std;
using namespace cv;



static void help()
{
	cout << "\nThis program demonstrates the use of cv::CascadeClassifier class to detect objects (Face + eyes). You can use Haar or LBP features.\n"
		"This classifier can recognize many kinds of rigid objects, once the appropriate classifier is trained.\n"
		"It's most known use is for faces.\n"
		"Usage:\n"
		"./facedetect [--cascade=<cascade_path> this is the primary trained classifier such as frontal face]\n"
		"   [--nested-cascade[=nested_cascade_path this an optional secondary classifier such as eyes]]\n"
		"   [--scale=<image scale greater or equal to 1, try 1.3 for example>]\n"
		"   [--try-flip]\n"
		"   [filename|camera_index]\n\n"
		"see facedetect.cmd for one call:\n"
		"./facedetect --cascade=\"data/haarcascades/haarcascade_frontalface_alt.xml\" --nested-cascade=\"data/haarcascades/haarcascade_eye_tree_eyeglasses.xml\" --scale=1.3\n\n"
		"During execution:\n\tHit any key to quit.\n"
		"\tUsing OpenCV version " << CV_VERSION << "\n" << endl;
}

void detectAndDraw(Mat& img, CascadeClassifier& cascade,
	CascadeClassifier& nestedCascade,
	CascadeClassifier& mouthCascade,
	double scale, bool tryflip);

string cascadeName;
string nestedCascadeName;
string mouthCascadeName;
long double lowest_right = 0, lowest_left = 0, lowest_mouth = 0;
long double highest_left = 0, highest_right = 0, highest_mouth = 0;

int main(int argc, const char** argv)
{
	VideoCapture capture;
	Mat frame, image;
	string inputName;
	bool tryflip;
	CascadeClassifier cascade, nestedCascade, mouthCascade;
	double scale;

	cv::CommandLineParser parser(argc, argv,
		"{help h||}"
		"{cascade|data/haarcascades/haarcascade_frontalface_alt.xml|}"
		"{nested-cascade|data/haarcascades/haarcascade_eye_tree_eyeglasses.xml|}"
		"{scale|1|}{try-flip||}{@filename||}"
	);
	if (parser.has("help"))
	{
		help();
		return 0;
	}
	cascadeName = parser.get<string>("cascade");
	nestedCascadeName = parser.get<string>("nested-cascade");
	mouthCascadeName = "data/haarcascades/haarcascade_smile.xml";
	scale = parser.get<double>("scale");
	if (scale < 1)
		scale = 1;
	tryflip = parser.has("try-flip");
	inputName = parser.get<string>("@filename");
	if (!parser.check())
	{
		parser.printErrors();
		return 0;
	}
	if (!nestedCascade.load(samples::findFileOrKeep(nestedCascadeName)))
		cerr << "WARNING: Could not load classifier cascade for nested objects" << endl;
	if (!mouthCascade.load(samples::findFileOrKeep(mouthCascadeName)))
		cerr << "WARNING: Could not load classifier cascade for nested objects" << endl;
	if (!cascade.load(samples::findFile(cascadeName)))
	{
		cerr << "ERROR: Could not load classifier cascade" << endl;
		help();
		return -1;
	}
	if (inputName.empty() || (isdigit(inputName[0]) && inputName.size() == 1))
	{
		int camera = inputName.empty() ? 0 : inputName[0] - '0';
		if (!capture.open(camera))
		{
			cout << "Capture from camera #" << camera << " didn't work" << endl;
			return 1;
		}
	}
	else if (!inputName.empty())
	{
		image = imread(samples::findFileOrKeep(inputName), IMREAD_COLOR);
		if (image.empty())
		{
			if (!capture.open(samples::findFileOrKeep(inputName)))
			{
				cout << "Could not read " << inputName << endl;
				return 1;
			}
		}
	}
	else
	{
		image = imread(samples::findFile("lena.jpg"), IMREAD_COLOR);
		if (image.empty())
		{
			cout << "Couldn't read lena.jpg" << endl;
			return 1;
		}
	}

	if (capture.isOpened())
	{
		cout << "Video capturing has been started ..." << endl;

		for (;;)
		{
			capture >> frame;
			if (frame.empty())
				break;

			Mat frame1 = frame.clone();
			detectAndDraw(frame1, cascade, nestedCascade, mouthCascade, scale, tryflip);

			char c = (char)waitKey(10);
			if (c == 27 || c == 'q' || c == 'Q') {
#ifdef LEARNING
				FILE* outputFile = fopen("C:\\TEMP\\output.txt", "w");
				fprintf(outputFile, "Left lowest: %lf highest: %lf\n", lowest_left, highest_left);
				fprintf(outputFile, "Right lowest: %lf highest: %lf\n", lowest_right, highest_right);
				fprintf(outputFile, "Mouth lowest: %lf highest: %lf\n", lowest_mouth, highest_mouth);
				fclose(outputFile);
#endif
				break;
			}
		}
	}
	else
	{
		cout << "Detecting face(s) in " << inputName << endl;
		if (!image.empty())
		{
			detectAndDraw(image, cascade, nestedCascade, mouthCascade, scale, tryflip);
			waitKey(0);
		}
		else if (!inputName.empty())
		{
			/* assume it is a text file containing the
			list of the image filenames to be processed - one per line */
			FILE* f = fopen(inputName.c_str(), "rt");
			if (f)
			{
				char buf[1000 + 1];
				while (fgets(buf, 1000, f))
				{
					int len = (int)strlen(buf);
					while (len > 0 && isspace(buf[len - 1]))
						len--;
					buf[len] = '\0';
					cout << "file " << buf << endl;
					image = imread(buf, 1);
					if (!image.empty())
					{
						detectAndDraw(image, cascade, nestedCascade, mouthCascade, scale, tryflip);
						char c = (char)waitKey(0);
						if (c == 27 || c == 'q' || c == 'Q') {
#ifdef LEARNING
							FILE* outputFile = fopen("C:\\TEMP\\output.txt", "w");
							fprintf(outputFile, "Left lowest: %lf highest: %lf\n", lowest_left, highest_left);
							fprintf(outputFile, "Right lowest: %lf highest: %lf\n", lowest_right, highest_right);
							fprintf(outputFile, "Mouth lowest: %lf highest: %lf\n", lowest_mouth, highest_mouth);
							fclose(outputFile);
#endif
							break;
						}
					}
					else
					{
						cerr << "Aw snap, couldn't read image " << buf << endl;
					}
				}
				fclose(f);
			}
		}
	}

	return 0;
}

void detectAndDraw(Mat& img, CascadeClassifier& cascade,
	CascadeClassifier& nestedCascade,
	CascadeClassifier& mouthCascade,
	double scale, bool tryflip)
{
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
	if (tryflip)
	{
		flip(smallImg, smallImg, 1);
		cascade.detectMultiScale(smallImg, faces2,
			1.1, 2, 0
			//|CASCADE_FIND_BIGGEST_OBJECT
			//|CASCADE_DO_ROUGH_SEARCH
			| CASCADE_SCALE_IMAGE,
			Size(30, 30));
		for (vector<Rect>::const_iterator r = faces2.begin(); r != faces2.end(); ++r)
		{
			faces.push_back(Rect(smallImg.cols - r->x - r->width, r->y, r->width, r->height));
		}
	}
	t = (double)getTickCount() - t;
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

#ifdef LEARNING
			if (lowest_left == 0) {
				lowest_left = left_distance;
			}
			else if (lowest_left > left_distance) {
				lowest_left = left_distance;
			}

			if (lowest_right == 0) {
				lowest_right = right_distance;
			}
			else if (lowest_right > right_distance) {
				lowest_right = right_distance;
			}

			if (lowest_mouth == 0) {
				lowest_mouth = mouth_distance;
			}
			else if (lowest_mouth > mouth_distance) {
				lowest_mouth = mouth_distance;
			}

			if (highest_left == 0) {
				highest_left = left_distance;
			}
			else if (highest_left < left_distance) {
				highest_left = left_distance;
			}

			if (highest_right == 0) {
				highest_right = right_distance;
			}
			else if (highest_right < right_distance) {
				highest_right = right_distance;
			}

			if (highest_mouth == 0) {
				highest_mouth = mouth_distance;
			}
			else if (highest_mouth < mouth_distance) {
				highest_mouth = mouth_distance;
			}
#endif LEARNING

			if ((left_distance >= 1.629754 && left_distance <= 1.761912) && (right_distance >= 1.586607 && right_distance <= 1.801996) && (mouth_distance >= 1.352993 && mouth_distance <= 1.584232)) {
				printf("HI MICHAEL!\n");
			}
			else {
				printf("Left eye to face center: %lf Right eye to face center: %lf Mouth by eye distance: %lf\n", left_distance, right_distance, mouth_distance);
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
	imshow("result", img);
}
