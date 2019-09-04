#include "AddShopDialog.h"
#include <qmessagebox.h>
AddShopDialog::AddShopDialog(ShopInfo info, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddShopDialog)
{
    ui->setupUi(this);
	ui->lineName->setText(QString::fromStdString(info.name));
	ui->lineAccount->setText(QString::fromStdString(info.account));
	ui->linePassword->setText(QString::fromStdString(info.password));
	ui->lineShopId->setText(QString::fromStdString(info.shop_id));
	
	if (1){
		ui->radio2->setChecked(true);
		ui->radio1->setDisabled(true);
	}
	else{
		if (info.type == 0){
			ui->radio1->setChecked(true);
		}
		else{
			ui->radio2->setChecked(true);
		}
	}
	
	if (!info.name.empty()) ui->lineName->setEnabled(false);
}

AddShopDialog::~AddShopDialog()
{
    delete ui;
}

void AddShopDialog::GetShopInfo(ShopInfo& shopInfo){
	shopInfo.name = ui->lineName->text().remove(QRegExp("\\s")).toStdString();
	shopInfo.account = ui->lineAccount->text().remove(QRegExp("\\s")).toStdString();
	shopInfo.password = ui->linePassword->text().remove(QRegExp("\\s")).toStdString();
	shopInfo.shop_id = ui->lineShopId->text().remove(QRegExp("\\s")).toStdString();
	if (ui->radio1->isChecked()) shopInfo.type = 0;
	else if (ui->radio2->isChecked()) shopInfo.type = 1;
}

void AddShopDialog::on_pushButton_clicked()
{
	if (ui->lineName->text().isEmpty() ||
		ui->lineAccount->text().isEmpty() ||
		ui->linePassword->text().isEmpty() ||
		ui->lineShopId->text().isEmpty() ||
		(!ui->radio1->isChecked() && !ui->radio2->isChecked())){
		QMessageBox box(QMessageBox::Information, "Add Shop Error", QStringLiteral("店名、账号、密码、店铺id、店铺归属地不能为空"), QMessageBox::NoButton, this);
		box.setStandardButtons(QMessageBox::Ok);
		box.setButtonText(QMessageBox::Ok, QStringLiteral("确 定"));
		box.exec();
	}
	else{
		accept();
	}
}
