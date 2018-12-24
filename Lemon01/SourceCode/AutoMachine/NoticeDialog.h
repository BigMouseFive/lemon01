#ifndef NOTICEDIALOG_H
#define NOTICEDIALOG_H

#include <QDialog>

namespace Ui {
class NoticeDialog;
}

class NoticeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NoticeDialog(std::string shop, QWidget *parent = 0);
    ~NoticeDialog();

private:
    Ui::NoticeDialog *ui;
};

#endif // NOTICEDIALOG_H
