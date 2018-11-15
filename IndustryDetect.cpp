#include "IndustryDetect.h"

extern ofstream fout;
extern int appLayoutType;
extern QImage currentBasicImg;
//时间格式  1 时分  2 年月日  3 年月日时分
QString timeFormatStr[5] = { "","hhmm","yyyyMMdd","yyyyMMddhhmm","" };
QString errorStr = "segment failed";
QString blackErrorStr = "row segment error";

//true 表示相等 false 不相等
static inline bool mStringCompare(QString &str1, QString &str2) {
	if (str1.length() != str2.length()) {
		return false;
	}
	else {
		//相等的时候
		if (str1[0] == str2[0] && str1.compare(str2) == 0) {
			return true;
		}
		else {
			return false;
		}
	}
}

static void calCoff() {

	Mat white = imread("white.bmp", IMREAD_GRAYSCALE);
	Mat black = imread("black.bmp", IMREAD_GRAYSCALE);

	ofstream bFile("Config//Fpga//Bcfg.txt");
	ofstream kFile("Config//Fpga//Kcfg.txt");

	double bCoff[648] = { 0.0 };
	double kCoff[648] = { 0.0 };

	unsigned char mat[400][648];
	memcpy(mat, black.data, 400 * 648);

	//计算b的系数
	int sum = 0;
	for (int i = 0; i < 648; i++) {
		sum = 0;
		for (int j = 100; j < 200; j++) {
			sum += mat[j][i];
		}
		bCoff[i] = sum / 100;
	}

	unsigned char mat1[400][648];
	memcpy(mat1, white.data, 400 * 648);

	//计算k的系数
	double k = 0.0;
	double kTmp = 0.0;
	double kSum = 0.0;
	for (int i = 0; i < 648; i++) {
		kSum = 0.0;
		for (int j = 100; j < 200; j++) {
			kSum += mat1[j][i];
		}
		kTmp = kSum / 100.0;
		k = 240.0 / (kTmp - bCoff[i]);
		kCoff[i] = k;
	}
	//写入到文本文件中
	for (int i = 0; i < 648; i++) {
		kFile << kCoff[i] << "\n";
		bFile << bCoff[i] << "\n";
	}
	kFile.close();
	bFile.close();

	double temp;
	for (int j = 0; j < 400; j++)
	{
		for (int i = 0; i < 648; i++)
		{
			temp = kCoff[i] * (mat1[j][i] - bCoff[i]);

			if (temp < 0)
			{
				temp = 0;
			}
			if (temp > 255)
			{
				temp = 255;
			}
			mat1[j][i] = temp;
		}
	}
	Mat dst(400, 648, CV_8UC1);

	memcpy(dst.data, mat1, 400 * 648);
	//imwrite("1CorrectImg.bmp", dst);

}


IndustryDetect::IndustryDetect(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	appType = appLayoutType;
	if (appLayoutType == 1) {
		setMinimumHeight(930);
		setMinimumWidth(1300);
	}
	else {
		setMinimumHeight(720);
		setMaximumHeight(768);

		setMinimumWidth(900);
		setMaximumWidth(1024);
	}

	//setFixedSize(1300, 860);
	setWindowFlags(Qt::FramelessWindowHint | windowFlags());

	//填充背景
	QPalette pal(palette());
	pal.setColor(QPalette::Window, QColor(255, 255, 255));
	setAutoFillBackground(true);
	setPalette(pal);
	//***************************标题栏****************************//
	upTitleBar = new UpTitleBar(this);
	//***************************左边布局****************************//
	appConfig = AppConfig::getInstance();

	labelWidth = appConfig->width;
	labelHeight = appConfig->height;

	errCountDelay = appConfig->erroCountDelay;

	//优化 bool--int
	//初始化
	for (int i = 0; i < 2; i++) {
		bufferCountDelay[i] = new int[errCountDelay];
		for (int j = 0; j < errCountDelay; j++) {
			bufferCountDelay[i][j] = 0;
		}
	}

	imgLabel = new mQLabel(this);
	imgLabel->setFixedSize(labelWidth / appType, labelHeight / appType);
	imgLabel->setFrameShape(QFrame::Box);
	imgLabel->setStyleSheet("border-width: 1px;border-style: solid;border-color: rgb(170, 170, 200)");

	imgTest = QImage("test.bmp");

	imgLabel->setPixmap(QPixmap::fromImage(imgTest).scaled(labelWidth / appType, labelHeight / appType, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	//labelLayout->addWidget(imgLabel);


	//***************************右边布局****************************//
	dataShowWidget = new DataShowWidget;

	funcGropBox = new QGroupBox(QStringLiteral("功能区"));
	funcGropBox->setMinimumHeight(220);

	startWorkBtn = new QToolButton;
	startWorkBtn->setText(QStringLiteral("开始检测"));

	startWorkBtn->setIconSize(QSize(48, 48));
	startWorkBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	startWorkBtn->setIcon(QIcon("icon/captrue.ico"));

	startWorkBtn->setFixedSize(80, 70);

	startWorkBtn->setStyleSheet("QToolButton{background-color:transparent; font:12px 'Microsoft YaHei'; font: bold;  border:none;}"
		"QToolButton:hover{background-color:rgb(200,214,204);}");

	finishWorkBtn = new QToolButton;
	finishWorkBtn->setText(QStringLiteral("停止检测"));

	finishWorkBtn->setIconSize(QSize(48, 48));
	finishWorkBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	finishWorkBtn->setIcon(QIcon("icon/4.ico"));

	finishWorkBtn->setFixedSize(80, 70);

	finishWorkBtn->setStyleSheet("QToolButton{background-color:transparent; font:12px 'Microsoft YaHei'; font: bold; border:none; }"
		"QToolButton:hover{background-color:rgb(200,214,204);}");


	setBaseImgBtn = new QToolButton;
	setBaseImgBtn->setText(QStringLiteral("设置基准图像"));
	setBaseImgBtn->setIconSize(QSize(48, 48));
	setBaseImgBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	setBaseImgBtn->setIcon(QIcon("icon/baseImg.ico"));

	setBaseImgBtn->setFixedSize(80, 70);

	setBaseImgBtn->setStyleSheet("QToolButton{background-color:transparent; font:12px 'Microsoft YaHei'; font: bold;  border:none; }"
		"QToolButton:hover{background-color:rgb(200,214,204);}");

	aboutBtn = new QToolButton;
	aboutBtn->setText(QStringLiteral("关于"));
	aboutBtn->setIconSize(QSize(48, 48));
	aboutBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	aboutBtn->setIcon(QIcon("icon/about.ico"));

	aboutBtn->setFixedSize(80, 70);

	aboutBtn->setStyleSheet("QToolButton{background-color:transparent; font:12px 'Microsoft YaHei'; font: bold;  border:none; }"
		"QToolButton:hover{background-color:rgb(200,214,204);}");

	//resetCountBtn
	resetCountBtn = new QToolButton;
	resetCountBtn->setText(QStringLiteral("复位计数"));
	resetCountBtn->setIconSize(QSize(48, 48));
	resetCountBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	resetCountBtn->setIcon(QIcon("icon/reset.ico"));

	resetCountBtn->setFixedSize(80, 70);

	resetCountBtn->setStyleSheet("QToolButton{background-color:transparent; font:12px 'Microsoft YaHei'; font: bold;  border:none; }"
		"QToolButton:hover{background-color:rgb(200,214,204);}");

	//aboutBtn

	//myspeed = new mySpeed();
	speedLabel = new QLabel();
	speedLabel->setFixedSize(QSize(210, 50));
	speedLabel->setStyleSheet("QLabel{background-color: rgb(235, 235, 235);color: rgb(10,80,132);font:bold 16pt 'Roboto'; }");
	speedLabel->setAlignment(Qt::AlignCenter);
	speedLabel->setText(QStringLiteral("显示速度"));

	QVBoxLayout *leftDownLayout = new QVBoxLayout;

	funcGridLayout = new QGridLayout;
	//funcGridLayout->addWidget(speedLabel, 0, 3,1,1);
	funcGridLayout->addWidget(startWorkBtn, 0, 0);
	funcGridLayout->addWidget(finishWorkBtn, 0, 1);
	funcGridLayout->addWidget(setBaseImgBtn, 0, 2);
	funcGridLayout->addWidget(aboutBtn, 1, 1);
	funcGridLayout->addWidget(resetCountBtn, 1, 0);

	funcGropBox->setLayout(funcGridLayout);

	funcGropBox->setFixedWidth(648 / appLayoutType);
	leftDownLayout->addWidget(funcGropBox);
	leftDownLayout->addWidget(speedLabel);

	dataTypeWidget = new DataTypeWidget;

	leftMainLayout = new QVBoxLayout;
	leftMainLayout->addWidget(imgLabel);
	leftMainLayout->addLayout(leftDownLayout);
	leftMainLayout->setContentsMargins(0, 10, 0, 0);

	rightMainLayout = new QVBoxLayout;
	rightMainLayout->addWidget(dataShowWidget);
	rightMainLayout->addWidget(dataTypeWidget);
	//rightMainLayout->setContentsMargins(0, 0, 0, 0);

	mainLayout = new QHBoxLayout;
	mainLayout->addLayout(leftMainLayout);
	mainLayout->addLayout(rightMainLayout);
	mainLayout->setContentsMargins(15, 0, 10, 0);

	downStateBarWidget = new DownStateBarWidget(this);
	m_mianLayout = new QVBoxLayout(this);
	m_mianLayout->addWidget(upTitleBar);
	m_mianLayout->addLayout(mainLayout);
	m_mianLayout->addWidget(downStateBarWidget);
	m_mianLayout->setMargin(0);

	connect(imgLabel, SIGNAL(imgPatchInfo(QRect&)), dataTypeWidget, SLOT(addArea(QRect&)));
	connect(imgLabel, SIGNAL(showTypeEdit(int)), this, SLOT(showTypeEditWidget(int)));



	connect(dataTypeWidget, SIGNAL(areaOperation()), imgLabel, SLOT(controlAreaOperation()));
	connect(dataTypeWidget, SIGNAL(reduceAreaSig()), imgLabel, SLOT(reduceSelectAreaNum()));

	//隐藏属性编辑框
	typeEditWidget = new TypeEditWidget();
	typeEditWidget->hide();

	imgShowWidget = new imgLabelShowWidget();
	imgShowWidget->hide();

	aboutWidget = new AboutWidget();
	aboutWidget->hide();

	connect(imgLabel, SIGNAL(showLargeLabel()), this, SLOT(showLargeLabelSlot()));
	connect(aboutWidget, SIGNAL(correctSignals()), this, SLOT(calculateCorrectCoeff()));

	//typeEditWidget->setWindowModality(Qt::WindowModal);

	connect(dataTypeWidget, SIGNAL(showTypeEditWidget(int)), this, SLOT(showTypeEditWidget(int)));
	connect(typeEditWidget, SIGNAL(updateTypeTable(EditDatatype&)), dataTypeWidget, SLOT(updateRow(EditDatatype&)));
	connect(typeEditWidget, SIGNAL(updateTypeTable(EditDatatype&)), this, SLOT(clearLabel()));

	//获取单例对象
	imageget = ImageGet::getInstance();
	workThread = WorkThread::getinstance();
	workThreadForBasicImg = WorkThreadForBasicImg::getInstance();

	//设置基准图像
	baseImg = imageget->QImage2cvMat(imgTest);
	if (baseImg.channels() != 1) {
		cvtColor(baseImg, baseImg, CV_RGB2GRAY);
	}

	imageget->malgorithm.setBaseImg(baseImg);

	connect(startWorkBtn, SIGNAL(clicked()), this, SLOT(startWorkBtnClicked()));
	connect(finishWorkBtn, SIGNAL(clicked()), this, SLOT(finishWorkBtnClicked()));
	connect(setBaseImgBtn, SIGNAL(clicked()), this, SLOT(setBaseImgBtnClicked()));
	connect(aboutBtn, SIGNAL(clicked()), this, SLOT(showAboutWidgetSlot()));
	connect(resetCountBtn, SIGNAL(clicked()), this, SLOT(resetCountSlot()));

	connect(this, SIGNAL(updateDownstatus(int, int)), downStateBarWidget, SLOT(updateErrAndTotalconut(int, int)));

	connect(imageget, SIGNAL(sendDataInfo(DataInfo&)), this, SLOT(updateInfo(DataInfo&)), Qt::DirectConnection);
	connect(this, SIGNAL(setBasic_(QString)), imageget, SLOT(updateBasicImg(QString)), Qt::DirectConnection);

	//connect(workThread, SIGNAL(updateSpeed(double)), myspeed, SLOT(UpdateAngle(double)), Qt::DirectConnection);

	connect(workThread, SIGNAL(updateSpeed(double)), this, SLOT(updateSpeedSlot(double)), Qt::DirectConnection);

	connect(dataTypeWidget, SIGNAL(saveCurrentBasicImg(QString)), this, SLOT(saveCurrentBasicImgSlot(QString)));
	connect(dataTypeWidget, SIGNAL(setCurrentBasicImg(QString)), this, SLOT(mSetBasicImg(QString)));

	connect(imgShowWidget, SIGNAL(imgLabelShowBack()), this, SLOT(setShowType()));
	//connect(this, SIGNAL(updateSpeed(double)), myspeed, SLOT(UpdateAngle(double)));
	//updateSpeedSlot(100);

}

IndustryDetect::~IndustryDetect()
{
	//释放资源
	if (databuf != NULL) {
		delete[]databuf;
	}

	if (m_pUSBDevice != NULL)
	{
		delete m_pUSBDevice;
		m_pUSBDevice = NULL;
	}

	for (int i = 0; i < 2; i++) {
		if (bufferCountDelay[i] != NULL) {
			delete[]bufferCountDelay[i];
		}
	}
	for (int i = 0; i < 4; i++) {
		if (SQLDataVec[i] != NULL) {
			SQLDataVec[i]->clear();
			delete SQLDataVec[i];
			SQLDataVec[i] = NULL;
		}
	}
}

void IndustryDetect::sendWarningOrderToFPGA()
{
	//发送3333指令
	UCHAR bufOrder[2] = { 0x08, 0xF0 };

	long Two = 2;

	if (m_pUSBDevice->BulkOutEndPt->XferData(bufOrder, Two) == false)
	{
		downStateBarWidget->setConnnetStateInfo(QStringLiteral("发送3指令失败！"));

	}
}

void IndustryDetect::startWorkBtnClicked()
{
	// TODO: 在此添加控件通知处理程序代码  唤起处理图像线程
//ocrRegion[0][1] = 2;
	barcode = false;

	startWorkBtn->setEnabled(false);
	setBaseImgBtn->setEnabled(false);

	UCHAR bufOrder[2] = { 0x01, 0xF4 };  //码盘配置指令
	//实时更新相关设置参数
	//更新是否采集图像
	bool tempCap = upTitleBar->setDialog->ui.isSaveCheckBox->isChecked();
	imageget->isSave = tempCap;
	//更新保存路径
	imageget->saveImagePath = upTitleBar->setDialog->ui.pathEdit->toPlainText();
	//更新图像检测延迟行数
	errCountDelay = (upTitleBar->setDialog->ui.errorDelayEdit->toPlainText()).toInt();
	//mm====>转为行
	workThread->captureDelay = ((upTitleBar->setDialog->ui.capDealyEdit->toPlainText()).toInt()) * 11.7;

	//更新码盘参数
	if (upTitleBar->setDialog->ui.radioButton->isChecked()) {
		bufOrder[0] = 0x02;
	}
	else if (upTitleBar->setDialog->ui.radioButton_2->isChecked()) {
		bufOrder[0] = 0x01;
	}
	else {
		bufOrder[0] = 0x03;
	}

	int column = dataTypeWidget->getTableColumn();
	int row = dataTypeWidget->getTableRow();

	imageget->datatypeNum = dataTypeWidget->getTableRow();

	barcode = false;

	///***********************************获取数据信息*******************************/

	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < column; j++)
		{
			if (j == 0)
			{
				QWidget * widget = dataTypeWidget->dataTypeTable->cellWidget(i, j);//获得widget
				QComboBox *combox = (QComboBox*)widget;//强制转化为QComboBox
				QString string = combox->currentText();
				if (string == QStringLiteral("条码")) {
					imageget->datatype[i].type = TYPE_QRCODE;
				}
				if (string == QStringLiteral("点阵数字")) {
					imageget->datatype[i].type = TYPE_NUMBER;
				}
				if (string == QStringLiteral("汉字")) {
					imageget->datatype[i].type = TYPE_CHINESE;
				}
				if (string == QStringLiteral("字母")) {
					imageget->datatype[i].type = TYPE_CHAR;
				}
				if (string == QStringLiteral("实体数字")) {
					imageget->datatype[i].type = TYPE_NUMBER_NODOT;
				}
				if (string == QStringLiteral("字母数字")) {
					imageget->datatype[i].type = TYPE_CHARANDNUMBER;
				}
			}
			if (j == 1)
			{
				QWidget * widget = dataTypeWidget->dataTypeTable->cellWidget(i, j);//获得widget
				QComboBox *combox = (QComboBox*)widget;//强制转化为QComboBox
				QString string = combox->currentText();
				imageget->datatype[i].angle = string.toInt();
			}

			if (j == 2)
			{
				imageget->datatype[i].charNum.clear();//清除上一次数据

				QString str = dataTypeWidget->dataTypeTable->item(i, j)->text();
				//qDebug() << str << endl;
				if (str == "0") {
					continue;
				}

				isocrRegion[i] = false;

				//bool a = str.contains("-");
				if (str.contains('-'))
				{					QList<QString>list1;					list1 = str.split(',');					int tempint = list1[0].toInt();					imageget->datatype[i].charNum.push_back(tempint);					QList<QString> qlist = list1[1].split('-');					ocrRegion[i][0] = qlist[0].right(1).toInt();					ocrRegion[i][1] = qlist[1].left(1).toInt();
					isocrRegion[i] = true;
				}
				else
				{
					QList<QString>list;
					list = str.split(',');
					//qDebug() << 111 << endl;
					for (int k = 0; k < list.size(); k++) {
						imageget->datatype[i].charNum.push_back(list[k].toInt());
					}
				}

			}

			if (j == 3) {
				QString str = dataTypeWidget->dataTypeTable->item(i, j)->text();

				if (str == "0") {
					continue;
				}
				imageget->datatype[i].rowNum = str.toInt();
				//qDebug() << compareTmp[i]  << endl;
			}

			if (j == 4) {
				QWidget * widget = dataTypeWidget->dataTypeTable->cellWidget(i, j);//获得widget
				QComboBox *combox = (QComboBox*)widget;//强制转化为QComboBox
				QString string = combox->currentText();
				if (string == QStringLiteral("数据库比对")) {
					compareType[i] = DATABASE_COMP;
				}
				if (string == QStringLiteral("时间比对")) {
					compareType[i] = TIME_COMP;
				}
				if (string == QStringLiteral("流水号比对")) {
					compareType[i] = FLOWNUM_COMP;
				}
				if (string == QStringLiteral("固定内容比对")) {
					compareType[i] = CONTENT_COMP;
				}
			}

			if (j == 5)
			{
				QString str = dataTypeWidget->dataTypeTable->item(i, j)->text();
				QList<QString>list;
				list = str.split(',');
				imageget->datatype[i].imgRect.x = list[0].toInt();
				imageget->datatype[i].imgRect.y = list[1].toInt();
				imageget->datatype[i].imgRect.width = list[2].toInt();
				imageget->datatype[i].imgRect.height = list[3].toInt();
			}
			if (j == 6)
			{
				if (compareType[i] == CONTENT_COMP) {
					compareTmp[i] = dataTypeWidget->dataTypeTable->item(i, j)->text();
				}

				if (compareType[i] == TIME_COMP) {
					//时间格式  1 时分  2 年月日  3 年月日时分
					//QString str = dataTypeWidget->dataTypeTable->item(i, j)->text();
					////compareTmp[i] = dataTypeWidget->dataTypeTable->item(i, j)->text();
					//timeFormat[i] = timeFormatStr[str.toInt()];
					//isGettime[i] = true;
				}
				if (compareType[i] == FLOWNUM_COMP) {
					//compareTmp[i] = dataTypeWidget->dataTypeTable->item(i, j)->text();
					QString str = dataTypeWidget->dataTypeTable->item(i, j)->text();
					QList<QString>listTemp;
					listTemp = str.split(',');
					flowNumIncrease[i] = listTemp[1].toInt();
					flowNumStart[i] = listTemp[0].toInt();
				}
				if (compareType[i] == DATABASE_COMP) {
					//compareTmp[i] = dataTypeWidget->dataTypeTable->item(i, j)->text();
					//清理vector
					for (int i = 0; i < 4; i++) {
						txtDataVec[i].clear();
					}

					QString str = dataTypeWidget->dataTypeTable->item(i, j)->text();
					//loadData(str);
					//加载txt数据库
					if (str.toInt() == 0) {
						barcode = true;
					}

					if (str.toInt() == 1) {
						ifstream infile("data.txt");
						string tempstr;
						while (getline(infile, tempstr)) {
							txtDataVec[i].push_back(tempstr);
						}
						/*if (infile.is_open()) {
							int num = 0;
							infile >> num;
							for (int m = 0; m < num; m++) {
								txtDataVec[i].push_back(tempstr);
							}
						}*/
					}

					//加载db数据库信息
					if (str.toInt() == 2) {
						SQLDataVec[i] = labelDAOImpl.listByPrepare();
						//for (int n = 0; n < SQLDataVec[i]->size(); n++) {
						//	qDebug()<< SQLDataVec[i]->at(n) <<endl;
						//}
					}
				}

			}
		}
	}

	//qDebug() << imageget->datatype[0].angle << "---" << imageget->datatype[0].charNum[0] << "---" << imageget->datatype[0].rowNum << "---";
	//	qDebug() << ocrRegion[0][0] << ocrRegion[0][1] << ocrRegion[1][1] <<ocrRegion[1][1] << endl;
		/***********************************发送指令1111给fpga 告诉fpga 上位机做好接受数据的准备*******************************/

	if (m_pUSBDevice == NULL) {
		m_pUSBDevice = new CCyUSBDevice(NULL);
	}


	long Two = 2;
	if (m_pUSBDevice->BulkOutEndPt == NULL)
	{
		startWorkBtn->setEnabled(true);
		downStateBarWidget->setConnnetStateInfo(QStringLiteral("连接设备失败！"));
		return;
	}
	else
	{
		downStateBarWidget->setConnnetStateInfo(QStringLiteral("连接设备成功！"));
		//emit UpdateTitle(1);  //设备连接成功
	}

	if (m_pUSBDevice->BulkOutEndPt->XferData(bufOrder, Two) == false)
	{
		downStateBarWidget->setConnnetStateInfo(QStringLiteral("发送指令失败！"));
		startWorkBtn->setEnabled(true);
		return;
	}


	UCHAR bufOrderClose[2] = { 0x07,0xF0 };

	Two = 2;

	if (m_pUSBDevice->BulkOutEndPt->XferData(bufOrderClose, Two) == false)
	{
		//textEditNum->append(QStringLiteral("发送指令失败!"));
		//startWorkBtn->setEnabled(true);
		//emit UpdateTitle(2); //设备连接失败
		downStateBarWidget->setConnnetStateInfo(QStringLiteral("连接设备失败！"));
		return;
	}


	//downStateBarWidget->setConnnetStateInfo(QStringLiteral("设备挂起！"));


	//====================================AD寄存器配置===============================================\\

	//long length = 18;

	//UCHAR CorrectbufOrder[18] = {
	//	0x69,0xb0,
	//	0xf0,0xb2,
	//	0x1f,0xb4,//1_pga		0x3f,0xb4
	//	0x1f,0xb6,//2_pga		0x3f,0xb6	
	//	0x1f,0xb8,//3_pga		0x3f,0xb8
	//	0x00,0xba,//1_offset	0x1a,0xba
	//	0x00,0xbc,//2_offset
	//	0x00,0xbe,//3_offset
	//	0xbb,0xbb 
	//};

	//if (m_pUSBDevice->BulkOutEndPt->XferData(CorrectbufOrder, length) == false)
	//{
	//	downStateBarWidget->setConnnetStateInfo(QStringLiteral("发送指令失败！"));
	//	startWorkBtn->setEnabled(true);
	//	return;
	//}


	//开启工作线程
	workThread->isRun = true;
	workThread->start();

}


void IndustryDetect::updateInfo(DataInfo & datainfo)
{
	totalCount++;
	//countdelay 标记为1 做递增处理
	for (int i = 0; i < errCountDelay; i++) {
		if (bufferCountDelay[0][i] == 1) {
			bufferCountDelay[1][i]++;
		}
	}

	int r = dataTypeWidget->getTableRow();
	if (showLargeType == false) {
		imgLabel->setPixmap(QPixmap::fromImage(datainfo.img).scaled(labelWidth / appType, labelHeight / appType, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	}
	else {
		imgShowWidget->imgLabel->setPixmap(QPixmap::fromImage(datainfo.img).scaled(labelWidth, labelHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	}
	//QString str = datainfo.numResult;

	datainfo.state = true;

	if (r != 0)
	{
		if (barcode == true) {
			dataShowWidget->updateList(datainfo);  //更新列表
			return;
		};

		for (int i = 0; i < r; i++)
		{
			if (isGettime[i] == true) {
				currentDateTime = QDateTime::currentDateTime();
			}

			if (isocrRegion[i] == true) {
				//界面定义的3 是数组的2
				//QString str;
				datainfo.reResult[i] = datainfo.reResult[i].mid(ocrRegion[i][0] - 1, ocrRegion[i][1] - ocrRegion[i][0] + 1);
				//= str;
			}

			switch (compareType[i])
			{
			case DATABASE_COMP:
				if (SQLDataVec[i]->at(totalCount) != datainfo.reResult[i]) {
					datainfo.state = false;
				}
				break;
			case FLOWNUM_COMP:
				if (flowNumStart[i] != datainfo.reResult[i].toInt()) {
					datainfo.state = false;
				}
				flowNumStart[i] = flowNumStart[i] + flowNumIncrease[i];
				break;
			case CONTENT_COMP:
				//qDebug() << totalCount << ":" << compareTmp[0] << datainfo.reResult[0] << endl;
				if (!(mStringCompare(compareTmp[i], datainfo.reResult[i]))) {
					datainfo.state = false;

				}
				break;
			case TIME_COMP:
				if (mStringCompare(blackErrorStr, datainfo.reResult[i]) || mStringCompare(errorStr, datainfo.reResult[i])) {
					datainfo.state = false;
				}
				break;
			default:
				break;
			}
		}

		if (datainfo.state == false)
		{
			dataShowWidget->updateBadDataList(datainfo);
			datainfo.img.save("image\\error\\" + QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss") + ".jpg");
		}

		dataShowWidget->updateList(datainfo);  //更新列表
	}

	//发送报警指令
	for (int i = 0; i < errCountDelay; i++) {
		if (bufferCountDelay[1][i] == errCountDelay) {
			//置0处理 方便下次操作
			bufferCountDelay[0][i] = 0;
			bufferCountDelay[1][i] = 0;
			sendWarningOrderToFPGA();
		}
	}

	if (datainfo.state == false)
	{
		errCount++;
		if (errCountDelay == 0) {
			sendWarningOrderToFPGA();
		}
		else {
			bufferCountDelay[0][totalCount % errCountDelay] = 1; //标记为1 做递增处理
		}
	}

	emit updateDownstatus(totalCount, errCount);
}

void IndustryDetect::setBaseImgBtnClicked()
{
	QMessageBox::StandardButton reply;
	reply = QMessageBox::information(NULL, "Information", QStringLiteral("是否设置基准图像？"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

	if (reply == QMessageBox::No) {
		return;
	}

	setBaseImgBtn->setEnabled(false);

	UCHAR bufOrder0[2] = { 0x01, 0xF4 };  //码盘配置指令
	//实时更新相关设置参数
	//更新是否采集图像
	bool tempCap = upTitleBar->setDialog->ui.isSaveCheckBox->isChecked();
	imageget->isSave = tempCap;
	//更新保存路径
	imageget->saveImagePath = upTitleBar->setDialog->ui.pathEdit->toPlainText();
	//更新图像检测延迟行数
	//errCountDelay = (upTitleBar->setDialog->ui.errorDelayEdit->toPlainText()).toInt();
	//mm====>转为行
	workThreadForBasicImg->captureDelay = ((upTitleBar->setDialog->ui.capDealyEdit->toPlainText()).toInt()) * 11.7;

	//更新码盘参数
	if (upTitleBar->setDialog->ui.radioButton->isChecked()) {
		bufOrder0[0] = 0x02;
	}
	else if (upTitleBar->setDialog->ui.radioButton_2->isChecked()) {
		bufOrder0[0] = 0x01;
	}
	else {
		bufOrder0[0] = 0x03;
	}

	/****************start*****************/

	if (m_pUSBDevice == NULL) {
		m_pUSBDevice = new CCyUSBDevice(NULL);
	}

	long Two = 2;
	if (!(m_pUSBDevice->IsOpen()))
	{
		setBaseImgBtn->setEnabled(true);
		downStateBarWidget->setConnnetStateInfo(QStringLiteral("连接设备失败！"));
		return;
	}
	else
	{
		downStateBarWidget->setConnnetStateInfo(QStringLiteral("连接设备成功！"));
		//emit UpdateTitle(1);  //设备连接成功
	}

	if (m_pUSBDevice->BulkOutEndPt->XferData(bufOrder0, Two) == false)
	{
		downStateBarWidget->setConnnetStateInfo(QStringLiteral("发送指令失败！"));
		setBaseImgBtn->setEnabled(true);
		return;
	}

	downStateBarWidget->setConnnetStateInfo(QStringLiteral("请开启设备..."));

	UCHAR bufOrderClose[2] = { 0x07,0xF0 };

	Two = 2;

	if (m_pUSBDevice->BulkOutEndPt->XferData(bufOrderClose, Two) == false)
	{
		//textEditNum->append(QStringLiteral("发送指令失败!"));
		//startWorkBtn->setEnabled(true);
		//emit UpdateTitle(2); //设备连接失败
		downStateBarWidget->setConnnetStateInfo(QStringLiteral("连接设备失败！"));
		return;
	}


	setBaseImgBtn->setEnabled(true);

	workThreadForBasicImg->isRun = true;
	workThreadForBasicImg->start();
	//MessageBox(NULL, TEXT("基准图设置完成，请框选识别区域！"), TEXT("Warnning"), MB_OK);
	//workThreadForBasicImg->
	//downStateBarWidget->setConnnetStateInfo(QStringLiteral("设置基准图成功！"));
}

void IndustryDetect::calculateCorrectCoeff()
{

	QMessageBox::StandardButton reply;
	reply = QMessageBox::information(NULL, "Information", QStringLiteral("是否生成校正系数？"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

	if (reply == QMessageBox::No) {
		return;
	}

	//1):复位fpga的k和b的值，采集原始图像
	CCyUSBDevice *mUsb = new CCyUSBDevice(NULL);		//定义 CCyUSBDevice指针
	if (!(mUsb->IsOpen())) {
		return;
	}

	UCHAR coeffBuf[1296 * 2 + 2] = { 0 };
	UCHAR buffTmp[1296] = { 0 };
	long dataSize;
	double k = 0.0;
	for (int i = 0; i < 1296; i += 2) {
		buffTmp[i] = 1.0 * 16.0/*((k*16.0) > 255 ? 255 : (int)k * 16)*/;
	}
	//int b = 0;
	for (int i = 1; i < 1296; i += 2) {

		buffTmp[i] = 0;
	}
	coeffBuf[0] = 0x02;
	coeffBuf[1] = 0xF0;
	memcpy(coeffBuf + 2, buffTmp, 1296);
	memcpy(coeffBuf + 1298, buffTmp, 1296);
	dataSize = 1296 + 1296 + 2;
	mUsb->BulkOutEndPt->XferData(coeffBuf, dataSize);
	//2):唤起校正系数生成线程函数生产校正系数
	/*imageget->modType = CORRECTMODE;
	imageget->start();*/

	int dataSizeTmp = (648 * 1000 / 1024 + 1) * 1024;   //传入xfdata长度是以引用方式传入，会被内部操作修改值，设置一个temp，方便赋值
	UCHAR *dataBuff = new unsigned char[dataSizeTmp];
	UCHAR *pImgDataBuff = new unsigned char[dataSizeTmp];
	UCHAR *pWhiteImgDataBuff = new unsigned char[dataSizeTmp + 2048];
	Mat whiteImg;
	Mat blackImg;

	dataSize = 2;

	//1):发送指令 校正开始采集图像
	//CCyUSBDevice*mUsb = new CCyUSBDevice(NULL);

	UCHAR correctStartOrderBuf[2] = { 0x03,0xF0 };
	//UCHAR pwmCfgBuff[2];
	//int whiteColorBais = 0; //颜色偏差
	//int pwmValue = 200;

	mUsb->BulkOutEndPt->XferData(correctStartOrderBuf, dataSize);
	//2):发送关灯
	UCHAR bufOrderClose[2] = { 0x07,0xF0 };

	dataSize = 2;

	if (mUsb->BulkOutEndPt->XferData(bufOrderClose, dataSize) == false)
	{
		//textEditNum->append(QStringLiteral("发送指令失败!"));
		//startWorkBtn->setEnabled(true);
		//emit UpdateTitle(2); //设备连接失败
		return;
	}
	//3):PWM = 250
	/**********************************黑色图像***********************************/
	UCHAR pwmCfgBuff[2];

	pwmCfgBuff[0] = 0;
	pwmCfgBuff[1] = 0xF8;

	dataSize = 2;
	mUsb->BulkOutEndPt->XferData(pwmCfgBuff, dataSize);

	//4):接受数据
	//4):1 开灯
	UCHAR bufOrder[2] = { 0x00,0xFC };
	dataSize = 2;
	if (mUsb->BulkOutEndPt->XferData(bufOrder, dataSize) == false)
	{

		return;
	}

	dataSize = dataSizeTmp;

	mUsb->BulkInEndPt->TimeOut = 10000; //触发信号3s   无限等待 
	//qDebug() << "black data before" << pImgDataBuff[12] << pImgDataBuff[13] << endl;

	if (!(mUsb->BulkInEndPt->XferData(dataBuff, dataSize))) {
		MessageBox(NULL, TEXT("接受校正数据失败!"), TEXT("信息"), MB_OK);
		return;
	}

	dataSize = dataSizeTmp;
	memcpy(pImgDataBuff, dataBuff, dataSize);//拷贝
	//qDebug() << "black data after" << pImgDataBuff[12] << pImgDataBuff[13] << endl;

	//4):2 关灯
	dataSize = 2;
	if (mUsb->BulkOutEndPt->XferData(bufOrderClose, dataSize) == false)
	{

		return;
	}
	//构造图像
	QImage img = QImage((const unsigned char*)(pImgDataBuff), 648, 1000, 648, QImage::Format_Grayscale8);
	img.save("black.bmp");
	//blackImg = Mat(1000, 648, CV_8UC1, pImgDataBuff); //h*w
	//imwrite("black.bmp", blackImg);
	//取白色的值 如果白色的值最大的值大于240 调整PWM使亮度低于240 然后再次采集图像调节图像整体亮度，低于240则退出循环

	//int *pData = whiteBlackImg.ptr<int>(100);		//取第100行
	//int dataArry[648] = { 0 };
	//for (int i = 0; i < 648; i++) {
	//	dataArry[i] = pData[i];
	//}
	////找到数组中最大值
	//int max = dataArry[0];
	//for (int i = 0; i < 648; i++) {
	//	if (max <= dataArry[i]) {
	//		max = dataArry[i];
	//	}
	//}
	//if (max < 240) {
	//	//保存pwm的值
	//	pwmFile << pwmValue;
	//	pwmFile.close();
	//	break;    //小于240跳出循环计算校正系数 否则继续采集
	//}

	////3):调整pwm
	//whiteColorBais = max - 240;
	//pwmValue -= whiteColorBais;
	//if (pwmValue > 255) {
	//	if (513 > pwmValue && pwmValue > 255) {
	//		pwmCfgBuff[0] = pwmValue - 256;
	//		pwmCfgBuff[1] = 0xF8 + 1;
	//	}
	//	if (767 >= pwmValue && pwmValue >= 513) {
	//		pwmCfgBuff[0] = pwmValue - 513;
	//		pwmCfgBuff[1] = 0xF8 + 2;
	//	}
	//	if (1024 >= pwmValue && pwmValue > 767) {
	//		pwmCfgBuff[0] = pwmValue - 767;
	//		pwmCfgBuff[1] = 0xF8 + 3;
	//	}
	//}
	//else {
	//	pwmCfgBuff[0] = pwmValue;
	//	pwmCfgBuff[1] = 0xF8;
	//}

	//5):PWM = 0

	//temp = 200;

	pwmCfgBuff[0] = 180;
	pwmCfgBuff[1] = 0xF8;
	dataSize = 2;
	mUsb->BulkOutEndPt->XferData(pwmCfgBuff, dataSize);
	//******************************************白色图像*************************************************
	//4):接受数据
	//4):1 开灯
	//UCHAR bufOrder[2] = { 0x00,0xFC };
	dataSize = 2;
	if (mUsb->BulkOutEndPt->XferData(bufOrder, dataSize) == false)
	{

		return;
	}

	dataSize = dataSizeTmp;

	mUsb->BulkInEndPt->TimeOut = 10000; //触发信号3s   无限等待 
	//qDebug() << "black data be" << pWhiteImgDataBuff[12] << pWhiteImgDataBuff[13] << endl;
	if (!(mUsb->BulkInEndPt->XferData(dataBuff, dataSize))) {
		MessageBox(NULL, TEXT("接受校正数据失败!"), TEXT("信息"), MB_OK);
		return;
	}
	dataSize = dataSizeTmp + 2048;
	memcpy(pWhiteImgDataBuff, dataBuff, dataSize);//拷贝
	//qDebug() << "black data af"<< pWhiteImgDataBuff[12] << pWhiteImgDataBuff[13] << endl;

	//4):2 关灯
	dataSize = 2;
	if (mUsb->BulkOutEndPt->XferData(bufOrderClose, dataSize) == false)
	{

		return;
	}
	//构造图像 图像为 先黑色后白色
	whiteImg = Mat(1000, 648, CV_8UC1, pWhiteImgDataBuff + 2048); //h*w
	imwrite("white.bmp", whiteImg);

	//4):计算校正系数
		/*Mat dest;
		calCoefFilterLinerStrech(whiteBlackImg, dest);*/
	calCoff();

	//释放内存
	if (dataBuff != NULL) {
		delete[]dataBuff;
		dataBuff = nullptr;
	}
	if (pImgDataBuff != NULL) {
		delete pImgDataBuff;
		pImgDataBuff = nullptr;
	}
	if (pWhiteImgDataBuff != NULL) {
		delete pWhiteImgDataBuff;
		pWhiteImgDataBuff = nullptr;
	}	

	if (mUsb != NULL) {
		delete mUsb;
		mUsb = nullptr;
	}

	MessageBox(NULL, TEXT("校正完成！请重启软件！"), TEXT("Info"), MB_OK);
}

void IndustryDetect::showTypeEditWidget(int row)
{

	dataTypeWidget->rowNum = row;
	typeEditWidget->show();
}


void IndustryDetect::clearLabel()
{
	imgLabel->clear();
}

void IndustryDetect::updateSpeedSlot(double d)
{
	QString str((QStringLiteral("速度: ") + "%1" + QStringLiteral(" 张/分钟")).arg((int)(60.0 / d)));
	speedLabel->setText(str);
}

void IndustryDetect::saveCurrentBasicImgSlot(QString str)
{
	QString imgPath = str;

	currentBasicImg.save(imgPath);
}

void IndustryDetect::mSetBasicImg(QString strPath)
{
	QString path = strPath;
	QImage basicImg(path);

	emit setBasic_(strPath);
	//imwrite("a.jpg",baseImg1);
	imgLabel->setPixmap(QPixmap::fromImage(basicImg).scaled(labelWidth / appType, labelHeight / appType, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void IndustryDetect::setShowType()
{
	//showLargeType = true;
}

void IndustryDetect::showLargeLabelSlot()
{
	imgShowWidget->show();
}

void IndustryDetect::showAboutWidgetSlot()
{
	aboutWidget->show();
}

void IndustryDetect::resetCountSlot()
{	
	totalCount = 0;
	errCount = 0;
	emit updateDownstatus(0, 0);
	speedLabel->setText(QStringLiteral("显示速度"));
}


void IndustryDetect::finishWorkBtnClicked()
{
	//errCount = totalCount = 0;
	//totalCount = 0;
	workThread->isRun = false;
	//WorkThreadForBasicImg->isRun = false;
	for (int i = 0; i < 4; i++) {

		if (SQLDataVec[i] != NULL) {
			SQLDataVec[i]->clear();
			delete SQLDataVec[i];
			SQLDataVec[i] = NULL;
		}
	}

	if (!(startWorkBtn->isEnabled())) {
		startWorkBtn->setEnabled(true);
	}
	
	if (!(setBaseImgBtn->isEnabled())) {
		setBaseImgBtn->setEnabled(true);
	}


	//发送指令
	//UCHAR bufOrder[2] = { 0x55, 0x55 };

	UCHAR bufOrderClose[2] = { 0x05, 0xF0 };

	long Two = 2;
	if (!(m_pUSBDevice->IsOpen()))
	{
		//textEditNum->append(QStringLiteral("发送指令失败!"));
		//startWorkBtn->setEnabled(true);
		//emit UpdateTitle(2); //设备连接失败
		downStateBarWidget->setConnnetStateInfo(QStringLiteral("连接设备失败！"));
		return;
	}

	if (m_pUSBDevice->BulkOutEndPt->XferData(bufOrderClose, Two) == false)
	{
		//textEditNum->append(QStringLiteral("发送指令失败!"));
		//startWorkBtn->setEnabled(true);
		//emit UpdateTitle(2); //设备连接失败
		downStateBarWidget->setConnnetStateInfo(QStringLiteral("连接设备失败！"));
		return;
	}
	else
	{
		downStateBarWidget->setConnnetStateInfo(QStringLiteral("正在停止设备..."));
	}

	downStateBarWidget->setConnnetStateInfo(QStringLiteral("设备挂起！"));


	//fout.close();
}
