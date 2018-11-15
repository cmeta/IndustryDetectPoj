#pragma once

#include <QWidget>
#include <QTableWidget>

#include <QPushButton>
#include<qtoolbutton.h>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include<QScrollBar>
#include <string>
#include <QTextCodec>
#include<qmessagebox.h>
#include<iostream>
#include<fstream>
#include<qfiledialog.h>
#include"qmenu.h"
#include"TypeEditWidget.h"
#include "ui_DataTypeWidget.h"

//数据类型操作区域  一个表格可变数据类型设置区域和可变数据类型操作区域
class DataTypeWidget : public QWidget
{
	Q_OBJECT

public:
	DataTypeWidget(QWidget *parent = Q_NULLPTR);
	~DataTypeWidget();
	

	//表格可变数据类型设置区域
	QTableWidget *dataTypeTable;
	//操作区域
	QToolButton *areaOperationBtn;  //用于解锁绘图区域
	QToolButton *addAreaBtn;        //用于新增表项
	QToolButton *reduceAreaBtn;
	QToolButton *saveDataTypeBtn;
	QToolButton *loadDatatypeBtn;

	QGridLayout *btnGridLayout;
	QVBoxLayout *mianvLayout;

	 bool buttonChange = true;		//控制按钮文字变化

	 int getTableRow();     //返回列表的行
	 int getTableColumn();   //返回列表的列
	 int rowNum = 0;
	 int colNum = 0;

signals:
	void areaOperation();   //发送给label
	void reduceAreaSig();
	void showTypeEditWidget(int);
	void saveCurrentBasicImg(QString);
	void setCurrentBasicImg(QString);

public slots:
	void addArea(QRect&);
	void reduceArea();
	void emitAreaOperation();
	void exportDataType();
	void importDataType();
	void popMenuEdit(QPoint point);
	void editList();
	void updateRow(EditDatatype&);

private:
	Ui::DataTypeWidget ui;
};
