#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include "qlabel.h"
#include "qpushbutton.h"
#include"AppConfig.h"

#include "ui_imgLabelShowWidget.h"
class imgLabelShowWidget : public QWidget
{
	Q_OBJECT

public:
	imgLabelShowWidget(QWidget *parent = Q_NULLPTR);
	~imgLabelShowWidget();
	QVBoxLayout *mainVLayout;
	QLabel *imgLabel;
	QPushButton *backBtn;

public slots:
	void imgLabelBack();
signals:
	void imgLabelShowBack();

private:
	Ui::imgLabelShowWidget ui;
};
