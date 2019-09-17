#ifndef LEMON01_H
#define LEMON01_H
#include <QtWidgets/QWidget>
#include "ui_lemon01.h"
#include "SourceCode\AutoMachine\AutoMachine.h"
#include "DataManager.h"
#include <qmenu.h>
#include <qaction.h>
#include <qprogressdialog.h>
#include <map>
#include <string>
#include <QStyledItemDelegate>

class NoFocusDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	NoFocusDelegate(){};
	~NoFocusDelegate(){};

	void NoFocusDelegate::paint(QPainter* painter, const QStyleOptionViewItem & option, const QModelIndex &index) const
	{
		QStyleOptionViewItem itemOption(option);
		if (itemOption.state & QStyle::State_HasFocus)
		{
			itemOption.state = itemOption.state ^ QStyle::State_HasFocus;
		}
		QStyledItemDelegate::paint(painter, itemOption, index);
	}
};
class Lemon01 : public QWidget
{
	Q_OBJECT

public:
	Lemon01(QWidget *parent = 0);
	~Lemon01();

private slots:
	void SlotAddTime();

	void SlotAutoPlay();
	void SlotAutoStop();
	void SlotAutoPause();
	void SlotAutoUpdate();
	void SlotExportGoldCar();
	void SlotExportChangePrice();
	void SlotAutoNotice();
	void SlotAutoFinish(std::string);
	void SlotAutoFailed(std::string);

	void SlotListContextRequested(const QPoint&);
	void SlotTableContextRequested(const QPoint&);
	void SlotMyShopContextRequested(const QPoint&);
	void SlotWhiteContextRequested(const QPoint&);

	void SlotDelAct(bool);
	void SlotCloseAct(bool);
	void SlotAddAct(bool);
	void SlotUpdateAct(bool); 

	void SlotDelEanAct(bool);
	void SlotDelAllEanAct(bool);
	void SlotAddEanAct(bool);
	void SlotUpdateEanAct(bool);
	void SlotImportProductAttrAct(bool);

	void SlotSetPriceAct(bool);
	void SlotIgnoreAct(bool);

	void SlotAddWhiteShopAct(bool);
	void SlotDelAllWhiteShopAct(bool);
	void SlotImportWhiteShopAct(bool);

	void SlotAddWhiteEanAct(bool);
	void SlotDelAllWhiteEanAct(bool);
	void SlotImportWhiteEanAct(bool);

	void SlotListItemClicked(QListWidgetItem *item);
	void SlotDelInMyShop(QListWidgetItem *item);
	void SlotDelInWhiteList(QListWidgetItem *item);
	void SlotCompareTime();
	void SlotAddShop();
	void SlotUpdateShop(ShopInfo&);
	void SlotUpdateCPAttr(const QString& str);

private:
	void WriteLockFile();
	bool CanRunByLockFile();
	int HelperHandler(int method, std::string src, std::string dst, std::string shop = "");
	void DisplayAttr(AutoMachine*);
	bool IsEanInTable(std::string);
	int aaaab(); 
	int aaaab_addtime();
	void testRegister();
	void QuitThisSystem();
	void setEmpty(CPAttr* attr = nullptr);
	void setShopState(int control, std::string name = "");
	void initList();
	void InsertItemInList(std::string);
	void RemoveItemInList(std::string);
	bool readXml();
	bool delXml(std::string name);
	bool writeXml(ShopInfo& shopInfo);
private:
	Ui::Lemon01Class ui;
	std::map<std::string, AutoMachine*> threadMap;
	std::map<std::string, ShopInfo> infoMap;

	QAction* delAct;
	QAction* closeAct;
	QAction* addAct;
	QAction* updateAct;

	QAction* delEanAct;
	QAction* addEanAct;
	QAction* updateEanAct;
	QAction* importProductAttrAct;
	QAction* delAllEanAct;


	QAction* setPriceAct;
	QAction* ignoreAct;

	QAction* addWhiteShopAct;
	QAction* delAllWhiteShopAct;
	QAction* importWhiteShopAct;

	QAction* addWhiteEanAct;
	QAction* delAllWhiteEanAct;
	QAction* importWhiteEanAct;

	QProgressDialog* progress;
	QModelIndex noticeIndex;
	std::string selectName;
	std::string currentShop;
	int tableRow;
	int _show[20];
	int hide[32];
	int _hide[32];
	char currentDir[200];
	char input[200];
	char filepath[200];
	char endTime[20];
	DWORD endtime;	
	int total;
	int lastret;
};

#endif // LEMON01_H
