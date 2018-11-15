#include "mQLabel.h"
#include"qdebug.h"
mQLabel::mQLabel(QWidget *parent)
	: QLabel(parent)
{
	setMouseTracking(false);  //不追踪鼠标事件 即按键才会触发鼠标移动事
	//qDebug() << appType;
}

mQLabel::~mQLabel()
{

}

void mQLabel::paintEvent(QPaintEvent *event)
{
	QLabel::paintEvent(event);
	QPainter painter(this);
	
	QPen pen;
	pen.setColor(Qt::red);
	pen.setWidth(2);
	pen.setStyle(Qt::DotLine);
	painter.setPen(pen);

	painter.drawRect(QRect(startPos.x(), startPos.y(), endPos.x()- startPos.x(), endPos.y() - startPos.y()));
	
	/*----------------画左上角编号---------------------------*/
	//绘制矩形填充区域
	QPainter painter1(this);
	painter1.setBrush(QColor(0, 0, 0));
	painter1.drawRect(startPos.x(), startPos.y() - 15, (endPos.x() - startPos.x()) / 9, 15);

	// 设置画笔颜色
	painter.setPen(QColor(255, 255, 240));

	// 设置字体：微软雅黑、点大小、斜体
	QFont font;
	font.setFamily("Microsoft YaHei");
	font.setPointSize(8);
	//font.setItalic(true);
	painter.setFont(font);

	// 绘制文本
	QString s = QString::number(selectAreaNum, 10) /*+ "_" + QString::number(endPos.x(), 10) + ","+QString::number(endPos.y(), 10)*/;

	painter.drawText(QRect(startPos.x(), startPos.y()-15, (endPos.x() - startPos.x())/10, 15) ,Qt::AlignHCenter, s);

}

void mQLabel::mousePressEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton && isAreaOperation)
	{
		startPos = e->pos();
	}

	if (e->button() == Qt::RightButton)
	{
		QAction showOntimeItem(QStringLiteral("1:1 显示"), this);
		QAction cameraAdjustItem(QStringLiteral("相机调节"), this);
		QAction cancelItem(QStringLiteral("取消"), this);

		connect(&showOntimeItem, SIGNAL(triggered()), this, SLOT(showOntime()));
		connect(&cameraAdjustItem, SIGNAL(triggered()), this, SLOT(cameraAdjust()));
		connect(&cancelItem, SIGNAL(triggered()), this, SLOT(cancel()));

		QMenu menu1(this);
		menu1.addAction(&showOntimeItem);
		menu1.addAction(&cameraAdjustItem);
		menu1.addAction(&cancelItem);

		menu1.exec(QCursor::pos());
	}
	
}

void mQLabel::mouseMoveEvent(QMouseEvent *e)
{
	//QLabel::paintEvent(e);
	//QPainter *painter = new QPainter(this);
	
	//painter->drawRect(QRect(startPos.x(), startPos.y(), 100, 100));
	if (isAreaOperation)
	{
		endPos = e->pos();

		this->update();
	}
	

}

void mQLabel::mouseReleaseEvent(QMouseEvent *)
{
	if (isAreaOperation)
	{
		QAction selectItem(QStringLiteral("确定选区"), this);
		QAction cancelItem(QStringLiteral("取消选区"), this);

		connect(&selectItem, SIGNAL(triggered()), this, SLOT(selectArea()));
		connect(&cancelItem, SIGNAL(triggered()), this, SLOT(cancelArea()));

		QMenu menu(this);
		menu.addAction(&selectItem);

		menu.addAction(&cancelItem);

		menu.exec(QCursor::pos());
	}

}

void mQLabel::clear()
{
	startPos.setX(0);
	startPos.setY(0);
	endPos.setX(0);
	endPos.setY(0);

	update();
}

void mQLabel::cancelArea()
{
	clear();
}
void mQLabel::showOntime()
{
	emit showLargeLabel();

}
void mQLabel::cameraAdjust()
{
	return;
}
void mQLabel::cancel()
{
	return;
}
void mQLabel::controlAreaOperation()
{

	isAreaOperation = !isAreaOperation;
	
}
void mQLabel::reduceSelectAreaNum()
{
	if (selectAreaNum >= 2)
	{
		selectAreaNum--;
	}
	
}
void mQLabel::selectArea()
{
	//QImage imgTest = QImage("test7.jpg");
	//QImage img = imgTest.copy(startPos.x(), startPos.y(), endPos.x() - startPos.x(), endPos.y() - startPos.y());

	//img.save("imgTest//cutImg.jpg");
	//QRect rect = QRect(startPos.x(), startPos.y(), endPos.x() - startPos.x(), endPos.y() - startPos.y());
	emit imgPatchInfo(QRect(startPos.x()*appType, startPos.y()*appType, (endPos.x() - startPos.x())*appType, (endPos.y() - startPos.y())*appType));
	emit showTypeEdit(selectAreaNum - 1);
	selectAreaNum++;     //选择区域数量加一
	//clear();  //清空画布
}
