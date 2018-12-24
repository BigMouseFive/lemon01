#ifndef ADDEANATTR_H
#define ADDEANATTR_H

#include <QDialog>
#include "ui_AddEanAttr.h"
#include "DataModel.h"
class AddEanAttr : public QDialog
{
	Q_OBJECT

public:
	AddEanAttr(CPComplexAttr attr = CPComplexAttr(), QWidget *parent = 0);
	~AddEanAttr();
	void GetEanAttr(CPComplexAttr& attr);

private slots:
	void on_ok_clicked();

private:
	Ui::AddEanAttr ui;
};

#endif // ADDEANATTR_H
