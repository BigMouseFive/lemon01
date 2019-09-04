#include "AddEanAttr.h"
#include <qmessagebox.h>
AddEanAttr::AddEanAttr(CPComplexAttr attr, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	ui.ean->setText(QString::fromStdString(attr.ean));
	ui.leastPrice->setValue(attr.least_price);
	ui.maxTimes->setValue(attr.max_times);
}

AddEanAttr::~AddEanAttr()
{

}

void AddEanAttr::GetEanAttr(CPComplexAttr& attr){
	attr.ean = ui.ean->text().remove(QRegExp("\\s")).toStdString();
	attr.least_price = ui.leastPrice->value();
	attr.max_times = ui.maxTimes->value();
}

void AddEanAttr::on_ok_clicked()
{
	if (ui.ean->text().isEmpty()){
		QMessageBox box(QMessageBox::Information, "Add SKU Attr Error", QStringLiteral("SKU不能为空"), QMessageBox::NoButton, this);
		box.setStandardButtons(QMessageBox::Ok);
		box.setButtonText(QMessageBox::Ok, QStringLiteral("确 定"));
		box.exec();
	}
	else{
		accept();
	}
}