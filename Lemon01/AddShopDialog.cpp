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
	
	connect(ui->radio1, SIGNAL(toggled(bool)), this, SLOT(SlotRadio1(bool)));
	connect(ui->radio2, SIGNAL(toggled(bool)), this, SLOT(SlotRadio2(bool)));
	connect(ui->radio3, SIGNAL(toggled(bool)), this, SLOT(SlotRadio3(bool)));
	connect(ui->radio4, SIGNAL(toggled(bool)), this, SLOT(SlotRadio4(bool)));

	if (0){
		ui->radio2->setChecked(true);
		ui->radio1->setDisabled(true);
	}
	else{
		if (info.type == 0){  // 0-µÏ°Ý 1-É³ÌØ
			if (info.platform == 0) // 0-souq 1-noon
				ui->radio1->setChecked(true);
			else
				ui->radio3->setChecked(true);
		}
		else{
			if (info.platform == 0) // 0-souq 1-noon
				ui->radio2->setChecked(true);
			else
				ui->radio4->setChecked(true);
		}
	}
	if (!info.name.empty()) ui->lineName->setEnabled(false);
}

AddShopDialog::~AddShopDialog()
{
    delete ui;
}

void AddShopDialog::GetShopInfo(ShopInfo& shopInfo){
	shopInfo.name = ui->lineName->text().trimmed().toStdString();
	shopInfo.account = ui->lineAccount->text().trimmed().toStdString();
	shopInfo.password = ui->linePassword->text().trimmed().toStdString();
	if (ui->lineShopId->text().isEmpty())
		shopInfo.shop_id = "souq";
	else
		shopInfo.shop_id = ui->lineShopId->text().trimmed().toStdString();
	if (ui->radio1->isChecked()) {
		shopInfo.type = 0;
		shopInfo.platform = 0;
	}
	else if (ui->radio2->isChecked()){
		shopInfo.type = 1;
		shopInfo.platform = 0;
	}
	else if (ui->radio3->isChecked()){
		shopInfo.type = 0;
		shopInfo.platform = 1;
	}
	else if (ui->radio4->isChecked()){
		shopInfo.type = 1;
		shopInfo.platform = 1;
	}

}

void AddShopDialog::on_pushButton_clicked()
{
	if (ui->lineName->text().isEmpty() ||
		ui->lineAccount->text().isEmpty() ||
		ui->linePassword->text().isEmpty() ||
		(ui->lineShopId->text().isEmpty() && !ui->radio1->isChecked() && !ui->radio2->isChecked())){
		QMessageBox box(QMessageBox::Information, "Add Shop Error", QStringLiteral("µêÃû¡¢ÕËºÅ¡¢ÃÜÂë¡¢µêÆÌid¡¢µêÆÌ¹éÊôµØ²»ÄÜÎª¿Õ"), QMessageBox::NoButton, this);
		box.setStandardButtons(QMessageBox::Ok);
		box.setButtonText(QMessageBox::Ok, QStringLiteral("È· ¶¨"));
		box.exec();
	}
	else{
		accept();
	}
}

void AddShopDialog::SlotRadio1(bool b){
	if (b){
		ui->lineShopId->setHidden(1);
		ui->labelShopId->setHidden(1);
		ui->label_5->setHidden(1);
		ui->label_4->setHidden(1);
	}
}
void AddShopDialog::SlotRadio2(bool b){
	if (b){
		ui->lineShopId->setHidden(1);
		ui->labelShopId->setHidden(1);
		ui->label_5->setHidden(1);
		ui->label_4->setHidden(1);
	}
}
void AddShopDialog::SlotRadio3(bool b){
	if (b){
		ui->lineShopId->setHidden(0);
		ui->labelShopId->setHidden(0);
		ui->label_5->setHidden(0);
		ui->label_4->setHidden(0);
	}
}
void AddShopDialog::SlotRadio4(bool b){
	if (b){
		ui->lineShopId->setHidden(0);
		ui->labelShopId->setHidden(0);
		ui->label_5->setHidden(0);
		ui->label_4->setHidden(0);
	}
}