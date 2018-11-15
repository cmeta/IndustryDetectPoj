#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QHBoxLayout>
#include<QScrollBar>
#include "ImageGet.h"
#include<QMenu>
#include "ui_DataShowWidget.h"


//显示表格数据
class DataShowWidget : public QWidget
{
	Q_OBJECT

public:
	DataShowWidget(QWidget *parent = Q_NULLPTR);
	~DataShowWidget();

	QTableWidget *goodDataTable;
	QTableWidget *badDataTable;
	QVBoxLayout* dataShowVlayout;
	int detectNum = 0; //检测的结果数

public slots:
	void updateList(DataInfo&);
	void updateBadDataList(DataInfo&);
	void popMenuGood(QPoint point);
	void popMenuBad(QPoint point);

	void clearBadList();
	void clearGoodList();


private:
	Ui::DataShowWidget ui;
};
