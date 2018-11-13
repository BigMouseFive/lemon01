#pragma once
#include <string>
#include <vector>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#define SQL_OK 0
#define SQL_NOT_FOUND 1
#define SQL_OPEN_ERROR 2
#define SQL_EXEC_ERROR 3
typedef struct _ShopInfo{
	std::string name;
	std::string account;
	std::string password;
	_ShopInfo(std::string Tname = "", std::string Taccount = "", std::string Tpassword = ""){
		name = Tname;
		account = Taccount;
		password = Tpassword;
	}
}ShopInfo;

class DataManager{
public:
	static DataManager* GetInstance();
	DataManager();
	~DataManager();

	int ConnectDataBase();
	int ConnectDataBase(std::string shopName);
	int InitDataBase();

	//table shop 
	int AddShop(ShopInfo& shopInfo);
	int DelShop(std::string shopName);
	int GetShops(std::vector<ShopInfo>& vec, ShopInfo& cond);

	//table CPAttr 
	
	//table CPComplexAttr

private:
	QSqlDatabase dataBase;
};