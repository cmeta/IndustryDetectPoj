#pragma once
#include<iostream>
#include<qsettings.h>
#include<QString>


class AppConfig
{
private:
	AppConfig();
	~AppConfig();
	AppConfig(const AppConfig&);
	AppConfig& operator=(const AppConfig&);
	static AppConfig* appConfig;

public:

	static AppConfig*getInstance();
	//QSettings setting
	class DelIns {

		~DelIns() {
			if (appConfig != NULL) {
				delete appConfig;
				appConfig = NULL;
			}
		}
	};
	static DelIns delIns;

	void initSetting(QString &); //初始化设置
	//设置的相关变量  
	bool isSaveImg=false;  //是否保存图像
	int width=0;		//图像的宽和高
	int height=0;
	int detectNum=0;  //检测的数量
	int captureDelay=0;  //采集图像延时
	int erroCountDelay=0;		//错误数量延时
	QString saveImgPath;	//图像保存路径
	int encoder = 0;
};

