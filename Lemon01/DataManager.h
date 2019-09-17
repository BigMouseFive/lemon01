#pragma once
#include <vector>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include "DataModel.h"
#include <map>
#define SQL_OK 0
#define SQL_NOT_FOUND 1
#define SQL_OPEN_ERROR 2
#define SQL_EXEC_ERROR 3
#define SQL_ATTR_ERROR 4

#define DATABASE_NAME "windows.storage"
#define DATABASE_USER "echizen"
#define DATABASE_PASS "ryoma"
#define DATABASE_ALIAS "global"
class DataManager{
public:
	static DataManager* GetInstance();
	DataManager();
	~DataManager();

	int ConnectDataBase();
	int ConnectDataBase(std::string shopName);
	int InitDataBase();

	//table shop 
	int AddShop(ShopInfo&);
	int DelShop(std::string shopName);
	int GetShops(std::vector<ShopInfo>&, ShopInfo&);

	//table CPAttr 
	int AddCPAttr(CPAttr&);
	int DelCPAttr(std::string shopName);
	int GetCPAttr(CPAttr&, std::string shopName);
	int UpdateControl(int, std::string shopName);
	int UpdateCPAttr(CPAttr&, std::string shopName);
	int UpdateMyShop(std::string my_shop, std::string shopName);

	//table CPComplexAttr
	int AddCPComplexAttr(CPComplexAttr&, std::string shopName);
	int DelCPComplexAttr(std::string, std::string shopName);
	int DelAllCPComplexAttr(std::string shopName);
	int GetCPComplexAttr(std::map<std::string, CPComplexAttr>&, std::string shopName);

	//table whiteList
	int AddWhiteList(std::string ean, std::string shopName, std::string variant_name = "");
	int DelWhiteList(std::string ean, std::string shopName);
	int DelAllWhiteList(std::string shopName);
	int GetWhiteList(std::vector<std::string>&, std::string shopName);

	//table notice
	int GetNotice(std::string shopName, int timeStamp, std::vector<ShopNotice>& vec);
private:
	QSqlDatabase dataBase;
};