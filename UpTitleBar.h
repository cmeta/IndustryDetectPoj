#pragma once

#include <QWidget>
#include "ui_UpTitleBar.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QImage>
#include <QLabel>
#include <QPushButton>
#include <QtGui/QMouseEvent>
#include<qt_windows.h>
#include"AppConfig.h"
#include"SettingDialog.h"

//#include <CyAPI.h>

//标题栏
class UpTitleBar : public QWidget
{
	Q_OBJECT

public:
	UpTitleBar(QWidget *parent = Q_NULLPTR);
	~UpTitleBar();
	//UI
	QLabel *titleLabel;
	QLabel *textLabel;
	QWidget *logo;
	QWidget *blank;
	QPushButton *CloseBtn;
	QPushButton *MaximizeBtn;
	QPushButton *MinimazeBtn;
	QPushButton *settingBtn;

	SettingDialog *setDialog;
	//QLabel *labelInfo;
	//QLabel *stateLabel;  //显示进度信息

	QHBoxLayout *hLayoutMain;

	QHBoxLayout *hLayoutLogo;

	//QHBoxLayout *hLayoutState;

protected:
	// 重写按下鼠标事件
	virtual void mousePressEvent(QMouseEvent *event);
	//重写移动鼠标事件
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void mouseDoubleClickEvent(QMouseEvent *event);
public slots:
	void showSettingDialog();
signals:
	//void _showSettingDialog();
private:
	Ui::UpTitleBar ui;
	bool isMoving;
	QPoint movePosition;
};
