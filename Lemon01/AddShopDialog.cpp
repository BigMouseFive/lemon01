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
	if (info.type == 0){
		ui->radio1->setChecked(true);
	}
	else{
		ui->radio2->setChecked(true);
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
	if (ui->radio1->isChecked()) shopInfo.type = 0;
	else if (ui->radio2->isChecked()) shopInfo.type = 1;
}

void AddShopDialog::on_pushButton_clicked()
{
	if (ui->lineName->text().isEmpty() ||
		ui->lineAccount->text().isEmpty() ||
		ui->linePassword->text().isEmpty() ||
		(!ui->radio1->isChecked() && !ui->radio2->isChecked())){
		QMessageBox box(QMessageBox::Information, "Add Shop Error", QStringLiteral("�������˺š����롢���̹����ز���Ϊ��"), QMessageBox::NoButton, this);
		box.setStandardButtons(QMessageBox::Ok);
		box.setButtonText(QMessageBox::Ok, QStringLiteral("ȷ ��"));
		box.exec();
	}
	else{
		accept();
	}
}
