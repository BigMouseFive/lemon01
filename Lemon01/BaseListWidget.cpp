#include "BaseListWidget.h"
#include <QApplication>
#include <QMimeData>
#include <QDrag>
BaseListWidget::BaseListWidget(QWidget* parent)
	:QListWidget(parent){
}
BaseListWidget::~BaseListWidget(){

}
void BaseListWidget::mousePressEvent(QMouseEvent *event){
	if (event->button() == Qt::LeftButton)
		startPos = event->pos();//如果按下左键，记录当前的坐标
	//这里调用系统自带的处理函数，以便实现通用的功能
	QListWidget::mousePressEvent(event);
}
void BaseListWidget::mouseMoveEvent(QMouseEvent *event){
	if (!acceptDrops()){
		if (event->buttons() & Qt::LeftButton) {
			//计算一个曼哈顿长度
			int distance = (event->pos() - startPos).manhattanLength();
			//这里做一个判断大于=开始拖动的距离执行performDrag处理
			if (distance >= QApplication::startDragDistance())
				performDrag();
		}
	}
	QListWidget::mouseMoveEvent(event);//最后调用系统自带的函数处理事件
}
void BaseListWidget::dragEnterEvent(QDragEnterEvent *event){
	BaseListWidget *source = qobject_cast<BaseListWidget *>(event->source());
	// event->source()获取对象
	if (source && source != this) {//来自同一应用程序则返回其指针
		event->setDropAction(Qt::MoveAction);//然后设置接受 Qt::MoveAction 类型的拖动
		event->accept();
	}
}
void BaseListWidget::dragMoveEvent(QDragMoveEvent *event){
	BaseListWidget *source = qobject_cast<BaseListWidget *>(event->source());
	// event->source()获取对象
	if (source && source != this) {//来自同一应用程序则返回其指针
		event->setDropAction(Qt::MoveAction);//然后设置接受 Qt::MoveAction 类型的拖动
		event->accept();
	}
}
std::string BaseListWidget::getMyShop(){
	std::string my_shop = "";
	for (int i = 0; i < count(); i++){
		my_shop += item(i)->text().toStdString() + ",";
	}

	if (!my_shop.empty()) my_shop.pop_back();
	return my_shop;
}

int BaseListWidget::addItemDR(QString name){
	int is_new = 1;
	auto items = findItems(name, Qt::MatchFixedString);
	QListWidgetItem* item = nullptr;
	if (items.empty()){
		addItem(name);
		item = this->item(this->count());
	}
	else{
		item = items.at(0);
		is_new = 0;
	}
	setCurrentItem(item);
	return is_new;
}

void BaseListWidget::dropEvent(QDropEvent *event){
	BaseListWidget *source = qobject_cast<BaseListWidget *>(event->source());
	if (source && source != this) {
		addItemDR(event->mimeData()->text());
		//我们取出 QDrag 中的 mimeData 数据，调用 addItem() 添加到当前的列表中
		event->setDropAction(Qt::MoveAction);
		event->accept();
	}
}
void BaseListWidget::performDrag(){
	QListWidgetItem *item = currentItem();
	//用于与QListWidget项视图类项目,currentItem返回当前项
	if (item) {
		//提供了用于记录的MIME类型信息数据容器
		QMimeData *mimeData = new QMimeData;
		mimeData->setText(item->text());

		QDrag *drag = new QDrag(this);//该QDrag类提供了通过拖放数据传输MIME支持
		drag->setMimeData(mimeData);//设置数据
		drag->setPixmap(QPixmap(":/Lemon01/shop"));//设置图片
		if (drag->exec(Qt::MoveAction) == Qt::MoveAction)
		{
			//delete item;
		}
	}
}