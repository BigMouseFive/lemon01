#include "DataManager.h"
#include <QVariant>
#include <QDebug>
//阿联酋	0 
//沙特	1
#define DATABASE_NAME "DataBase.db"
#define DATABASE_USER "echizen"
#define DATABASE_PASS "ryoma"
#define DATABASE_ALIAS "global"
#define CREATE_SHOP "create table if not exists shopInfo (\
					shop text primary key,	\
					account	text not null, \
					password text not null, \
					type int not null,\
					shop_id text not null);"
#define CREATE_CPATTR "create table if not exists CPAttr (\
					shop text primary key,	\
					minute int not null, \
					max_times int not null, \
					max_percent double not null, \
					percent double not null, \
					lowwer double not null,\
					control int not null,\
					my_shop text,\
					white_list_enable int not null default 0,\
					foreign key(shop) references shopInfo(shop) ON UPDATE CASCADE ON DELETE CASCADE);"
#define CREATE_CPCOMPLEXATTR "create table if not exists CPComplexAttr (\
							shop text not null, \
							ean	text not null, \
							least_price	double not null, \
							max_times int not null,\
							primary key(shop,ean),\
							foreign key(shop) references shopInfo(shop) ON UPDATE CASCADE ON DELETE CASCADE);"
#define CREATE_WHITELIST "create table if not exists whiteList (\
							shop text not null, \
							ean	text not null, \
							variant_name text not null default '', \
							primary key(shop,ean), \
							foreign key(shop) references shopInfo(shop) ON UPDATE CASCADE ON DELETE CASCADE);"

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
		auto a = dataBase.lastError().text();
		return SQL_OPEN_ERROR;
	}
	QSqlQuery sql_query(dataBase);
	sql_query.prepare("PRAGMA foreign_keys=ON");
	if (!sql_query.exec()) {
		auto a = sql_query.lastError().text();
		return SQL_EXEC_ERROR;
	}
	return SQL_OK;
}
int DataManager::ConnectDataBase(std::string shopName){
	if (QSqlDatabase::contains(shopName.c_str()))
	{
		dataBase = QSqlDatabase::database(shopName.c_str());
	}
	else
	{
		std::string dataBaseName = "DNCAT/dist/deprecated/" + shopName + ".db";
		dataBase = QSqlDatabase::addDatabase("QSQLITE", shopName.c_str());
		dataBase.setDatabaseName(dataBaseName.c_str());
		dataBase.setUserName(DATABASE_USER);
		dataBase.setPassword(DATABASE_PASS);
	}
	if (!dataBase.open(DATABASE_USER, DATABASE_PASS)){
		auto a = dataBase.lastError().text();
		return SQL_OPEN_ERROR;
	}
	return SQL_OK;
}
int DataManager::InitDataBase(){
	if (ConnectDataBase() != SQL_OK) return SQL_OPEN_ERROR;
	QSqlQuery sql_query(dataBase);
	sql_query.prepare("PRAGMA foreign_keys = ON;");
	if (!sql_query.exec()) {
		auto a = sql_query.lastError().text();
		dataBase.close();
		return SQL_EXEC_ERROR;
	}
	//sql_query.clear();
	sql_query.prepare(CREATE_SHOP);
	if (!sql_query.exec()) {
		auto a = sql_query.lastError().text();
		dataBase.close();
		return SQL_EXEC_ERROR;
	}

	//sql_query.clear();
	sql_query.prepare(CREATE_CPATTR);
	if (!sql_query.exec()) {
		auto a = sql_query.lastError().text();
		dataBase.close();
		return SQL_EXEC_ERROR;
	}

	//sql_query.clear();
	sql_query.prepare(CREATE_CPCOMPLEXATTR);
	if (!sql_query.exec()) {
		auto a = sql_query.lastError().text();
		dataBase.close();
		return SQL_EXEC_ERROR;
	}
	
	//sql_query.clear();
	sql_query.prepare(CREATE_WHITELIST);
	if (!sql_query.exec()) {
		auto a = sql_query.lastError().text();
		dataBase.close();
		return SQL_EXEC_ERROR;
	}
	
	dataBase.close();
	return SQL_OK;
}

//table shop 增删改查
int DataManager::AddShop(ShopInfo& shopInfo){
	if (ConnectDataBase() != SQL_OK) return SQL_OPEN_ERROR;
	QSqlQuery sql_query(dataBase), sql_query2(dataBase);

	sql_query.prepare("select * from shopInfo where shop=?");
	sql_query.addBindValue(shopInfo.name.c_str());
	if (!sql_query.exec()) {
		auto a = sql_query.lastError().text();
		dataBase.close();
		return SQL_EXEC_ERROR;
	}

	if (sql_query.next()) 
		sql_query2.prepare("update shopInfo set account=?,password=?,type=?,shop_id=? where shop=?");
	else
		sql_query2.prepare("insert into shopInfo(account,password,type,shop_id,shop) values(?,?,?,?,?)");
	sql_query2.addBindValue(shopInfo.account.c_str());
	sql_query2.addBindValue(shopInfo.password.c_str());
	sql_query2.addBindValue(shopInfo.type);
	sql_query2.addBindValue(shopInfo.shop_id.c_str());
	sql_query2.addBindValue(shopInfo.name.c_str());
	if (!sql_query2.exec()) {
		auto a = sql_query2.lastError().text();
		dataBase.close();
		return SQL_EXEC_ERROR;
	}

	dataBase.close();
	return SQL_OK;
}
int DataManager::DelShop(std::string shopName){
	if (shopName.empty()) return SQL_ATTR_ERROR;
	if (ConnectDataBase() != SQL_OK) return SQL_OPEN_ERROR;
	QSqlQuery sql_query(dataBase);
	sql_query.prepare("delete from shopInfo where shop=?");
	sql_query.addBindValue(shopName.c_str());
	if (!sql_query.exec()) {
		auto a = sql_query.lastError().text();
		return SQL_EXEC_ERROR;
	}

	dataBase.close();
	return SQL_OK;
}
int DataManager::GetShops(std::vector<ShopInfo>& vec, ShopInfo& cond){
	if (ConnectDataBase() != SQL_OK) return SQL_OPEN_ERROR;
	QSqlQuery sql_query(dataBase);
	QString str = "select shop,account,password,type,shop_id from shopInfo";
	sql_query.prepare(str);
	if (!sql_query.exec()) {
		auto a = sql_query.lastError().text();
		return SQL_EXEC_ERROR;
	}
	while (sql_query.next()){
		ShopInfo info;
		info.name = sql_query.value(0).toString().toStdString();
		info.account = sql_query.value(1).toString().toStdString();
		info.password = sql_query.value(2).toString().toStdString();
		info.type = sql_query.value(3).toInt();
		info.shop_id = sql_query.value(4).toString().toStdString();
		vec.push_back(info);
	}
	dataBase.close();
	return SQL_OK;
}

//table CPAttr 
int DataManager::AddCPAttr(CPAttr& cpAttr){
	if (ConnectDataBase() != SQL_OK) return SQL_OPEN_ERROR;
	QSqlQuery sql_query(dataBase), sql_query2(dataBase);

	sql_query.prepare("select * from CPAttr where shop=?");
	sql_query.addBindValue(cpAttr.shop.c_str());
	if (!sql_query.exec()) {
		auto a = sql_query.lastError().text();
		dataBase.close();
		return SQL_EXEC_ERROR;
	}
	if (sql_query.next())
		sql_query2.prepare("update CPAttr set minute=?,max_times=?,max_percent=?,percent=?,lowwer=?,control=?,my_shop=?,white_list_enable=? where shop=?");
	else
		sql_query2.prepare("insert into CPAttr(minute,max_times,max_percent,percent,lowwer,control,my_shop,white_list_enable,shop) values(?,?,?,?,?,?,?,?,?)");
	sql_query2.addBindValue(cpAttr.minute);
	sql_query2.addBindValue(cpAttr.max_times);
	sql_query2.addBindValue(cpAttr.max_percent);
	sql_query2.addBindValue(cpAttr.percent);
	sql_query2.addBindValue(cpAttr.lowwer);
	sql_query2.addBindValue(cpAttr.control);
	sql_query2.addBindValue(cpAttr.my_shop.c_str());
	sql_query2.addBindValue(cpAttr.white_list_enable);
	sql_query2.addBindValue(cpAttr.shop.c_str());
	if (!sql_query2.exec()) {
		auto a = sql_query2.lastError().text();
		dataBase.close();
		return SQL_EXEC_ERROR;
	}
	dataBase.close();
	return SQL_OK;
}
int DataManager::DelCPAttr(std::string shopName){
	if (shopName.empty()) return SQL_ATTR_ERROR;
	if (ConnectDataBase() != SQL_OK) return SQL_OPEN_ERROR;
	QSqlQuery sql_query(dataBase);

	sql_query.prepare("delete from CPAttr where shop=?");
	sql_query.addBindValue(shopName.c_str());
	if (!sql_query.exec()) {
		auto a = sql_query.lastError().text();
		dataBase.close();
		return SQL_EXEC_ERROR;
	}
	dataBase.close();
	return SQL_OK;
}
int DataManager::GetCPAttr(CPAttr& info, std::string shopName){
	if (ConnectDataBase() != SQL_OK) return SQL_OPEN_ERROR;
	QSqlQuery sql_query(dataBase);
	QString str = "select shop,minute,max_times,max_percent,percent,lowwer,control,my_shop,white_list_enable from CPAttr where shop=?";
	sql_query.prepare(str);
	sql_query.addBindValue(shopName.c_str());
	if (!sql_query.exec()) {
		auto a = sql_query.lastError().text();
		dataBase.close();
		return SQL_EXEC_ERROR;
	}
	int ret = SQL_NOT_FOUND;
	if (sql_query.next()){
		info.shop = sql_query.value(0).toString().toStdString();
		info.minute = sql_query.value(1).toInt();
		info.max_times = sql_query.value(2).toInt();
		info.max_percent = sql_query.value(3).toDouble();
		info.percent = sql_query.value(4).toDouble();
		info.lowwer = sql_query.value(5).toDouble();
		info.control = sql_query.value(6).toInt();
		info.my_shop = sql_query.value(7).toString().toStdString();
		info.white_list_enable = sql_query.value(8).toInt();
		ret = SQL_OK;
	}
	dataBase.close();
	return ret;
}

int DataManager::UpdateControl(int control, std::string shopName){
	if (ConnectDataBase() != SQL_OK) return SQL_OPEN_ERROR;
	QSqlQuery sql_query(dataBase);

	sql_query.prepare("update CPAttr set control=? where shop=?");
	sql_query.addBindValue(control);
	sql_query.addBindValue(shopName.c_str());
	if (!sql_query.exec()) {
		auto a = sql_query.lastError().text();
		dataBase.close();
		return SQL_EXEC_ERROR;
	}
	dataBase.close();
	return SQL_OK;
}
int DataManager::UpdateMyShop(std::string my_shop, std::string shopName){
	if (ConnectDataBase() != SQL_OK) return SQL_OPEN_ERROR;
	QSqlQuery sql_query(dataBase);

	sql_query.prepare("update CPAttr set my_shop=? where shop=?");
	sql_query.addBindValue(my_shop.c_str());
	sql_query.addBindValue(shopName.c_str());
	if (!sql_query.exec()) {
		auto a = sql_query.lastError().text();
		dataBase.close();
		return SQL_EXEC_ERROR;
	}
	dataBase.close();
	return SQL_OK;
}

int DataManager::UpdateCPAttr(CPAttr& info, std::string shopName){
	if (ConnectDataBase() != SQL_OK) return SQL_OPEN_ERROR;
	QSqlQuery sql_query(dataBase);

	sql_query.prepare("update CPAttr set minute=?,max_times=?,max_percent=?,percent=?,lowwer=? where shop=?");
	sql_query.addBindValue(info.minute);
	sql_query.addBindValue(info.max_times);
	sql_query.addBindValue(info.max_percent);
	sql_query.addBindValue(info.percent);
	sql_query.addBindValue(info.lowwer);
	sql_query.addBindValue(shopName.c_str());
	if (!sql_query.exec()) {
		auto a = sql_query.lastError().text();
		dataBase.close();
		return SQL_EXEC_ERROR;
	}
	dataBase.close();
	return SQL_OK;
}


//table CPComplexAttr
int DataManager::AddCPComplexAttr(CPComplexAttr& cpCAttr, std::string shopName){
	if (ConnectDataBase() != SQL_OK) return SQL_OPEN_ERROR;
	QSqlQuery sql_query(dataBase), sql_query2(dataBase);

	sql_query.prepare("select * from CPComplexAttr where shop=? and ean=?");
	sql_query.addBindValue(shopName.c_str());
	sql_query.addBindValue(cpCAttr.ean.c_str());
	if (!sql_query.exec()) {
		auto a = sql_query.lastError().text();
		dataBase.close();
		return SQL_EXEC_ERROR;
	}
	if (sql_query.next())
		sql_query2.prepare("update CPComplexAttr set least_price=?,max_times=? where shop=? and ean=?");
	else
		sql_query2.prepare("insert into CPComplexAttr(least_price,max_times,shop,ean) values(?,?,?,?)");
	sql_query2.addBindValue(cpCAttr.least_price);
	sql_query2.addBindValue(cpCAttr.max_times);
	sql_query2.addBindValue(shopName.c_str());
	sql_query2.addBindValue(cpCAttr.ean.c_str());
	if (!sql_query2.exec()) {
		auto a = sql_query2.lastError().text();
		dataBase.close();
		return SQL_EXEC_ERROR;
	}
	dataBase.close();
	return SQL_OK;
}
int DataManager::DelCPComplexAttr(std::string ean, std::string shopName){
	if (shopName.empty() || ean.empty()) return SQL_ATTR_ERROR;
	if (ConnectDataBase() != SQL_OK) return SQL_OPEN_ERROR;
	QSqlQuery sql_query(dataBase);

	sql_query.prepare("delete from CPComplexAttr where shop=? and ean=?");
	sql_query.addBindValue(shopName.c_str());
	sql_query.addBindValue(ean.c_str());
	if (!sql_query.exec()) {
		auto a = sql_query.lastError().text();
		dataBase.close();
		return SQL_EXEC_ERROR;
	}
	dataBase.close();
	return SQL_OK;
}
int DataManager::DelAllCPComplexAttr(std::string shopName){
	if (shopName.empty()) return SQL_ATTR_ERROR;
	if (ConnectDataBase() != SQL_OK) return SQL_OPEN_ERROR;
	QSqlQuery sql_query(dataBase);

	sql_query.prepare("delete from CPComplexAttr where shop=?");
	sql_query.addBindValue(shopName.c_str());
	if (!sql_query.exec()) {
		auto a = sql_query.lastError().text();
		dataBase.close();
		return SQL_EXEC_ERROR;
	}
	dataBase.close();
	return SQL_OK;
}
int DataManager::GetCPComplexAttr(std::map<std::string, CPComplexAttr>& vec, std::string shopName){
	if (ConnectDataBase() != SQL_OK) return SQL_OPEN_ERROR;
	QSqlQuery sql_query(dataBase);
	QString str = "select ean,least_price,max_times from CPComplexAttr where shop=?";
	sql_query.prepare(str);
	sql_query.addBindValue(shopName.c_str());
	if (!sql_query.exec()) {
		auto a = sql_query.lastError().text();
		dataBase.close();
		return SQL_EXEC_ERROR;
	}
	vec.clear();
	while (sql_query.next()){
		CPComplexAttr info;
		info.ean = sql_query.value(0).toString().toStdString();
		info.least_price = sql_query.value(1).toInt();
		info.max_times = sql_query.value(2).toInt();
		vec[info.ean] = info;
	}
	dataBase.close();
	return SQL_OK;
}

//table whiteList
int DataManager::AddWhiteList(std::string ean, std::string shopName, std::string variant_name){
	if (ConnectDataBase() != SQL_OK) return SQL_OPEN_ERROR;
	QSqlQuery sql_query(dataBase), sql_query2(dataBase);

	sql_query.prepare("select * from whiteList where shop=? and ean=?");
	sql_query.addBindValue(shopName.c_str());
	sql_query.addBindValue(ean.c_str());
	if (!sql_query.exec()) {
		auto a = sql_query.lastError().text();
		dataBase.close();
		return SQL_EXEC_ERROR;
	}
	if (sql_query.next())
		sql_query2.prepare("update whiteList set variant_name=? where shop=? and ean=?");
	else
		sql_query2.prepare("insert into whiteList(variant_name,shop,ean) values(?,?,?)");
	sql_query2.addBindValue(variant_name.c_str());
	sql_query2.addBindValue(shopName.c_str());
	sql_query2.addBindValue(ean.c_str());
	if (!sql_query2.exec()) {
		auto a = sql_query2.lastError().text();
		dataBase.close();
		return SQL_EXEC_ERROR;
	}
	dataBase.close();
	return SQL_OK;
}
int DataManager::DelWhiteList(std::string ean, std::string shopName){
	if (shopName.empty() || ean.empty()) return SQL_ATTR_ERROR;
	if (ConnectDataBase() != SQL_OK) return SQL_OPEN_ERROR;
	QSqlQuery sql_query(dataBase);

	sql_query.prepare("delete from whiteList where shop=? and ean=?");
	sql_query.addBindValue(shopName.c_str());
	sql_query.addBindValue(ean.c_str());
	if (!sql_query.exec()) {
		auto a = sql_query.lastError().text();
		dataBase.close();
		return SQL_EXEC_ERROR;
	}
	dataBase.close();
	return SQL_OK;
}
int DataManager::DelAllWhiteList(std::string shopName){
	if (shopName.empty()) return SQL_ATTR_ERROR;
	if (ConnectDataBase() != SQL_OK) return SQL_OPEN_ERROR;
	QSqlQuery sql_query(dataBase);

	sql_query.prepare("delete from whiteList where shop=?");
	sql_query.addBindValue(shopName.c_str());
	if (!sql_query.exec()) {
		auto a = sql_query.lastError().text();
		dataBase.close();
		return SQL_EXEC_ERROR;
	}
	dataBase.close();
	return SQL_OK;
}
int DataManager::GetWhiteList(std::vector<std::string>& vec, std::string shopName){
	if (ConnectDataBase() != SQL_OK) return SQL_OPEN_ERROR;
	QSqlQuery sql_query(dataBase);
	QString str = "select ean from whiteList where shop=?";
	sql_query.prepare(str);
	sql_query.addBindValue(shopName.c_str());
	if (!sql_query.exec()) {
		auto a = sql_query.lastError().text();
		dataBase.close();
		return SQL_EXEC_ERROR;
	}
	vec.clear();
	while (sql_query.next()){
		vec.push_back(sql_query.value(0).toString().toStdString());
	}
	dataBase.close();
	return SQL_OK;
}

int DataManager::GetNotice(std::string shopName, int timeStamp, std::vector<ShopNotice>& vec){
	if (ConnectDataBase(shopName) != SQL_OK) return SQL_OPEN_ERROR;
	QSqlQuery sql_query(dataBase);
	QString str = "select ean,memo,stamp from notice where stamp>?";
	sql_query.prepare(str);
	sql_query.addBindValue(timeStamp);
	if (!sql_query.exec()) {
		auto a = sql_query.lastError().text();
		return SQL_EXEC_ERROR;
	}
	while (sql_query.next()){
		ShopNotice info;
		info.ean = sql_query.value(0).toString().toStdString();
		info.notice = sql_query.value(1).toString().toStdString();
		info.timeStamp = sql_query.value(2).toInt();
		vec.push_back(info);
	}
	dataBase.close();
	return SQL_OK;
}
