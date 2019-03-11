// Apple_and_Banana.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include<vector>
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
			output.ptr<uchar>(i)[j] =buf[output.ptr<uchar>(i)[j]];
		}
	}
	return output;
}

void Callback_V(int tra, void* ptr)
{
	std::vector<Mat>* cha = (std::vector<Mat>*)ptr;
	(*cha)[2]= Mat((*cha)[2].rows,(*cha)[2].cols,CV_8UC1,Scalar(tra));
	Mat HS_img;
	merge(*cha, HS_img);
	imshow("HS", HS_img);
}

void Callback_S_Thre_APPLE(int tra, void* ptr)
{
	Mat S_Thre;
	Mat* ROI = (Mat*)ptr;
	threshold((*ROI), S_Thre, tra, 255, 1);
	namedWindow("buf", WINDOW_AUTOSIZE);
	imshow("buf", S_Thre);
	Size S_size(S_Thre.rows, S_Thre.cols);

	resize(S_Thre, S_Thre, Size( 3, 3), 0, 0, INTER_LINEAR);
//	resize(S_Thre, S_Thre, Size(S_Thre.rows / 5, S_Thre.cols / 5), 0, 0, INTER_NEAREST);

//	resize(S_Thre, S_Thre, S_size, 0, 0, INTER_NEAREST);

	threshold(S_Thre, S_Thre,0, 255, 0);
	imshow("S_Threshold_APPLE",S_Thre);
	*ROI = S_Thre;

}

int main()
{
	Mat image = colorReduce(imread("Exa.jpg"),32);
	imshow("", image);

	Mat HSV_img;
	cvtColor(image, HSV_img, COLOR_RGB2HSV);
	imshow("HSV", HSV_img);

	std::vector<Mat> cha;
	split(image, cha);
	imshow("H", cha[0]);
	imshow("S", cha[1]);
	cout << cha[1].rows << ' ' << cha[1].cols << endl;
	Mat ROI = cha[1](Rect(32,0, cha[1].rows, cha[1].cols-63));
	cout << ROI.rows << ' ' << ROI.cols << endl;
	//namedWindow("HS");
	//int Slider_V = 0;
	//createTrackbar("V:", "HS", &Slider_V, 255, Callback_V,&cha);
	//Callback_V(Slider_V,&cha);

	int S_Thre_Slider_APPLE = 157;
	
	namedWindow("S_Threshold_APPLE",WINDOW_NORMAL);
	createTrackbar("S", "S_Threshold_APPLE", &S_Thre_Slider_APPLE, 255, Callback_S_Thre_APPLE, &ROI);
	Callback_S_Thre_APPLE(S_Thre_Slider_APPLE, &ROI);
	cout << ROI.rows << ' ' << ROI.cols << endl<<ROI;


	
	waitKey(0);
}


