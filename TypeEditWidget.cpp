#include "TypeEditWidget.h"



TypeEditWidget::TypeEditWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowModality(Qt::WindowModal);

	connect(ui.okBtn, SIGNAL(clicked()), this, SLOT(okBtnClicked()));
	QStringList list = { QStringLiteral("条码"), QStringLiteral("点阵数字"), QStringLiteral("实体数字") ,
		QStringLiteral("汉字") , QStringLiteral("字母") ,QStringLiteral("字母数字") };
	ui.comboBoxType->addItems(list);
	//list.clear();
	QStringList listAn = { "0","90","180", "270"};
	ui.comboBoxAngle->addItems(listAn);

	QStringList listcomp = { QStringLiteral("数据库比对"),QStringLiteral("时间比对"),QStringLiteral("流水号比对"),
		QStringLiteral("固定内容比对") };
	ui.comboBoxCompareType->addItems(listcomp);
}

TypeEditWidget::~TypeEditWidget()
{
}

void TypeEditWidget::okBtnClicked() {
	//EditDatatype *editdataType = new EditDatatype();
	editDatatype.type = ui.comboBoxType->currentText();
	editDatatype.angle = ui.comboBoxAngle->currentText();
	editDatatype.charNum = ui.textEditCharNum->toPlainText();
	editDatatype.rowNum = ui.textEditRowNum->toPlainText();
	editDatatype.compareConf = ui.textEditCompareSet->toPlainText();
	editDatatype.compareType= ui.comboBoxCompareType->currentText();
	
	emit updateTypeTable(editDatatype);

	this->hide();
}

EditDatatype::EditDatatype()
{
}

EditDatatype::~EditDatatype()
{
}