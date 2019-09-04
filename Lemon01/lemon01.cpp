#include "utils.h"
#include "MacAddress.h"
#include "lemon01.h"
#include "AddShopDialog.h"
#include "AddEanAttr.h"
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
#include <qmessagebox.h>
#include "SourceCode\AutoMachine\NoticeDialog.h"

#define FILE_PATH "lemon.xml"

using namespace std;
string getHostMacAddress()
{
	std::vector<string> vtMacAddress;
	CTemporary temporary;
	temporary.GetMacAddress(vtMacAddress);
	return vtMacAddress[0];
}

Lemon01::Lemon01(QWidget *parent)
	: QWidget(parent)
	, lastret(999)
{
	ui.setupUi(this);
	readXml();
	initList();
	ui.label_3->setHidden(true);
	ui.label->setHidden(true);
	ui.upload->setHidden(true);
	ui.times->setHidden(true);

	delAct = new QAction(QStringLiteral("ɾ������"), this);
	closeAct = new QAction(QStringLiteral("�رյ���"), this);
	addAct = new QAction(QStringLiteral("��ӵ���"), this);
	updateAct = new QAction(QStringLiteral("�޸ĵ���"), this);
	delEanAct = new QAction(QStringLiteral("ɾ����¼"), this);
	addEanAct = new QAction(QStringLiteral("��Ӽ�¼"), this);
	updateEanAct = new QAction(QStringLiteral("�޸ļ�¼"), this);
	setPriceAct = new QAction(QStringLiteral("���ü۸�"), this);
	ignoreAct = new QAction(QStringLiteral("����"), this);
	{
		ui.listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
		ui.myShopList->setAcceptDrops(true);

		ui.tableWidget->setColumnCount(3);
		ui.tableWidget->setColumnWidth(0, 200);
		ui.tableWidget->horizontalHeader()->setStretchLastSection(true);
		ui.tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
		ui.tableWidget->verticalHeader()->setVisible(false);
	}

	{
		connect(delAct, SIGNAL(triggered(bool)), this, SLOT(SlotDelAct(bool)));
		connect(closeAct, SIGNAL(triggered(bool)), this, SLOT(SlotCloseAct(bool)));
		connect(addAct, SIGNAL(triggered(bool)), this, SLOT(SlotAddAct(bool)));
		connect(updateAct, SIGNAL(triggered(bool)), this, SLOT(SlotUpdateAct(bool))); 
		connect(delEanAct, SIGNAL(triggered(bool)), this, SLOT(SlotDelEanAct(bool)));
		connect(addEanAct, SIGNAL(triggered(bool)), this, SLOT(SlotAddEanAct(bool)));
		connect(updateEanAct, SIGNAL(triggered(bool)), this, SLOT(SlotUpdateEanAct(bool)));
		connect(setPriceAct, SIGNAL(triggered(bool)), this, SLOT(SlotSetPriceAct(bool)));
		connect(ignoreAct, SIGNAL(triggered(bool)), this, SLOT(SlotIgnoreAct(bool)));
		connect(ui.btnAddTime, SIGNAL(clicked()), this, SLOT(SlotAddTime()));
		connect(ui.play, SIGNAL(clicked()), this, SLOT(SlotAutoPlay()));
		connect(ui.pause, SIGNAL(clicked()), this, SLOT(SlotAutoPause()));
		connect(ui.stop, SIGNAL(clicked()), this, SLOT(SlotAutoStop()));
		connect(ui.update, SIGNAL(clicked()), this, SLOT(SlotAutoUpdate()));
		connect(ui.notice, SIGNAL(clicked()), this, SLOT(SlotAutoUpdate()));
		connect(ui.myShopList, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
			this, SLOT(SlotDelInMyShop(QListWidgetItem *)));
		connect(ui.tableWidget, SIGNAL(customContextMenuRequested(const QPoint&)),
			this, SLOT(SlotTableContextRequested(const QPoint&)));
	}
	
	setEmpty();
	testRegister();
	char s[100];
	time_t limit_time = endtime - 2208988800;
	strftime(s, sizeof(s), "%Y-%m-%d", localtime(&limit_time));
	ui.limit_time->setText(QStringLiteral("����ʱ�䣺") + QString::fromLocal8Bit(s));
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(SlotCompareTime()));
	timer->start(1000*60*10);
}
Lemon01::~Lemon01()
{
	QuitThisSystem();
}

void Lemon01::setShopState(int control, std::string name){
	QPixmap pix;
	if(control == MACHINE_STOP){
		ui.play->setHidden(0);
		ui.pause->setHidden(1);
		ui.stop->setHidden(1);
		ui.update->setHidden(0);
		pix.load(":/Lemon01/shop");
	}
	else if (control == MACHINE_PAUSE){
		ui.play->setHidden(0);
		ui.pause->setHidden(1);
		ui.stop->setHidden(0);
		ui.update->setHidden(0);
		pix.load(":/Lemon01/pause_yellow");
	}
	else if (control == MACHINE_PLAY){
		ui.play->setHidden(1);
		ui.pause->setHidden(0);
		ui.stop->setHidden(0);
		ui.update->setHidden(0);
		pix.load(":/Lemon01/run");
	}
	if (!name.empty()){
		auto item = ui.listWidget->findItems(QString::fromStdString(name), Qt::MatchFixedString);
		if (!item.empty()){

			item[0]->setIcon(QIcon(pix.scaled(25, 25)));
		}
	}
}
void Lemon01::setEmpty(CPAttr* attr){
	noticeIndex = QModelIndex();
	if (attr){
		ui.selectShop->setText(QString::fromStdString(attr->shop));
		ui.lowwerspin->setValue(attr->lowwer);
		ui.percentspin->setValue(attr->percent * 100);
		ui.times->setValue(attr->max_times);
		ui.upload->setValue(attr->max_percent * 100);
		ui.time->setValue(attr->minute);
		if (attr->my_shop.find(',') != std::string::npos){
			QStringList shops = QString::fromStdString(attr->my_shop).split(",");
			ui.myShopList->addItems(shops);
		}
		setShopState(attr->control);
	}
	else{
		ui.lowwerspin->setValue(0);
		ui.percentspin->setValue(0);
		ui.times->setValue(0);
		ui.upload->setValue(0);
		ui.time->setValue(0);
		setShopState(MACHINE_STOP);
		ui.play->setHidden(1);
		ui.update->setHidden(1);
		currentShop = "";
		ui.selectShop->setText("");
		ui.myShopList->clear();
		ui.tableWidget->clear();
		QStringList headers;
		headers << "SKU" << QStringLiteral("��ͼ�") << QStringLiteral("���ļ۴���");
		ui.tableWidget->setHorizontalHeaderLabels(headers);
	}
}

void Lemon01::initList(){
	QStringList list;
	for (auto it = infoMap.begin(); it != infoMap.end(); ++it){
		list.append(QString::fromStdString(it->second.name));
	}
	ui.listWidget->addItems(list);
	for (int i = 0; i < ui.listWidget->count(); ++i){
		QPixmap pix(":/Lemon01/shop");
		ui.listWidget->item(i)->setIcon(QIcon(pix.scaled(25, 25)));
	}
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
int Lemon01::aaaab_addtime(){
	QMessageBox msg(this);
	msg.setWindowTitle(QStringLiteral("���ʱ��"));
	msg.setText(QStringLiteral("�Բ����������ע���벻��ȷ"));
	msg.setIcon(QMessageBox::NoIcon);
	msg.setWindowIcon(QIcon(":/Lemon01/lemon.png"));
	msg.addButton(QStringLiteral("ȷ��"), QMessageBox::ActionRole);
	//��ȡ�ļ�currentdir\\DNCAT\\ulfkeileaif.dat
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
	if (1){
		string str("");
		char _input[40];
		char c;
		for (i = 0; i < 20; ++i){
			if (_show[i] >= 0 && _show[i] <= 9) c = '0' + _show[i];
			else c = 'A' + _show[i] - 10;
			str.push_back(c);
		}
		bool isOk;
		QString qstr = QStringLiteral("���кţ�") + QString::fromStdString(str) + "\n";
		QInputDialog *inputDialog = new QInputDialog(this);
		inputDialog->setWindowTitle(QStringLiteral("���ʱ��"));
		inputDialog->setLabelText(qstr);
		auto list = inputDialog->findChild<QLabel*>();
		list->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
		inputDialog->setAcceptDrops(true);
		inputDialog->setOkButtonText(QStringLiteral("ȷ��"));
		inputDialog->setCancelButtonText(QStringLiteral("ȡ��"));
		inputDialog->setWindowIcon(QIcon(":/Lemon01/lemon.png"));
		inputDialog->setFixedSize(400, 300);
		QString text;
		if (inputDialog->exec()) {
			text = inputDialog->textValue();
			strncpy(input, text.toStdString().c_str(), 200);
			inputDialog->deleteLater();
		}
		else{
			inputDialog->deleteLater();
			return -1;
		}
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
		if (strlen(input) != 32){
			msg.exec();
			return -1;
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
						return -1;
					}
				}
			}
		}
		file.open(filepath, ios::out | ios::trunc);
		if (!file)
			printf("\n----------���湺�����кų���-----------\n");
		else{
			_input[32] = '\0';
			file.write(_input, strlen(_input));
			file.close();
		}
		msg.setText(QStringLiteral("���ʱ���ɹ�"));
		msg.exec();
	}
	locale::global(loc);
	return 1;
}
int Lemon01::aaaab(){

	QMessageBox msg(this);
	msg.setWindowTitle(QStringLiteral("ע��"));
	msg.setText(QStringLiteral("�Բ����������ע���벻��ȷ"));
	msg.setIcon(QMessageBox::NoIcon);
	msg.setWindowIcon(QIcon(":/Lemon01/lemon.png"));
	msg.addButton(QStringLiteral("ȷ��"), QMessageBox::ActionRole);
	//��ȡ�ļ�currentdir\\DNCAT\\ulfkeileaif.dat
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
		QString qstr = QStringLiteral("���кţ�") + QString::fromStdString(str) + "\n";
		QInputDialog *inputDialog = new QInputDialog();
		inputDialog->setWindowTitle(QStringLiteral("ע��"));
		inputDialog->setLabelText(qstr);
		auto list = inputDialog->findChild<QLabel*>();
		list->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
		inputDialog->setAcceptDrops(true);
		inputDialog->setOkButtonText(QStringLiteral("ȷ��"));
		inputDialog->setCancelButtonText(QStringLiteral("ȡ��"));
		inputDialog->setWindowIcon(QIcon(":/Lemon01/lemon.png"));
		inputDialog->setFixedSize(400, 300);
		QString text;
		if (inputDialog->exec()) {
			text = inputDialog->textValue();
			strncpy(input, text.toStdString().c_str(), 200);
			inputDialog->deleteLater();

		}
		else{
			inputDialog->deleteLater();
			exit(0);
			return 0;
		}
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
		file.open(filepath, ios::out | ios::trunc);
		if (!file)
			printf("\n----------���湺�����кų���-----------\n");
		else{
			_input[32] = '\0';
			file.write(_input, strlen(_input));
			file.close();
		}
		msg.setText(QStringLiteral("��ӭʹ���Զ��ļۿ���ϵͳ"));
		msg.exec();
	}

	locale::global(loc);
	return 1;
}
void Lemon01::testRegister(){
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
	aaaab();
}
void Lemon01::SlotCompareTime(){
	int ret = ::CompareTime(endtime);
	QMessageBox msg;
	msg.setWindowTitle(QStringLiteral("֪ͨ"));
	msg.setText(QStringLiteral("�Բ����������ע���벻��ȷ"));
	msg.setIcon(QMessageBox::NoIcon);
	msg.setWindowIcon(QIcon(":/Lemon01/lemon.png"));
	msg.addButton(QStringLiteral("ȷ��"), QMessageBox::ActionRole);
	if (ret == 300){
		//û�л�ȡ��ʱ��
		msg.setText(QStringLiteral("�Բ���û�гɹ���ȡ���������ʱ��\n����ϵ���Ǽ�����Ա������˲��㣬������"));
		msg.exec();
		QuitThisSystem();
		exit(0);
	}
	else if (ret == 301){
		//����  û�г���ʹ����
	}
	else if (ret == 302){
		//����ʹ����48Сʱ
		msg.setText(QStringLiteral("�Բ������ѳ���ʹ������48Сʱ�ˣ��޷���ʹ�ô����"));
		msg.exec();
		::DeleteFile(ATL::CA2T(filepath));
		QuitThisSystem();
		exit(0);
	}
	else{
		//����ʹ���� ������48Сʱ֮��
		if (ret != lastret){
			lastret = ret;
			QString timetext = QString("%1").arg(48 - ret);
			QString text = QStringLiteral("�Բ������ѳ���ʹ������\n����Ϊ���ṩ��48Сʱ�Ļ���ʱ��\nĿǰ��ʣ") + timetext + QStringLiteral("Сʱ");
			msg.setText(text);
			msg.exec();
		}
	}
}

void Lemon01::SlotListContextRequested(const QPoint& point){
	selectName.clear();
	auto item = ui.listWidget->itemAt(point);
	QMenu menu;
	menu.addAction(addAct);
	if (item){
		selectName = item->text().toStdString();
		menu.addAction(delAct);
		menu.addAction(updateAct);
	}
	menu.exec(ui.listWidget->mapToGlobal(point) + QPoint(10, 10));
}
void Lemon01::SlotDelAct(bool flag){
	if (delXml(selectName)){
		RemoveItemInList(selectName);
		SlotCloseAct(flag);
		if (selectName == currentShop)
			setEmpty();
	}
}
void Lemon01::SlotCloseAct(bool flag){
	auto iter = threadMap.find(selectName);
	if (iter != threadMap.end()){
		iter->second->killProcess();
		delete iter->second;
		threadMap.erase(iter);
	}
}
void Lemon01::SlotAddAct(bool){
	SlotAddShop();
}
void Lemon01::SlotUpdateAct(bool){
	auto iter = infoMap.find(selectName);
	if (iter != infoMap.end()){
		SlotUpdateShop(iter->second);
	}
}
void Lemon01::SlotAddShop(){
	AddShopDialog a(ShopInfo(), this);
	auto ret = a.exec();
	if (ret == AddShopDialog::Accepted){
		ShopInfo shopInfo;
		a.GetShopInfo(shopInfo);
		auto it = infoMap.find(shopInfo.name);
		if (it == infoMap.end()){
			writeXml(shopInfo);
			InsertItemInList(shopInfo.name);
		}
		else{
			QMessageBox box(QMessageBox::Information, "Add Shop Error", QStringLiteral("�˵������Ѵ���, �뿼���޸�"), QMessageBox::NoButton, this);
			box.setStandardButtons(QMessageBox::Ok);
			box.setButtonText(QMessageBox::Ok, QStringLiteral("ȷ ��"));
			box.exec();
		}
	}
}
void Lemon01::SlotUpdateShop(ShopInfo& shopInfo){
	AddShopDialog a(shopInfo);
	auto ret = a.exec();
	if (ret == AddShopDialog::Accepted){
		a.GetShopInfo(shopInfo);
		writeXml(shopInfo);
	}
}
void Lemon01::SlotTableContextRequested(const QPoint& point){
	tableRow = -1;
	auto item = ui.tableWidget->itemAt(point);
	QMenu menu;
	menu.addAction(addEanAct);
	if (item){
		tableRow = item->row();
		menu.addAction(delEanAct);
		menu.addAction(updateEanAct);
	}
	menu.exec(ui.tableWidget->mapToGlobal(point) + QPoint(10, 10));
}

void Lemon01::SlotDelEanAct(bool flag){
	if (tableRow >= 0){
		ui.tableWidget->removeRow(tableRow);
	}
}
void Lemon01::SlotAddEanAct(bool){
	AddEanAttr a;
	auto ret = a.exec();
	if (ret == AddEanAttr::Accepted){
		CPComplexAttr attr;
		a.GetEanAttr(attr);
		if (!IsEanInTable(attr.ean)){
			int row = ui.tableWidget->rowCount();
			ui.tableWidget->insertRow(row);
			auto item = new QTableWidgetItem(QString::fromStdString(attr.ean));
			item->setFlags(item->flags() & (~Qt::ItemIsEditable));
			ui.tableWidget->setItem(row, 0, item);
			ui.tableWidget->setItem(row, 1, new QTableWidgetItem(QString("%1").arg(attr.least_price)));
			ui.tableWidget->setItem(row, 2, new QTableWidgetItem(QString("%1").arg(attr.max_times)));
		}
	}
}
void Lemon01::SlotUpdateEanAct(bool){
	CPComplexAttr attr;
	attr.ean = ui.tableWidget->item(tableRow, 0)->text().toStdString();
	attr.least_price = ui.tableWidget->item(tableRow, 1)->text().toDouble();
	attr.max_times = ui.tableWidget->item(tableRow, 2)->text().toInt();
	std::string tmp_ean = attr.ean;
	AddEanAttr a(attr);
	auto ret = a.exec();
	if (ret == AddEanAttr::Accepted){
		a.GetEanAttr(attr);
		if (tmp_ean == attr.ean || !IsEanInTable(attr.ean)){
			ui.tableWidget->item(tableRow, 0)->setText(QString::fromStdString(attr.ean));
			ui.tableWidget->item(tableRow, 1)->setText(QString("%1").arg(attr.least_price));
			ui.tableWidget->item(tableRow, 2)->setText(QString("%1").arg(attr.max_times));
		}
	}
}

void Lemon01::SlotSetPriceAct(bool){

}
void Lemon01::SlotIgnoreAct(bool){
	if (noticeIndex != QModelIndex()){
		auto machine = threadMap.find(currentShop);
		if (machine->second){
			machine->second->model->removeRow(noticeIndex.row());
		}
	}
}

bool Lemon01::IsEanInTable(std::string ean){
	for (int i = 0; i < ui.tableWidget->rowCount(); ++i){
		if (ui.tableWidget->item(i, 0)->text().compare(QString::fromStdString(ean)) == 0){
			QMessageBox box(QMessageBox::Information, "Add SKU Error", QStringLiteral("��SKU�Ѵ���, �뿼���޸�"), QMessageBox::NoButton, this);
			box.setStandardButtons(QMessageBox::Ok);
			box.setButtonText(QMessageBox::Ok, QStringLiteral("ȷ ��"));
			box.exec();
			return true;
		}
	}
	return false;
}

void Lemon01::SlotListItemClicked(QListWidgetItem *item){
	if (item == nullptr) return;
	ui.selectShop->setText(item->text());
	currentShop = item->text().toStdString();
	AutoMachine* machine = nullptr;
	auto iter = threadMap.find(currentShop);
	if (iter == threadMap.end()){
		//����
		machine = new AutoMachine(currentShop);
		threadMap[currentShop] = machine;
		connect(machine, SIGNAL(SigFailed(std::string)), this, SLOT(SlotAutoFailed(std::string)));
		connect(machine, SIGNAL(SigStop(std::string)), this, SLOT(SlotAutoFinish(std::string)));
	}
	else
		machine = iter->second;
	DisplayAttr(machine);
}
void Lemon01::SlotDelInMyShop(QListWidgetItem *item){
	if (item)
		ui.myShopList->takeItem(ui.myShopList->row(item));
}
void Lemon01::RemoveItemInList(std::string name){
	auto item = ui.listWidget->findItems(QString::fromStdString(name), Qt::MatchFixedString);
	if (!item.empty())
		ui.listWidget->takeItem(ui.listWidget->row(item[0]));
}
void Lemon01::InsertItemInList(std::string name){
	QPixmap pix(":/Lemon01/shop");
	new QListWidgetItem(QIcon(pix.scaled(25, 25)), QString::fromStdString(name), ui.listWidget);
}

void Lemon01::DisplayAttr(AutoMachine* machine){
	ui.myShopList->clear();
	ui.tableWidget->clear();
	auto cpAttr = machine->GetCPAttr();
	auto cpComplexAttr = machine->GetCPComplexAttr();
	setEmpty(cpAttr);

	QStringList headers;
	headers << "SKU" << QStringLiteral("��ͼ�") << QStringLiteral("���ļ۴���");
	ui.tableWidget->setHorizontalHeaderLabels(headers);
	ui.tableWidget->setRowCount(cpComplexAttr->size());
	int i = 0;
	for (auto iter = cpComplexAttr->begin(); iter != cpComplexAttr->end(); ++iter, ++i){
		auto item = new QTableWidgetItem(QString::fromStdString(iter->second.ean));
		item->setFlags(item->flags() & (~Qt::ItemIsEditable));
		ui.tableWidget->setItem(i, 0, item);
		ui.tableWidget->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(iter->second.least_price)));
		ui.tableWidget->setItem(i, 2, new QTableWidgetItem(QString("%1").arg(iter->second.max_times)));
	}
}

void Lemon01::SlotAutoFinish(std::string name){
	setShopState(MACHINE_STOP, name);
	auto it = threadMap.find(name);
	if (it != threadMap.end()){
		it->second->SetStopState();
	}
}
void Lemon01::SlotAutoFailed(std::string name){
	setShopState(MACHINE_STOP, name);
	QMessageBox::information(this, QString::fromStdString(name), QStringLiteral("�Զ��ļ�����ʧ��"), QMessageBox::NoButton);
	auto it = threadMap.find(name);
	if (it != threadMap.end()){
		it->second->SetStopState();
	}
}
void Lemon01::SlotAutoStop(){
	auto iter = threadMap.find(currentShop);
	if (iter != threadMap.end()){
		if (iter->second){
			auto machine = iter->second;
			machine->Stop();
		}
	}
}

void Lemon01::SlotAddTime(){
	aaaab_addtime();
	char s[100];
	time_t limit_time = endtime - 2208988800;
	strftime(s, sizeof(s), "%Y-%m-%d", localtime(&limit_time));
	ui.limit_time->setText(QStringLiteral("����ʱ�䣺") + QString::fromLocal8Bit(s));
}
void Lemon01::SlotAutoPlay(){
	auto iter = threadMap.find(currentShop);
	if (iter != threadMap.end()){
		if (iter->second){
			auto machine = iter->second;
			machine->PLay();
			setShopState(MACHINE_PLAY, currentShop);
		}
	}
}
void Lemon01::SlotAutoPause(){
	auto iter = threadMap.find(currentShop);
	if (iter != threadMap.end()){
		if (iter->second){
			auto machine = iter->second;
			machine->Pause();
			setShopState(MACHINE_PAUSE, currentShop);
		}
	}
}
void Lemon01::SlotAutoUpdate(){
	auto iter = threadMap.find(currentShop);
	if (iter != threadMap.end()){
		if (iter->second){
			auto machine = iter->second;
			CPAttr attr;
			attr.lowwer = ui.lowwerspin->value();
			attr.max_percent = ui.upload->value() / 100.0;
			attr.max_times = ui.times->value();
			attr.minute = ui.time->value();
			attr.percent = ui.percentspin->value() / 100.0;
			attr.my_shop = "";
			if (ui.myShopList->count() > 0){
				std::string shops = ui.myShopList->item(0)->text().toStdString();
				for (int i = 1; i < ui.myShopList->count(); ++i){
					shops += "," + ui.myShopList->item(i)->text().toStdString();
				}
				attr.my_shop = shops;
			}
			std::vector<CPComplexAttr> vec;
			for (int i = 0; i < ui.tableWidget->rowCount(); ++i){
				CPComplexAttr cattr;
				cattr.ean = ui.tableWidget->item(i, 0)->text().toStdString();
				cattr.least_price = ui.tableWidget->item(i, 1)->text().toDouble();
				cattr.max_times = ui.tableWidget->item(i, 2)->text().toInt();
				vec.push_back(cattr);
			}
			machine->UpdateAttr(attr, vec);
		}
	}
}
void Lemon01::SlotAutoNotice(){
	//�ж�currentShop�Ƿ����
	if (!currentShop.empty()){
		//��ʾ֪ͨ��Ϣ
		//��ʹ��NoticeDialog dlg(currentShop);

	}
}
bool Lemon01::readXml()
{
	std::vector<ShopInfo> vec;
	ShopInfo cond;
	if (DataManager::GetInstance()->GetShops(vec, cond) == SQL_OK){
		for (auto iter = vec.begin(); iter != vec.end(); ++iter){
			infoMap[iter->name] = *iter;
		}
		return true;
	}
	return false;
}
bool Lemon01::writeXml(ShopInfo& shopInfo)
{
	if (DataManager::GetInstance()->AddShop(shopInfo) == SQL_OK){
		infoMap[shopInfo.name] = shopInfo;
		return true;
	}
	return false;
}
bool Lemon01::delXml(std::string name)
{
	if (DataManager::GetInstance()->DelShop(name) == SQL_OK){
		auto iter = infoMap.find(name);
		if (iter != infoMap.end()){
			infoMap.erase(iter);
		}
		return true;
	}
	return false;
}
