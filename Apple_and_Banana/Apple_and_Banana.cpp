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
	VideoCapture cap("Exa.mp4");
	int frames=cap.get(CAP_PROP_FRAME_COUNT);
	int n=0;

	int S_Thre_Slider_APPLE =113;//171
	int V_Slider = 0;
	int Sli_Thre_V = 145;

	int Sli_thre1 = 250, Sli_thre2 = 500;
    int Hough_Thre = 82; //71;80-109

	int ADD_missline = 0;
	while (1)
	{
		Mat frame;
		cap >> frame;
		if (n==frames-13) 
		{
			cap.set(CAP_PROP_POS_FRAMES, 0);
			n = 0;
			/*cout << Hough_Thre << ": " << ADD_missline << endl;
			
			if (Hough_Thre >= 140)
			{
				while (1)
				{
					string in;
					cin >> in;
					if(in=="break")
						break;
				}
                break;
				
			}
			ADD_missline = 0;
			Hough_Thre++;
			continue;*/
		}
		n++;
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
        Mat img_canny = img_output.clone();
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
		cout << "________________________________________________" << endl;

		vector<va_ptr> lines_col,lines_row;
		Mat lines_show(500,500,CV_8UC3,Scalar(0,0,0));
		for (size_t i = 0; i < lines_perp.size(); ++i)
		{
            if (lines_perp[i][1] > 0.78&&lines_perp[i][1] < 2.36)//45度
            {
                lines_col.push_back(va_ptr(abs(lines_perp[i][0]), lines_perp[i][1]));
               
            }
			else
				lines_row.push_back(va_ptr(abs(lines_perp[i][0]), lines_perp[i][1]>1.7?-CV_PI+lines_perp[i][1]:lines_perp[i][1]));
			//circle(lines_show, Point((double)(lines_perp[i][0]) / img_output.rows * 500, (lines_perp[i][1] <=0.78 ? 100 : 250)), 0.5, Scalar(255, 255, 255), -1, 1);

		}
        
        if (lines_col.size() <= 3|| lines_row.size() <= 3)continue;
		sort(lines_col.begin(), lines_col.end());
		sort(lines_row.begin(), lines_row.end());
	    	


		//vector<va_ptr> buf = find_dense_point(lines_col, img_canny);
        vector<Vec2d> lines_fin_col = find_dense_point(lines_col, frame, Scalar(0, 0, 255));
        vector<Vec2d> lines_fin_row = find_dense_point(lines_row, frame, Scalar(0, 0, 255));

        if (lines_fin_col.size() != 4 || lines_fin_row.size() != 4)continue;

        Vec2d lines_fin_col_cen, lines_fin_row_cen;
        for (size_t i = 0; i < lines_fin_col.size(); ++i)
        {
            lines_fin_col_cen[0] += lines_fin_col[i][0] / lines_fin_col.size();
            lines_fin_col_cen[1] += lines_fin_col[i][1] / lines_fin_col.size();
        }
        for (size_t i = 0; i < lines_fin_row.size(); ++i)
        {
            lines_fin_row_cen[0] += (lines_fin_row[i][1] > 1.7 ? -lines_fin_row[i][0]: -lines_fin_row[i][0]) / lines_fin_row.size();
            lines_fin_row_cen[1] += (lines_fin_row[i][1] > 1.7 ? -CV_PI + lines_fin_row[i][1] : lines_fin_row[i][1]) / lines_fin_row.size();
        }
        double& rho1 = lines_fin_col_cen[0], theta1 = lines_fin_col_cen[1];
        double& rho2 = lines_fin_row_cen[0], theta2 = lines_fin_row_cen[1];
        Point2d center((rho2*sin(theta1) - rho1 * sin(theta2)) / sin(theta2 - theta1), (rho2*cos(theta1) - rho1 * cos(theta2) / sin(theta2 - theta1)));

        for (size_t i = 0; i < lines_col.size(); ++i)
        {
            lines_col[i].value = lines_col[i].value - center.x*cos(lines_col[i].ptr) - center.y*sin(lines_col[i].ptr);
        }
        for (size_t i = 0; i < lines_row.size(); ++i)
        {
            lines_row[i].value = lines_row[i].value - center.x*cos(lines_row[i].ptr) - center.y*sin(lines_row[i].ptr);
        }
        lines_fin_col=find_dense_point(lines_col, frame, Scalar(0, 255, 0),center);
        lines_fin_row=find_dense_point(lines_row, frame, Scalar(0, 255, 0),center);


        circle(img_canny, center, 5, Scalar(255, 255, 255), -1, 1);
       // cout << center.x << ' ' << center.y << "  center" << endl;

        imshow("origin", frame);

        imshow("boundary", img_canny);

		imshow("lines_prep_show", lines_show);
	
		imshow("HoughLines", img_output);

        double rho_col[4], theta_col[4], rho_row[4], theta_row[4];
        for (size_t i = 0; i < lines_fin_col.size(); ++i)
        {
            rho_col[i] = lines_fin_col[i][0];
            theta_col[i] = lines_fin_col[i][1];
        }
        for (size_t i = 0; i < lines_fin_row.size(); ++i)
        {
            rho_row[i] = lines_fin_row[i][0];
            theta_row[i] = lines_fin_row[i][1];
        }
        Point p_crossover[16];
        for (size_t i = 0; i < 4; i++)
        {
            for (size_t j = 0; j < 4; ++j)
            {
                p_crossover[4*i+j] = Point((rho_row[j] * sin(theta_col[i]) - rho_col[i] * sin(theta_row[j])) / sin(theta_col[i]-theta_row[j]), 
                                        (rho_row[j] * cos(theta_col[i]) - rho_col[i] * cos(theta_row[j]) / sin(theta_row[j] - theta_col[i])));
            }
        }



        Mat img_ROI = image(Rect(p_crossover[0], p_crossover[15]));
		Mat HSV_img;
		cvtColor(img_ROI, HSV_img, COLOR_RGB2HSV);
		//imshow("HSV", HSV_img);

		std::vector<Mat> cha;
		split(HSV_img, cha);
	//	imshow("H", cha[0]);
	//	imshow("S", cha[1]);
	//	imshow("V", cha[2]);


		namedWindow("S_Threshold_APPLE",WINDOW_AUTOSIZE);
		createTrackbar("S_Threshold", "S_Threshold_APPLE", &S_Thre_Slider_APPLE, 255, Callback_S_Thre_APPLE, &cha[0]);
		Callback_S_Thre_APPLE(S_Thre_Slider_APPLE, &cha[0]);

        



        
		waitKey(30);
	}

	waitKey(0);
}

