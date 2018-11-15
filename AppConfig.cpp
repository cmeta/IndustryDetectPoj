#include "AppConfig.h"



AppConfig::AppConfig()
{
}


AppConfig::~AppConfig()
{
}

AppConfig* AppConfig::appConfig = NULL;

AppConfig * AppConfig::getInstance()
{
	if (appConfig == NULL) {
		appConfig = new AppConfig();
	}
	return appConfig;
}

void AppConfig::initSetting(QString &set)
{
	QSettings setting(set, QSettings::IniFormat);

	if (!setting.contains("app/issave")) {
		setting.setValue("app/issave", 0); //不存在则创建则赋初值
		isSaveImg = false;
	}
	else
		isSaveImg = setting.value("app/issave").toBool();

	if (!setting.contains("app/saveimagepath")) {
		setting.setValue("app/saveimagepath", "image\\test\\"); //不存在则创建则赋初值
		saveImgPath = "image\\test\\";
	}
	else
		saveImgPath = setting.value("app/saveimagepath").toString();
	
	if (!setting.contains("app/erroCountDelay")) {
		setting.setValue("app/erroCountDelay", 0); //不存在则创建则赋初值
		erroCountDelay = 0;
	}
	else
		erroCountDelay = setting.value("app/erroCountDelay").toInt();


	if (!setting.contains("picture/width")) {
		setting.setValue("picture/width", 648); //不存在则创建则赋初值
		//setting.setValue("picture/width", 648);	
		width = 648;
	}
	else
		width = setting.value("picture/width").toInt();

	if (!setting.contains("picture/height")) {
		setting.setValue("picture/height", 648); //不存在则创建则赋初值
		//setting.setValue("picture/width", 648);	
		height = 648;
	}
	else
		height = setting.value("picture/height").toInt();

	if (!setting.contains("picture/captureDelay")) {
		setting.setValue("picture/captureDelay", 0); //不存在则创建则赋初值
		//setting.setValue("picture/width", 648);	
		captureDelay = 0;
	}
	else
		captureDelay = setting.value("picture/captureDelay").toInt();


	if (!setting.contains("picture/encoder")) {
		setting.setValue("picture/encoder", 0); //不存在则创建则赋初值
		//setting.setValue("picture/width", 648);	
		encoder = 0;
	}
	else
		encoder = setting.value("picture/encoder").toInt();

	if (!setting.contains("detect/detectmaxnum")) {
		setting.setValue("detect/detectmaxnum", 3); //不存在则创建则赋初值
		//setting.setValue("picture/width", 648);	
		detectNum = 3;
	}
	else
		detectNum = setting.value("detect/detectmaxnum").toInt();


}
