#include "UpTitleBar.h"

UpTitleBar::UpTitleBar(QWidget *parent)
	: QWidget(parent)
{
	//注意添加logo和blank 布局的顺序 窗体分了两部分logo和blank logo传入label 的构造函数 logo是label父类，具有父类特征，
	//所以在添加布局的时候应该先添加logo和blank
	ui.setupUi(this);
	//调整大小
	const float DEFAULT_DPI = 96.0;

	HDC screen = GetDC(NULL);
	FLOAT dpiX = static_cast<FLOAT>(GetDeviceCaps(screen, LOGPIXELSX));
	ReleaseDC(0, screen);

	float scale = dpiX / DEFAULT_DPI;

	resize(scale* parent->width(), scale * 60);
	setFixedHeight(60);
	//setMaximumHeight();
	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	setSizePolicy(sizePolicy);

	//填充背景
	QPalette pal(palette());

	QLinearGradient linear(QPointF(0, 0), QPointF(scale* parent->width(), 0));
	//linear.setColorAt(1, QColor(157, 130, 145));
	//linear.setColorAt(1, QColor(33, 112, 255));
	linear.setColorAt(1, QColor(128, 168, 255));
	linear.setColorAt(0.7, QColor(142, 134, 255));
	linear.setColorAt(0.3, QColor(130, 140, 255));
	linear.setColorAt(0, QColor(128, 168, 255));
	pal.setBrush(backgroundRole(), QBrush(linear));

	//pal.setColor(QPalette::Window, QColor(74, 144, 226));

	setAutoFillBackground(true);
	setPalette(pal);
	QHBoxLayout *hmainyout = new QHBoxLayout(this);

	QHBoxLayout *hyout1 = new QHBoxLayout(this);
	titleLabel = new QLabel();
	titleLabel->setFixedSize(90, 50);
	titleLabel->setStyleSheet("background-color:rgb(255,255,255)");
	QImage logimg("icon/title.ico");
	titleLabel->setAlignment(Qt::AlignCenter);
	titleLabel->setPixmap(QPixmap::fromImage(logimg).scaled(scale * 150, scale * 70, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	
	textLabel = new QLabel();

	textLabel->setStyleSheet("color:white;font:16px 'Microsoft YaHei'; font:bold");
	textLabel->setFixedSize(210, 60);
	//textLabel->setStyleSheet("background-color:rgb(255,255,255)");
	QImage logimg1("icon/title1.png");
	textLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	textLabel->setPixmap(QPixmap::fromImage(logimg1).scaled(scale * 200, scale * 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));

	//textLabel->adjustSize();

	hyout1->addWidget(titleLabel);
	//hyout1->addSpacing(0);
	hyout1->addWidget(textLabel);
	//hyout1->setSpacing(5);
	hyout1->setContentsMargins(0, 0, 0, 0);

	/*--------------------logo控件-------------------------------*/

	/*-------------blank控件-------------------*/
	QSizePolicy sizeFixed(QSizePolicy::Expanding, QSizePolicy::Fixed);
	//button

	//	CloseButt->setStyleSheet(styleSheet1);
	//构建最小化、最大化、关闭按钮  
	MinimazeBtn = new QPushButton(this);
	MaximizeBtn = new QPushButton(this);
	CloseBtn = new QPushButton(this);
	settingBtn = new QPushButton(this);

	//设置显示策略
	MinimazeBtn->setFixedSize(QSize(scale * 30, scale * 25));
	MaximizeBtn->setFixedSize(QSize(scale * 30, scale * 25));
	CloseBtn->setFixedSize(QSize(scale * 30, scale * 25));
	settingBtn->setFixedSize(QSize(scale * 30, scale * 25));

	////获取最小化、关闭按钮图标  
	//QPixmap minPix = style()->standardPixmap(QStyle::SP_TitleBarMinButton);
	//QPixmap closePix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton);
	//QPixmap maxPix = style()->standardPixmap(QStyle::SP_TitleBarMaxButton);

	//MinimazeButt->setText(QStringLiteral("最小化"));
	//设置最小化、关闭按钮图标  
	MinimazeBtn->setIconSize(QSize(12, 12));
	MinimazeBtn->setIcon(QIcon("icon/1.ico"));
	MinimazeBtn->setToolTip(QStringLiteral("最小化"));

	MaximizeBtn->setIconSize(QSize(12, 12));
	MaximizeBtn->setIcon(QIcon("icon/8.ico"));
	MaximizeBtn->setToolTip(QStringLiteral("最大化"));

	CloseBtn->setIconSize(QSize(12, 12));
	CloseBtn->setIcon(QIcon("icon/2.ico"));
	CloseBtn->setToolTip(QStringLiteral("关闭"));

	settingBtn->setIconSize(QSize(18, 18));
	settingBtn->setIcon(QIcon("icon/set.ico"));
	settingBtn->setToolTip(QStringLiteral("设置"));

	//设置最小化、关闭按钮的样式  
	MinimazeBtn->setStyleSheet("QPushButton{background-color:transparent;border-style:non; }"
		"QPushButton:hover{background-color:rgb(0,124,204);}");
	MaximizeBtn->setStyleSheet("QPushButton{background-color:transparent;border-style:non;}"
		"QPushButton:hover{background-color:rgb(0,124,204);}");
	CloseBtn->setStyleSheet("QPushButton{background-color:transparent;border-style:non; }"
		"QPushButton:hover{background-color:rgb(230,30,30);}");

	settingBtn->setStyleSheet("QPushButton{background-color:transparent;border-style:non; }"
		"QPushButton:hover{background-color:rgb(0,124,204);}");

	//	MaximizeButt->setStyleSheet(styleSheet2);

	//connect(CloseButt, SIGNAL(clicked()), this, SLOT(test()));

	/*-------------blank布局-------------------*/
	//按钮布局

	QHBoxLayout *buttLayout = new QHBoxLayout(this);		//new QHBoxLayout(blank)
	buttLayout->addStretch(3);

	buttLayout->addWidget(settingBtn);
	buttLayout->addWidget(MinimazeBtn);
	buttLayout->addWidget(MaximizeBtn);
	buttLayout->addWidget(CloseBtn);
	buttLayout->setContentsMargins(60, 0, 0, 0);


	//按钮总体布局
	//QVBoxLayout *blankLayout = new QVBoxLayout(blank);
	hmainyout->addLayout(hyout1);
	hmainyout->addLayout(buttLayout);
	//hmainyout

	/***************************************信号连接**********************/

	QObject::connect(CloseBtn, SIGNAL(clicked()), this->parent(), SLOT(close()));
	QObject::connect(MaximizeBtn, SIGNAL(clicked()), this->parent(), SLOT(showMaximized()));
	QObject::connect(MinimazeBtn, SIGNAL(clicked()), this->parent(), SLOT(showMinimized()));
	QObject::connect(settingBtn, SIGNAL(clicked()), this, SLOT(showSettingDialog()));

	setDialog = new SettingDialog(this);
	setDialog->loadSetting(AppConfig::getInstance());
	//setDialog->hide();
}

UpTitleBar::~UpTitleBar()
{
}

void  UpTitleBar::showSettingDialog() {
	setDialog->show();//展示设置
}

void UpTitleBar::mousePressEvent(QMouseEvent *event)
{
	isMoving = true;
	//记录下鼠标相对于窗口的位置  
	//event->globalPos()鼠标按下时，鼠标相对于整个屏幕位置  
	//pos() this->pos()鼠标按下时，窗口相对于整个屏幕位置  
	movePosition = event->globalPos() - this->window()->pos();
	return QWidget::mousePressEvent(event);
}

void UpTitleBar::mouseReleaseEvent(QMouseEvent *event) {
	isMoving = false;
}

void UpTitleBar::mouseDoubleClickEvent(QMouseEvent * event)
{
	if (Qt::WindowState::WindowMaximized == window()->windowState())
		window()->setWindowState(Qt::WindowState::WindowNoState);
	else
		window()->setWindowState(Qt::WindowState::WindowMaximized);
	return QWidget::mouseDoubleClickEvent(event);
}

void UpTitleBar::mouseMoveEvent(QMouseEvent *event) {
	//鼠标移动事件需要移动窗口，窗口移动到哪里呢？就是要获取鼠标移动中，窗口在整个屏幕的坐标，然后move到这个坐标，怎么获取坐标？  
	//通过事件event->globalPos()知道鼠标坐标，鼠标坐标减去鼠标相对于窗口位置，就是窗口在整个屏幕的坐标  
	if (isMoving && (event->buttons() && Qt::LeftButton)//鼠标左键按下
		&& (event->globalPos() - movePosition).manhattanLength() > QApplication::startDragDistance())
	{
		this->window()->move(event->globalPos() - movePosition);
		movePosition = event->globalPos() - this->window()->pos();
	}
	return QWidget::mouseMoveEvent(event);
}

