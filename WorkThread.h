#pragma once

#include <QThread>
#include "ImageGet.h"
#include <fstream>
#include "qt_windows.h"
#include "cyapi.h"


class WorkThread : public QThread
{
	Q_OBJECT

	WorkThread(QObject *parent = 0);
	~WorkThread();
	WorkThread(const ImageGet&);
	WorkThread& operator=(const WorkThread&);

	//static ImageGet* ImageGetInstance;
	static WorkThread *workthread;
public:
	CCyUSBDevice * m_pUSBDevice =NULL;

	static WorkThread *getinstance();

	ImageGet *imageGet1 = ImageGet::getInstance();
	bool countFlag = true;

	volatile bool isRun = true;				//控制线程运行标志位
	bool isStart = true;			//控制接受数据标志位
								
	unsigned char*databuf = NULL;
	unsigned char*inbuf = NULL;

	long bufLength = 0;

	volatile int captureDelay = 0;

	UCHAR bufTest512[512];
	//析构单例
	class delInstance {

		~delInstance(){
			delete workthread;
			workthread = NULL;
		};
	};
	static delInstance del;
	
	void run();

signals:
	void updateSpeed(double);
};


