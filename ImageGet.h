#pragma once
/***************************************************************\
* @Copyright(c) 2017 HUST-BIAC All rights reserved
* @ClassName  ImageGet
* @Author  ZhaoYu
* @Date  2018年6月24日
* @Version 1.0
* @Description  CIS 采集图像配置文件以及ImageGet工具类
*
******************************************************************/

#include<iostream>
#include <QThread>
#include <QString>
#include <QDateTime>
#include <QDir>
#include <QImage>
#include "qt_windows.h"
#include "cyapi.h"

#include <fstream>
#include<opencv/cv.h>
#include<opencv/highgui.h>
#include <opencv/cxcore.h>
#include <opencv2/opencv.hpp>  
#include<qtextcodec.h>

#include "mAlgorithm.hpp"
#include"AppConfig.h"

using namespace std;
using namespace cv;

//CIS 1代表第一种CIS管类型   CIS管2代表第二种类型
//#define  CIS3

#ifdef CIS1
//采集图像设置
#define PACKAGES 140			//总包个数 
#define BUFLENGTH  1024*1024	//接收缓存长度 
#define DATALENGTH 140*1024*1024	//接收数据的大小 

#define MAT_HEIGHT 6500		//原图高  采集长度
#define MAT_WIDTH 7344	   //原图宽  传感器个数

//黑白校正设置 
#define COL  7344
#define ROW	 6500

#define StartCol 0
#define EndCol 7344

//取黑纸3300行到3400行 白纸 1100行到1300行
#define BLACK_ROW_START 3300
#define BLACK_ROW_END 3400

#define WHITE_ROW_START 1100
#define WHITE_ROW_END 1300


#endif // CIS1

#ifdef CIS2

#define PACKAGES 270//总包个数 6
#define BUFLENGTH  1024*1024	//接收缓存长度 2M
#define DATALENGTH 270*1024*1024	//接收数据的大小 12M

#define MAT_WIDTH 10000 //原图宽  采集长度
#define MAT_HEIGHT 7344 //原图高  传感器个数

#endif // CIS2

#ifdef CIS3

#define PACKAGES 1//总包个数 6
#define BUFLENGTH  411*1024	//接收缓存长度 2M
#define DATALENGTH 411*1024	//接收数据的大小 12M

#define MAT_WIDTH  648 //原图宽 300dpi
#define MAT_HEIGHT 649 //原图高  1030

#endif // CIS2

typedef enum ModeType {
	PROCESSIMG = 0,
	SETBASICIMG,
	CORRECTMODE,
}ModeType;


class DataInfo {
public:
	QImage img;
	QString reResult[6];   //最多4个结果，可以动态分配 作为常访问数据写为数组访问速度更快 
	int resultNum;
	bool state;
};

//将其设置为单例模式 

class ImageGet : public QThread
{
	Q_OBJECT

	//把复制构造函数和=操作符也设为私有,防止被复制
	ImageGet(const ImageGet&);
	ImageGet& operator=(const ImageGet&);

	static ImageGet* ImageGetInstance;

	ImageGet(QObject *parent = 0);
	~ImageGet();
public:

	static ImageGet* getInstance();

	void processImgThread();   //处理数据函数

	void setBasicImg();

	void run();  //线程

	void test();

	cv::Mat QImage2cvMat(QImage &image);
	
	void genColorCorrectCoff();
	void calCoefFilterLinerStrech(Mat &imgOrigin, Mat &dst, int color);
	void calCoefFilterLinerStrech(Mat &imgOrigin, Mat &dst);
public slots:
	void updateBasicImg(QString);

public:
	ModeType modType;
	//功能码 1 图像合成 2 图像校正 3 图像校正系数生成

	//char buffer1[100]; //测试保存路径
	unsigned char* data = NULL;  //图像数据指针

	QImage img;

	Mat DestImg;  //保存采集的图像

	QString fileName;		//图片保存路径

	
	DataInfo dataInfo;   //返回数据

	_DataType datatype[10];  //数据类型  最多四个这里写成6 也可以动态分配

	mAlgorithm malgorithm;  //算法

	int datatypeNum;   //数据类型的个数

	Mat mCurImg;
	Mat mBasicImg;
	int count = 0;

	bool isSave = false;  //是否保存图像
	QString saveImagePath; //保存图像路径

	//int setBiacImg=0;
	//图像的宽度和高度
	int width, height = 0;

	class delInstance{
		~delInstance() {
			delete ImageGetInstance;
			ImageGetInstance = NULL;
		}
	};

	static delInstance del;

signals:
	void sendDataInfo(DataInfo&);

};





