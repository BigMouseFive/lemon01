#ifndef ADDSHOPDIALOG_H
#define ADDSHOPDIALOG_H

#include <QDialog>
#include <string>
#include "ui_AddShopDialog.h"
#include "DataModel.h"

class AddShopDialog : public QDialog
{
    Q_OBJECT

public:
    AddShopDialog(ShopInfo info = ShopInfo(), QWidget *parent = 0);
    ~AddShopDialog();
	void GetShopInfo(ShopInfo& shopInfo);
private slots:
    void on_pushButton_clicked();
	void SlotRadio1(bool);
	void SlotRadio2(bool);
	void SlotRadio3(bool);
	void SlotRadio4(bool);

private:
    Ui::AddShopDialog *ui;
};

#endif // ADDSHOPDIALOG_H
