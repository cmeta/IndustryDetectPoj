#pragma once

#include <QWidget>
#include<qlabel.h>
#include<qpushbutton.h>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "ui_AboutWidget.h"

class AboutWidget : public QWidget
{
	Q_OBJECT

public:
	AboutWidget(QWidget *parent = Q_NULLPTR);
	~AboutWidget();
	
	QLabel*aboutLabel;
	QPushButton* correctBtn;
	QPushButton* backBtn;
	QVBoxLayout*mainLayout;
	QHBoxLayout*btnLayout;
public slots:
	void backBtnClicked();
	void correctBtnClicked();
signals:
	void correctSignals();

private:
	Ui::AboutWidget ui;
};
