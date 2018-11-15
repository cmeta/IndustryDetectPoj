#include "SettingDialog.h"

SettingDialog::SettingDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	connect(ui.backBtn, SIGNAL(clicked()), this, SLOT(backBtnClicked()));
	connect(ui.saveCfgBtn, SIGNAL(clicked()), this, SLOT(saveSettingBtnClicked()));

}

SettingDialog::~SettingDialog()
{
}

void SettingDialog::loadSetting(const AppConfig *appCfg)
{
	if (appCfg->isSaveImg == true) {
		ui.isSaveCheckBox->setChecked(true);
	}
	else {
		ui.isSaveCheckBox->setChecked(false);
	}

	ui.pathEdit->setText(appCfg->saveImgPath);
	ui.errorDelayEdit->setText(QString::number(appCfg->erroCountDelay,10));
	ui.widthEdit->setText(QString::number(appCfg->width, 10));
	ui.heightEdit->setText(QString::number(appCfg->height, 10));
	ui.capDealyEdit->setText(QString::number(appCfg->captureDelay, 10));
	ui.detectNumEdit->setText(QString::number(appCfg->detectNum, 10));
	if (appCfg->encoder == 2000) {
		ui.radioButton->setChecked(true);
	}
	else if (appCfg->encoder == 1024) {
		ui.radioButton_2->setChecked(true);
	}
	else {
		ui.radioButton_3->setChecked(true);
	}
	//ui.radioButton
}

void SettingDialog::saveSettingBtnClicked()
{
	//没有记录不会保存配置
	QSettings setting("App.ini", QSettings::IniFormat);

	if (setting.contains("app/issave")) {
		bool checked = ui.isSaveCheckBox->isChecked();
		setting.setValue("app/issave", checked); //不存在则创建则赋初值
		//isSaveImg = false;
	}

	if (setting.contains("app/saveimagepath")) {
		QString str = ui.pathEdit->toPlainText();
		setting.setValue("app/saveimagepath", str); //不存在则创建则赋初值
		//saveImgPath = "image\\test\\";
	}

	if (setting.contains("app/erroCountDelay")) {
		QString str = ui.errorDelayEdit->toPlainText();
		setting.setValue("app/erroCountDelay", str.toInt()); //不存在则创建则赋初值	
	}


	if (setting.contains("picture/width")) {
		QString str = ui.widthEdit->toPlainText();
		setting.setValue("picture/width", str.toInt()); //不存在则创建则赋初值
		//setting.setValue("picture/width", 648);	
	}
	

	if (setting.contains("picture/height")) {
		QString str = ui.heightEdit->toPlainText();
		setting.setValue("picture/height", str.toInt()); //不存在则创建则赋初值
		//setting.setValue("picture/width", 648);	
	}
	
	if (setting.contains("picture/captureDelay")) {
		QString str = ui.capDealyEdit->toPlainText();
		setting.setValue("picture/captureDelay", str.toInt()); //不存在则创建则赋初值
		//setting.setValue("picture/width", 648);	
	}

	if (setting.contains("picture/encoder")) {
		//QString str = ui.capDealyEdit->toPlainText();
		if (ui.radioButton->isChecked()) {
			setting.setValue("picture/encoder", 2000); //不存在则创建则赋初值
		}
		else if (ui.radioButton_2->isChecked()) {
			setting.setValue("picture/encoder", 1024); //不存在则创建则赋初值
		}
		else {
			setting.setValue("picture/encoder", 500); //不存在则创建则赋初值
		}
		//setting.setValue("picture/width", 648);	
	}
	

	if (setting.contains("detect/detectmaxnum")) {
		QString str = ui.detectNumEdit->toPlainText();
		setting.setValue("detect/detectmaxnum", str.toInt()); //不存在则创建则赋初值
		//setting.setValue("picture/width", 648);	
	}

	QMessageBox::information(NULL, "Infomation", QStringLiteral("保存成功，重启软件生效！"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
}
void SettingDialog::backBtnClicked() {

	this->hide();
}