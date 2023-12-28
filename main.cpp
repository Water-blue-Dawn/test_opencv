#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

void distinguish(Mat imgDil, Mat img)
{
	//创建轮廓
	vector<vector<Point>> contours;
	//找到轮廓
	vector<Vec<int, 4>> hierarchy;
	findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	for (int i = 0; i < contours.size(); i++)
	{
		//每个轮廓面积
		int area = contourArea(contours[i]);
		if (area > 1000)
		{
			//找到每个轮廓的周长
			float peri = arcLength(contours[i], true);
			//角点向量，仅存储轮廓中的角点 
			vector<vector<Point>> conPoly(contours.size());
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
			//定义每个轮廓的角点数
			int objCor = (int)conPoly[i].size();
			//存储矩形边界框向量
			vector<Rect> boundRect(contours.size());
			if (objCor >= 2 and objCor <= 4 and area<=2500) 
			{
				//绘制矩形边界框，将含纳每一个独立的形状
				boundRect[i] = boundingRect(conPoly[i]);
				//将边界框打印在原图上
				rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 2);
			}
		}
	}
}

Mat ColorFindContours(Mat srcImage,
	int iLowH, int iHighH,
	int iLowS, int iHighS,
	int iLowV, int iHighV)
{
	Mat bufImg;
	Mat imgHSV;
	//转为HSV
	cvtColor(srcImage, imgHSV, COLOR_BGR2HSV);
	inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), bufImg);
	return bufImg;
}

Mat ColorFindContours2(Mat srcImage)
{
	Mat des1 = ColorFindContours(srcImage,
		350 / 2, 360 / 2,         // 色调最小值~最大值
		(int)(255 * 0.70), 255,   // 饱和度最小值~最大值
		(int)(255 * 0.60), 255);  // 亮度最小值~最大值

	Mat des2 = ColorFindContours(srcImage,
		0, 16 / 2,                // 色调最小值~最大值
		(int)(255 * 0.70), 255,   // 饱和度最小值~最大值
		(int)(255 * 0.60), 255);  // 亮度最小值~最大值

	return des1 + des2;
}
int main()
{
	Mat img = imread("xuxieban.png");
	Mat hsv;
	// 转换HSV
	cvtColor(img, hsv, COLOR_BGR2HSV);
	// 蓝色
	Scalar lower_yellow(100, 43, 46);
	Scalar upper_yellow(124, 255, 255);
	// 红色
	Scalar lower_red(156, 43, 46);
	Scalar upper_red(180, 255, 255);
	Mat mask1, mask2;
	// 只保留蓝色
	inRange(hsv, lower_yellow, upper_yellow, mask1);
	//只保留红色
	inRange(hsv, lower_red, upper_red, mask2);
	Mat test = mask1 + mask2;
	//高斯模糊
	Mat imgBlur;
	GaussianBlur(test, imgBlur, Size(3, 3), 3, 0);
	//坎尼边缘检测
	Mat imgCanny;
	Canny(imgBlur, imgCanny, 25, 75);
	//膨胀
	Mat imgDil;
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 5));
	dilate(imgCanny, imgDil, kernel);
	//腐蚀
	Mat	element = getStructuringElement(MORPH_RECT,Size(3,2));
	Mat dstImage;
	erode(imgDil,dstImage,element);
	//报告，我也不知道为什么，但只有腐蚀就是有一两个识别不到，先膨胀再腐蚀可以欸

	distinguish(dstImage, img);
	imwrite("output.png", img);
	waitKey(0);
	return 0;
}
