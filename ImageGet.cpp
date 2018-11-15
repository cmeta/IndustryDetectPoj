#include "ImageGet.h"
#include <QDebug>

ofstream fout("speedTest.txt", ios::out | ios::app);
QImage currentBasicImg;

inline std::string GBK2UTF8(std::string &str)
{
	QString temp = QString::fromLocal8Bit(str.c_str());
	std::string ret = temp.toUtf8().data();
	return ret;
}

inline QString GBK2UTF8(const QString &str)
{
	QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");
	return utf8->toUnicode(str.toUtf8());
}

inline QString UTF82GBK(const QString &str) {
	QTextCodec *gbk = QTextCodec::codecForName("GB2312");
	return gbk->toUnicode(str.toLocal8Bit());
}

//std::ofstream out1("Log1.txt", ios::out | ios::binary);

ImageGet::ImageGet(QObject *parent)
	:QThread(parent)
{
	//isSave = AppConfig::getInstance()->isSaveImg;
	//saveImagePath = AppConfig::getInstance()->saveImgPath;
	width = AppConfig::getInstance()->width;
	height = AppConfig::getInstance()->height;

}

ImageGet::~ImageGet()
{

}

ImageGet* ImageGet::ImageGetInstance = NULL;

ImageGet*ImageGet::getInstance()
{
	if (ImageGetInstance == NULL)
	{
		ImageGetInstance = new ImageGet();
	}
	return ImageGetInstance;
};

// Qimg转Mat函数
cv::Mat ImageGet::QImage2cvMat(QImage &image)
{
	cv::Mat mat;
	//qDebug() << image.format();
	image.format();
	switch (image.format())
	{
	case QImage::Format_ARGB32:
	case QImage::Format_RGB32:
	case QImage::Format_ARGB32_Premultiplied:
		mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
		break;
	case QImage::Format_RGB888:
		mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
		cv::cvtColor(mat, mat, CV_BGR2RGB);
		break;
	case QImage::Format_Indexed8:
		mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
		break;
	case QImage::Format_Grayscale8:
		mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
		break;
	}
	return mat;
}
void ImageGet::updateBasicImg(QString str) {
	//qDebug() << str << endl;
	QImage img1(str);
	mBasicImg = QImage2cvMat(img1);

	if (mBasicImg.channels() != 1) {
		cvtColor(mBasicImg, mBasicImg, CV_BGR2GRAY);
	}
	//imwrite("test1111.bmp", mBasicImg);
	//qDebug() << "save template img" << a<< endl;
	malgorithm.baseImg = mBasicImg.clone();    //等号操作符只拷贝矩阵头 矩阵指针和信息头 不拷贝矩阵 提高速度

}

/***************************************************\
*
* @FunctionName processImgThread
* @author Zhaoyu
* @date   2018年6月24日
* @param[out]
* @param[in]
* @return     void
* @brief 1 保存图像 2 识别图像
*
****************************************************/

void ImageGet::processImgThread()
{
	//构造图像
	img = QImage((const unsigned char*)(data), width, height, width, QImage::Format_Grayscale8);
	dataInfo.img = img;

	//**********************************保存图像到以日期命名的文件夹**********************************//
	if (isSave == true) {
		fileName.clear();

		QDateTime time = QDateTime::currentDateTime();
		fileName = saveImagePath + time.toString("yyyy-MM-dd_hh-mm-ss.zzz");
		QDir dir;
		dir.mkpath(fileName);

		fileName = fileName + "\\test.bmp";

		img.save(fileName);
	}


	/**********************************识别图像信息**********************************/

	//1):将当前QIMG图像转换为mat格式的图像


	mCurImg = QImage2cvMat(img);

	if (mCurImg.channels() != 1) {
		cvtColor(mCurImg, mCurImg, CV_BGR2GRAY);
	}
	//2):将当前图像输入到算法对象
	malgorithm.curImg = mCurImg;    //等号操作符只拷贝矩阵头 矩阵指针和信息头 不拷贝矩阵 提高速度

	//3):识别图像信息 datatypeNum 为框选区域的数量
	for (int i = 0; i < datatypeNum; i++) {
		//clock_t start, ends;
		//start = clock();
		dataInfo.reResult[i] = QString::fromStdString(GBK2UTF8(malgorithm.predict(datatype[i])));
		//ends = clock();
		//fout << "算法用时:" << ends - start << "ms 结果：" << dataInfo.reResult[i].toStdString() <<endl;
	}

	//4):将识别的信息发送到UI界面更新UI
	dataInfo.resultNum = datatypeNum;

	emit sendDataInfo(dataInfo);

}

void ImageGet::setBasicImg()
{
	img = QImage((const unsigned char*)(data), width, height, width, QImage::Format_Grayscale8);
	dataInfo.img = img; 

	count++;
	if (count == 3) {

		//img.save("test.bmp");
		/*QFile fileTemp("test.bmp");
		bool a = fileTemp.remove();*/
		int a =std::remove("test.bmp");

		mBasicImg = QImage2cvMat(img);
		currentBasicImg = img;

		if (mBasicImg.channels() != 1) {
			cvtColor(mBasicImg, mBasicImg, CV_BGR2GRAY);
		}
		imwrite("test.bmp", mBasicImg);
		//qDebug() << "save template img" << a<< endl;
		malgorithm.setBaseImg(mBasicImg);    //等号操作符只拷贝矩阵头 矩阵指针和信息头 不拷贝矩阵 提高速度

		//emit sendDataInfo(count);
	}

	dataInfo.resultNum = 0;

	emit sendDataInfo(dataInfo);
}


void ImageGet::run()
{

	switch (modType)
	{
	case PROCESSIMG:
		if (data == NULL) return;
		this->processImgThread();
		break;
	case SETBASICIMG:
		if (data == NULL) return;
		this->setBasicImg();
		break;
	case CORRECTMODE:
		//this->genColorCorrectCoff();
		break;
	default:
		break;
	}


}


void ImageGet::genColorCorrectCoff()
{
	return;
}

void ImageGet::calCoefFilterLinerStrech(Mat &img_origin, Mat &dst, int color)
{
	/*
	std::ofstream bofs;//定义ofstream类的对象bofs,ofstream是从内存到硬盘，ifstream是从硬盘到内存
	std::ofstream wofs;
	std::ofstream mofs;
	std::ofstream nofs;
	unsigned char MAT[MAT_HEIGHT][MAT_WIDTH];

	int RowNumB = BLACK_ROW_END - BLACK_ROW_START;
	int RowNumW = WHITE_ROW_END - WHITE_ROW_START;

	if (color == 1)//RGB的R
	{
		bofs.open("Coeff//1_b_Coeff.txt");	//调用成员函数open打开1_B_Coeff.txt,假如open函数只有文档名一个参数，则是以读/写普通文档打开,没有这个文档就创建一个
		wofs.open("Coeff//1_w_Coeff.txt");
		mofs.open("Coeff//1_m_coeff.txt");
		nofs.open("Coeff//1_n_coeff.txt");
	}
	else if (color == 2)//G
	{
		bofs.open("Coeff//2_b_Coeff.txt");
		wofs.open("Coeff//2_w_Coeff.txt");
		mofs.open("Coeff//2_m_coeff.txt");
		nofs.open("Coeff//2_n_coeff.txt");
	}
	else if (color == 3)//B
	{
		bofs.open("Coeff//3_b_Coeff.txt");
		wofs.open("Coeff//3_w_Coeff.txt");
		mofs.open("Coeff//3_m_coeff.txt");
		nofs.open("Coeff//3_n_coeff.txt");
	}

	memcpy(MAT, img_origin.data, MAT_HEIGHT * MAT_WIDTH);//从img_origin->imageData所指向的内存地址的起始位置将MAT_HEIGHT*MAT_WIDTH大小的数据拷贝到MAT所指向的内存地址

	memset(BCoeff, 0, MAT_WIDTH * sizeof(double));
	memset(WCoeff, 0, MAT_WIDTH * sizeof(double));

	for (int j = StartCol; j < EndCol; j++)
	{

		////B_Coeff
		//for (int i = 3300; i < 3500; i++)//这个是逐条校正，每一条算两个校正参数,取十个像素点求平均值，减少误差   (600 700 1200 1300)
		//{
		//	BCoeff[j] = BCoeff[j] + MAT[i][j];
		//}
		//BCoeff[j] = BCoeff[j] / 200;

		////W_Coeff
		//for (int i = 1000; i < 1200; i++)
		//{
		//	WCoeff[j] = WCoeff[j] + MAT[i][j];
		//}

		//WCoeff[j] = WCoeff[j] / 200;

		//MCoeff[j] = 240 / (WCoeff[j] - BCoeff[j]);  //
		//NCoeff[j] = 0 - MCoeff[j] * BCoeff[j];

		//B_Coeff

		for (int i = BLACK_ROW_START; i < BLACK_ROW_END; i++)//这个是逐条校正，每一条算两个校正参数,取十个像素点求平均值，减少误差   (600 700 1200 1300)
		{
			BCoeff[j] = BCoeff[j] + MAT[i][j];
		}
		BCoeff[j] = BCoeff[j] / RowNumB;

		//W_Coeff
		for (int i = WHITE_ROW_START; i < WHITE_ROW_END; i++)
		{
			WCoeff[j] = WCoeff[j] + MAT[i][j];
		}

		WCoeff[j] = WCoeff[j] / RowNumW;


		MCoeff[j] = 240 / (WCoeff[j] - BCoeff[j]);  //220
		NCoeff[j] = 10 - MCoeff[j] * BCoeff[j];
	}


	double temp;
	for (int j = StartCol; j < EndCol; j++)
	{
		bofs << BCoeff[j] << "\n";
		wofs << WCoeff[j] << "\n";
		mofs << MCoeff[j] << "\n";
		nofs << NCoeff[j] << "\n";

		for (int i = 0; i < MAT_HEIGHT; i++)
		{

			temp = (MAT[i][j]) * MCoeff[j] + NCoeff[j];
			/*if (MAT[i][j] == 0)
			{
				temp = MAT[i][j];
			}
			else
			{
				temp = (MAT[i][j]) * MCoeff[j] + NCoeff[j];
			}*/

			/*
						if (temp < 0)
						{
							temp = 0;
						}
						if (temp > 255)
						{
							temp = 255;
						}
						MAT[i][j] = temp;
					}
				}

				memcpy(dst.data, MAT, MAT_HEIGHT * MAT_WIDTH); */

}
void ImageGet::calCoefFilterLinerStrech(Mat & imgOrigin, Mat & dst)
{
	ofstream bFile("Config//Fpga//Bcfg.txt");
	ofstream kFile("Config//Fpga//Kcfg.txt");

	double bCoff[648] = { 0.0 };
	double kCoff[648] = { 0.0 };

	unsigned char mat[500][648];
	memcpy(mat, imgOrigin.data, 500 * 648);

	//计算b的系数
	int sum = 0;
	for (int i = 0; i < 648; i++) {
		sum = 0;
		for (int j = 100; j < 200; j++) {
			sum += mat[j][i];
			bCoff[i] = sum / 50;
		}
	}
	//计算k的系数
	double k = 0.0;
	for (int i = 0; i < 648; i++) {
		k = 240.0 / (mat[400][i] - bCoff[i]);
		kCoff[i] = k;
	}
	//写入到文本文件中
	for (int i = 0; i < 648; i++) {
		kFile << kCoff[i] << "\n";
		bFile << bCoff[i] << "\n";
	}
	kFile.close();
	bFile.close();

	//用该系数校正黑白图

}
void ImageGet::test()
{

}

