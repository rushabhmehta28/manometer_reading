#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <vector>
#include <opencv2/imgproc.hpp>
#include <math.h>
#include <yaml.h>

using namespace cv;
int main(int argc, char** argv)
{
	if (argc != 2) {
		printf("usage: DisplayImage.out <Image_Path>\n");
		return -1;
	}
	Mat image;
	image = imread(argv[1], 1);
	if (!image.data) {
		printf("No image data \n");
		return -1;
	}

	Mat blurred; double sigma = 1, threshold = 5, amount = 1;
	GaussianBlur(image, blurred, Size(), sigma, sigma);
	Mat lowContrastMask = abs(image - blurred) < threshold;
	Mat sharpened = image*(1+amount) + blurred*(-amount);
	image.copyTo(sharpened, lowContrastMask);
	
	
	Mat greyMat;
	cvtColor(image, greyMat, COLOR_RGB2GRAY);

	Mat Blurred_greyMat;
	medianBlur(greyMat, Blurred_greyMat, 5);

	std::vector<Vec3f> circles;
	HoughCircles(Blurred_greyMat, circles, HOUGH_GRADIENT, 1,
                 Blurred_greyMat.rows*0.1,  // change this value to detect circles with different distances to each other
                 100, 30, Blurred_greyMat.rows*0.4, Blurred_greyMat.rows*0.45 // change the last two parameters
            // (min_radius & max_radius) to detect larger circles
    );
	float avg_x = 0, avg_y = 0, avg_r = 0;
	for( size_t i = 0; i < circles.size(); i++ )
	{ 
		Vec3i c = circles[i];
		avg_x = avg_x + c[0];
		avg_y = avg_y + c[1];
		avg_r = avg_r + c[2];
	}
	avg_x = avg_x/circles.size();
	avg_y = avg_y/circles.size();
	avg_r = avg_r/circles.size();

	Mat detected_edges;
	Canny(Blurred_greyMat, detected_edges, 150, 200, 5);
	std::vector<Vec4i> linesP;
	HoughLinesP(detected_edges, linesP, 1, CV_PI/180, 50, avg_r * 0.5, 10);
	
	float distance_min = 2000;
	int index = 0;
	float diff1LowerBound = 0;
	float diff2LowerBound = 0.3;
	float diff1UpperBound = 0.3;
	float diff2UpperBound = 1.0;

	int min_angle = 50;
	int max_angle = 360-40;
	int min_value = 0;
	int max_value = 14;
	std::string units = "Bar";
	
	for( size_t i = 0; i < linesP.size(); i++ )
	{
		
		Vec4i l = linesP[i];
		//Point point1 = Point(l[0], l[1]);
		//Point point2 = Point(l[2], l[3]);
		//line( image, point1, point2, Scalar(0,255,00), 2,  LINE_AA);
		//std::cout<<"The index is"<< index <<std::endl;
		//if (i == 0)
		//{
		//	float distance_min = 2000;
		//	int index = 0;
		//}	
		float diff1 = pow(pow((l[0]-avg_x),2.0) + pow((l[1]-avg_y),2.0),0.5);
		float diff2 = pow(pow((l[2]-avg_x),2.0) + pow((l[3]-avg_y),2.0),0.5);
		float length = pow(pow((l[2]-l[0]),2.0) + pow((l[3]-l[1]),2.0),0.5);
		if (diff1 > diff2)
		{
			std::swap(diff1, diff2);
		}
		if ( (diff1 < (diff1UpperBound * avg_r)) && (diff1 > (diff1LowerBound * avg_r)) && (diff2 < (diff2UpperBound * avg_r)) && (diff2 > (diff2LowerBound*avg_r)))
		{
			float slope = l[3] - l[1];
			float c = -slope*l[2] + l[3];
			float distance = pow((pow((slope*avg_x + avg_y + c),2)),0.5)/pow((pow(slope,2.0) + 1),0.5);
			float length = pow((pow((l[0]-l[2]),2.0) + pow((l[1]-l[3]),2.0) ),0.5);
			if 	((distance < distance_min) && (length >  Blurred_greyMat.rows * 0.1))
			{	
				distance_min = distance;
				index = i;
			}
		}
	}
	Vec4i selected_line = linesP[index];
	if (linesP.size()) 
	{
		float dist_pt_0 = pow(pow((selected_line[0]-avg_x),2.0) + pow((selected_line[1]-avg_y),2.0),0.5); //((selected_line[0]-avg_x)^2.0 + (selected_line[1]-avg_y)^2.0)^0.5;
		float dist_pt_1 = pow(pow((selected_line[2]-avg_x),2.0) + pow((selected_line[3]-avg_y),2.0),0.5); //((selected_line[2]-avg_x)^2.0 + (selected_line[3]-avg_y)^2.0)^0.5;
		float x_angle, y_angle;
		if (dist_pt_0 > dist_pt_1)
			{
				x_angle = selected_line[0]-avg_x;
				y_angle = avg_y - selected_line[1];
			} else
			{
				x_angle = selected_line[2]-avg_x;
				y_angle = avg_y - selected_line[3];
			}

		float res = atan( y_angle / x_angle )*180/3.14159265;
		float final_angle;
		if ((x_angle > 0) && (y_angle > 0))  //in quadrant I
			{
				final_angle = 270 - res;
				//std::cout<<"I am in first quadrant"<<std::endl;
			}
		if ((x_angle < 0) && (y_angle > 0))  //in quadrant II
			{
				final_angle = 90 - res;
				//std::cout<<"I am in second quadrant"<<std::endl;
			}
		if ((x_angle < 0) && (y_angle < 0))  //in quadrant III
			{
				//std::cout<<"I am in thrid quadrant"<<std::endl;
				final_angle = 90 - res;
			}
		if ((x_angle > 0) && (y_angle < 0))  //in quadrant IV
			{
				final_angle = 270 - res;
				//std::cout<<"I am in forth quadrant"<<std::endl;
			}

		
		int angle_range = max_angle - min_angle;
		int value_range = max_value - min_value;
		float final_value = ((final_angle - min_angle)*value_range)/angle_range + min_value;
		std::cout << "The final angle is :"<< final_angle <<"°. Value is "<< final_value<<" Bar" << std::endl;
	} else
	{
		std::cout << "No lines found"  << std::endl;
	}
    //Ptr<SiftFeatureDetector> detector = SiftFeatureDetector::create();
    //std::vector<KeyPoint> keypoints;
    //detector->detect(image, keypoints);
    //Mat output;
    //drawKeypoints(greyMat, keypoints, output);
	Point point1 = Point(selected_line[0], selected_line[1]);
	Point point2 = Point(selected_line[2], selected_line[3]);
	
	// Printing the outputs
	Point center = Point(avg_x, avg_y);
	circle( image, center, 1, Scalar(0,255,00), 3, LINE_AA);
	circle( image, center, avg_r, Scalar(0,255,00), 3, LINE_AA);
	line( image, point1, point2, Scalar(0,255,00), 2,  LINE_AA);
	//showing the image
	namedWindow("Display Image", WINDOW_AUTOSIZE);
	imshow("Display Image", image);
	waitKey(0);
	return 0;
}