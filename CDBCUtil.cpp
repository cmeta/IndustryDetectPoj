#include "CDBCUtil.h"

CDBCUtil::CDBCUtil()
{

	QSettings setting("db.ini", QSettings::IniFormat);

	if (!setting.contains("dbSettings/driverName")) {
		setting.setValue("dbSettings/driverName", "QSQLITE"); //不存在则创建则赋初值
		//isSaveImg = false;
	}
	else
		driverName = setting.value("dbSettings/driverName").toString();	
	
	if (!setting.contains("dbSettings/hostName")) {
		setting.setValue("dbSettings/hostName", 0); //不存在则创建则赋初值
		//isSaveImg = false;
	}
	else
		hostName = setting.value("dbSettings/hostName").toString();

	if (!setting.contains("dbSettings/databaseName")) {
		setting.setValue("dbSettings/databaseName", "data.db"); //不存在则创建则赋初值
		//isSaveImg = false;
	}
	else
		databaseName = setting.value("dbSettings/databaseName").toString();	
	

	if (!setting.contains("dbSettings/userName")) {
		setting.setValue("dbSettings/userName", 0); //不存在则创建则赋初值
		//isSaveImg = false;
	}
	else
		userName = setting.value("dbSettings/userName").toString();	
	
	if (!setting.contains("dbSettings/passWord")) {
		setting.setValue("dbSettings/passWord", 0); //不存在则创建则赋初值
		//isSaveImg = false;
	}
	else
		passWord = setting.value("dbSettings/passWord").toString();

	
	//  sqlitedata.db 是 SQLite 的数据库文件名
	//    如果此文件不存在则自动创建，如果已经存在则使用已经存在的文件
	if (hostName != "0") {
		db.setHostName(hostName);
	}

	if (databaseName != "0") {
		db.setDatabaseName(databaseName);
	}

	if (userName != "0") {
		db.setUserName(userName);
	}
	if (passWord != "0") {
		db.setPassword(passWord);
	}

	db.open();

}


CDBCUtil::~CDBCUtil()
{
}

QSqlDatabase CDBCUtil::getConn()
{
	if (driverName == "0") {
		return QSqlDatabase();
	}
	return db;
}
