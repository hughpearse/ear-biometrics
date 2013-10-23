/*
 * To compile:
 * g++ -I/usr/include/opencv2 process.cpp -o process.out -lopencv_core -lopencv_highgui
 *
 * Author: Hugh Pearse
 *
 */
#include <iostream>
#include <string>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

class WatershedSegmenter{
private:
	cv::Mat markers;
public:
	void setMarkers(cv::Mat& markerImage){
		markerImage.convertTo(markers, CV_32S);
	}

	cv::Mat process(cv::Mat &image){
		cv::watershed(image, markers);
		markers.convertTo(markers,CV_8U);
		return markers;
	}
};

Mat autocrop(int thresh, Mat src){
	Mat src_gray;
	RNG rng(12345);

	cvtColor(src, src_gray, CV_BGR2GRAY);
	blur(src_gray, src_gray, Size(3,3));

	Mat threshold_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	/// Detect edges using Threshold
	threshold( src_gray, threshold_output, thresh, 255, THRESH_BINARY );
	/// Find contours
	findContours( threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

	/// Find the rotated rectangles and ellipses for each contour
	vector<RotatedRect> minRect( contours.size() );
	vector<RotatedRect> minEllipse( contours.size() );

	for( int i = 0; i < contours.size(); i++ )
	{
		minRect[i] = minAreaRect( Mat(contours[i]) );
		if( contours[i].size() > 5 ){
			minEllipse[i] = fitEllipse( Mat(contours[i]) );
		}
	}

	/// Draw contours + rotated rects + ellipses
	Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
	int i = 0;
	RotatedRect box = minAreaRect( Mat(contours[i]) );
	double angle = box.angle;

	if (angle < -45.)
		angle += 90.;

	Point2f vertices[4];
	box.points(vertices);
	//for(int i = 0; i < 4; ++i)
		//line(src, vertices[i], vertices[(i + 1) % 4], cv::Scalar(255, 0, 0), 1, CV_AA);

	Mat rot_mat = cv::getRotationMatrix2D(box.center, angle, 1);
	Mat rotated;
	warpAffine(src, rotated, rot_mat, src.size(), cv::INTER_CUBIC);

	Size box_size = box.size;
	if (box.angle < -45.)
		swap(box_size.width, box_size.height);
	Mat cropped;
	getRectSubPix(rotated, box_size, box.center, cropped);	

	//imshow("Result", cropped);
	//waitKey(0);
	//imwrite(filename,cropped);
	return cropped;
}

Mat myWatershed(Mat input){
	cv::Mat image = input;
	cv::Mat blank(image.size(),CV_8UC4,cv::Scalar(0x00));
	cv::Mat dest;
	dest.convertTo(dest,CV_8UC4);
	//imshow("originalimage", image);

	// Create markers image
	cv::Mat markers(image.size(),CV_8U,cv::Scalar(-1));
	//Rect(topleftcornerX, topleftcornerY, width, height);
	//top rectangle
	markers(Rect(0,0,image.cols, 5)) = Scalar::all(1);
	//bottom rectangle
	markers(Rect(0,image.rows-5,image.cols, 5)) = Scalar::all(1);
	//left rectangle
	markers(Rect(0,0,5,image.rows)) = Scalar::all(1);
	//right rectangle
	markers(Rect(image.cols-5,0,5,image.rows)) = Scalar::all(1);
	//centre rectangle
	int centreW = image.cols/4;
	int centreH = image.rows/4;
	markers(Rect((image.cols/2)-(centreW/2),(image.rows/2)-(centreH/2), centreW, centreH)) = Scalar::all(2);
	markers.convertTo(markers,CV_BGR2GRAY);
	//imshow("markers", markers);

	//Create watershed segmentation object
	WatershedSegmenter segmenter;
	segmenter.setMarkers(markers);
	cv::Mat wshedMask = segmenter.process(image);
	cv::Mat mask;
	convertScaleAbs(wshedMask, mask, 1, 0);
	double thresh = threshold(mask, mask, 1, 255, THRESH_BINARY);
	bitwise_and(image, image, dest, mask);

	/*
	//imshow("final_result", dest);
	//cv::waitKey(0);
	std::string newname = argv[1];
	newname.erase(newname.end()-3, newname.end()-0);
	newname.append("PNG");

	unsigned found = newname.find_last_of("/\\");
	newname = newname.substr(found+1);
	newname.insert(0,"");    
	cout << newname << "\n";
	imwrite(newname,dest);
	*/
	return dest;
}

string getMetrics(Mat image){
	std::string metrics = "";
	//number of metrics output is 10 - last one omitted
	int intervals = 10+1;
	int offset = image.rows/intervals;
	for(int i = offset; i < offset*intervals; i=i+offset){
		int left = 0;
		for(int j = 0; j < image.cols; j++){
			Vec4b bgrtPixel = image.at<Vec4b>(i, j);
			int blue = bgrtPixel.val[0];
			int green = bgrtPixel.val[1];
			int red = bgrtPixel.val[2];
			int trans = bgrtPixel.val[3];
			//cout << "red: " << red << "\n";
			//cout << "green: " << green << "\n";
			//cout << "blue: " << blue << "\n";
			//cout << "trans: " << trans << "\n";

			if((red == 0x00 && green == 0x00 && blue == 0x00) || trans == 0x00 || (red == 0xff && green == 0xff && blue == 0xff)){
				left++;
			} else {
				break;
			}
		}
		ostringstream convert;
		convert << left;
		metrics += convert.str();
		//if(i < offset*(intervals-1))
		metrics += ",";
	}
	//std::string newname = argv[1];
	//unsigned found = newname.find_last_of("/\\");
	//newname = newname.substr(found+1);
	//cout << newname << "\n";
	return metrics;
}

int main(int argc, char* argv[]){
	try{  
		if(argc != 2){
			throw 1;
		}
	} catch(int e) {
		cout << "Usage: ./app input.png output.png" << "\n";
	}
	cv::Mat image = cv::imread(argv[1]);
	image = myWatershed(image);
	int thresh = 1;
	image = autocrop(thresh, image);
	string metrics = getMetrics(image);
	cout << metrics << argv[1] << "\n";
	return 0;
}
