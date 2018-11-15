#include "LabelDAOImpl.h"



LabelDAOImpl::LabelDAOImpl()
{
	//cdbcUtil.getConn();
}


LabelDAOImpl::~LabelDAOImpl()
{
}

void LabelDAOImpl::saveByPrepare(mLabel &label)
{
	QSqlQuery insertQuery(cdbcUtil.getConn());

	insertQuery.prepare("INSERT INTO label1(id, content) VALUES (?, ?)");
	insertQuery.bindValue(0, label.id);
	insertQuery.bindValue(1, label.content);

	insertQuery.exec();

	//cdbcUtil.getConn();
}

void LabelDAOImpl::deleteByPrepare(int id)
{
	QSqlQuery deleteQuery(cdbcUtil.getConn());

	deleteQuery.prepare("DELETE FROM label1 WHERE id=?");
	deleteQuery.bindValue(0, id);
	//deleteQuery.bindValue(1, label.content);
	deleteQuery.exec();
}

QString LabelDAOImpl::getByPrepare(int id)
{
	QSqlQuery selectQuery(cdbcUtil.getConn());

	selectQuery.prepare("SELECT content FROM label1 WHERE id = ?");
	selectQuery.bindValue(0, id);
	//deleteQuery.bindValue(1, label.content);
	selectQuery.exec();
	selectQuery.next();
	return selectQuery.value("content").toString();

}

void LabelDAOImpl::updateByPrepare(int id, mLabel label)
{
	QSqlQuery updateQuery(cdbcUtil.getConn());

	updateQuery.prepare(QStringLiteral("UPDATE label1 SET content = ? WHERE id = ?"));
	updateQuery.bindValue(0, label.content);
	updateQuery.bindValue(1, id);

	//deleteQuery.bindValue(1, label.content);
	updateQuery.exec();
	//selectQuery.next();
	//return selectQuery.value("content").toString();
}

std::vector<QString>* LabelDAOImpl::listByPrepare()
{
	QSqlQuery listQuery(cdbcUtil.getConn());

	listQuery.exec("SELECT * FROM label1");

	std::vector<QString>* tmpVector = new std::vector<QString>();

	while (listQuery.next()) {
		mLabel ltemp;
		ltemp.id = listQuery.value("id").toInt();
		ltemp.content = listQuery.value("content").toString();

		tmpVector->push_back(ltemp.content);
		
	}
	//updateQuery.bindValue(0, label.content);
	//updateQuery.bindValue(1, id);

	//deleteQuery.bindValue(1, label.content);
	return tmpVector;
}
