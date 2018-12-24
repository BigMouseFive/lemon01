#include "NoticeDialog.h"
#include "ui_NoticeDialog.h"

NoticeDialog::NoticeDialog(std::string shop, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NoticeDialog)
{
    ui->setupUi(this);
}

NoticeDialog::~NoticeDialog()
{
    delete ui;
}
