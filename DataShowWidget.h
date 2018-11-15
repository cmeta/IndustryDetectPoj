#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QHBoxLayout>
#include<QScrollBar>
#include "ImageGet.h"
#include<QMenu>
#include "ui_DataShowWidget.h"


//��ʾ�������
class DataShowWidget : public QWidget
{
	Q_OBJECT

public:
	DataShowWidget(QWidget *parent = Q_NULLPTR);
	~DataShowWidget();

	QTableWidget *goodDataTable;
	QTableWidget *badDataTable;
	QVBoxLayout* dataShowVlayout;
	int detectNum = 0; //���Ľ����

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
