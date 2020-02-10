#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <stdio.h>
# define M_PI           3.14159265358979323846


using namespace cv;
using namespace std;

int edgeThresh = 1;
int edgeThreshScharr = 1;

Mat image, gray, blurImage, edge1, edge2, cedge, hsv, mask1;

const char* window_name1 = "Edge map : Canny default (Sobel gradient)";
const char* window_name2 = "Edge map : Canny with custom gradient (Scharr)";

int Hu = 255, Su = 40, Vu = 255, Hl = 0, Sl = 0, Vl = 182;

void trackbars()
{
	namedWindow("HSV Tuning", WINDOW_AUTOSIZE);
	createTrackbar("Upper H", "HSV Tuning", &Hu, 255);
	createTrackbar("Upper S", "HSV Tuning", &Su, 255);
	createTrackbar("Upper V", "HSV Tuning", &Vu, 255);
	createTrackbar("Lower H", "HSV Tuning", &Hl, 255);
	createTrackbar("Lower S", "HSV Tuning", &Sl, 255);
	createTrackbar("Lower V", "HSV Tuning", &Vl, 255);
}

void croppedArea(Mat edge1)
{

}

Mat detectLineSegments(Mat edge1)
{
	vector<Vec2f> lines; //Holds the result

	HoughLines(edge1, lines, 1, CV_PI / 180, 150, 0, 0); //Runs detection

	//Draw lines
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		line(image, pt1, pt2, Scalar(0, 0, 255), 3, LINE_AA);
	}
	return edge1;
}

/*
static void onTrackbar(int, void*)
{
	blur(hsv, blurImage, Size(3, 3));

	// Run the edge detector on grayscale
	Canny(blurImage, edge1, edgeThresh, edgeThresh * 3, 3);
	cedge = Scalar::all(0);

	image.copyTo(cedge, edge1);
	imshow(window_name1, cedge);

	/// Canny detector with scharr
	Mat dx, dy;
	Scharr(blurImage, dx, CV_16S, 1, 0);
	Scharr(blurImage, dy, CV_16S, 0, 1);
	Canny(dx, dy, edge2, edgeThreshScharr, edgeThreshScharr * 3);
	/// Using Canny's output as a mask, we display our result
	cedge = Scalar::all(0);
	image.copyTo(cedge, edge2);
	//imshow(window_name2, cedge);

	imshow("HSV", hsv);
}
*/

int main(int argc, char* argv[])
{
	
	while (1) {
		
		image = imread("TapeRoad.jpg");
		resize(image, image, Size(), 0.2, 0.2);

		if (image.empty())
		{
			printf("Cannot read image file");
			//help();
			return -1;
		}
		//cedge.create(image.size(), image.type());
		cvtColor(image, hsv, COLOR_RGB2HSV);

		trackbars();

		inRange(hsv, Scalar(Hl, Sl, Vl), Scalar(Hu, Su, Vu), mask1);

		blur(mask1, blurImage, Size(3, 3));

		Canny(blurImage, edge1, edgeThresh, edgeThresh * 3, 3);

		edge1 = detectLineSegments(edge1);

		// Create a window
		//namedWindow(window_name1, 1);
		//namedWindow(window_name2, 1);

		// create a toolbar
		//createTrackbar("Canny threshold default", window_name1, &edgeThresh, 100, onTrackbar);
		//createTrackbar("Canny threshold Scharr", window_name2, &edgeThreshScharr, 400, onTrackbar);

		// Show the image
		//onTrackbar(0, 0);
		imshow("Edges", image);
		//imshow("image", image);
		// Wait for a key stroke; the same function arranges events processing
		waitKey(1);

		
	}
	return 0;
}




