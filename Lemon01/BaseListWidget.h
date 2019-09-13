#pragma once
#include <qlistwidget.h>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QPoint>
class BaseListWidget : public QListWidget{
	Q_OBJECT

public:
	BaseListWidget(QWidget* parent = nullptr);
	~BaseListWidget();
	int addItemDR(QString name);
	std::string getMyShop();

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void dropEvent(QDropEvent *event);

private:
	void performDrag();
	QPoint startPos;
};


