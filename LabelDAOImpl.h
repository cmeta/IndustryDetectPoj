#pragma once
#include<CDBCUtil.h>
#include<Label.h>
#include"CDBCUtil.h"
#include<vector>
//数据库连接后 对象的增删改查

class LabelDAOImpl
{
public:
	CDBCUtil cdbcUtil;
	//QSqlQuery query;
	
	LabelDAOImpl();
	~LabelDAOImpl();
	//保存label 对象
	void saveByPrepare(mLabel &label);
	void deleteByPrepare(int id);
	QString getByPrepare(int id);
	void updateByPrepare(int id, mLabel label);
	std::vector<QString>* listByPrepare();


};

