#pragma once

#include"qtextcodec.h"
#include"qsqldatabase.h"
#include"qsqlerror.h"
#include"qsqldriver.h"
#include"qsettings.h"
#include"qsqldriver.h"
#include"qsqlrecord.h"
#include"qsqlquery.h"

//获取数据库连接

class CDBCUtil
{

public:
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	CDBCUtil();
	~CDBCUtil();
	QSqlDatabase getConn();

	QString driverName;
	QString hostName;
	QString databaseName;
	QString userName;
	QString passWord;
};

