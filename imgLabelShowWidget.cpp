#include "imgLabelShowWidget.h"

imgLabelShowWidget::imgLabelShowWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	AppConfig*appCfg = AppConfig::getInstance();
	//setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
	setFixedWidth(648);
	setFixedHeight(appCfg->height + 50);

	mainVLayout = new QVBoxLayout(this);

	imgLabel = new QLabel;
	imgLabel->setFixedSize(QSize(648, appCfg->height));
	backBtn = new QPushButton(QStringLiteral("·µ»Ø"));
	backBtn->setFixedSize(QSize(80, 30));
	mainVLayout->addWidget(imgLabel);
	mainVLayout->addWidget(backBtn, Qt::AlignCenter);

	connect(backBtn, SIGNAL(clicked()), this, SLOT(imgLabelBack()));
}
void imgLabelShowWidget::imgLabelBack() {
	this->hide();
	emit imgLabelShowBack();
}

imgLabelShowWidget::~imgLabelShowWidget()
{
}
