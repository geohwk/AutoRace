#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include "opencv2/imgproc/imgproc.hpp"
#include <string>
#include <thread>
#include <chrono>
#include <time.h>
#pragma warning(disable:4996)
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

using namespace cv;
using namespace std;
using std::cout;


//CONSTANTS
const int contourRatio = 3; //contour aspect ratio less than this size gets disregarded

//GLOBALS
int Hu = 100, Su = 255, Vu = 255, Hl = 0, Sl = 150, Vl = 150;
			
Point2f rect_points1[4];
Point2f rect_points2[4];
Point pt1, pt2, centralPt1, centralPt2, centralPt3, centralPt4;
int frameIncrement = 0;

String Difference;
String Direction;
string imagePath;
double differenceVal;
double RectPoints1Array[4] = { 0 };
double RectPoints2Array[4] = { 0 };
int closestPointLeft = 0;
int closestPointRight = 0;
double difference1;
double difference2;

double storedArea1 = 0;
double storedArea2 = 0;
double area = 0;
int contour1 = 0;
int contour2 = 0;
int rightLine = 0;

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
	time_t my_time = time(NULL);
	VideoCapture cap(0); // open the video camera no. 0

	RNG rng(12345);

	Mat drawing1, drawing2, mask1, mask2, background, hsv;
	Scalar color = Scalar(rng.uniform(0, 0), rng.uniform(0, 255), rng.uniform(0, 255));
	Scalar color2 = Scalar(rng.uniform(0, 255), rng.uniform(0, 0), rng.uniform(0, 255));
	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the video cam" << endl;
		return -1;
	}
	 
	trackbars();

	//Create contour object
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	

	//Invert the image
	//flip(background, background, 1);
	
	//frame = background;
	while(1){
		//for (int i = 0; i < 30; i++)
		//{
			cap >> background;
		//}
		time_t my_time = time(NULL);
		//printf("Start of HSV: %s", ctime(&my_time));
		//Converting image from BGR to HSV color space.
		cvtColor(background, hsv, COLOR_BGR2HSV);

		// Creating masks to detect the upper and lower red color.
		inRange(hsv, Scalar(Hl, Sl, Vl), Scalar(Hu, Su, Vu), mask1);

		
		
		//printf("Start of Find Contours: %s", ctime(&my_time));
		//Find contours from binary image
		findContours(mask1, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
		//printf("End of Find Contours: %s", ctime(&my_time));
		//Make drawing canvas 
		drawing1 = Mat::zeros(mask1.size(), CV_8UC3);
		drawing2 = Mat::zeros(mask1.size(), CV_8UC3);
		
		//Minimum Area Rectangle object creation
		vector<RotatedRect> minRect1(contours.size());
		vector<RotatedRect> minRect2(contours.size());

		storedArea1 = 0;
		storedArea2 = 0;
		area = 0;
		contour1 = 0;
		contour2 = 0;
	
		//printf("Start of sorting: %s", ctime(&my_time));
		if (contours.size() > 0) //If array of contours is greater than 0
		{
			for (int i = 0; i < contours.size(); i++) //Inrement through contour array
			{
				area = abs(contourArea(contours[i], 1)); //Acquires contour area from currently selected contour
				
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
			
			if (minRect1[contour1].center.x > minRect2[contour2].center.x)
			{
				minRect1[contour1].points(rect_points2);
				minRect2[contour2].points(rect_points1);
			}
			else {
				minRect1[contour1].points(rect_points1);
				minRect2[contour2].points(rect_points2);
			}
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

		//Take x values of contour 1
		for (int i = 0; i < 4; i++)
		{
				if (rect_points1[i].x > background.cols / 2)
				{
					if (rect_points1[i].x <= rect_points1[closestPointRight].x)
					{
						closestPointRight = i;
						difference2 = rect_points1[closestPointRight].x - (background.cols / 2);

						centralPt1.x = rect_points1[closestPointRight].x;
						centralPt1.y = 0;

						centralPt2.x = rect_points1[closestPointRight].x;
						centralPt2.y = background.rows;
					}
				}
				else
				{
					if (rect_points1[i].x >= rect_points1[closestPointLeft].x)
					{
						closestPointLeft = i;
						difference1 = background.cols / 2 - rect_points1[closestPointLeft].x;

						centralPt3.x = rect_points1[closestPointLeft].x;
						centralPt3.y = 0;

						centralPt4.x = rect_points1[closestPointLeft].x;
						centralPt4.y = background.rows;
					}
				}
		}
		
		

		for (int i = 0; i < 4; i++)
		{
				if (rect_points2[i].x > background.cols / 2)
				{
					if (rect_points2[i].x <= rect_points2[closestPointRight].x)
					{
						closestPointRight = i;
						difference2 = rect_points2[closestPointRight].x - (background.cols / 2);

						centralPt1.x = rect_points2[closestPointRight].x;
						centralPt1.y = 0;

						centralPt2.x = rect_points2[closestPointRight].x;
						centralPt2.y = background.rows;
					}
				}
				else
				{
					if (rect_points2[i].x >= rect_points2[closestPointLeft].x)
					{
						closestPointLeft = i;
						difference1 = background.cols / 2 - rect_points2[closestPointLeft].x;

						centralPt3.x = rect_points2[closestPointLeft].x;
						centralPt3.y = 0;

						centralPt4.x = rect_points2[closestPointLeft].x;
						centralPt4.y = background.rows;
					}
				}
		}
		//printf("Start of End: %s", ctime(&my_time));
		differenceVal = difference1 - difference2;
		
		//cout << "Difference From center:" << differenceVal << endl;
		
		pt1.x = background.cols / 2;
		pt1.y = 0;

		pt2.x = pt1.x;
		pt2.y = background.rows;

		//std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		line(background, pt1, pt2, CV_RGB(255, 0, 0));
		line(background, centralPt1, centralPt2, CV_RGB(0, 0, 255));
		line(background, centralPt3, centralPt4, CV_RGB(0, 255, 0));

		Difference = "Difference: " + to_string(differenceVal);
		if(differenceVal < 0){
			Direction = "Direction: Turn Left";
		}
		else {
			Direction = "Direction: Turn Right";
		}

		putText(background, Difference, Point(30, 30),FONT_HERSHEY_PLAIN, 0.8, Scalar(200, 200, 250), 1, false);
		putText(background, Direction, Point(30, 50), FONT_HERSHEY_PLAIN, 0.8, Scalar(200, 200, 250), 1, false);
		//Creat Viewing Windows
		imshow("Original Image", background);
		//imshow("Binary", mask1);
		waitKey(1);
		//printf("Difference: %d/r", differenceVal);
		
	}
	
	return 0;

}

