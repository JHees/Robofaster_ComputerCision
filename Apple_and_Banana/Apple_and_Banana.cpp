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
	vector<Mat>* cha = (vector<Mat>*)ptr;
	(*cha)[2]= Mat((*cha)[2].rows,(*cha)[2].cols,CV_8UC1,Scalar(tra));
}

void Callback_S_Thre_APPLE(int tra, void* ptr)
{
	Mat S_Thre;
	Mat* ROI = (Mat*)ptr;
	threshold((*ROI), S_Thre, tra, 255, 1);


	imshow("S_Threshold_APPLE",S_Thre);

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

void Callback_Canny1(int tra, void* ptr)
{

}

void Callback_Canny2(int tra, void*)
{

}
int main()
{
	VideoCapture* cap = new VideoCapture ("Exa.mp4");
	int S_Thre_Slider_APPLE =113;//171
	int V_Slider = 0;
	int Sli_Thre_V = 145;

	int Sli_thre1 = 75, Sli_thre2 = 500;
	while (1)
	{
		Mat frame;
		*cap >> frame; 
		if (frame.data == NULL) 
		{
			delete cap;
			VideoCapture* cap = new VideoCapture("Exa.mp4");
			continue;
		}
		Mat image = colorReduce(frame,1);
		imshow("origin", frame);
		//imshow("reduce", image);

		Mat Gray_img;
		cvtColor(image,Gray_img, COLOR_RGB2GRAY);
		blur(Gray_img, Gray_img, Size(3, 3));
		//imshow("Gray", Gray_img);
		vector<vector<Point>> contours;
		Mat img_output;
		namedWindow("canny");
		createTrackbar("thre1","canny",  &Sli_thre1,500, Callback_Canny1);
		Callback_Canny1(Sli_thre1, NULL);
		createTrackbar("thre2","canny",  &Sli_thre2, 500, Callback_Canny2);
		Callback_Canny2(Sli_thre2, NULL);
		Canny(Gray_img, img_output, getTrackbarPos("thre1","canny"), getTrackbarPos("thre2", "canny"), 3);
		imshow("canny", img_output);
		cout << getTrackbarPos("thre1", "canny") << ' ' << getTrackbarPos("thre2", "canny") << endl;
		//Mat HSV_img;
		//cvtColor(image, HSV_img, COLOR_RGB2HSV);
		//imshow("HSV", HSV_img);

	//	std::vector<Mat> cha;
	//	split(HSV_img, cha);
	////	imshow("H", cha[0]);
	////	imshow("S", cha[1]);
	////	imshow("V", cha[2]);

	//

	//	
	//	namedWindow("Thre_V");
	//	createTrackbar("Thre_V", "Thre_V", &Sli_Thre_V, 255, Callback_Thre_V, &cha);
	//	Callback_Thre_V(Sli_Thre_V, &cha);

	//	
	//	namedWindow("After_Callback_V", WINDOW_AUTOSIZE);
	//	createTrackbar("V", "After_Callback_V", &V_Slider, 255, Callback_V, &cha);
	//	Callback_V(V_Slider, &cha);
	//	Mat ROI;
	//	merge(cha, ROI);
	//	imshow("After_Callback_V", ROI);

	//	namedWindow("S_Threshold_APPLE",WINDOW_AUTOSIZE);
	//	createTrackbar("S_Threshold", "S_Threshold_APPLE", &S_Thre_Slider_APPLE, 255, Callback_S_Thre_APPLE, &ROI);
	//	Callback_S_Thre_APPLE(S_Thre_Slider_APPLE, &ROI);
	//	//cout << ROI.rows << ' ' << ROI.cols << endl<<ROI;




		waitKey(30);
	}



	delete cap;
	waitKey(0);
}

//TODO: 分文件！！！
// 1. magic number 的计算
// 2. 图片范围的识别
// 3.

