#include "utils.h"
#include "MacAddress.h"
#include "lemon01.h"
#include <QFile>
#include <QDomDocument>
#include <QXmlStreamWriter>
#include <qmessagebox.h>
#include <QTextStream>
#include <QNetworkInterface>
#include <QList>
#include <fstream>
#include <locale>
#include <QInputDialog>
#include <QDialogButtonBox>
#include <QTimer>
#define FILE_PATH "lemon.xml"

using namespace std;
string getHostMacAddress()
{
	std::vector<string> vtMacAddress;
	CTemporary temporary;
	temporary.GetMacAddress(vtMacAddress);
	return vtMacAddress[0];
}
int Lemon01::aaaab(){
	
	QMessageBox msg;
	msg.setWindowTitle(QStringLiteral("注册"));
	msg.setText(QStringLiteral("对不起，您输入的注册码不正确"));
	msg.setIcon(QMessageBox::NoIcon);
	msg.setWindowIcon(QIcon(":/Lemon01/lemon.png"));
	msg.addButton(QStringLiteral("确定"), QMessageBox::ActionRole);
	//读取文件currentdir\\DNCAT\\ulfkeileaif.dat
	memset(currentDir, 0, sizeof(currentDir));
	GetModuleFileNameA(NULL, currentDir, 200);
	int i = 0;
	int j = 0;
	while (currentDir[i] != '\0'){
		if (currentDir[i] == '\\'){
			currentDir[i] = '/';
			j = i;
		}
		i++;
	}
	currentDir[j] = '\0';
	fstream file;
	memset(input, 0, sizeof(input));
	memset(filepath, 0, sizeof(filepath));
	strcat(filepath, currentDir);
	strcat(filepath, "/DNCAT");
	strcat(filepath, "/ulfkeileaif.pyc");
	::locale loc = ::locale::global(::locale(""));
	file.open(filepath, ios::in);
	bool fileExist = false;
	if (file){
		fileExist = true;
		file.read(input, sizeof(input));
		string input_tmp(input);
		auto iter = input_tmp.begin();
		for (; iter != input_tmp.end();)
		{
			if (*iter == ' ' || *iter == '-')
				iter = input_tmp.erase(iter);
			else{
				if (*iter >= 'a' && *iter <= 'z')
					*iter = *iter - ('a' - 'A');
				iter++;
			}
		}
		memset(input, 0, sizeof(input));
		strncpy(input, input_tmp.c_str(), sizeof(input));
		if (strlen(input) == 32){
			int tmp = 0;
			for (int i = 0; i < 32; i++){

				if (input[i] >= '0' && input[i] <= '9')	input[i] = input[i] - '0';
				else if (input[i] >= 'A' && input[i] <= 'Z') input[i] = input[i] - 'A' + 10;
			}
			{
				memset(endTime, 0, sizeof(endTime));
				endTime[4] = input[2] / 3;
				endTime[5] = (input[0] - 3) / 2;
				endTime[6] = input[31];
				endTime[7] = (input[26] - 4) / 2;
				endTime[1] = (input[25] - endTime[7]) / 2;
				endTime[8] = input[15] - 4;
				endTime[3] = (input[20] - endTime[8]) / 2;
				endTime[9] = input[28] - 7;
				endTime[2] = input[23] - endTime[9];
				endTime[10] = input[11] - endTime[5];
				endTime[11] = (input[6] - 2) / 2;
				endTime[0] = input[29] - endTime[3] - endTime[11];
				endtime = 0;
				for (int j = 0; j <= 9; ++j){
					endtime = endtime * 10 + endTime[j];
				}
				endtime = endtime + 1996111725;
				total = 0;
				for (int j = 10; j <= 11; ++j){
					total = total * 10 + endTime[j];
				}
				hide[2] = input[2];
				hide[0] = input[0];
				hide[31] = input[31];
				hide[26] = input[26];
				hide[25] = input[25];
				hide[15] = input[15];
				hide[20] = input[20];
				hide[28] = input[28];
				hide[23] = input[23];
				hide[11] = input[11];
				hide[6] = input[6];
				hide[29] = input[29];
				hide[1] = _hide[1] + endTime[5];
				hide[3] = _hide[3] + endTime[6];
				hide[4] = _hide[4] + endTime[7];
				hide[5] = _hide[5] + endTime[8];
				hide[7] = _hide[7] + endTime[9];
				hide[8] = _hide[8] + endTime[6];
				hide[9] = _hide[9] + endTime[7];
				hide[10] = _hide[10] + endTime[8];
				hide[12] = _hide[12] + endTime[8];
				hide[13] = _hide[13] + endTime[5];
				hide[14] = _hide[14] + endTime[7];
				hide[16] = _hide[16] + endTime[9];
				hide[17] = _hide[17] + endTime[7];
				hide[18] = _hide[18] + endTime[6];
				hide[19] = _hide[19] + endTime[6];
				hide[21] = _hide[21] + endTime[5];
				hide[22] = _hide[22] + endTime[8];
				hide[24] = _hide[24] + endTime[9];
				hide[27] = _hide[27] + endTime[7];
				hide[30] = _hide[30] + endTime[5];
				for (int i = 0; i < 32; ++i)
					hide[i] %= 36;
			}
			{
				for (int i = 0; i < 32; i++){
					if (input[i] != hide[i]){
						file.close();
						::DeleteFile(ATL::CA2T(filepath));
						int ret = 
						fileExist = false;
						break;
					}
				}
			}
			if (fileExist){
				file.close();
				SlotCompareTime();
			}
		}
		else{
			file.close();
			::DeleteFile(ATL::CA2T(filepath));
			fileExist = false;
		}
	}
	if (!fileExist){
		string str("");
		char _input[40];
		char c;
		for (i = 0; i < 20; ++i){
			if (_show[i] >= 0 && _show[i] <= 9) c = '0' + _show[i];
			else c = 'A' + _show[i] - 10;
			str.push_back(c);
		}
		bool isOk;
		QString qstr = QStringLiteral("序列号：") + QString::fromStdString(str) + "\n";
		QInputDialog *inputDialog = new QInputDialog();
		inputDialog->setWindowTitle(QStringLiteral("注册"));
		inputDialog->setLabelText(qstr);
		inputDialog->setOkButtonText(QStringLiteral("确定"));
		inputDialog->setCancelButtonText(QStringLiteral("取消"));
		inputDialog->setWindowIcon(QIcon(":/Lemon01/lemon.png"));
		inputDialog->setFixedSize(400, 300);
		QString text;
		if (inputDialog->exec()) {
			text = inputDialog->textValue();
			strncpy(input, text.toStdString().c_str(), 200);
		}
		else{
			exit(0);
			return 0;
		}string input_tmp(input);
		auto iter = input_tmp.begin();
		for (; iter != input_tmp.end();)
		{
			if (*iter == ' ' || *iter == '-')
				iter = input_tmp.erase(iter);
			else{
				if (*iter >= 'a' && *iter <= 'z')
					*iter = *iter - ('a' - 'A');
				iter++;
			}
		}
		memset(input, 0, sizeof(input));
		strncpy(input, input_tmp.c_str(), sizeof(input));
		if (strlen(input) != 32){
			msg.exec();
			exit(0);
			return 0;
		}
		{
			int tmp = 0;
			for (int i = 0; i < 32; i++){
				_input[i] = input[i];
				if (input[i] >= '0' && input[i] <= '9')	input[i] = input[i] - '0';
				else if (input[i] >= 'A' && input[i] <= 'Z') input[i] = input[i] - 'A' + 10;
			}
			{
				memset(endTime, 0, sizeof(endTime));
				endTime[4] = input[2] / 3;
				endTime[5] = (input[0] - 3) / 2;
				endTime[6] = input[31];
				endTime[7] = (input[26] - 4) / 2;
				endTime[1] = (input[25] - endTime[7]) / 2;
				endTime[8] = input[15] - 4;
				endTime[3] = (input[20] - endTime[8]) / 2;
				endTime[9] = input[28] - 7;
				endTime[2] = input[23] - endTime[9];
				endTime[10] = input[11] - endTime[5];
				endTime[11] = (input[6] - 2) / 2;
				endTime[0] = input[29] - endTime[3] - endTime[11];
				endtime = 0;
				for (int j = 0; j <= 9; ++j){
					endtime = endtime * 10 + endTime[j];
				}
				endtime = endtime + 1996111725;
				total = 0;
				for (int j = 10; j <= 11; ++j){
					total = total * 10 + endTime[j];
				}
				hide[2] = input[2];
				hide[0] = input[0];
				hide[31] = input[31];
				hide[26] = input[26];
				hide[25] = input[25];
				hide[15] = input[15];
				hide[20] = input[20];
				hide[28] = input[28];
				hide[23] = input[23];
				hide[11] = input[11];
				hide[6] = input[6];
				hide[29] = input[29];
				hide[1] = _hide[1] + endTime[5];
				hide[3] = _hide[3] + endTime[6];
				hide[4] = _hide[4] + endTime[7];
				hide[5] = _hide[5] + endTime[8];
				hide[7] = _hide[7] + endTime[9];
				hide[8] = _hide[8] + endTime[6];
				hide[9] = _hide[9] + endTime[7];
				hide[10] = _hide[10] + endTime[8];
				hide[12] = _hide[12] + endTime[8];
				hide[13] = _hide[13] + endTime[5];
				hide[14] = _hide[14] + endTime[7];
				hide[16] = _hide[16] + endTime[9];
				hide[17] = _hide[17] + endTime[7];
				hide[18] = _hide[18] + endTime[6];
				hide[19] = _hide[19] + endTime[6];
				hide[21] = _hide[21] + endTime[5];
				hide[22] = _hide[22] + endTime[8];
				hide[24] = _hide[24] + endTime[9];
				hide[27] = _hide[27] + endTime[7];
				hide[30] = _hide[30] + endTime[5];
				for (int i = 0; i < 32; ++i)
					hide[i] %= 36;
			}
			{
				for (int i = 0; i < 32; i++){
					if (input[i] != hide[i]){
						msg.exec();
						exit(0);
						return 0;
					}
				}
			}
			SlotCompareTime();
		}
		file.open(filepath, ios::out|ios::trunc);
		if (!file)
			printf("\n----------保存购买序列号出错-----------\n");
		else{
			_input[32] = '\0';
			file.write(_input, strlen(_input));
			file.close();
		}
		msg.setText(QStringLiteral("欢迎使用自动改价控制系统"));
		msg.exec();
	}
	
	locale::global(loc);
	return 1;
}
Lemon01::Lemon01(QWidget *parent)
	: QWidget(parent)
	, lastret(999)
{
	ui.setupUi(this);
	readXml();
	initList();
	setEmpty();
	ui.listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	delAct = new QAction(QStringLiteral("删除"), this);
	closeAct = new QAction(QStringLiteral("关闭"), this);
	
	{
		connect(delAct, SIGNAL(triggered(bool)), this, SLOT(SlotDelAct(bool)));
		connect(closeAct, SIGNAL(triggered(bool)), this, SLOT(SlotCloseAct(bool)));
		connect(ui.btnNewMachine, SIGNAL(clicked()), this, SLOT(SlotNewAutoMachine()));
	}

	{
		std::string macadddress = ::getHostMacAddress();
		int i = 0;
		int tmp = 0;
		for (auto it : macadddress){
			if (it == ':') continue;
			i++;
			if (it >= '0' && it <= '9')	tmp = it - '0';
			else if (it >= 'A' && it <= 'Z') tmp = it - 'A' + 10;
			switch (i){
			case 1:  _show[7] = tmp; hide[7] = tmp; break;//A
			case 2:  _show[14] = tmp; hide[14] = tmp; break;//B
			case 3:  _show[12] = tmp; hide[12] = tmp; break;//C
			case 4:  _show[1] = tmp; hide[1] = tmp; break;//D
			case 5:  _show[5] = tmp; hide[5] = tmp; break;//E
			case 6:  _show[4] = tmp; hide[4] = tmp; break;//F
			case 7:  _show[9] = tmp; hide[9] = tmp; break;//G
			case 8:  _show[19] = tmp; hide[19] = tmp; break;//H
			case 9:  _show[17] = tmp; hide[17] = tmp; break;//I
			case 10: _show[3] = tmp; hide[3] = tmp; break;//J
			case 11: _show[16] = tmp; hide[16] = tmp; break;//K
			case 12: _show[10] = tmp; hide[10] = tmp; break;//L
			}
		}
		_show[0] = _show[9] * 2 + _show[17];
		_show[2] = _show[10] * 3 + _show[12];
		_show[6] = _show[4] + 8 + _show[9];
		_show[8] = _show[16] * 4 + _show[17];
		_show[11] = _show[14] * 4 + _show[12];
		_show[13] = _show[17] * 5 + _show[5];
		_show[15] = _show[5] + 8 + _show[16];
		_show[18] = _show[3] * 3 + _show[7];
		
		_show[1] = _show[1] + 9;
		_show[3] = _show[3] + 4;
		_show[4] = _show[4] + 4;
		_show[5] = _show[5] * 2;
		_show[7] = _show[7] + 5;
		_show[9] = _show[9] + 13;
		_show[10] = _show[10] + 1;
		_show[12] = _show[12] + 4;
		_show[14] = _show[14] + 6;
		_show[16] = _show[16] + 3;
		_show[19] = _show[19] * 2;

		hide[0] = hide[19] * 2 + hide[1];
		hide[2] = hide[1] * 3 + hide[12];
		hide[6] = hide[16] + 8 + hide[3];
		hide[8] = hide[1] * 4 + hide[5];
		hide[11] = hide[4] * 4 + hide[19];
		hide[13] = hide[10] + 5 + hide[4];
		hide[15] = hide[14] + 8 + hide[12];
		hide[18] = hide[1] * 3 + hide[7];
		hide[20] = hide[7] * hide[12] + hide[1];
		hide[21] = hide[14] * hide[1] + hide[4];
		hide[22] = hide[17] * hide[3] + hide[19];
		hide[23] = hide[12] * hide[16] + hide[5];
		hide[24] = hide[1] * hide[5] + hide[9];
		hide[25] = hide[7] * hide[16] + hide[10];
		hide[26] = hide[3] * hide[1] + hide[17];
		hide[27] = hide[9] * hide[4] + hide[7];
		hide[28] = hide[4] * hide[16] + hide[12];
		hide[29] = hide[9] * hide[3] + hide[14];
		hide[30] = hide[19] * hide[10] + hide[1];
		hide[31] = hide[19] * hide[9] + hide[4];

		hide[7] += 4; hide[14] += 3; hide[12] += 6;
		hide[1] += 8; hide[5] += 4; hide[4] *= 6;
		hide[9] += 7; hide[19] += 8; hide[17] += 6;
		hide[3] *= 7; hide[16] += 2; hide[10] += 5;
		for (i = 0; i < 20; ++i)
			_show[i] %= 36;
		for (i = 0; i < 32; ++i){
			_hide[i] = hide[i];
		}
		//aaaab();
	}

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(SlotCompareTime()));
	timer->start(1000*60*10);
}
Lemon01::~Lemon01()
{
	QuitThisSystem();
}
void Lemon01::setEmpty(){
	ui.accountline->setText("");
	ui.passwordline->setText("");
	ui.lowwerspin->setValue(0);
	ui.percentspin->setValue(10);
	ui.times->setValue(5);
	ui.upload->setValue(15);
	ui.time->setValue(0);
	ui.mode->setCurrentIndex(0);
}
void Lemon01::initList(){
	QStringList list;
	for (auto it = infoMap.begin(); it != infoMap.end(); ++it){
		list.append(QString::fromStdString(it->second.name));
	}
	list.append(QStringLiteral("新账号"));
	ui.listWidget->addItems(list);
	for (int i = 0; i < ui.listWidget->count() - 1; ++i){
		QPixmap pix(":/Lemon01/shop");
		ui.listWidget->item(i)->setIcon(QIcon(pix.scaled(20,20)));
	}
	connect(ui.listWidget, SIGNAL(currentItemChanged(QListWidgetItem *,QListWidgetItem *)),
		this, SLOT(SlotListItemClicked0(QListWidgetItem *, QListWidgetItem *)));
	connect(ui.listWidget, SIGNAL(itemClicked(QListWidgetItem *)),
		this, SLOT(SlotListItemClicked(QListWidgetItem *)));
	connect(ui.listWidget, SIGNAL(customContextMenuRequested(const QPoint&)), 
		this, SLOT(SlotListContextRequested(const QPoint&)));

}
void Lemon01::QuitThisSystem(){
	for (auto iter = infoMap.begin(); iter != infoMap.end(); ++iter){
		selectName = iter->first;
		SlotCloseAct(false);
	}
}

void Lemon01::SlotCompareTime(){
	int ret = ::CompareTime(endtime);
	QMessageBox msg;
	msg.setWindowTitle(QStringLiteral("通知"));
	msg.setText(QStringLiteral("对不起，您输入的注册码不正确"));
	msg.setIcon(QMessageBox::NoIcon);
	msg.setWindowIcon(QIcon(":/Lemon01/lemon.png"));
	msg.addButton(QStringLiteral("确定"), QMessageBox::ActionRole);
	if (ret == 300){
		//没有获取到时间
		msg.setText(QStringLiteral("对不起，没有成功获取您计算机的时间\n请联系我们技术人员，造成了不便，请多包涵"));
		msg.exec();
		QuitThisSystem();
		exit(0);
	}
	else if (ret == 301){
		//正常  没有超过使用期
	}
	else if (ret == 302){
		//超出使用期48小时
		msg.setText(QStringLiteral("对不起，您已超过使用期限48小时了，无法再使用此软件"));
		msg.exec();
		::DeleteFile(ATL::CA2T(filepath));
		QuitThisSystem();
		exit(0);
	}
	else{
		//超出使用期 但是在48小时之内
		if (ret != lastret){
			lastret = ret;
			QString timetext = QString("%1").arg(48 - ret);
			QString text = QStringLiteral("对不起，您已超过使用期限\n我们为您提供了48小时的缓冲时间\n目前还剩") + timetext + QStringLiteral("小时");
			msg.setText(text);
			msg.exec();
		}
	}
}
void Lemon01::SlotListItemClicked0(QListWidgetItem * a, QListWidgetItem *b){
	SlotListItemClicked(a);
}
void Lemon01::SlotListContextRequested(const QPoint& point){
	selectName.clear();
	auto item = ui.listWidget->itemAt(point);
	if (item){
		if (item->text() == QStringLiteral("新账号")) return;
		selectName = item->text().toStdString();
		QMenu menu;
		menu.addAction(delAct);
		if (threadMap.find(selectName) != threadMap.end())
			menu.addAction(closeAct);
		menu.exec(mapToGlobal(point)+ QPoint(10,10));
	}
	
}
void Lemon01::SlotDelAct(bool flag){
	SlotCloseAct(flag);
	auto iter = infoMap.find(selectName);
	if (iter != infoMap.end()){
		infoMap.erase(iter);
	}
	delXml(selectName);
	auto item = ui.listWidget->findItems(QString::fromStdString(selectName), Qt::MatchFixedString);
	if (!item.empty())
		ui.listWidget->takeItem(ui.listWidget->row(item[0]));
}
void Lemon01::SlotCloseAct(bool flag){
	auto iter = threadMap.find(selectName);
	if (iter != threadMap.end()){
		iter->second->killProcess();
	}
}
void Lemon01::SlotListItemClicked(QListWidgetItem *item){
	std::string text = item->text().toStdString();
	auto it = infoMap.find(text);
	if (it != infoMap.end()){
		ui.accountline->setText(QString::fromStdString(it->second.account));
		ui.passwordline->setText(QString::fromStdString(it->second.password));
	}
	else{
		setEmpty();
	}
	auto iter = threadMap.find(text);
	if (iter != threadMap.end()){
		ui.btnNewMachine->setDisabled(true);
		ui.btnNewMachine->setText(QStringLiteral("运行中......"));
	}
	else{
		ui.btnNewMachine->setDisabled(false);
		ui.btnNewMachine->setText(QStringLiteral("运行"));
	}
}
void Lemon01::SlotAutoFinish(std::string name){
	auto it = infoMap.find(name);
	if (it != infoMap.end()){
		auto iter = threadMap.find(name);
		if (iter != threadMap.end()){
			AutoMachine* tmp = iter->second;
			ShopInfo& info = it->second;
			info.name = name;
			info.account = name;
			info.password = tmp->_password;
			writeXml(info);
		}
	}
	else{
		int i = ui.listWidget->count() - 1;
		ui.listWidget->insertItem(i, QString::fromStdString(name)); 
		QPixmap pix(":/Lemon01/shop");
		ui.listWidget->item(i)->setIcon(QIcon(pix.scaled(20, 20)));
		auto iter = threadMap.find(name);
		if (iter != threadMap.end()){
			AutoMachine* tmp = iter->second;
			ShopInfo info;
			info.name = name;
			info.account = name;
			info.password = tmp->_password;
			infoMap[name] = info;
			writeXml(info);
		}
	}
	ui.btnNewMachine->setDisabled(true);
	ui.btnNewMachine->setText(QStringLiteral("运行中......"));
	auto item = ui.listWidget->findItems(QString::fromStdString(name), Qt::MatchFixedString);
	item[0]->setBackgroundColor(QColor("#36ab60"));
}
void Lemon01::SlotAutoFailed(std::string name){
	QMessageBox::information(this, QString::fromStdString(name), QStringLiteral("自动改价启动失败"), QMessageBox::NoButton);
	auto it = threadMap.find(name);
	if (it != threadMap.end()){
		if (it->second)
			delete it->second;
		it->second = nullptr;
		threadMap.erase(it);
	}
}
void Lemon01::SlotAutoStop(std::string name){
	auto it = threadMap.find(name);
	if (it != threadMap.end()){
		if (it->second)
			delete it->second;
		it->second = nullptr;
		threadMap.erase(it);
	}
	if (ui.listWidget->currentItem()->text().toStdString() == name){
		ui.btnNewMachine->setDisabled(false);
		ui.btnNewMachine->setText(QStringLiteral("运行"));
	}
	auto item = ui.listWidget->findItems(QString::fromStdString(name), Qt::MatchFixedString);
	if (!item.empty())
		item[0]->setBackgroundColor(QColor("#252629"));
}
void Lemon01::SlotNewAutoMachine(){
	if (threadMap.size() >= total){
		QString totaltext = QString("%1").arg(total);
		QString text = QStringLiteral("对不起，您被授权同时只能开启") + totaltext + QStringLiteral("个店铺\n如果需要增加店铺数量，请联系我们");
		QMessageBox msg;
		msg.setWindowTitle(QStringLiteral("通知"));
		msg.setText(text);
		msg.setIcon(QMessageBox::NoIcon);
		msg.setWindowIcon(QIcon(":/Lemon01/lemon.png"));
		msg.addButton(QStringLiteral("确定"), QMessageBox::ActionRole);
		msg.exec();
		return;
	}
	std::string name = ui.accountline->text().toStdString();
	if (name.empty()) return;
	std::string password = ui.passwordline->text().toStdString();
	if (password.empty()) return;
	float percent = float(ui.percentspin->value()) / 100;
	float lowwer = ui.lowwerspin->value();
	int times = ui.times->value();
	float upload = float(ui.upload->value()) / 100;
	int mintue = ui.time->value();
	int mode = ui.mode->currentIndex();
	auto iter = threadMap.find(name);
	if (iter == threadMap.end()){
		AutoMachine* thread = new AutoMachine(name, password, percent, lowwer, times, upload, mintue, mode);
		threadMap[name] = thread;
		connect(thread, SIGNAL(success(std::string)), this, SLOT(SlotAutoFinish(std::string)));
		connect(thread, SIGNAL(failed(std::string)), this, SLOT(SlotAutoFailed(std::string)));
		connect(thread, SIGNAL(stop(std::string)), this, SLOT(SlotAutoStop(std::string)));

		thread->start();
	}
	else{
		//该店铺已经开启了自动改价
	}
}
void Lemon01::readXml()
{
	std::vector<ShopInfo> vec;
	ShopInfo cond;
	DataManager::GetInstance()->GetShops(vec, cond);
	for (auto iter = vec.begin(); iter != vec.end(); ++iter){
		infoMap[iter->name] = *iter;
	}
	/*QString filePath = FILE_PATH;
	QFile file;
	if (!file.exists(filePath)){
		file.setFileName(filePath);
		createXml();
	}
	file.setFileName(filePath);
	if (file.open(QFile::ReadOnly | QFile::Text))
	{
		QDomDocument dom;
		if (dom.setContent(&file))
		{
			QDomElement rootDom = dom.documentElement();
			if (rootDom.tagName() == "Lemon")
			{
				QDomNode subNode = rootDom.firstChild();
				while (!subNode.isNull())
				{
					QDomElement subElement = subNode.toElement();
					if (subElement.tagName() == "item")
					{
						Info* info = new Info;
						info->name = subElement.attribute("name").toStdString();
						info->password = subElement.attribute("password").toStdString();
						info->percent = subElement.attribute("percent").toFloat();
						info->lowwer = subElement.attribute("lowwer").toFloat();
						info->times = subElement.attribute("times").toInt();
						info->upload = subElement.attribute("upload").toFloat();
						info->minute = subElement.attribute("minute").toInt();
						info->mode = subElement.attribute("mode").toInt();
						infoMap[info->name] = info;
					}
					subNode = subNode.nextSibling();
				}
			}
		}
		file.close();
	}*/
}

//TODO::DEL
void Lemon01::createXml(){
	/*QString filePath = FILE_PATH;
	QFile file(filePath);
	file.open(QIODevice::WriteOnly | QIODevice::Text);
	QDomDocument doc;
	QDomProcessingInstruction instruction;
	instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"GB2312\"");
	doc.appendChild(instruction);
	QDomElement root = doc.createElement("Lemon");
	doc.appendChild(root);
	QTextStream out(&file);
	doc.save(out, 4);
	file.close();*/
}
void Lemon01::writeXml(ShopInfo& shopInfo)
{
	DataManager::GetInstance()->AddShop(shopInfo);
	/*QString filePath = FILE_PATH;
	QFile file;
	if (!file.exists(filePath)){
		file.setFileName(filePath);
		createXml();
	}
	file.setFileName(filePath);
	if (file.open(QFile::ReadOnly | QFile::Text))
	{
		QDomDocument dom;
		if (dom.setContent(&file))
		{
			file.close();
			QDomElement rootDom = dom.documentElement();
			if (rootDom.tagName() == "Lemon")
			{
				QDomElement elem = dom.createElement(tr("item"));;
				elem.setAttribute("name", QString::fromStdString(name));
				elem.setAttribute("password", QString::fromStdString(password));
				elem.setAttribute("percent", QString("%1").arg(percent));
				elem.setAttribute("lowwer", QString("%1").arg(lowwer));
				elem.setAttribute("times", QString("%1").arg(times));
				elem.setAttribute("upload", QString("%1").arg(upload));
				elem.setAttribute("minute", QString("%1").arg(minute));
				elem.setAttribute("mode", QString("%1").arg(mode));
				QDomNodeList list = rootDom.childNodes();
				int i = 0;
				for (; i < list.size(); ++i){
					if (list.at(i).toElement().attribute("name") == QString::fromStdString(name)){
						rootDom.replaceChild(elem, list.at(i));
						break;
					}
				}
				if (i == list.size())
					rootDom.appendChild(elem);
				file.setFileName(filePath);
				if (file.open(QFile::WriteOnly | QFile::Text)){
					QTextStream out(&file);
					dom.save(out, 4);
					file.close();
					return;
				}
			}
		}
		file.close(); 
	}*/
}
void Lemon01::delXml(std::string name)
{
	DataManager::GetInstance()->DelShop(name);
	/*QString filePath = FILE_PATH;
	QFile file;
	if (!file.exists(filePath)){
		file.setFileName(filePath);
		createXml();
	}
	file.setFileName(filePath);
	if (file.open(QFile::ReadOnly | QFile::Text))
	{
		QDomDocument dom;
		if (dom.setContent(&file))
		{
			file.close();
			QDomElement rootDom = dom.documentElement();
			if (rootDom.tagName() == "Lemon")
			{
				QDomNodeList nodes = rootDom.childNodes();
				int i = 0;
				for (; i < nodes.size(); ++i){
					QDomElement elem = nodes.at(i).toElement();
					if (elem.attribute("name").toStdString() == name){
						break;
					}
				}
				if (i < nodes.size()){
					rootDom.removeChild(nodes.at(i));
				}
				file.setFileName(filePath);
				if (file.open(QFile::WriteOnly | QFile::Text)){
					QTextStream out(&file);
					dom.save(out, 4);
					file.close();
					return;
				}
			}
		}
		file.close();
	}*/
}
