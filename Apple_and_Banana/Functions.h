#pragma once

#include "pch.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include<vector>
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

void Callback_V(int tra, void* ptr)
{
	vector<Mat>* cha = (vector<Mat>*)ptr;
	(*cha)[2] = Mat((*cha)[2].rows, (*cha)[2].cols, CV_8UC1, Scalar(tra));
}

void Callback_S_Thre_APPLE(int tra, void* ptr)
{
	Mat S_Thre;
	Mat* ROI = (Mat*)ptr;
	threshold((*ROI), S_Thre, tra, 255, 1);


	imshow("S_Threshold_APPLE", S_Thre);

	vector<Mat> cha_ROI;
	split(S_Thre, cha_ROI);
	//imshow("ROI_H", cha_ROI[0]);
	//imshow("ROI_S", cha_ROI[1]);
	imshow("ROI_V", cha_ROI[2]);
	//	*ROI = S_Thre;

}

void Callback_Thre_V(int tra, void* ptr)
{
	Mat Thre_V;
	vector<Mat>*cha = (vector<Mat>*)ptr;
	threshold((*cha)[2], Thre_V, tra, 255, 1);
	//imshow("Thre_V before blur", Thre_V);
	//blur(Thre_V, Thre_V, Size(3, 3));
	vector<vector<Point>> con;
	findContours(Thre_V, con, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	for (int i = 0; i < con.size(); ++i)
		for (int j = 0; j < con[i].size(); ++j)
		{
			cout << con[i][j] << ' ';
			Thre_V.ptr<Scalar>(i)[j] = Scalar(125);
		}
	cout << endl;

	imshow("Thre_V", Thre_V);
}

void Callback_empty(int tra, void* ptr)
{

}


vector<Vec2d> find_dense_point(const vector<va_ptr>& lines,Mat& img_canny,Mat& lines_show)
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
        cout << f_sum_rho << ' ' << f_sum_theta << endl;
        lines_fin.push_back(Vec2d(f_sum_rho, f_sum_theta));
        f_sum_rho = 0;
        f_sum_theta = 0;
    }

    for (size_t i = 0; i < lines_fin.size(); ++i)
    {
        double rho = lines_fin[i][0], theta = lines_fin[i][1];
        if (theta < 0)
        {
            theta += CV_PI;
            rho = -rho;
        }
            
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a * rho, y0 = b * rho;
        pt1.x = cvRound(x0 + 1000 * (-b));
        pt1.y = cvRound(y0 + 1000 * (a));
        pt2.x = cvRound(x0 - 1000 * (-b));
        pt2.y = cvRound(y0 - 1000 * (a));
        line(img_canny, pt1, pt2, Scalar(55, 100, 195), 2, LINE_AA);
        //cout << rho << ' ' <<int(theta / CV_PI * 180) << endl;
    }
    for (size_t i = 0; i < lines.size(); ++i)
    {
        circle(lines_show, Point((double)(lines[i].value) / img_canny.cols * 500, (double)(lines[i].ptr) <= 0.78 ? 100 : 250), 1, Scalar(255, 255, 255), -1, 1);
    }

    for (size_t i = 0; i < ret.size(); ++i)
    {
        circle(lines_show, Point((double)(lines[ret[i].ptr].value) / img_canny.cols * 500, (double)(lines[ret[i].ptr].ptr) <= 0.78 ? 100 : 250), 2, Scalar(51, 255, 91), -1, 1);
        //cout << buf[i].value << "! ";
        //circle(lines_show, Point(double(i) / img_output.rows * 500, buf_fin[i] * 10), 0.5, Scalar(0, 255, 0), -1, 1);
    }

    for (size_t i = 0; i < lines_fin.size(); ++i)
    {
        circle(lines_show, Point((double)(lines_fin[i][0]) / img_canny.cols * 500, (double)(lines_fin[i][1]) <= 0.78 ? 100 : 250), 3, Scalar(0, 0, 255), -1, 1);
    }
	return lines_fin;
}


