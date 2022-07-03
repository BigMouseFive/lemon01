#pragma once
#include <string>
#include <vector>

#define MACHINE_STOP 0
#define MACHINE_PLAY 1
#define MACHINE_PAUSE 2

#define HELPER_GOLDCAR		0
#define HELPER_WHITESHOP	1
#define HELPER_WHITELIST	2
#define HELPER_PRODUCTATTR	3
#define HELPER_CHANGEPRICE	4
#define HELPER_PRODUCTATTR_EXPORT 5

typedef struct _ShopInfo{
	std::string name;
	std::string account;
	std::string password;
	int type;
	int platform; 
	std::string shop_id;
	_ShopInfo(std::string Tname = "", std::string Taccount = "", std::string Tpassword = "", int Ttype = 0, std::string Tshop_id = "", int Tplatform = 0){
		name = Tname;
		account = Taccount;
		password = Tpassword;
		type = Ttype;
		shop_id = Tshop_id;
		platform = Tplatform;
	}
}ShopInfo;
typedef struct _CPAttr{
	std::string shop;
	int minute;
	int max_times;
	double max_percent;
	double percent;
	double lowwer;
	int control;
	std::string my_shop;
	int white_list_enable;
	_CPAttr(){
		shop = "";
		minute = -1;
		max_times = -1;
		max_percent = -1;
		percent = -1;
		lowwer = -1;
		control = -1;
		my_shop = "";
		white_list_enable = -1;
	}
	void Copy(_CPAttr& attr){
		minute = attr.minute;
		max_times = attr.max_times;
		max_percent = attr.max_percent;
		percent = attr.percent;
		lowwer = attr.lowwer;
		my_shop = attr.my_shop;
		white_list_enable = attr.white_list_enable;
	}
}CPAttr;

typedef struct _CPComplexAttr{
	std::string ean;
	double least_price;
	int max_times;
	_CPComplexAttr(std::string ean = "", double least_price = 0, int max_times = 10){
		this->ean = ean;
		this->least_price = least_price;
		this->max_times = max_times;
	}
}CPComplexAttr;


typedef struct _ShopNotice{
	std::string ean;
	std::string variant_name;
	std::string notice;
	int timeStamp;
}ShopNotice;