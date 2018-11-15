#include "DataShowWidget.h"

DataShowWidget::DataShowWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	if (AppConfig::getInstance()->detectNum > 4) {
		detectNum = 4;
	}
	else {
		detectNum = AppConfig::getInstance()->detectNum;
	}

	goodDataTable = new QTableWidget(0, detectNum + 2);
	badDataTable = new QTableWidget(0, detectNum + 2);
	dataShowVlayout = new QVBoxLayout(this);

	QStringList header;
	for (int i = 0; i < detectNum; i++) {
		header << QStringLiteral("合格/检验结果");
	}	

	header<< QStringLiteral("状态")
		  << QStringLiteral("时间");

	goodDataTable->setHorizontalHeaderLabels(header);
	for (size_t i = 0; i < detectNum; i++)
	{
		goodDataTable->setColumnWidth(i, 250);
	}

	goodDataTable->setColumnWidth(detectNum, 90);
	goodDataTable->setColumnWidth(detectNum + 1, 120);
	goodDataTable->setMaximumHeight(480);
	goodDataTable->setMinimumHeight(280);

	goodDataTable->setMaximumWidth(1250);
	//goodDataTable->sethe
	//goodDataTable->setclo
	//goodDataTable->setWindowTitle(QString("合格数据预览"));
	goodDataTable->setFont(QFont("Consolas"));
	//goodDataTable->setShowGrid(true); //设置不显示格子线
	goodDataTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	goodDataTable->horizontalHeader()->setStretchLastSection(true); //设置充满表宽度

	goodDataTable->verticalScrollBar()->setStyleSheet("QScrollBar:vertical"
		"{"
		"width:8px;"
		"background:rgba(0,0,0,0%);"
		"margin:0px,0px,0px,0px;"
		"padding-top:9px;"
		"padding-bottom:9px;"
		"}"
		"QScrollBar::handle:vertical"
		"{"
		"width:8px;"
		"background:rgba(0,0,0,25%);"
		" border-radius:4px;"
		"min-height:20;"
		"}"
		"QScrollBar::handle:vertical:hover"
		"{"
		"width:8px;"
		"background:rgba(0,0,0,50%);"
		" border-radius:4px;"
		"min-height:20;"
		"}"
		"QScrollBar::add-line:vertical"
		"{"
		"height:9px;width:8px;"
		"border-image:url(:/images/a/3.png);"
		"subcontrol-position:bottom;"
		"}"
		"QScrollBar::sub-line:vertical"
		"{"
		"height:9px;width:8px;"
		"border-image:url(:/images/a/1.png);"
		"subcontrol-position:top;"
		"}"
		"QScrollBar::add-line:vertical:hover"
		"{"
		"height:9px;width:8px;"
		"border-image:url(:/images/a/4.png);"
		"subcontrol-position:bottom;"
		"}"
		"QScrollBar::sub-line:vertical:hover"
		"{"
		"height:9px;width:8px;"
		"border-image:url(:/images/a/2.png);"
		"subcontrol-position:top;"
		"}"
		"QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical"
		"{"
		"background:rgba(0,0,0,10%);"
		"border-radius:4px;"
		"}"
	);


	goodDataTable->setAlternatingRowColors(true);
	goodDataTable->horizontalHeader()->setFixedHeight(25);
	goodDataTable->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(79,129,189);font:14px 'Microsoft YaHei'; font: bold;color: white;}");   //border:none;
	// set header bg color, horizontalscroll vericalscrollbar style
	//goodDataTable->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}");


	goodDataTable->setStyleSheet(
		"QTableWidget{background: rgb(233,237,244);alternate-background-color:rgb(208,216,232);}");
	//goodDataTable->setStyleSheet("background: rgb(233,237,244);alternate-background-color:rgb(48,51,55);selection-background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgb(56,56,56),stop:1 rgb(76,76,76));"); //设置选中背景色
	// no highlight to header
	goodDataTable->setSelectionBehavior(QAbstractItemView::SelectRows); //设置选择行为，以行为单位
	goodDataTable->setSelectionMode(QAbstractItemView::SingleSelection); //设置选择模式，选择单行

	//不合格数据表格构造
	header.clear();
	for (int i = 0; i < detectNum; i++) {
		header << QStringLiteral("不合格/检验结果");
	}

	header << QStringLiteral("状态")
		<< QStringLiteral("时间");

	badDataTable->setHorizontalHeaderLabels(header);
	badDataTable->setMaximumHeight(250);
	badDataTable->setMaximumWidth(1250);

	for (size_t i = 0; i < detectNum; i++)
	{
		badDataTable->setColumnWidth(i, 200);
	}

	badDataTable->setColumnWidth(detectNum, 90);
	badDataTable->setColumnWidth(detectNum + 1, 120);

	badDataTable->setFont(QFont("Consolas"));

	//badDataTable->horizontalScrollBar();
	badDataTable->setAlternatingRowColors(true);
	badDataTable->horizontalHeader()->setStretchLastSection(true); //设置充满表宽度
	badDataTable->horizontalHeader()->setFixedHeight(25);
	badDataTable->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(192,80,77);font:14px 'Microsoft YaHei'; font: bold;color: white;}");
	// set header bg color, horizontalscroll vericalscrollbar style
	//goodDataTable->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}");
	badDataTable->verticalScrollBar()->setStyleSheet(
		"QTableWidget{background: rgb(244,233,233);alternate-background-color:rgb(232,208,208);}");
	//badDataTable->setAlternatingRowColors(true);
	badDataTable->setSelectionBehavior(QAbstractItemView::SelectRows); //设置选择行为，以行为单位
	badDataTable->setSelectionMode(QAbstractItemView::SingleSelection); //设置选择模式，选择单行


	badDataTable->verticalScrollBar()->setStyleSheet("QScrollBar:vertical"
		"{"
		"width:8px;"
		"background:rgba(0,0,0,0%);"
		"margin:0px,0px,0px,0px;"
		"padding-top:9px;"
		"padding-bottom:9px;"
		"}"
		"QScrollBar::handle:vertical"
		"{"
		"width:8px;"
		"background:rgba(0,0,0,25%);"
		" border-radius:4px;"
		"min-height:20;"
		"}"
		"QScrollBar::handle:vertical:hover"
		"{"
		"width:8px;"
		"background:rgba(0,0,0,50%);"
		" border-radius:4px;"
		"min-height:20;"
		"}"
		"QScrollBar::add-line:vertical"
		"{"
		"height:9px;width:8px;"
		"border-image:url(:/images/a/3.png);"
		"subcontrol-position:bottom;"
		"}"
		"QScrollBar::sub-line:vertical"
		"{"
		"height:9px;width:8px;"
		"border-image:url(:/images/a/1.png);"
		"subcontrol-position:top;"
		"}"
		"QScrollBar::add-line:vertical:hover"
		"{"
		"height:9px;width:8px;"
		"border-image:url(:/images/a/4.png);"
		"subcontrol-position:bottom;"
		"}"
		"QScrollBar::sub-line:vertical:hover"
		"{"
		"height:9px;width:8px;"
		"border-image:url(:/images/a/2.png);"
		"subcontrol-position:top;"
		"}"
		"QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical"
		"{"
		"background:rgba(0,0,0,10%);"
		"border-radius:4px;"
		"}"
	);

	dataShowVlayout->addWidget(goodDataTable);
	dataShowVlayout->addWidget(badDataTable);
	dataShowVlayout->setContentsMargins(10, 10, 0, 0);
	//根据内容调整列
	//goodDataTable->resizeColumnToContents(0);
	//goodDataTable->resizeColumnToContents(1);

	//右键菜单实现
	badDataTable->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	goodDataTable->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);

	connect(goodDataTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(popMenuGood(QPoint)));
	connect(badDataTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(popMenuBad(QPoint)));
}

DataShowWidget::~DataShowWidget()
{

}



void DataShowWidget::updateBadDataList(DataInfo &dataInfo)
{
	badDataTable->scrollToBottom();

	int rNum = dataInfo.resultNum;

	QString qstr;
	badDataTable->insertRow(badDataTable->rowCount());
	Qt::ItemFlags flag = (Qt::ItemIsSelectable) | (Qt::ItemIsEnabled);
	int row = badDataTable->rowCount() - 1;
	badDataTable->setRowHeight(row, 20);

	//插入结果
	for (int i = 0; i < rNum; i++) {
		qstr = dataInfo.reResult[i];
		badDataTable->setItem(row, i, new QTableWidgetItem(qstr));
		badDataTable->item(row, i)->setTextAlignment(Qt::AlignCenter);
		badDataTable->item(row, i)->setFlags(flag);
	}
	//显示最后一行
		//if (row - 1 > 0)
		//	goodDataTable->scrollToItem(goodDataTable->item(row - 1, 0), QAbstractItemView::PositionAtBottom);
	
	badDataTable->setItem(row, detectNum, new QTableWidgetItem("0"));

	QDateTime time = QDateTime::currentDateTime();
	badDataTable->setItem(row, detectNum +1, new QTableWidgetItem(time.toString("yyyy-MM-dd_hh-mm-ss")));
}

void DataShowWidget::popMenuGood(QPoint point)
{
	QTableWidgetItem* curItem = goodDataTable->currentItem();  //获取当前被点击的节点
	if (curItem == NULL)return;   //这种情况是右键的位置不在treeItem的范围内，即在空白位置右击

	//RowNum = curItem->row();
	
	QAction clearListItem(QStringLiteral("清除所有记录"), this);
	
	connect(&clearListItem, SIGNAL(triggered()), this, SLOT(clearGoodList()));
	
	//connect(&createGZHFileItem, SIGNAL(triggered()), this, SLOT(createGZHFilemark()));

	QMenu menu(goodDataTable);
	
	menu.addAction(&clearListItem);
	
	//menu.addAction(&createGZHFileItem);
	menu.exec(QCursor::pos());  //在当前鼠标位置显示
}

void DataShowWidget::popMenuBad(QPoint point)
{
	QTableWidgetItem* curItem = badDataTable->currentItem();  //获取当前被点击的节点
	if (curItem == NULL)return;   //这种情况是右键的位置不在treeItem的范围内，即在空白位置右击

	//RowNum = curItem->row();

	QAction clearListItem(QStringLiteral("清除所有记录"), this);

	connect(&clearListItem, SIGNAL(triggered()), this, SLOT(clearBadList()));

	//connect(&createGZHFileItem, SIGNAL(triggered()), this, SLOT(createGZHFilemark()));

	QMenu menu(badDataTable);

	menu.addAction(&clearListItem);

	//menu.addAction(&createGZHFileItem);
	menu.exec(QCursor::pos());  //在当前鼠标位置显示

}

void DataShowWidget::clearGoodList()
{
	for (int i = 0; i < goodDataTable->rowCount(); i++) {
		for (int j = 0; j < goodDataTable->columnCount(); j++)
			delete goodDataTable->item(i, j);
	}
	goodDataTable->reset();
	goodDataTable->setRowCount(0);
}

void DataShowWidget::clearBadList()
{
	for (int i = 0; i < badDataTable->rowCount(); i++) {
		for (int j = 0; j < badDataTable->columnCount(); j++)
			delete badDataTable->item(i, j);
	}
	badDataTable->reset();
	badDataTable->setRowCount(0);
}


void DataShowWidget::updateList(DataInfo& dataInfo)
{

	goodDataTable->scrollToBottom();

	int rNum = dataInfo.resultNum;
	QString qstr;
	goodDataTable->insertRow(goodDataTable->rowCount());
	Qt::ItemFlags flag = (Qt::ItemIsSelectable) | (Qt::ItemIsEnabled);
	int row = goodDataTable->rowCount() - 1;
	goodDataTable->setRowHeight(row, 20);

	//插入结果
	for (int i = 0; i < rNum; i++) {
		qstr = dataInfo.reResult[i];
		goodDataTable->setItem(row, i, new QTableWidgetItem(qstr));
		goodDataTable->item(row, i)->setTextAlignment(Qt::AlignCenter);
		goodDataTable->item(row, i)->setFlags(flag);
	}

	if (dataInfo.state == false) {
		goodDataTable->setItem(row, detectNum, new QTableWidgetItem("0"));
		for (int i = 0; i < rNum; i++) {
			goodDataTable->item(row, i)->setBackgroundColor(QColor(240, 240, 30));
		}
		goodDataTable->item(row, detectNum)->setBackgroundColor(QColor(240, 240, 30));
	}
	else {
		goodDataTable->setItem(row, detectNum, new QTableWidgetItem("1"));
	}

	QDateTime time = QDateTime::currentDateTime();
	goodDataTable->setItem(row, detectNum + 1, new QTableWidgetItem(time.toString("yyyy-MM-dd_hh-mm-ss")));

	//goodDataTable->item(row, 2)->setText(QStringLiteral("1"));

	//显示最后一行
		//if (row - 1 > 0)
		//	goodDataTable->scrollToItem(goodDataTable->item(row - 1, 0), QAbstractItemView::PositionAtBottom);

}
