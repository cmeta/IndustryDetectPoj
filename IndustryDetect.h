#pragma once

#include <QtWidgets/QWidget>
#include "ui_IndustryDetect.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPixmap>
#include <QImage>
#include <QPushButton>
#include <QGroupBox>
#include <time.h>

#include "mQLabel.h"
#include "DataShowWidget.h"
#include "DataTypeWidget.h"
#include "UpTitleBar.h"
#include "DownStateBarWidget.h"
#include "mySpeed.h"
#include"TypeEditWidget.h"

#include "ImageGet.h"
#include "WorkThread.h"
#include"WorkThreadForBasicImg.h"
#include"TypeEditWidget.h"
#include"AppConfig.h"
#include"LabelDAOImpl.h"
#include"imgLabelShowWidget.h"
#include<vector>
#include"AboutWidget.h"

//定义比对方式
typedef enum CompareType {
	DATABASE_COMP = 0,
	TIME_COMP,
	CONTENT_COMP,
	FLOWNUM_COMP,
}CompareType;

//主界面

class IndustryDetect : public QWidget
{
	Q_OBJECT

public:
	IndustryDetect(QWidget *parent = Q_NULLPTR);
	~IndustryDetect();

	mQLabel *imgLabel;
	DataTypeWidget *dataTypeWidget;
	DataShowWidget *dataShowWidget;
	UpTitleBar *upTitleBar;
	DownStateBarWidget *downStateBarWidget;
	//mySpeed *myspeed;
	QLabel *speedLabel;
	TypeEditWidget*typeEditWidget;
	imgLabelShowWidget* imgShowWidget;
	AboutWidget*aboutWidget;
	//QVBoxLayout *labelLayout;
	QVBoxLayout *leftMainLayout;
	QVBoxLayout *rightMainLayout;
	QHBoxLayout *mainLayout;  //正文区布局
	QVBoxLayout *m_mianLayout;                         //总体布局

	QGroupBox *funcGropBox;
	QToolButton *startWorkBtn;
	QToolButton *finishWorkBtn;
	QToolButton *setBaseImgBtn;
	QToolButton *correctImgBtn;
	QToolButton *resetCountBtn;
	QToolButton *aboutBtn;
	//QToolButton *reduceLightBtn;
	//QToolButton *increaseLightBtn;

	QGridLayout* funcGridLayout;

	//功能字段
	ImageGet *imageget;
	
	WorkThread *workThread;
	WorkThreadForBasicImg*workThreadForBasicImg;

	AppConfig* appConfig;
	QImage RGBImage;

	CCyUSBDevice *m_pUSBDevice = NULL;
	
	//比对功能相关变量定义
	CompareType compareType[4];

	vector<string> txtDataVec[4];

	vector<QString> *SQLDataVec[4] = { NULL,NULL,NULL,NULL };

	LabelDAOImpl labelDAOImpl;

	int errCount=0, totalCount = 0;
	//计时操作的变量 更新速度表
	bool isFirstCount = true;
	clock_t start, end;

	QImage imgTest;
	Mat baseImg;
	Mat baseImg1;
	QString compareTmp[4] = {"","","",""};
	int flowNumIncrease[4];
	int flowNumStart[4];
	//时间格式
	QString timeFormat[4];
	bool isGettime[4] = { false ,false,false,false };
	QDateTime currentDateTime;
	//int timeFormatIndex;

	int labelWidth, labelHeight = 0;
	unsigned char *databuf = NULL;

	void sendWarningOrderToFPGA();

	int errCountDelay = 0;
	int *bufferCountDelay[2];

	bool barcode = false;
	//识别区间
	int ocrRegion[10][2] = { 0 };
	bool isocrRegion[5] = { false ,false , false , false , false };

	//
	int appType = 0;

	volatile bool showLargeType = false;
public slots:
	void finishWorkBtnClicked();
	void startWorkBtnClicked();
	void updateInfo(DataInfo&);
	void setBaseImgBtnClicked();
	void calculateCorrectCoeff();

	void showTypeEditWidget(int);
	void clearLabel();

	void updateSpeedSlot(double);

	void saveCurrentBasicImgSlot(QString);

	void mSetBasicImg(QString);

	void setShowType();

	void showLargeLabelSlot();

	void showAboutWidgetSlot();

	void resetCountSlot();
signals:
	void updateSpeed(double);
	void updateDownstatus(int,int);
	void setBasic_(QString);
private:
	Ui::IndustryDetectClass ui;
};
