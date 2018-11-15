#pragma once

#include <QThread>
#include"ImageGet.h"
class WorkThreadForBasicImg : public QThread
{
	Q_OBJECT

	WorkThreadForBasicImg(QObject *parent = 0);
	~WorkThreadForBasicImg();
	WorkThreadForBasicImg(const ImageGet&);
	WorkThreadForBasicImg& operator=(const WorkThreadForBasicImg&);

	//static ImageGet* ImageGetInstance;
	static WorkThreadForBasicImg *workThreadForBasicImg;
public:
	CCyUSBDevice * m_pUSBDevice = NULL;

	static WorkThreadForBasicImg *getInstance();

	ImageGet *imageGet1 = ImageGet::getInstance();

	volatile bool isRun = true;				//控制线程运行标志位
	bool isStart = true;			//控制接受数据标志位

	unsigned char*databuf = NULL;
	unsigned char*inbuf = NULL;

	long bufLength = 0;

	volatile int captureDelay = 0;

	UCHAR bufTest512[512];
	//析构单例
	class delInstance {

		~delInstance() {
			delete workThreadForBasicImg;
			workThreadForBasicImg = NULL;
		};
	};
	static delInstance del;
	//UCHAR bufTest2560[2560];

	void run();
};
