#pragma once

#include "pch.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include<vector>

template<typename T>
constexpr auto ABS(T x) { return ((x) > 0 ? (x) :(0-x)); }

using namespace cv;
using namespace std;

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


vector<double> find_dense_point(const vector<int>& lines, vector<double>& fin,int tra)
{
	vector<double> ret;
	for (size_t i = 0; i < lines.size(); ++i)
	{
		for (size_t j = (lines[i] - tra) > 0 ? lines[i] - tra : 0; j <= lines[i] + tra && j < fin.size(); ++j)
		{
			++fin[j];
		}
		for (size_t j = (lines[i] - tra*2/3) > 0 ? lines[i] - tra*2/3 : 0; j <= lines[i] + tra*2/3 && j < fin.size(); ++j)
		{
			++fin[j];
		}
	}
	vector<double> buf_fin;
	const int fuzzy = 5;
	for (size_t i = 0; i < fin.size(); ++i)
	{
		double buf = 0;
		for (size_t j = i - fuzzy/2 > 0 ? i -fuzzy/2 : 0; j < i + fuzzy/2 && j < fin.size(); ++j)
		{
			 buf += fin[j];
		}
		buf_fin.push_back(buf / fuzzy);
	}
	return buf_fin;
}
