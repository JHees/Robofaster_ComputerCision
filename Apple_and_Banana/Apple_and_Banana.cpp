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
	VideoCapture cap("Ex2.mp4");
	int frames=cap.get(CAP_PROP_FRAME_COUNT);
	int n=0;
	int error_frame = 0;


	int Sli_thre_S =110;//171


	int Sli_thre1 = 250, Sli_thre2 = 500;

    int Hough_Thre = 82; //71;80-109
    


    const int BUFFER_SIZE = 3;
    deque<int> buffer(BUFFER_SIZE, 0);
    int ret_last = 0;
    vector<named_va> time_collect;

    namedWindow("Win_Parameter");
    createTrackbar("thre1", "Win_Parameter", &Sli_thre1, 800, Callback_empty);
    createTrackbar("thre2", "Win_Parameter", &Sli_thre2, 800, Callback_empty);
    createTrackbar("S_Threshold", "Win_Parameter", &Sli_thre_S, 255, Callback_empty);
    createTrackbar("Hough", "Win_Parameter", &Hough_Thre, 500, Callback_empty);
	while (1)
	{
#ifdef _DEBUGtime
time_collect.clear();
time_collect.push_back(named_va("time_start",(double)getTickCount()));
#endif

		Mat frame;
		cap >> frame;
#ifdef _DEBUGtime
time_collect.push_back(named_va("time_cap", (double)getTickCount()));
#endif
		if (n==frames-13) 
		{
			cap.set(CAP_PROP_POS_FRAMES, 0);
			n = 0;
			continue;
		}
		n++;
        
        Mat image;
        colorReduce(frame, image, 32);
        
		imshow("origin", frame);
		//imshow("reduce", image);

#ifdef _DEBUGtime
time_collect.push_back(named_va("time_colorReduce",(double)getTickCount()));
#endif

        blur(image, image, Size(3,3));

#ifdef _DEBUGtime
time_collect.push_back(named_va("time_blur", (double)getTickCount()));
#endif
        Mat img_canny;
		cvtColor(image, img_canny, COLOR_RGB2GRAY);
		Canny(img_canny, img_canny, getTrackbarPos("thre1","Win_Parameter"), getTrackbarPos("thre2", "Win_Parameter"), 3);
        imshow("canny", img_canny);

#ifdef _DEBUGtime
time_collect.push_back(named_va("time_canny",(double)getTickCount()));
#endif

		vector<Vec2f> lines,lines_perp;
		//namedWindow("HoughLines");
		
		HoughLines(img_canny, lines, 1, CV_PI / 180, getTrackbarPos("Hough","Win_Parameter"), 0, 0);

#ifdef _DEBUGtime
time_collect.push_back(named_va("time_HoughLines",(double)getTickCount()));
#endif
        
        
        cout << "________________________________________________" << endl;
		for (size_t i = 0; i < lines.size(); ++i)
		{
            const double i_error_theta = 8 * CV_PI / 180;
			if(!(lines[i][1] <i_error_theta|| lines[i][1] >CV_PI- i_error_theta ||(lines[i][1] >CV_PI/2- i_error_theta && lines[i][1] <CV_PI/2+ i_error_theta)))
			{
				continue;
			}
			lines_perp.push_back(lines.at(i));
#ifdef _DEBUGimg
            draw_lines_polar(img_canny, lines[i], Scalar(55, 100, 195));
#endif

#ifdef _DEBUGmes
			cout << rho << ' ' <<int(theta / CV_PI * 180) << endl;
#endif
		} 
		vector<va_ptr> lines_col,lines_row;
		Mat lines_show(500, 500, CV_8UC3, Scalar(0, 0, 0)), lines_show2(500, 500, CV_8UC3, Scalar(0, 0, 0));
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
        
        if (lines_col.size() <= 3 || lines_row.size() <= 3)
        {
            imshow("boundary", img_canny);
            waitKey(30);
            cout << "error lines size." << endl;
            error_frame++;
            continue;
            
        }

		sort(lines_col.begin(), lines_col.end());
		sort(lines_row.begin(), lines_row.end());

        Mat empty;
        vector<Vec2d> lines_fin_col = find_dense_point(lines_col, frame, Scalar(0, 0, 255),lines_show);
        vector<Vec2d> lines_fin_row = find_dense_point(lines_row, frame, Scalar(0, 0, 255),lines_show);
#ifdef _DEBUGtime
 time_collect.push_back(named_va("time_firstFindLines",(double)getTickCount()));
#endif
        if (lines_fin_col.size() != 4 || lines_fin_row.size() != 4)
        {
            imshow("boundary", img_canny);
            waitKey(30);
            cout << "error fin line size." << endl;
            error_frame++;
            continue;
            
        }

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
        double& rho1 = lines_fin_col_cen[0], &theta1 = lines_fin_col_cen[1];
        double& rho2 = lines_fin_row_cen[0], &theta2 = lines_fin_row_cen[1];
#ifdef _DEBUGtime
time_collect.push_back(named_va("time_FindCenterPoint",(double)getTickCount()));
#endif
        Point2d center(cvRound((rho2*sin(theta1) - rho1 * sin(theta2)) / sin(theta2 - theta1)),cvRound(((rho2*cos(theta1) - rho1 * cos(theta2) / sin(theta2 - theta1)))));
        for (size_t i = 0; i < lines_col.size(); ++i)
        {
            lines_col[i].value = lines_col[i].value - center.x*cos(lines_col[i].ptr) - center.y*sin(lines_col[i].ptr);
        }
        for (size_t i = 0; i < lines_row.size(); ++i)
        {
            lines_row[i].value = lines_row[i].value - center.x*cos(lines_row[i].ptr) - center.y*sin(lines_row[i].ptr);
        }
        
        lines_fin_col=find_dense_point(lines_col, frame, Scalar(0, 255, 0), lines_show2,center);
        lines_fin_row=find_dense_point(lines_row, frame, Scalar(0, 255, 0), empty,center);


#ifdef _DEBUGtime
time_collect.push_back(named_va("time_secondFindLines",(double)getTickCount()));
#endif
        circle(img_canny, center, 5, Scalar(255, 255, 255), -1, 1);
        cout << center.x << ' ' << center.y << "  center" << endl;
        draw_lines_polar(frame, lines_fin_col[0], Scalar(0, 255, 0));
        draw_lines_polar(frame, lines_fin_col[1], Scalar(0, 255, 0));
        draw_lines_polar(frame, lines_fin_col[2], Scalar(0, 255, 0));
        draw_lines_polar(frame, lines_fin_col[3], Scalar(0, 255, 0));
        draw_lines_polar(frame, lines_fin_row[0], Scalar(0, 255, 0));
        draw_lines_polar(frame, lines_fin_row[1], Scalar(0, 255, 0));
        draw_lines_polar(frame, lines_fin_row[2], Scalar(0, 255, 0));
        draw_lines_polar(frame, lines_fin_row[3], Scalar(0, 255, 0));

#ifdef _DEBUGimg
        imshow("boundary", img_canny);
		imshow("lines_prep_show", lines_show);
		//imshow("HoughLines", img_output);
#endif
        double col_rho[4], col_theta[4], row_rho[4], row_theta[4];
        for (size_t i = 0; i < lines_fin_col.size(); ++i)
        {
            col_rho[i] = lines_fin_col[i][0];
            col_theta[i] = lines_fin_col[i][1];
        }
        for (size_t i = 0; i < lines_fin_row.size(); ++i)
        {
            row_rho[i] = lines_fin_row[i][0];
            row_theta[i] = lines_fin_row[i][1];
        }
        vector<Point2d> p_crossover;
        for (size_t i = 0; i < 4; i++)
        {
            for (size_t j = 0; j < 4; ++j)
            {
                p_crossover.push_back(Point2d((row_rho[j] * sin(col_theta[i]) - col_rho[i] * sin(row_theta[j])) / sin(col_theta[i]-row_theta[j]), 
                                        (row_rho[j] * cos(col_theta[i]) - col_rho[i] * cos(row_theta[j])) / sin(row_theta[j] - col_theta[i])));
            }
        }
        for (size_t i = 0; i < 16; ++i)
        {
            circle(frame, p_crossover[i], 5, Scalar(255, 255, 255), -1, 1);
        }

        Point2f p_corner[4] = { p_crossover[0],p_crossover[3],p_crossover[12],p_crossover[15] };
        Mat matrix_Warp(3, 3, CV_32FC1);
        Point2f p_cornerTar[4];
        double x_max = p_corner[0].x, x_min = p_corner[0].x, y_max = p_corner[0].y, y_min = p_corner[0].y;
        for (size_t i = 1; i < 4; ++i)
        {
            x_max = p_corner[i].x > x_max ? p_corner[i].x : x_max;
            x_min = p_corner[i].x < x_min ? p_corner[i].x : x_min;
            y_max = p_corner[i].y > y_max ? p_corner[i].y : y_max;
            y_min = p_corner[i].y < y_min ? p_corner[i].y : y_min;

        }

        p_cornerTar[0] = Point2f(x_min, y_min);
        p_cornerTar[1] = Point2f(x_max, y_min);
        p_cornerTar[2] = Point2f(x_min, y_max);
        p_cornerTar[3] = Point2f(x_max, y_max);
        matrix_Warp=getPerspectiveTransform(p_corner, p_cornerTar);

        Mat img_ROI_bf = image(Rect(p_cornerTar[0], p_cornerTar[3]));
        Mat img_ROI(img_ROI_bf.rows,img_ROI_bf.cols,CV_8UC3);
        warpPerspective(img_ROI_bf, img_ROI, matrix_Warp,Size(img_ROI_bf.cols,img_ROI_bf.rows));
        imshow("beforeWarp", img_ROI_bf);
        imshow("afterWarp", img_ROI);
		Mat HSV_img;
        cvtColor(img_ROI, HSV_img, COLOR_RGB2HSV);
		vector<Mat> cha;
		split(HSV_img, cha);
		//namedWindow("S_Threshold_APPLE",WINDOW_AUTOSIZE);
		
		Callback_S_Thre_APPLE(getTrackbarPos("S_Threshold", "Win_Parameter"), &cha[0]);
#ifdef _DEBUGtime
time_collect.push_back(named_va("time_ThresholdS", (double)getTickCount()));
#endif
        Mat &img_ret = cha[0];
       int result = 0;
        bool flag_ = 0;
        int ret = 0;
        for (int i = 0; i < img_ret.rows; ++i)
        {
            for (int j = 0; j < img_ret.cols; ++j)
            {
                if ((img_ret.ptr<uchar>(i)[j]) == 255 && flag_ == 0)
                {
                    flag_ = 1;
                    result = i * img_ret.rows + j + 1;
                }
                else if ((img_ret.ptr<uchar>(i)[j]) == 255 && flag_ == 1)
                {
                    flag_ = 0;
                    result = 0;
                }
            }
        }
        buffer.pop_front();
        buffer.push_back(result);
        for (size_t i = 0; i < buffer.size()-1; ++i)
        {
            if (buffer[i] != buffer[i + 1])
            {
                flag_ = 0;
                break;
            }
        }
        if (flag_)
            ret = buffer[0];
#ifdef _DEBUGret
        ret_output(frame, p_crossover, ret);
        imshow("Robofaster_ret_output", frame);
        cout << "ret: " << ret << endl;
        cout << n << endl;
#endif
 

#ifdef _DEBUGtime
 time_collect.push_back(named_va("time_retCorrectandOutput", (double)getTickCount()));

        cout << "ret: " << ret_last << endl;
        for (int i = 1; i < time_collect.size(); ++i)
        {
            cout << time_collect[i] - time_collect[i - 1];
        }
        
 time_collect.push_back(named_va("time_end", (double)getTickCount()));
 cout << time_collect[time_collect.size() - 1] - time_collect[0];
#endif

    waitKey(30);
	}
	waitKey(0);
}

