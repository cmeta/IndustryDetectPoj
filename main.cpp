/*
	@date 2018/7/18
	@author zhaoyu
	@description 工业检测上位机

*/
#include "IndustryDetect.h"
#include "model.h"
#include <QSplashScreen>
#include <QPixmap>
#include <QDateTime>
#include <QElapsedTimer>
#include<QTextCodec>
#include <QtWidgets/QApplication>
#include"AppConfig.h"
#include"qdesktopwidget.h"

int appLayoutType = 0;

//添加启动页面
static void calCoff11() {

	Mat white = imread("white.bmp", IMREAD_GRAYSCALE);
	Mat black = imread("black.bmp", IMREAD_GRAYSCALE);

	ofstream bFile("Config//Fpga//Bcfg.txt");
	ofstream kFile("Config//Fpga//Kcfg.txt");

	double bCoff[648] = { 0.0 };
	double kCoff[648] = { 0.0 };

	unsigned char mat[400][648];
	memcpy(mat, black.data, 400 * 648);

	//计算b的系数
	int sum = 0;
	for (int i = 0; i < 648; i++) {
		sum = 0;
		for (int j = 100; j < 200; j++) {
			sum += mat[j][i];
		}
		bCoff[i] = sum / 100;
	}

	unsigned char mat1[400][648];
	memcpy(mat1, white.data, 400 * 648);

	//计算k的系数
	double k = 0.0;
	double kTmp = 0.0;
	double kSum = 0.0;
	for (int i = 0; i < 648; i++) {
		kSum = 0.0;
		for (int j = 100; j < 200; j++) {
			kSum += mat1[j][i];
		}
		kTmp = kSum / 100.0;
		k = 240.0 / (kTmp - bCoff[i]);
		kCoff[i] = k;
	}
	//写入到文本文件中
	for (int i = 0; i < 648; i++) {
		kFile << kCoff[i] << "\n";
		bFile << bCoff[i] << "\n";
	}
	kFile.close();
	bFile.close();

	double temp;
	for (int j = 0; j < 400; j++)
	{
		for (int i = 0; i < 648; i++)
		{
			temp = kCoff[i] * (mat1[j][i] - bCoff[i]);

			if (temp < 0)
			{
				temp = 0;
			}
			if (temp > 255)
			{
				temp = 255;
			}
			mat1[j][i] = temp;
		}
	}
	Mat dst(400, 648, CV_8UC1);

	memcpy(dst.data, mat1, 400*648);
	imwrite("1CorrectImg.bmp", dst);

}

int main(int argc, char *argv[])
{


	//calCoff11();

	QApplication a(argc, argv);

	//QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));  //设置编码格式为UTF-8
	//获取电脑分辨率
	QDesktopWidget* desktopWidget = QApplication::desktop();
	//获取可用桌面大小  
	//QRect c = desktopWidget->availableGeometry();
	//获取设备屏幕大小  
	QRect screenRect = desktopWidget->screenGeometry();

	//qDebug() << c<<endl<<screenRect;

	int g_nActScreenX = screenRect.width();
	int g_nActScreenY = screenRect.height();


	if (g_nActScreenX <= 1200 || g_nActScreenY <= 800) {
		appLayoutType = 2;  //认为是工控机
	}
	else {
		appLayoutType = 1;	//认为是pc
	}

	//初始化识别模型
	RecognModel *model1 = RecognModel::getInstance();
	model1->init_model();

	//初始化配置
	AppConfig*appCfg = AppConfig::getInstance();
	appCfg->initSetting(QStringLiteral("App.ini"));

	//启动页面
	QPixmap pixmap("start.jpg");
	QSplashScreen screen(pixmap);
	screen.show();
	//screen.setFixedSize(20);
	//screen.showMessage(QStringLiteral("欢迎使用工业检测软件"), Qt::AlignCenter, Qt::white);

	/*int delayTime = 8;
	QElapsedTimer timer;
	timer.start();
	while (timer.elapsed() < (delayTime * 1000))
	{
		/*使程序在显示启动画面的同时仍能响应鼠标等其他事件
		screen.showMessage(QStringLiteral("初始化..."), Qt::AlignLeft | Qt::AlignBottom, Qt::white);
		screen.showMessage(QStringLiteral("检测设备连接..."), Qt::AlignLeft | Qt::AlignBottom, Qt::white);
		Sleep(1000);
		screen.showMessage(QStringLiteral("初始化字库..."), Qt::AlignLeft | Qt::AlignBottom, Qt::white);
		a.processEvents();
		break;
	}
*/

//**********************************发送 AD 配置 PWM配置 校正系数配置***********************************\\
	//初始化变量 包括指令

	//UCHAR adCfgBuff[18];
	//UCHAR pwmCfgBuff[2];
	//UCHAR correctCfgBuff[1296 + 1296 + 2];  //分开发
	//UCHAR buffTmp[1296];
	//CCyUSBDevice *usbDevice = new CCyUSBDevice(NULL);
	////bool a11 = usbDevice->IsOpen();
	//if (!(usbDevice->IsOpen())) {
	//	MessageBox(NULL, TEXT("设备连接失败！"), TEXT("提示"), MB_OK);
	//	return 0;
	//}
	////***************************************发送AD配置************************************
	//ifstream adFile("Config//Fpga//AdCfg.txt");
	//adCfgBuff[0] = 0x01;
	//adCfgBuff[1] = 0xF0;
	//int temp = 0;
	//for (int i = 2; i < 18; i++) {
	//	adFile >> temp;
	//	adCfgBuff[i] = temp;
	//}
	//long len = 18;
	//usbDevice->BulkOutEndPt->XferData(adCfgBuff, len);

	////****************************************发送PWM配置***********************************
	//
	//ifstream pwmFile("Config//Fpga//PwmCfg");
	//pwmFile >> temp;
	//
	//if (temp > 255) {
	//	if (513 > temp && temp > 255) {
	//		pwmCfgBuff[0] = temp - 256;
	//		pwmCfgBuff[1] = 0xF8 + 1;
	//	}
	//	if (767 >= temp && temp >= 513) {
	//		pwmCfgBuff[0] = temp - 513;
	//		pwmCfgBuff[1] = 0xF8 + 2;
	//	}
	//	if (1024 >= temp && temp > 767) {
	//		pwmCfgBuff[0] = temp - 767;
	//		pwmCfgBuff[1] = 0xF8 + 3;
	//	}
	//}
	//else {
	//	pwmCfgBuff[0] = temp;
	//	pwmCfgBuff[1] = 0xF8;
	//}

	//len = 2;
	//usbDevice->BulkOutEndPt->XferData(pwmCfgBuff, len);

	////**************************************发送校正系数配置*******************************
	//ifstream correctBFile("Config//Fpga//Bcfg.txt");
	//ifstream correctKFile("Config//Fpga//Kcfg.txt");

	////UCHAR correctCfgOrder[2] = { 0x02 ,0xF0 };

	//double k = 0.0;
	//double k0 = 0.0;
	//double b0 = 0.0;

	//for (int i = 0; i < 1296; i += 2) {
	//	correctKFile >> k;
	//	buffTmp[i] = ((k*16.0) > 255 ? 255 : (int)(k * 16.0));
	//}
	//int b;
	//for (int i = 1; i < 1296; i += 2) {

	//	correctBFile >> b;
	//	buffTmp[i] = b;
	//}
	//int bais = 0;
	//UCHAR buffTmpArry[1296];
	//if (bais == 0) {
	//	memcpy(buffTmpArry, buffTmp, 1296);
	//}
	////左移
	//if (bais > 0) {
	//	int a = bais * 2;
	//	int *pData = new int[a];
	//	memcpy(pData, buffTmp, a);
	//	memcpy(buffTmpArry, buffTmp + a, 1296 - a);
	//	memcpy(buffTmpArry + 1296 - a, pData, a);
	//}
	//if (bais < 0) {
	//	int a = abs(bais) * 2;
	//	int *pData = new int[a];
	//	memcpy(pData, buffTmp + 1296 - a, a);
	//	memcpy(buffTmpArry, pData, a);
	//	memcpy(buffTmpArry + a, buffTmp, 1296 - a);
	//}

	//correctCfgBuff[0] = 0x02;
	//correctCfgBuff[1] = 0xF0;
	//memcpy(correctCfgBuff + 2, buffTmpArry, 1296);
	//memcpy(correctCfgBuff + 1298, buffTmpArry, 1296);

	//len = 1296 + 1296 + 2;
	//usbDevice->BulkOutEndPt->XferData(correctCfgBuff, len);

	//
	////**************************结束指令发送***************************************

	//adFile.close();
	//pwmFile.close();
	//correctBFile.close();
	//correctKFile.close();


	//ofstream t("Config//Fpga//Bcfg.txt");
	//ofstream t1("Config//Fpga//Kcfg.txt");
	//for (int i = 0; i < 648; i++) {
	//	t <<0<<endl;
	//	t1 <<1<<endl;
	//}
	//t.close();
	//t1.close();

	IndustryDetect w;
	w.show();

	/*主窗体对象初始化完成后，结束启动画面*/
	
	screen.finish(&w);

	return a.exec();
}
