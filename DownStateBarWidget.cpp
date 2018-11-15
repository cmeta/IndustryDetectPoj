#include "DownStateBarWidget.h"
#include<QTimer>
#include <QDateTime>

DownStateBarWidget::DownStateBarWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//调整大小
	const float DEFAULT_DPI = 96.0;

	HDC screen = GetDC(NULL);
	FLOAT dpiX = static_cast<FLOAT>(GetDeviceCaps(screen, LOGPIXELSX));
	ReleaseDC(0, screen);

	float scale = dpiX / DEFAULT_DPI;

	resize(scale* parent->width(), scale * 28);
	setMaximumHeight(35);
	setMinimumHeight(28);
	//setMaximumHeight();
	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	setSizePolicy(sizePolicy);

	//填充背景
	QPalette pal(palette());
	QLinearGradient linear(QPointF(0, 0), QPointF(scale* parent->width(), 0));
	//linear.setColorAt(1, QColor(157, 130, 145));
	//linear.setColorAt(1, QColor(33, 112, 255));
	linear.setColorAt(1, QColor(128, 168, 255));
	linear.setColorAt(0.7, QColor(92, 134, 255));
	linear.setColorAt(0.3, QColor(138, 150, 255));
	linear.setColorAt(0, QColor(128, 168, 255));
	pal.setBrush(backgroundRole(), QBrush(linear));

	//pal.setColor(QPalette::Window, QColor(0, 122, 204));
	setAutoFillBackground(true);
	setPalette(pal);

	connectStateInfo = new QLabel;
	connectStateInfo->setFixedHeight(scale * 27);

	QFont font0;  // ("Microsoft YaHei", 12, 50);
	font0.setFamily("Microsoft YaHei");
	font0.setPixelSize(20);
	font0.setBold(true);
	font0.setPointSize(10);

	connectStateInfo->setFont(font0);
	QPalette pa0;
	pa0.setColor(QPalette::WindowText, QColor(255, 255, 248));
	connectStateInfo->setPalette(pa0);
	connectStateInfo->setText(QStringLiteral("显示连接信息"));
	connectStateInfo->setAlignment(Qt::AlignCenter);

	totalImgNum = new QLabel;		//显示图像采集总数
									//totalImgNum = new QLabel();
	totalImgNum->setFixedHeight(scale * 27);
	
	QFont font1;  // ("Microsoft YaHei", 12, 50);
	font1.setFamily("Microsoft YaHei");
	font1.setPixelSize(20);
	font1.setBold(true);
	font1.setPointSize(10);

	totalImgNum->setFont(font1);
	QPalette pa1;
	pa1.setColor(QPalette::WindowText, QColor(255,255,248));
	totalImgNum->setPalette(pa1);
	totalImgNum->setText(QStringLiteral("显示图像采集总数"));
	totalImgNum->setAlignment(Qt::AlignCenter);


	errorRecordNum = new QLabel;    //错误记录总数
	errorRecordNum->setAlignment(Qt::AlignCenter);
	errorRecordNum->setFixedHeight(scale * 27);
	
	QFont font2;  // ("Microsoft YaHei", 12, 50);
	font2.setFamily("Microsoft YaHei");
	font2.setPixelSize(20);
	font2.setBold(true);
	font2.setPointSize(10);

	errorRecordNum->setFont(font2);
	errorRecordNum->setPalette(pa1);
	errorRecordNum->setText(QStringLiteral("显示错误记录总数"));


	currentTime = new QLabel;		//当前时间
	currentTime->setAlignment(Qt::AlignCenter);
	currentTime->setFixedHeight(scale * 27);

	QFont font3;  // ("Microsoft YaHei", 12, 50);
	font3.setFamily("Microsoft YaHei");
	font3.setPixelSize(20);
	font3.setBold(true);
	font3.setPointSize(10);

	currentTime->setFont(font3);
	currentTime->setPalette(pa1);
	//currentTime->setText(QStringLiteral("显示时间"));
	//一秒钟更新一次时间
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
	timer->start(1000);

	mainhLayout = new QHBoxLayout(this);   //主布局  
	mainhLayout->addWidget(connectStateInfo);
	mainhLayout->addWidget(totalImgNum);
	mainhLayout->addWidget(errorRecordNum);
	mainhLayout->addWidget(currentTime);
	mainhLayout->setMargin(0);
	mainhLayout->setContentsMargins(10, 0, 0, 10);

}

DownStateBarWidget::~DownStateBarWidget()
{
}

void DownStateBarWidget::setConnnetStateInfo(QString & str)
{
	connectStateInfo->setText(str);
}

void DownStateBarWidget::updateErrAndTotalconut(int total, int err)
{
	totalImgNum->setText(QStringLiteral("检测总数 = ")+QString::number(total,10));
	errorRecordNum->setText(QStringLiteral("错误数 = ")+QString::number(err,10));

}

void  DownStateBarWidget::timerUpdate() {
	QDateTime time = QDateTime::currentDateTime();
	QString str = time.toString("yyyy-MM-dd hh:mm:ss dddd");
	currentTime->setText(str);
}
