#pragma once

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPoint>
#include <QAction>
#include <QMenu>
#include <QImage>

extern int appLayoutType;
//图像操作组件
class mQLabel : public QLabel
{
	Q_OBJECT

public:
	mQLabel(QWidget *parent = 0);
	~mQLabel();
	
	void paintEvent(QPaintEvent* );
	virtual void mousePressEvent(QMouseEvent* );   //重写鼠标按下 和 移动事件
	virtual void mouseMoveEvent(QMouseEvent*);
	virtual void mouseReleaseEvent(QMouseEvent*);


	void clear();									//清理绘图界面
	QPoint startPos;   //开始位置
	QPoint endPos;		//结束位置

	int selectAreaNum = 1;   //选择区域的编号

	bool isAreaOperation = false;		//是否进行区域选择操作
	int appType = appLayoutType;
	
public slots:
	//选区之后的 槽函数
	void selectArea();
	void cancelArea();

	//右键槽函数
	void  showOntime();
	void  cameraAdjust();
	void cancel();

	//控制区域操作槽函数
	void controlAreaOperation();
	void reduceSelectAreaNum();

signals:
	void imgPatchInfo(QRect& );
	void showTypeEdit(int);
	void showLargeLabel();
private:

};
