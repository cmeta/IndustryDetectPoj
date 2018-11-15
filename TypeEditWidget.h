#pragma once

#include <QWidget>
#include "ui_TypeEditWidget.h"

class EditDatatype
{
public:
	QString type="";
	QString angle="";
	QString charNum = "";
	QString rowNum = "";
	QString compareType = "";
	QString compareConf = "";
	EditDatatype();
	~EditDatatype();
};

class TypeEditWidget : public QWidget
{
	Q_OBJECT

public:
	TypeEditWidget(QWidget *parent = Q_NULLPTR);
	~TypeEditWidget();
	Ui::TypeEditWidget ui;
	int row = 0;
	EditDatatype editDatatype;
signals:
	void updateTypeTable(EditDatatype&);
	//ui.combox
public slots:
	void okBtnClicked();
};
