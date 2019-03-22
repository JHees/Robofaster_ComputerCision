#pragma once

#include "pch.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include<vector>
#include<queue>
#include<math.h>


using namespace cv;
using namespace std;



class va_ptr
{
public:
	va_ptr() {};
	va_ptr(double va, double ptr) :value(va), ptr(ptr) {};
	double value;
	double ptr;

	bool operator<(va_ptr va)
	{
		return value < va.value;
	}
};



Mat colorReduce(const Mat& input, int div)
{
	Mat output = input.clone();
	int row = output.rows;
	int col = output.cols*output.channels();
	uchar buf[256];
	for (int i = 0; i < 256; ++i)
	{
		buf[i] = i / div * div + div / 2;
	}
	for (int i = 0; i < row; ++i)
	{
		for (int j = 0; j < col; ++j)
		{
			output.ptr<uchar>(i)[j] = buf[output.ptr<uchar>(i)[j]];
		}
	}
	return output;
}



void Callback_S_Thre_APPLE(int tra, void* ptr)
{
	//Mat S_Thre;
	//Mat* ROI = (Mat*)ptr;
	//threshold((*ROI), S_Thre, tra, 255, 1);


	//imshow("S_Threshold_APPLE", S_Thre);

	//vector<Mat> cha_ROI;
	//split(S_Thre, cha_ROI);
	////imshow("ROI_H", cha_ROI[0]);
	////imshow("ROI_S", cha_ROI[1]);
	//imshow("ROI_V", cha_ROI[2]);
	////	*ROI = S_Thre;
    Mat S_Thre;
    Mat* ROI = (Mat*)ptr;
    threshold((*ROI), S_Thre, tra, 255, 1);
    //namedWindow("buf", WINDOW_AUTOSIZE);
   // imshow("buf", S_Thre);
    Size S_size(S_Thre.rows, S_Thre.cols);

    resize(S_Thre, S_Thre, Size(3, 3), 0, 0, INTER_LINEAR);
    //	resize(S_Thre, S_Thre, Size(S_Thre.rows / 5, S_Thre.cols / 5), 0, 0, INTER_NEAREST);

    	//resize(S_Thre, S_Thre, S_size, 0, 0, INTER_NEAREST);

    threshold(S_Thre, S_Thre, 180, 255, 0);
    imshow("S_Threshold_APPLE", S_Thre);
    *ROI = S_Thre;
   // cout << *ROI<<endl;
}



void Callback_empty(int tra, void* ptr)
{

}


vector<Vec2d> find_dense_point(const vector<va_ptr>& lines,Mat& img_,Scalar Sca, const Point& center=Point(0,0))
{
	vector<va_ptr> ret;
	vector<va_ptr> buf;
	for (size_t i = 0; i < lines.size()-1; ++i)
	{
		buf.push_back(va_ptr(lines[i + 1].value - lines[i].value, i));
	}
	sort(buf.begin(), buf.end());
	vector<va_ptr> buf2;
	for (size_t i = 0; i < buf.size() - 1; ++i)
	{
		buf2.push_back(va_ptr(buf[i + 1].value - buf[i].value, i));
	}
	sort(buf2.begin(), buf2.end());
	//for (size_t i = 0; i < buf2.size(); ++i)
	//{
	//	cout <<  '(' <<i<<','<< buf2[i].ptr<<','<< buf2[i].value <<") ";
	//	circle(lines_show, Point(double(i+1) / buf.size() * 500, buf2[i].value * 5), 2, Scalar(51, 255, 233), -1, 1);
	//}
	//cout << endl;

	vector<va_ptr> buf3;
	for (size_t i = 0; i < buf2.size() - 1; ++i)
	{
		
		buf3.push_back(va_ptr(buf2[i + 1].value - buf2[i].value, i));
	}

    sort(buf3.begin(), buf3.end());
	//for (size_t i = 0; i < buf3.size(); ++i)
	//{
	//	cout << '(' << i << ',' << buf3[i].ptr << ',' << buf3[i].value << ") ";
	//	circle(lines_show, Point(double(i+2) / buf.size() * 500, buf3[i].value * 5), 2, Scalar(255, 191, 51), -1, 1);
	//}
	
	int ptr =buf3.back().ptr+1;
	//cout << endl<<ptr<<endl;
	if(ptr<buf2.size())
	for (size_t i = ptr; i < buf2.size(); ++i)
	{
		if (buf2[ptr].ptr > buf2[i].ptr)
		{
			ptr = i;
		}
	}
	ptr=buf2[ptr].ptr+1;
	//cout << ptr << endl;
	for (size_t i = 0; i < buf.size(); ++i)
	{
		if (i >= ptr)
		{
			//circle(lines_show, Point(double(i) / buf.size() * 500, buf[i].value*5), 2, Scalar(255, 0, 0), -1, 1);
			ret.push_back(buf[i]);
		}
		//else
			//circle(lines_show, Point(double(i) / buf.size() * 500, buf[i].value*5), 2, Scalar(0, 0, 255), -1, 1);
		//cout << '(' << i << ',' << buf[i].ptr << ',' << buf[i].value << ") ";
	}
	//cout << endl;
    vector<Vec2d> lines_fin;

    double f_sum_rho = 0, f_sum_theta = 0;
    vector<int> boundary;
    for (size_t i = 0; i < ret.size(); ++i)
    {
        boundary.push_back(ret[i].ptr);
    }
    sort(boundary.begin(), boundary.end());
    for (size_t j = 0; j <= boundary.size(); ++j)
    {

        for (size_t i = (j == 0 ? 0 : boundary[j - 1] + 1); i <= (j == boundary.size() ? lines.size() - 1 : boundary[j]); ++i)
        {
            f_sum_rho += lines[i].value;
            f_sum_theta += lines[i].ptr;
        }
        //(j == boundary.size() ? lines_col.size() - 1 : boundary[j])-(j == 0 ? 0 : boundary[j - 1] + 1)
        f_sum_rho /= (j == boundary.size() ? lines.size() - 1 : boundary[j]) - (j == 0 ? 0 : boundary[j - 1] + 1) + 1;
        f_sum_theta /= (j == boundary.size() ? lines.size() - 1 : boundary[j]) - (j == 0 ? 0 : boundary[j - 1] + 1) + 1;
       // cout << f_sum_rho << ' ' << f_sum_theta << endl;
        lines_fin.push_back(Vec2d(f_sum_rho, f_sum_theta));
        f_sum_rho = 0;
        f_sum_theta = 0;
    }
    if (center != Point(0, 0))
    {
        for (size_t i = 0; i < lines_fin.size(); ++i)
        {
            //lines_fin[i][0] = lines_fin[i][0] + sqrt(pow(center.x, 2) + pow(center.y, 2))*cos(lines_fin[i][1] - atan(center.y / center.x));
            lines_fin[i][0] = lines_fin[i][0] + center.x*cos(lines_fin[i][1]) + center.y*sin(lines_fin[i][1]);
        }
    }
    //if (!img_.empty())
        //for (size_t i = 0; i < lines_fin.size(); ++i)
        //{
        //    double rho = lines_fin[i][0], theta = lines_fin[i][1];
        //    if (theta < 0)
        //    {
        //        theta += CV_PI;
        //        rho = -rho;
        //    }
    //    draw_lines_polar(img_, lines_fin[i], Sca);
    ////    Point pt1, pt2;
    ////    double a = cos(theta), b = sin(theta);
    ////    double x0 = a * rho, y0 = b * rho;
    ////    pt1.x = cvRound(x0 + 1000 * (-b));// +center.x;
    ////    pt1.y = cvRound(y0 + 1000 * (a));// +center.y;
    ////    pt2.x = cvRound(x0 - 1000 * (-b));// +center.x;
    ////    pt2.y = cvRound(y0 - 1000 * (a));// +center.y;
    ////    line(img_, pt1, pt2, Sca, 1, LINE_AA);
    //    //cout << rho << ' ' <<int(theta / CV_PI * 180) << endl;
    //}

    //for (size_t i = 0; i < lines.size(); ++i)
    //{
    //    circle(lines_show, Point((double)(lines[i].value) / img_canny.cols * 500, (double)(lines[i].ptr) <= 0.78 ? 100 : 250), 1, Scalar(255, 255, 255), -1, 1);
    //}

    //for (size_t i = 0; i < ret.size(); ++i)
    //{
    //    circle(lines_show, Point((double)(lines[ret[i].ptr].value) / img_canny.cols * 500, (double)(lines[ret[i].ptr].ptr) <= 0.78 ? 100 : 250), 2, Scalar(51, 255, 91), -1, 1);
    //    //cout << buf[i].value << "! ";
    //    //circle(lines_show, Point(double(i) / img_output.rows * 500, buf_fin[i] * 10), 0.5, Scalar(0, 255, 0), -1, 1);
    //}

    //for (size_t i = 0; i < lines_fin.size(); ++i)
    //{
    //    circle(lines_show, Point((double)(lines_fin[i][0]) / img_canny.cols * 500, (double)(lines_fin[i][1]) <= 0.78 ? 100 : 250), 3, Scalar(0, 0, 255), -1, 1);
    //}

	return lines_fin;
}


void ret_output(Mat& img, const vector<Point>&p, int ret)
{
    if (ret == 0)
    {

        cout << "unknown" << endl;
        Mat img_ret = imread("unknown.png");
        Mat mask = imread("unknown.png", 0);
        Mat imgROI = img(Rect(1, 1, img_ret.cols, img_ret.rows));
        img_ret.copyTo(imgROI, mask);
        return;
    }
        
    --ret;
    //Rect R(p[ret/3*4+ret%3],p[ret / 3 * 4 + ret % 3+5]);
    //rectangle(img, R, Scalar(0, 0, 255), 3);// int lineType = 8, int shift = 0)
    Point c(abs(p[ret / 3 * 4 + ret % 3].x + p[ret / 3 * 4 + ret % 3 + 5].x) / 2, abs(p[ret / 3 * 4 + ret % 3].y + p[ret / 3 * 4 + ret % 3 + 5].y) / 2);
    Size s(abs(p[ret / 3 * 4 + ret % 3].x - p[ret / 3 * 4 + ret % 3 + 5].x) / 2, abs(p[ret / 3 * 4 + ret % 3].y - p[ret / 3 * 4 + ret % 3 + 5].y) / 2);
    ellipse(img, c, s, 0,0, 360, Scalar(0, 0, 255), 3);
   
    string img_ret_filename;
    img_ret_filename.push_back(ret + '0'+1);
    img_ret_filename += ".png";
    //cout << img_ret_filename << endl;
    Mat img_ret = imread(img_ret_filename);
    Mat mask = imread(img_ret_filename, 0);
    Mat imgROI = img(Rect(1,1, mask.cols, mask.rows));
    img_ret.copyTo(imgROI , mask);
}

void draw_lines_polar(Mat& img, const Vec2d& lines,const Scalar& Sca)
{
    double rho = lines[0], theta = lines[1];
    Point pt1, pt2;
    double a = cos(theta), b = sin(theta);
    double x0 = a * rho, y0 = b * rho;
    pt1.x = cvRound(x0 + 1000 * (-b));
    pt1.y = cvRound(y0 + 1000 * (a));
    pt2.x = cvRound(x0 - 1000 * (-b));
    pt2.y = cvRound(y0 - 1000 * (a));
    line(img, pt1, pt2, Sca, 1, LINE_AA);
}