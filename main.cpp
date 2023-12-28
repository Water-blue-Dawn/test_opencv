#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

void distinguish(Mat imgDil, Mat img)
{
	//��������
	vector<vector<Point>> contours;
	//�ҵ�����
	vector<Vec<int, 4>> hierarchy;
	findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	for (int i = 0; i < contours.size(); i++)
	{
		//ÿ���������
		int area = contourArea(contours[i]);
		if (area > 1000)
		{
			//�ҵ�ÿ���������ܳ�
			float peri = arcLength(contours[i], true);
			//�ǵ����������洢�����еĽǵ� 
			vector<vector<Point>> conPoly(contours.size());
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
			//����ÿ�������Ľǵ���
			int objCor = (int)conPoly[i].size();
			//�洢���α߽������
			vector<Rect> boundRect(contours.size());
			if (objCor >= 2 and objCor <= 4 and area<=2500) 
			{
				//���ƾ��α߽�򣬽�����ÿһ����������״
				boundRect[i] = boundingRect(conPoly[i]);
				//���߽���ӡ��ԭͼ��
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
	//תΪHSV
	cvtColor(srcImage, imgHSV, COLOR_BGR2HSV);
	inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), bufImg);
	return bufImg;
}

Mat ColorFindContours2(Mat srcImage)
{
	Mat des1 = ColorFindContours(srcImage,
		350 / 2, 360 / 2,         // ɫ����Сֵ~���ֵ
		(int)(255 * 0.70), 255,   // ���Ͷ���Сֵ~���ֵ
		(int)(255 * 0.60), 255);  // ������Сֵ~���ֵ

	Mat des2 = ColorFindContours(srcImage,
		0, 16 / 2,                // ɫ����Сֵ~���ֵ
		(int)(255 * 0.70), 255,   // ���Ͷ���Сֵ~���ֵ
		(int)(255 * 0.60), 255);  // ������Сֵ~���ֵ

	return des1 + des2;
}
int main()
{
	Mat img = imread("xuxieban.png");
	Mat hsv;
	// ת��HSV
	cvtColor(img, hsv, COLOR_BGR2HSV);
	// ��ɫ
	Scalar lower_yellow(100, 43, 46);
	Scalar upper_yellow(124, 255, 255);
	// ��ɫ
	Scalar lower_red(156, 43, 46);
	Scalar upper_red(180, 255, 255);
	Mat mask1, mask2;
	// ֻ������ɫ
	inRange(hsv, lower_yellow, upper_yellow, mask1);
	//ֻ������ɫ
	inRange(hsv, lower_red, upper_red, mask2);
	Mat test = mask1 + mask2;
	//��˹ģ��
	Mat imgBlur;
	GaussianBlur(test, imgBlur, Size(3, 3), 3, 0);
	//�����Ե���
	Mat imgCanny;
	Canny(imgBlur, imgCanny, 25, 75);
	//����
	Mat imgDil;
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 5));
	dilate(imgCanny, imgDil, kernel);
	//��ʴ
	Mat	element = getStructuringElement(MORPH_RECT,Size(3,2));
	Mat dstImage;
	erode(imgDil,dstImage,element);
	//���棬��Ҳ��֪��Ϊʲô����ֻ�и�ʴ������һ����ʶ�𲻵����������ٸ�ʴ���ԚG

	distinguish(dstImage, img);
	imwrite("output.png", img);
	waitKey(0);
	return 0;
}
