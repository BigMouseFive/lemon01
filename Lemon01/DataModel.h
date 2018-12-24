#pragma once
#include <string>
#include <vector>
#define MACHINE_STOP 0
#define MACHINE_PLAY 1
#define MACHINE_PAUSE 2
typedef struct _ShopInfo{
	std::string name;
	std::string account;
	std::string password;
	int type;
	_ShopInfo(std::string Tname = "", std::string Taccount = "", std::string Tpassword = "", int Ttype = 0){
		name = Tname;
		account = Taccount;
		password = Tpassword;
		type = Ttype;
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
	void Copy(_CPAttr& attr){
		minute = attr.minute;
		max_times = attr.max_times;
		max_percent = attr.max_percent;
		percent = attr.percent;
		lowwer = attr.lowwer;
		my_shop = attr.my_shop;
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
	std::string notice;
	int timeStamp;
}ShopNotice;