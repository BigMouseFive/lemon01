#include "DataManager.h"
#include <QVariant>
#include <QDebug>
#define DATABASE_NAME "DataBase.db"
#define DATABASE_USER "echizen"
#define DATABASE_PASS "ryoma"
#define DATABASE_ALIAS "global"
#define CREATE_SHOP "create table if not exists shopInfo (\
					shop text primary key,	\
					account	text not null, \
					password text not null);"
#define CREATE_CPATTR "create table if not exists CPAttr (\
					shop text primary key,	\
					minute int not null, \
					max_times int not null, \
					max_percent double not null, \
					percent double not null, \
					lowwer double not null,\
					foreign key(shop) reffrences shopInfo(shop));"
#define CREATE_CPCOMPLEXATTR "create table if not exists CPComplexAttr (\
							shop text not null, \
							ean	text not null, \
							least_price	text not null, \
							max_times text not null,\
							primary key(shop,ean),\
							foreign key(shop) reffrences shopInfo(shop)););"


DataManager* DataManager::GetInstance(){
	static DataManager* instance;
	if (instance == nullptr)
		instance = new DataManager();
	return instance;
}
DataManager::DataManager(){
	InitDataBase();
}
DataManager::~DataManager(){

}
int DataManager::ConnectDataBase(){
	if (QSqlDatabase::contains(DATABASE_ALIAS))
	{
		dataBase = QSqlDatabase::database(DATABASE_ALIAS);
	}
	else
	{
		dataBase = QSqlDatabase::addDatabase("QSQLITE", DATABASE_ALIAS);
		dataBase.setDatabaseName(DATABASE_NAME);
		dataBase.setUserName(DATABASE_USER);
		dataBase.setPassword(DATABASE_PASS);
	}
	if (!dataBase.open(DATABASE_USER, DATABASE_PASS)){
		auto a = dataBase.lastError();
		auto b = a.text();
		return SQL_OPEN_ERROR;
	}
	return SQL_OK;
}
int DataManager::ConnectDataBase(std::string shopName){
	if (QSqlDatabase::contains(shopName.c_str()))
	{
		dataBase = QSqlDatabase::database();
	}
	else
	{
		std::string dataBaseName = shopName + ".db";
		dataBase = QSqlDatabase::addDatabase("QSQLITE", shopName.c_str());
		dataBase.setDatabaseName(dataBaseName.c_str());
		dataBase.setUserName(DATABASE_USER);
		dataBase.setPassword(DATABASE_PASS);
	}
	return SQL_OK;
}
int DataManager::InitDataBase(){
	if (ConnectDataBase() != SQL_OK) return SQL_OPEN_ERROR;
	QSqlQuery sql_query;
	sql_query.prepare("PRAGMA foreign_keys = ON;");
	if (!sql_query.exec()) return SQL_EXEC_ERROR;

	sql_query.prepare(CREATE_SHOP);
	if (!sql_query.exec()) return SQL_EXEC_ERROR;

	sql_query.prepare(CREATE_CPATTR);
	if (!sql_query.exec()) return SQL_EXEC_ERROR;

	sql_query.prepare(CREATE_CPCOMPLEXATTR);
	if (!sql_query.exec()) return SQL_EXEC_ERROR; 
	
	dataBase.close();
	return SQL_OK;
}

//table shop ÔöÉ¾¸Ä²é
int DataManager::AddShop(ShopInfo& shopInfo){
	if (ConnectDataBase() != SQL_OK) return SQL_OPEN_ERROR;
	QSqlQuery sql_query, sql_query2;

	sql_query.prepare("select * from shopInfo where shop=?");
	sql_query.addBindValue(shopInfo.name.c_str());
	if (!sql_query.exec()) return SQL_EXEC_ERROR;

	if (sql_query.next()) 
		sql_query2.prepare("update shopInfo set account=?,password=? where shop=?");
	else
		sql_query2.prepare("insert into shopInfo(account,password,shop) values(?,?,?");
	sql_query2.addBindValue(shopInfo.account.c_str());
	sql_query2.addBindValue(shopInfo.password.c_str());
	sql_query2.addBindValue(shopInfo.name.c_str());
	if (!sql_query2.exec()) return SQL_EXEC_ERROR;

	dataBase.close();
	return SQL_OK;
}
int DataManager::DelShop(std::string shopName){
	if (ConnectDataBase() != SQL_OK) return SQL_OPEN_ERROR;
	QSqlQuery sql_query;

	sql_query.prepare("delete from shopInfo where shop=?");
	sql_query.addBindValue(shopName.c_str());
	if (!sql_query.exec()) return SQL_EXEC_ERROR;

	dataBase.close();
	return SQL_OK;
}
int DataManager::GetShops(std::vector<ShopInfo>& vec, ShopInfo& cond){
	if (ConnectDataBase() != SQL_OK) return SQL_OPEN_ERROR;
	QSqlQuery sql_query; 
	QString str = "select name,account,password from shopInfo";
	bool start = true;
	if (!cond.name.empty()){
		if (start) str += " where";
		else str += " and";
		str += " shop=?";
	}
	if (!cond.account.empty()){
		if (start) str += " where";
		else str += " and";
		str += " account=?";
	}
	if (!cond.password.empty()){
		if (start) str += " where";
		else str += " and";
		str += " password=?";
	}
	sql_query.prepare(str);
	if (!sql_query.exec()) return SQL_EXEC_ERROR;
	while (sql_query.next()){
		ShopInfo info;
		info.name = sql_query.value(0).toString().toStdString();
		info.account = sql_query.value(1).toString().toStdString();
		info.password = sql_query.value(2).toString().toStdString();
	}
	dataBase.close();
	return SQL_OK;
}

//table CPAttr 
//table CPComplexAttr