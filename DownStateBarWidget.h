#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include<qt_windows.h>

#include "ui_DownStateBarWidget.h"

//状态栏 用于显示状态 
class DownStateBarWidget : public QWidget
{
	Q_OBJECT

public:
	DownStateBarWidget(QWidget *parent = Q_NULLPTR);
	
	~DownStateBarWidget();

	void setConnnetStateInfo(QString&);  //设置连接信息

	QLabel *connectStateInfo;		//显示连接信息
	QLabel *totalImgNum;		//显示图像采集总数
	QLabel *errorRecordNum;    //错误记录总数
	QLabel *currentTime;		//当前时间

	QHBoxLayout *mainhLayout;   //主布局  

public slots:
	void timerUpdate();
	void updateErrAndTotalconut(int, int);
private:
	Ui::DownStateBarWidget ui;
};
