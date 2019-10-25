#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;
using std::cout;


//CONSTANTS
const int contourRatio = 5; //contour aspect ratio less than this size gets disregarded

//GLOBALS
int Hu = 100, Su = 255, Vu = 255, Hl = 0, Sl = 150, Vl = 150;
				
int frameIncrement = 0;

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


int main(int argc, char* argv[])
{
	VideoCapture cap(0); // open the video camera no. 0

	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the video cam" << endl;
		return -1;
	}
	 
	

	trackbars();

	/*
	for (int i = 0; i < 30; i++)
	{
		cap >> background;
	}
	*/

	//Invert the image
	//flip(background, background, 1);
	
	//frame = background;
	while(1){
		Mat background = imread("FakeRoad.png");
		//tuning();
		//Mat frame;
		//Capture frame-by-frame
		//cap >> frame;
		
		//Laterally invert the image / flip the image
		//flip(frame, frame, 1);

		//Converting image from BGR to HSV color space.
		Mat hsv;
		cvtColor(background, hsv, COLOR_BGR2HSV);

		Mat mask1, mask2;
		// Creating masks to detect the upper and lower red color.
		inRange(hsv, Scalar(Hl, Sl, Vl), Scalar(Hu, Su, Vu), mask1);
		//inRange(hsv, Scalar(170, 120, 70), Scalar(180, 255, 255), mask2);

		// Combine masks together
		//mask1 = mask1 + mask2;

		RNG rng(12345);

		//Create contour object
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;

		//Find contours from binary image
		findContours(mask1, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
		
		//Make drawing canvas 
		Mat drawing1 = Mat::zeros(mask1.size(), CV_8UC3);
		Mat drawing2 = Mat::zeros(mask1.size(), CV_8UC3);
		
		//Minimum Area Rectangle object creation
		vector<RotatedRect> minRect1(contours.size());
		vector<RotatedRect> minRect2(contours.size());

		double storedArea1 = 0;
		double storedArea2 = 0;
		double area = 0;
		int contour1 = 0;
		int contour2 = 0;

		//If contours are found
		if (contours.size() > 0)
		{
			for (int i = 0; i < contours.size(); i++)
			{
				area = contourArea(contours[i], 1);
				if (area < 0)
				{
					area = area * -1;
				}


				if (area > storedArea2 && area < storedArea1)
				{
					storedArea2 = area;
					contour2 = i;
				}
				if (area > storedArea2 && area > storedArea1)
				{
					storedArea2 = storedArea1;
					contour2 = contour1;
					storedArea1 = area;
					contour1 = i;
				}
			}

			minRect1[contour1] = minAreaRect(Mat(contours[contour1]));
			minRect2[contour2] = minAreaRect(Mat(contours[contour2]));
			Scalar color = Scalar(rng.uniform(0, 0), rng.uniform(0, 255), rng.uniform(0, 255));
			Scalar color2 = Scalar(rng.uniform(0, 255), rng.uniform(0, 0), rng.uniform(0, 255));
			Point2f rect_points1[4];
			Point2f rect_points2[4];
			minRect1[contour1].points(rect_points1);
			minRect2[contour2].points(rect_points2);

			if ((minRect1[contour1].size.height/minRect1[contour1].size.width > contourRatio)|| (minRect1[contour1].size.width / minRect1[contour1].size.height > contourRatio))
			{
				for (int j = 0; j < 4; j++)line(drawing1, rect_points1[j], rect_points1[(j + 1) % 4], color, 5, 8);
				background = drawing1 + background;
			}
			if ((minRect2[contour2].size.height / minRect2[contour2].size.width > contourRatio) || (minRect2[contour2].size.width / minRect2[contour2].size.height > contourRatio))
			{
				for (int j = 0; j < 4; j++)line(drawing2, rect_points2[j], rect_points2[(j + 1) % 4], color2, 5, 8);
				background = drawing2 + background;
			}
		}

		//Creat Viewing Windows
		imshow("Original Image", background);
		imshow("Binary", mask1);
		waitKey(1);
		
	}
	
	return 0;

}

