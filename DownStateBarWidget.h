#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include<qt_windows.h>

#include "ui_DownStateBarWidget.h"

//״̬�� ������ʾ״̬ 
class DownStateBarWidget : public QWidget
{
	Q_OBJECT

public:
	DownStateBarWidget(QWidget *parent = Q_NULLPTR);
	
	~DownStateBarWidget();

	void setConnnetStateInfo(QString&);  //����������Ϣ

	QLabel *connectStateInfo;		//��ʾ������Ϣ
	QLabel *totalImgNum;		//��ʾͼ��ɼ�����
	QLabel *errorRecordNum;    //�����¼����
	QLabel *currentTime;		//��ǰʱ��

	QHBoxLayout *mainhLayout;   //������  

public slots:
	void timerUpdate();
	void updateErrAndTotalconut(int, int);
private:
	Ui::DownStateBarWidget ui;
};
