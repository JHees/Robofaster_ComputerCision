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

class named_va
{
public:
    named_va() {};
    named_va(string s, double v) :name(s), value(v) {};
    string name;
    double value;
    named_va operator-(const named_va& va)
    {
        return named_va(name, value - va.value);
    }
    friend ostream& operator<<(ostream&, const named_va&);
};
void draw_lines_polar(Mat& img, const Vec2d& lines, const Scalar& Sca)
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

ostream& operator<<(ostream& out, const named_va& nv)
{
    out << nv.name << ": " << nv.value*1000/getTickFrequency() << "ms " << endl;
    return out;
}


void colorReduce(const Mat& input,Mat& output, int div)
{
	//int row = input.rows;
	//int col = input.cols*input.channels();
	//uchar buf[256];
	//for (int i = 0; i < 256; ++i)
	//{
	//	buf[i] = i / div * div + div / 2;
	//}
	//for (int i = 0; i < row; ++i)
	//{
	//	for (int j = 0; j < col; ++j)
	//	{
 //           input.ptr<uchar>(i)[j] = buf[input.ptr<uchar>(i)[j]];
	//	}
	//}
 //   return input;
    Mat Table(1, 256, CV_8U);
    uchar*p = Table.data;
    for (int i = 0; i < 256; ++i)
    {
        p[i] = i / div * div + div / 2;
    }
    LUT(input, Table, output);

}



void Callback_S_Thre_APPLE(int tra, void* ptr)
{
    Mat* ROI = (Mat*)ptr;
    threshold((*ROI), (*ROI), tra, 255, 1);
    //Size S_size((*ROI).rows, (*ROI).cols);
    resize((*ROI), (*ROI), Size(3, 3), 0, 0, INTER_LINEAR);

    threshold((*ROI), (*ROI), 180, 255, 0);
#ifdef _DEBUGimg
    imshow("S_Threshold_APPLE", (*ROI));
#endif
#ifdef _DEBUGmes
    cout << *ROI<<endl;
#endif
}



void Callback_empty(int tra, void* ptr)
{

}


vector<Vec2d> find_dense_point(vector<va_ptr>& lines,Mat& img_,Scalar Sca, Mat& lines_show,const Point& center=Point(0,0))
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

	vector<va_ptr> buf3;
	for (size_t i = 0; i < buf2.size() - 1; ++i)
	{
		
		buf3.push_back(va_ptr(buf2[i + 1].value - buf2[i].value, i));
	}

    sort(buf3.begin(), buf3.end());
	
	int ptr =buf3.back().ptr+1;

	if(ptr<buf2.size())
	for (size_t i = ptr; i < buf2.size(); ++i)
	{
		if (buf2[ptr].ptr > buf2[i].ptr)
		{
			ptr = i;
		}
	}
	ptr=buf2[ptr].ptr+1;
    
	for (size_t i = 0; i < buf.size(); ++i)
	{
		if (i >= ptr)
		{   
            if (!lines_show.empty())
			circle(lines_show, Point(double(i) / buf.size() * 500, buf[i].value*5), 2, Scalar(255, 0, 0), -1, 1);
			ret.push_back(buf[i]);
		}
        else
        {
            if (!lines_show.empty())
                circle(lines_show, Point(double(i) / buf.size() * 500, buf[i].value * 5), 2, Scalar(255, 244, 0), -1, 1);

        }
		//cout << '(' << i << ',' << buf[i].ptr << ',' << buf[i].value << ") ";
	}
	//cout << endl;


    if (center != Point(0, 0))
    {
        for (size_t i = 0; i < lines.size(); ++i)
        {
            //lines_fin[i][0] = lines_fin[i][0] + sqrt(pow(center.x, 2) + pow(center.y, 2))*cos(lines_fin[i][1] - atan(center.y / center.x));
            lines[i].value = lines[i].value + center.x*cos(lines[i].ptr) + center.y*sin(lines[i].ptr);
        }
    }
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

    if (!img_.empty())
    {
        for (size_t i = 0; i < lines_fin.size(); ++i)
        {
            draw_lines_polar(img_, lines_fin[i], Sca);
        }
        //cout << rho << ' ' <<int(theta / CV_PI * 180) << endl;
    }
    if (!lines_show.empty()) 
    {
        for (size_t i = 0; i < lines.size(); ++i)
        {
            circle(lines_show, Point((double)(lines[i].value ) * 500 / img_.cols, (double)(lines[i].ptr) <= 0.78 ? 100 : 250),0.5 , Scalar(255, 255, 255), -1, 1);
        }

        for (size_t i = 0; i < ret.size(); ++i)
        {
            circle(lines_show, Point((double)(lines[ret[i].ptr].value) * 500 / img_.cols, (double)(lines[ret[i].ptr].ptr) <= 0.78 ? 70 : 220), 2, Scalar(51, 255, 91), -1, 1);
            //cout << buf[i].value << "! ";
            //circle(lines_show, Point(double(i) / img_output.rows * 500, buf_fin[i] * 10), 0.5, Scalar(0, 255, 0), -1, 1);
        }

        for (size_t i = 0; i < lines_fin.size(); ++i)
        {
            circle(lines_show, Point((double)(lines_fin[i][0]) * 500 / img_.cols , (double)(lines_fin[i][1]) <= 0.78 ? 90 : 240), 3, Scalar(0, 0, 255), -1, 1);
        }

    }
    
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

