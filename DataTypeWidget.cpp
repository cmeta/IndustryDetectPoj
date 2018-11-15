#include "DataTypeWidget.h"
#include<string>
#include"qdebug.h"
#include"qdatetime.h"
extern int appLayoutType;
DataTypeWidget::DataTypeWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setMaximumHeight(300);
	setMaximumWidth(1250);
	//setFixedSize(648,180);
	//数据类型选择
	dataTypeTable = new QTableWidget;

	dataTypeTable->setColumnCount(7);
	dataTypeTable->setMaximumHeight(200);
	dataTypeTable->setMinimumHeight(90);

	QStringList header;
	header << QStringLiteral("数据类型") << QStringLiteral("角度")
		<< QStringLiteral("字符个数") << QStringLiteral("行数")
		<< QStringLiteral("比对方式") << QStringLiteral("位置信息")
		<< QStringLiteral("增量/固定内容/时间差/数据库类型");
	dataTypeTable->setHorizontalHeaderLabels(header);

	//设置宽度
	for (int i = 0; i < 4; i++) {
		dataTypeTable->setColumnWidth(i, 65);
	}
	dataTypeTable->setColumnWidth(6, 180);
	dataTypeTable->setColumnWidth(5, 120);
	dataTypeTable->setColumnWidth(7, 100);
	dataTypeTable->setColumnWidth(4, 100);

	dataTypeTable->setFont(QFont("Consolas"));
	dataTypeTable->horizontalHeader()->setStretchLastSection(true); //设置充满表宽度
	dataTypeTable->setAlternatingRowColors(true);
	dataTypeTable->horizontalHeader()->setFixedHeight(25);
	dataTypeTable->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(128,100,162);font:14px 'Microsoft YaHei'; font: bold;color: white;}");
	// set header bg color, horizontalscroll vericalscrollbar style
	//goodDataTable->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}");
	dataTypeTable->setStyleSheet(
		"QTableWidget{background: rgb(216,211,224);alternate-background-color:rgb(237,234,240);}");

	dataTypeTable->verticalScrollBar()->setStyleSheet("QScrollBar:vertical"
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
	//右键datatypeTable编辑参数
	dataTypeTable->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);

	connect(dataTypeTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(popMenuEdit(QPoint)));


	//dataTypeTable->setSelectionBehavior(QAbstractItemView::SelectRows); //设置选择行为，以行为单位
	//dataTypeTable->setSelectionMode(QAbstractItemView::SingleSelection); //设置选择模式，选择单行

	//数据类型操作
	areaOperationBtn = new QToolButton;
	//addAreaBtn = new QToolButton;
	reduceAreaBtn = new QToolButton;
	saveDataTypeBtn = new QToolButton;
	loadDatatypeBtn = new QToolButton;

	areaOperationBtn->setText(QStringLiteral("开始区域操作"));
	areaOperationBtn->setIconSize(QSize(48, 48));
	areaOperationBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	areaOperationBtn->setIcon(QIcon("icon/6.ico"));

	areaOperationBtn->setFixedSize(80, 70);

	areaOperationBtn->setStyleSheet("QToolButton{background-color:transparent; font:12px 'Microsoft YaHei'; font: bold;  border:none; }"
		"QToolButton:hover{background-color:rgb(200,214,204);}");


	/*addAreaBtn->setText(QStringLiteral("增加区域"));
	addAreaBtn->setIconSize(QSize(48, 48));
	addAreaBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	addAreaBtn->setIcon(QIcon("icon/3.ico"));

	addAreaBtn->setFixedSize(80, 70);

	addAreaBtn->setStyleSheet("QToolButton{background-color:transparent; font:12px 'Microsoft YaHei'; font: bold;  border:none; }"
		"QToolButton:hover{background-color:rgb(200,214,204);}");*/

	reduceAreaBtn->setText(QStringLiteral("减少区域"));
	reduceAreaBtn->setIconSize(QSize(48, 48));
	reduceAreaBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	reduceAreaBtn->setIcon(QIcon("icon/3.ico"));

	reduceAreaBtn->setFixedSize(80, 70);

	reduceAreaBtn->setStyleSheet("QToolButton{background-color:transparent; font:12px 'Microsoft YaHei'; font: bold;  border:none; }"
		"QToolButton:hover{background-color:rgb(200,214,204);}");


	loadDatatypeBtn->setText(QStringLiteral("加载类型配置"));

	loadDatatypeBtn->setIconSize(QSize(48, 48));
	loadDatatypeBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	loadDatatypeBtn->setIcon(QIcon("icon/loadType.ico"));

	loadDatatypeBtn->setFixedSize(80, 70);

	loadDatatypeBtn->setStyleSheet("QToolButton{background-color:transparent; font:12px 'Microsoft YaHei'; font: bold;  border:none; }"
		"QToolButton:hover{background-color:rgb(200,214,204);}");

	saveDataTypeBtn->setText(QStringLiteral("保存类型配置"));
	saveDataTypeBtn->setIconSize(QSize(48, 48));
	saveDataTypeBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	saveDataTypeBtn->setIcon(QIcon("icon/saveType.ico"));

	saveDataTypeBtn->setFixedSize(80, 70);

	saveDataTypeBtn->setStyleSheet("QToolButton{background-color:transparent; font:12px 'Microsoft YaHei'; font: bold;  border:none; }"
		"QToolButton:hover{background-color:rgb(200,214,204);}");


	btnGridLayout = new QGridLayout;
	btnGridLayout->addWidget(areaOperationBtn, 0, 0);
	btnGridLayout->addWidget(reduceAreaBtn, 0, 1);
	btnGridLayout->addWidget(saveDataTypeBtn, 0, 2);
	btnGridLayout->addWidget(loadDatatypeBtn, 0, 3);

	mianvLayout = new QVBoxLayout(this);
	mianvLayout->addWidget(dataTypeTable);
	mianvLayout->addLayout(btnGridLayout);
	mianvLayout->setMargin(0);
	mianvLayout->setContentsMargins(10, 10, 0, 0);

	//connect(addAreaBtn, SIGNAL(clicked()), this, SLOT(addArea()));
	connect(reduceAreaBtn, SIGNAL(clicked()), this, SLOT(reduceArea()));
	connect(areaOperationBtn, SIGNAL(clicked()), this, SLOT(emitAreaOperation()));
	connect(saveDataTypeBtn, SIGNAL(clicked()), this, SLOT(exportDataType()));
	connect(loadDatatypeBtn, SIGNAL(clicked()), this, SLOT(importDataType()));

	//connect(reduceAreaBtn, SIGNAL(clicked()), this, SLOT(reduceArea()));

}

DataTypeWidget::~DataTypeWidget()
{

}

int DataTypeWidget::getTableRow()
{
	return dataTypeTable->rowCount();
}

int DataTypeWidget::getTableColumn()
{
	return dataTypeTable->columnCount();
}

void DataTypeWidget::reduceArea()
{
	//减少区域
	int rowIndex = dataTypeTable->currentRow();
	if (rowIndex != -1)
		dataTypeTable->removeRow(rowIndex);
	emit reduceAreaSig();
}
void DataTypeWidget::emitAreaOperation()
{
	if (buttonChange)
	{
		areaOperationBtn->setText(QStringLiteral("结束区域操作"));
		areaOperationBtn->setIcon(QIcon("icon/7.ico"));
		buttonChange = !buttonChange;
	}
	else
	{
		areaOperationBtn->setText(QStringLiteral("开始区域操作"));
		areaOperationBtn->setIcon(QIcon("icon/6.ico"));
		buttonChange = !buttonChange;
	}
	emit areaOperation();
}

void DataTypeWidget::exportDataType()
{
	QTextCodec *code = QTextCodec::codecForName("GB2312");//解决中文路径问题
	QString s = QFileDialog::getSaveFileName(this, "Save File", "./Config", "");
	QByteArray   ByteArray1 = code->fromUnicode(s);

	if (s.isNull())
	{
		return;
		//点的是取消
	}

	int row = getTableRow();
	int column = getTableColumn();

	if (row == 0) {
		return;
	}
	/***********************************获取数据信息*******************************/
	std::ofstream fout(ByteArray1.data(), std::ios::out);
	//保存行数
	fout << row << std::endl;
	fout << column << std::endl;

	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < column; j++)
		{
			if (j == 0)
			{
				QWidget * widget = dataTypeTable->cellWidget(i, j);//获得widget
				QComboBox *combox = (QComboBox*)widget;//强制转化为QComboBox
				QString string = combox->currentText();
				if (string == QStringLiteral("条码")) {
					fout << 1 << std::endl;
				}
				if (string == QStringLiteral("点阵数字")) {
					fout << 2 << std::endl;
				}
				if (string == QStringLiteral("汉字")) {
					fout << 3 << std::endl;
				}
				if (string == QStringLiteral("字母")) {
					fout << 4 << std::endl;
				}
				if (string == QStringLiteral("实体数字")) {
					fout << 5 << std::endl;
				}	
				if (string == QStringLiteral("字母数字")) {
					fout << 6 << std::endl;
				}
			}
			if (j == 1)
			{
				QWidget * widget = dataTypeTable->cellWidget(i, j);//获得widget
				QComboBox *combox = (QComboBox*)widget;//强制转化为QComboBox
				QString string = combox->currentText();
				fout << string.toInt() << std::endl;
			}

			if (j == 2)
			{
				QString str = dataTypeTable->item(i, j)->text();
				if (str == NULL)
				{
					fout << 0 << std::endl;
				}
				else
				{
					fout << str.toStdString() << std::endl;
				}

			}
			//行数
			if (j == 3) {
				QString str = dataTypeTable->item(i, j)->text();
				if (str == NULL)
				{
					fout << 0 << std::endl;
				}
				else
				{
					fout << str.toInt() << std::endl;
				}
			}
			//对比方式
			if (j == 4) {
				QWidget * widget = dataTypeTable->cellWidget(i, j);//获得widget
				QComboBox *combox = (QComboBox*)widget;//强制转化为QComboBox
				QString string = combox->currentText();

				fout << string.toStdString() << std::endl;
			}
			//位置信息
			if (j == 5)
			{
				QString str = dataTypeTable->item(i, j)->text();
				//QList<QString>list;
				//list = str.split(',');
				fout << str.toStdString() << std::endl;
			}
			//
			if (j == 6)
			{
				QString str = dataTypeTable->item(i, j)->text();
				//QList<QString>list;
				//list = str.split(',');
				fout << str.toStdString() << std::endl;
			}
		}
	}

	//1取当前时间
	QDateTime currentDateTime = QDateTime::currentDateTime();
	QString rootPath = "Config//BasicImgDataBase//";
	QString basicImgPath = rootPath + currentDateTime.toString("yyyy-MM-dd_hh-mm-ss") + ".bmp";
	fout << basicImgPath.toStdString() << std::endl;

	emit saveCurrentBasicImg(basicImgPath);

	QMessageBox::information(NULL, "Infomation", QStringLiteral("导出成功！"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

}

void DataTypeWidget::importDataType()
{

	QTextCodec *code = QTextCodec::codecForName("GB2312");//解决中文路径问题

	QString s = QFileDialog::getOpenFileName(this, "Open File", "./Config", "");

	QByteArray   ByteArray = code->fromUnicode(s);

	if (s.isNull())
	{
		return;
		//点的是取消
	}

	int _row, _col = 0;
	std::ifstream fin(ByteArray.data());

	fin >> _row;
	fin >> _col;

	//插入行数
	for (int i = 0; i < _row; i++) {
		QString str;
		dataTypeTable->insertRow(dataTypeTable->rowCount());
		//	Qt::ItemFlags flag = (Qt::ItemIsSelectable) | (Qt::ItemIsEnabled);

			//设置行高度
		int row = dataTypeTable->rowCount() - 1;
		dataTypeTable->setRowHeight(row, 20);
		//类型
		QComboBox *combox1 = new QComboBox();
		combox1->addItem(QStringLiteral("条码"));// 可以插入icon
		combox1->addItem(QStringLiteral("点阵数字"));
		combox1->addItem(QStringLiteral("实体数字"));
		combox1->addItem(QStringLiteral("汉字"));
		combox1->addItem(QStringLiteral("字母"));
		combox1->addItem(QStringLiteral("字母数字"));
		dataTypeTable->setCellWidget(row, 0, (QWidget*)combox1);

		//QWidget * widget = ui.tableWidget_NetGate->cellWidget(currentrow, 4);//获得widget
		//QComboBox *combox = (QComboBox*)widget;//强制转化为QComboBox
		//combox->setEnabled(true);
		//QString string = combox->currentText();

		//角度
		QComboBox *combox2 = new QComboBox();
		combox2->addItem(QStringLiteral("0"));		// 可以插入icon
		combox2->addItem(QStringLiteral("90"));
		combox2->addItem(QStringLiteral("180"));
		combox2->addItem(QStringLiteral("270"));
		dataTypeTable->setCellWidget(row, 1, (QWidget*)combox2);
		//字符个数
		dataTypeTable->setItem(row, 2, new QTableWidgetItem(""));
		//行数
		dataTypeTable->setItem(row, 3, new QTableWidgetItem(""));
		//字符个数
		//dataTypeTable->item(row, 3)->setTextAlignment(Qt::AlignCenter);
		//模板文件

		//对比方式
		QComboBox *combox3 = new QComboBox();
		combox3->addItem(QStringLiteral("数据库比对"));// 可以插入icon
		combox3->addItem(QStringLiteral("流水号比对"));
		combox3->addItem(QStringLiteral("时间比对"));
		combox3->addItem(QStringLiteral("固定内容比对"));

		dataTypeTable->setCellWidget(row, 4, (QWidget*)combox3);

		dataTypeTable->setItem(row, 5, new QTableWidgetItem(""));
		dataTypeTable->setItem(row, 6, new QTableWidgetItem(""));
	}

	for (int i = 0; i < _row; i++)
	{
		for (int j = 0; j < _col; j++)
		{
			if (j == 0)
			{
				QWidget * widget = dataTypeTable->cellWidget(i, j);//获得widget
				QComboBox *combox = (QComboBox*)widget;//强制转化为QComboBox
				int flag = 0;
				fin >> flag;
				//QString string = combox->currentText();
				if (flag == 1) {
					combox->setCurrentText(QStringLiteral("条码"));
				}
				if (flag == 2) {
					combox->setCurrentText(QStringLiteral("点阵数字"));
				}
				if (flag == 3) {
					combox->setCurrentText(QStringLiteral("汉字"));
				}
				if (flag == 4) {
					combox->setCurrentText(QStringLiteral("字母"));
				}
				if (flag == 5) {
					combox->setCurrentText(QStringLiteral("实体数字"));
				}
				if (flag == 6) {
					combox->setCurrentText(QStringLiteral("字母数字"));
				}
			}

			if (j == 1)
			{
				QWidget * widget = dataTypeTable->cellWidget(i, j);//获得widget
				QComboBox *combox = (QComboBox*)widget;//强制转化为QComboBox
				std::string s;
				fin >> s;
				QString str = QString::fromStdString(s);
				combox->setCurrentText(str);
			}

			if (j == 2)
			{
				std::string charactorNum;
				fin >> charactorNum;
				dataTypeTable->item(i, j)->setText(QString::fromStdString(charactorNum));

			}
			//行数
			if (j == 3) {
				std::string rowCount;
				fin >> rowCount;
				dataTypeTable->item(i, j)->setText(QString::fromStdString(rowCount));

			}

			if (j == 4) {
				std::string compareWay;
				fin >> compareWay;

				QWidget * widget = dataTypeTable->cellWidget(i, j);//获得widget
				QComboBox *combox = (QComboBox*)widget;//强制转化为QComboBox

				combox->setCurrentText(QString::fromStdString(compareWay));

				//QString string = combox->currentText();

				//fout << string.toStdString() << std::endl;
			}
			if (j == 5)
			{
				std::string rect1;
				fin >> rect1;
				dataTypeTable->item(i, j)->setText(QString::fromStdString(rect1));

			}
			if (j == 6)
			{
				std::string way;
				fin >> way;
				dataTypeTable->item(i, j)->setText(QString::fromStdString(way));

			}
		}
	}

	std::string  path;
	fin >> path;
	emit setCurrentBasicImg(QString::fromStdString(path));
	//ROI区域位置

	//dataTypeTable->setItem(row, 5, new QTableWidgetItem(qstr));
	//dataTypeTable->item(row, 5)->setTextAlignment(Qt::AlignCenter);
	//dataTypeTable->item(row, 5)->setFlags(flag);


}

void DataTypeWidget::popMenuEdit(QPoint point)
{
	QTableWidgetItem* curItem = dataTypeTable->currentItem();  //获取当前被点击的节点
	if (curItem == NULL)return;   //这种情况是右键的位置不在treeItem的范围内，即在空白位置右击
	//colNum = dataTypeTable->columnCount() - 1;

	rowNum = curItem->row();

	QAction editListItem(QStringLiteral("编辑此行属性"), this);

	connect(&editListItem, SIGNAL(triggered()), this, SLOT(editList()));

	//connect(&createGZHFileItem, SIGNAL(triggered()), this, SLOT(createGZHFilemark()));

	QMenu menu(dataTypeTable);

	menu.addAction(&editListItem);

	//menu.addAction(&createGZHFileItem);
	menu.exec(QCursor::pos());  //在当前鼠标位置显示
}

void DataTypeWidget::editList()
{
	emit showTypeEditWidget(rowNum);
}

void DataTypeWidget::updateRow(EditDatatype &edt)
{


	//qDebug() << edt.compareType;
	for (int j = 0; j < 8; j++)
	{
		if (j == 0)
		{
			QWidget * widget = dataTypeTable->cellWidget(rowNum, j);//获得widget
			QComboBox *combox = (QComboBox*)widget;//强制转化为QComboBox
			combox->setCurrentText(edt.type);
		}

		if (j == 1)
		{
			QWidget * widget = dataTypeTable->cellWidget(rowNum, j);//获得widget
			QComboBox *combox = (QComboBox*)widget;//强制转化为QComboBox
			combox->setCurrentText(edt.angle);
		}

		if (j == 2)
		{
			dataTypeTable->item(rowNum, j)->setText(edt.charNum);

		}
		//行数
		if (j == 3) {
			dataTypeTable->item(rowNum, j)->setText(edt.rowNum);
		}

	
		if (j == 6)
		{
			dataTypeTable->item(rowNum, j)->setText(edt.compareConf);

		}
		if (j == 4) {
			QWidget * widget1 = dataTypeTable->cellWidget(rowNum, j);//获得widget
			QComboBox *combox1 = (QComboBox*)widget1;//强制转化为QComboBox
			QString s = edt.compareType;
			combox1->setCurrentText(s);
		}
	}

}

void DataTypeWidget::addArea(QRect& rect)
{
	QString str;
	dataTypeTable->insertRow(dataTypeTable->rowCount());
	//	Qt::ItemFlags flag = (Qt::ItemIsSelectable) | (Qt::ItemIsEnabled);

	//设置行高度
	int row = dataTypeTable->rowCount() - 1;
	dataTypeTable->setRowHeight(row, 20);
	//类型
	QComboBox *combox1 = new QComboBox();
	combox1->addItem(QStringLiteral("条码"));// 可以插入icon
	combox1->addItem(QStringLiteral("点阵数字"));
	combox1->addItem(QStringLiteral("实体数字"));
	combox1->addItem(QStringLiteral("汉字"));
	combox1->addItem(QStringLiteral("字母"));
	combox1->addItem(QStringLiteral("字母数字"));

	dataTypeTable->setCellWidget(row, 0, (QWidget*)combox1);

	//QWidget * widget = ui.tableWidget_NetGate->cellWidget(currentrow, 4);//获得widget
	//QComboBox *combox = (QComboBox*)widget;//强制转化为QComboBox
	//combox->setEnabled(true);
	//QString string = combox->currentText();

	//角度
	QComboBox *combox2 = new QComboBox();
	combox2->addItem(QStringLiteral("0"));		// 可以插入icon
	combox2->addItem(QStringLiteral("90"));
	combox2->addItem(QStringLiteral("180"));
	combox2->addItem(QStringLiteral("270"));
	dataTypeTable->setCellWidget(row, 1, (QWidget*)combox2);

	dataTypeTable->setItem(row, 2, new QTableWidgetItem(""));
	dataTypeTable->setItem(row, 3, new QTableWidgetItem(""));
	//字符个数
	//dataTypeTable->item(row, 3)->setTextAlignment(Qt::AlignCenter);
	//模板文件

	//对比方式
	QComboBox *combox3 = new QComboBox(this);
	combox3->addItem(QStringLiteral("数据库比对"));// 可以插入icon
	combox3->addItem(QStringLiteral("时间比对"));// 可以插入icon
	combox3->addItem(QStringLiteral("流水号比对"));
	combox3->addItem(QStringLiteral("固定内容比对"));

	dataTypeTable->setCellWidget(row, 4, (QWidget*)combox3);
	//ROI区域位置

	QString qstr = QString::number(rect.x(), 10) + "," + QString::number(rect.y(), 10)
		+ "," + QString::number(rect.width(), 10) + "," + QString::number(rect.height(), 10);
	dataTypeTable->setItem(row, 5, new QTableWidgetItem(qstr));
	dataTypeTable->item(row, 5)->setTextAlignment(Qt::AlignCenter);
	dataTypeTable->item(row, 5)->setFlags(Qt::NoItemFlags);
	dataTypeTable->setItem(row, 6, new QTableWidgetItem(""));

}