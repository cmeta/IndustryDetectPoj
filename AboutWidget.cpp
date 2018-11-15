#include "AboutWidget.h"
#include<qimage.h>
AboutWidget::AboutWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setFixedSize(QSize(600, 500));
	aboutLabel = new QLabel();
	aboutLabel->setFixedSize(QSize(550, 450));
	QImage imgAbout("icon//about.png");
	aboutLabel->setPixmap(QPixmap::fromImage(imgAbout).scaled(550, 450, Qt::KeepAspectRatio, Qt::SmoothTransformation));

	correctBtn = new QPushButton(QStringLiteral("校正图像"));
	correctBtn->setFixedSize(QSize(80, 30));
	backBtn = new QPushButton(QStringLiteral("返回"));
	backBtn->setFixedSize(QSize(80, 30));
	mainLayout = new QVBoxLayout(this);
	btnLayout = new QHBoxLayout();
	btnLayout->addWidget(correctBtn);
	btnLayout->addWidget(backBtn);
	mainLayout->addWidget(aboutLabel);
	mainLayout->addLayout(btnLayout);

	connect(backBtn, SIGNAL(clicked()), this, SLOT(backBtnClicked()));
	connect(correctBtn, SIGNAL(clicked()), this, SLOT(correctBtnClicked()));

}
void AboutWidget::backBtnClicked() {
	this->hide();
}

AboutWidget::~AboutWidget()
{
}

void AboutWidget::correctBtnClicked()
{
	this->hide();
	emit correctSignals();
}
