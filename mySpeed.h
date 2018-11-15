#pragma once

#pragma once

#include <QWidget>
#include "ui_mySpeed.h"
#include <QTimer>

class mySpeed : public QWidget
{
	Q_OBJECT

public:
	mySpeed(QWidget *parent = 0);
	~mySpeed();
protected:
	void paintEvent(QPaintEvent *);

	void drawCrown(QPainter *painter);
	void drawBackground(QPainter *painter);
	void drawScale(QPainter *painter);
	void drawScaleNum(QPainter *painter);
	void drawTitle(QPainter *painter);
	void drawIndicator(QPainter *painter);
	void drawNumericValue(QPainter *painter);

private:
	QColor m_background;
	QColor m_foreground;


	int m_maxValue;
	int m_minValue;
	int m_startAngle;
	int m_endAngle;

	int m_scaleMajor;
	int m_scaleMinor;
	double m_value;
	int m_precision;
	QTimer *m_updateTimer;
	QString m_units;
	QString m_title;

	Ui::mySpeed ui;
public Q_SLOTS:
	void UpdateAngle(double);
};

