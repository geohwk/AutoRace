#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <stdio.h>

# define M_PI  3.14159265358979323846


using namespace cv;
using namespace std;

int edgeThresh = 1, edgeThreshScharr = 1, intersectX, intersectY;

Mat image, gray, blurImage, edge1, edge2, cedge, hsv, mask1;
vector<Vec2f> lines; //Holds the result

Point diffPnt1, diffPnt2;

double rightLine[2];
double leftLine[2];

const char* window_name1 = "Edge map : Canny default (Sobel gradient)";
const char* window_name2 = "Edge map : Canny with custom gradient (Scharr)";

int Hu = 255, Su = 158, Vu = 255, Hl = 0, Sl = 0, Vl = 182;

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
	HoughLines(edge1, lines, 1, CV_PI / 180, 100, 0, 0); //Runs detection

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

void ProcessImage()
{
	cvtColor(image, hsv, COLOR_RGB2HSV);

	inRange(hsv, Scalar(Hl, Sl, Vl), Scalar(Hu, Su, Vu), mask1);

	blur(mask1, blurImage, Size(5, 5));

	Canny(blurImage, edge1, edgeThresh, edgeThresh * 3, 3);

	edge1 = detectLineSegments(edge1);
}

void DetermineSide()
{
	double mLeft = 0, mRight = 0, interceptLeft, interceptRight;
	rightLine[0] = 0;
	leftLine[0] = 0;
	rightLine[1] = 0;
	leftLine[1] = 0;
	for (int i = 0; i < lines.size(); i++)
	{
		if ((lines[i][0] < 0) && ((lines[i][1] > rightLine[1]) || (leftLine[1] == 0)))
		{
			rightLine[0] = lines[i][0];
			rightLine[1] = lines[i][1];
		}
		else if ((lines[i][0] > 0) && ((lines[i][1] < leftLine[1]) || (leftLine[1] == 0)))
		{
			leftLine[0] = lines[i][0];
			leftLine[1] = lines[i][1];
		}
	}

	float rho = rightLine[0], theta = rightLine[1];
	Point pt1, pt2;
	double a = cos(theta), b = sin(theta);
	double x0 = a * rho, y0 = b * rho;
	pt1.x = cvRound(x0 + 1000 * (-b));
	pt1.y = cvRound(y0 + 1000 * (a));
	pt2.x = cvRound(x0 - 1000 * (-b));
	pt2.y = cvRound(y0 - 1000 * (a));
	if(rightLine[0] != 0)line(image, pt1, pt2, Scalar(255, 255, 0), 3, LINE_AA);

	

	if ((pt2.x != 0) && (pt1.x != 0)) 
	{
		mRight = (double)((double)pt2.y - (double)pt1.y) / ((double)pt2.x - (double)pt1.x);
		interceptRight = pt2.y - mRight * pt2.x;
	}

	rho = leftLine[0], theta = leftLine[1];
	a = cos(theta), b = sin(theta);
	x0 = a * rho, y0 = b * rho;
	pt1.x = cvRound(x0 + 1000 * (-b));
	pt1.y = cvRound(y0 + 1000 * (a));
	pt2.x = cvRound(x0 - 1000 * (-b));
	pt2.y = cvRound(y0 - 1000 * (a));
	if(leftLine[0] != 0)line(image, pt1, pt2, Scalar(0, 255, 0), 3, LINE_AA);

	if ((pt2.x != 0) && (pt1.x != 0))
	{
		mLeft = (double)((double)pt1.y - (double)pt2.y) / ((double)pt1.x - (double)pt2.x);
		interceptLeft = pt1.y - mLeft * pt1.x;
	}

	if ((mLeft != 0) && (mRight != 0)) 
	{
		intersectX = ((interceptRight - interceptLeft) / (mLeft - mRight));
		//intersectY = leftLine[1] * intersectX + leftLine[0];

		diffPnt1.x = intersectX;
		diffPnt1.y = 0;
		diffPnt2.x = diffPnt1.x;
		diffPnt2.y = image.rows;

		line(image, diffPnt1, diffPnt2, Scalar(0, 255, 255), 3, LINE_AA);

		diffPnt1.y = image.rows / 2;
		diffPnt1.x = image.cols / 2;
		diffPnt2.y = diffPnt1.y;

		line(image, diffPnt1, diffPnt2, Scalar(0, 100, 255), 3, LINE_AA);
	}
	


}

int main(int argc, char* argv[])
{
	VideoCapture cap(0);
	if (!cap.isOpened())cout << "Cannot open video feed" << endl;

	cap >> image;
	 
	while (1) {
		cap >> image;

		trackbars();

		ProcessImage();

		DetermineSide();
		
		// Show the image
		imshow("Edges", image);
		imshow("image", edge1);
		// Change to 0 to wait for keypress to continue
		waitKey(1);

		
	}
	return 0;
}




