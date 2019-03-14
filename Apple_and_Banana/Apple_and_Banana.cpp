// Apple_and_Banana.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include<vector>
#include"Functions.h"

using namespace cv;
using namespace std;

int main()
{
	VideoCapture* cap = new VideoCapture ("Exa.mp4");
	int S_Thre_Slider_APPLE =113;//171
	int V_Slider = 0;
	int Sli_Thre_V = 145;

	int Sli_thre1 = 250, Sli_thre2 = 500;
	int Hough_Thre = 89;
	int Sli_find_dense = 17;
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
		Mat image = colorReduce(frame,32);
		//imshow("origin", frame);
		imshow("reduce", image);

		Mat Gray_img;
		cvtColor(image,Gray_img, COLOR_RGB2GRAY);
		blur(Gray_img, Gray_img, Size(3,3));
		//imshow("Gray", Gray_img);
		vector<vector<Point>> contours;
		Mat img_output;
		namedWindow("canny");
		createTrackbar("thre1","canny",  &Sli_thre1,500, Callback_empty);
		Callback_empty(Sli_thre1, NULL);
		createTrackbar("thre2","canny",  &Sli_thre2, 500, Callback_empty);
		Callback_empty(Sli_thre2, NULL);
		Canny(Gray_img, img_output, getTrackbarPos("thre1","canny"), getTrackbarPos("thre2", "canny"), 3);
		imshow("canny", img_output);

		vector<Vec2f> lines,lines_perp;
		namedWindow("HoughLines");
		createTrackbar("Hough", "HoughLines", &Hough_Thre, 500, Callback_empty);
		HoughLines(img_output, lines, 1, CV_PI / 180, getTrackbarPos("Hough","HoughLines"), 0, 0);
		for (size_t i = 0; i < lines.size(); ++i)
		{
			float rho = lines[i][0], theta = lines[i][1];
			const int i_error_theta = 8;
			if(!(theta / CV_PI * 180 <i_error_theta|| theta / CV_PI * 180 >180- i_error_theta ||(theta / CV_PI * 180 >90- i_error_theta && theta / CV_PI * 180<90+ i_error_theta)))
			{
				continue;
			}
			lines_perp.push_back(lines.at(i));
			Point pt1, pt2;
			double a = cos(theta), b = sin(theta);
			double x0 = a * rho, y0 = b * rho;
			pt1.x = cvRound(x0 + 1000 * (-b));
			pt1.y = cvRound(y0 + 1000 * (a));
			pt2.x = cvRound(x0 - 1000 * (-b));
			pt2.y = cvRound(y0 - 1000 * (a));
			line(img_output, pt1, pt2, Scalar(55, 100, 195), 1, LINE_AA);
			//cout << rho << ' ' <<int(theta / CV_PI * 180) << endl;
		} 
		cout << "________________________________________________" <<lines_perp.size()<< endl;

		vector<int> lines_col,lines_row;
		vector<double> lines_fin;
		vector<double> buf_fin(img_output.cols, 0);
		Mat lines_show(500,500,CV_8UC3,Scalar(0,0,0));
		for (size_t i = 1; i < lines_perp.size(); ++i)
		{
			if (lines_perp[i][1] > 0.78&&lines_perp[i][1]<2.36)//45度
				lines_col.push_back(ABS(lines_perp[i][0]));
			else
				lines_row.push_back(ABS(lines_perp[i][0]));
			//circle(lines_show, Point((double)(lines_perp[i][0]) / img_output.rows * 500, (lines_perp[i][1] <=0.78 ? 100 : 250)), 0.5, Scalar(255, 255, 255), -1, 1);

		}

		sort(lines_col.begin(), lines_col.end());
		for (size_t i = 0; i < lines_col.size(); ++i)
			circle(lines_show, Point((double)(lines_col[i]) / img_output.rows * 500, 50), 0.5, Scalar(255, 255, 255), -1, 1);
		sort(lines_row.begin(), lines_row.end());
			

		namedWindow("lines_prep_show");
		createTrackbar("R", "lines_prep_show", &Sli_find_dense, img_output.cols/5 , Callback_empty);
		//lines_fin = find_dense_point(lines_col, buf_fin, getTrackbarPos("R", "lines_prep_show"));
		vector<double> buf = find_dense_point(lines_col, buf_fin, getTrackbarPos("R", "lines_prep_show"));
		for (size_t i = 0; i < img_output.cols; ++i)
		{
			circle(lines_show, Point(double(i) / img_output.rows * 500, buf[i] * 10), 0.5, Scalar(0, 0, 255), -1, 1);
			//circle(lines_show, Point(double(i) / img_output.rows * 500, buf_fin[i] * 10), 0.5, Scalar(0, 255, 0), -1, 1);
		}
		cout << lines_fin.size() << "asadadsdada" << endl;
		imshow("lines_prep_show", lines_show);


		imshow("HoughLines", img_output);
		
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

