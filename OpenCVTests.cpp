#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;
using std::cout;
int U = 1, L = 0, Hu = 100, Su = 255, Vu = 255, Hl = 0, Sl = 150, Vl = 150;
int contourWidth = 75;					//contour width less than this size gets disregarded

const int alpha_slider_max = 100;
int alpha_slider;
double alpha;
double beta;
Mat src1;
Mat src2;
Mat dst;

void tuning()
{
	
}

static void on_trackbar(int, void*)
{
	//alpha = (double)U / 255;
	//beta = (1.0 - alpha);
	//addWeighted(src1, alpha, src2, beta, 0.0, dst);
	//imshow("HSV Tuning", dst);
}

int main(int argc, char* argv[])
{
	VideoCapture cap(0); // open the video camera no. 0

	
	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the video cam" << endl;
		return -1;
	}
	

	//Mat 
	Mat background = imread("FakeRoad.png");
	namedWindow("HSV Tuning", WINDOW_AUTOSIZE);

	char TrackbarName[50];
	//sprintf(TrackbarName, "Alpha x %d", 255);
	createTrackbar("Upper H", "HSV Tuning", &Hu, 255, on_trackbar);
	createTrackbar("Upper S", "HSV Tuning", &Su, 255, on_trackbar);
	createTrackbar("Upper V", "HSV Tuning", &Vu, 255, on_trackbar);
	createTrackbar("Lower H", "HSV Tuning", &Hl, 255, on_trackbar);
	createTrackbar("Lower S", "HSV Tuning", &Sl, 255, on_trackbar);
	createTrackbar("Lower V", "HSV Tuning", &Vl, 255, on_trackbar);

	on_trackbar(alpha_slider, 0);

	
	for (int i = 0; i < 30; i++)
	{
		cap >> background;
	}
	

	//Invert the image
	flip(background, background, 1);
	
	//frame = background;
	while(1){
		//tuning();
		Mat frame;
		//Capture frame-by-frame
		cap >> frame;
		
		//Laterally invert the image / flip the image
		flip(frame, frame, 1);

		//Converting image from BGR to HSV color space.
		Mat hsv;
		cvtColor(frame, hsv, COLOR_BGR2HSV);

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
		Mat drawing = Mat::zeros(mask1.size(), CV_8UC3);
		
		//Minimum Area Rectangle object creation
		vector<RotatedRect> minRect(contours.size());

		double storedArea = 0;
		double area = 0;
		int entry = 0;

		//If contours are found
		if (contours.size() > 0)
		{
			for (int i = 0; i < contours.size(); i++)
			{
				storedArea = contourArea(contours[i], 1);
				if (storedArea < 0)
				{
					storedArea = storedArea * -1;
				}
				if (storedArea > area)
				{
					area = storedArea;
					entry = i;
				}
			}

			minRect[entry] = minAreaRect(Mat(contours[entry]));
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			Point2f rect_points[4];
			minRect[entry].points(rect_points);
			if (minRect[entry].size.height > contourWidth)
			{
				for (int j = 0; j < 4; j++)line(drawing, rect_points[j], rect_points[(j + 1) % 4], color, 1, 8);
				frame = drawing + frame;
			}
			
			
			//Mat kernel = Mat::ones(10, 10, CV_32F);
			//morphologyEx(mask1, mask1, cv::MORPH_OPEN, kernel);
		}
		namedWindow("Contours", WINDOW_AUTOSIZE);
		imshow("Contours", frame);
		imshow("Mask1", mask1);
		waitKey(1);
		
	}
	
	return 0;

}

