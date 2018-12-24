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
		startPos = event->pos();//��������������¼��ǰ������
	//�������ϵͳ�Դ��Ĵ��������Ա�ʵ��ͨ�õĹ���
	QListWidget::mousePressEvent(event);
}
void BaseListWidget::mouseMoveEvent(QMouseEvent *event){
	if (!acceptDrops()){
		if (event->buttons() & Qt::LeftButton) {
			//����һ�������ٳ���
			int distance = (event->pos() - startPos).manhattanLength();
			//������һ���жϴ���=��ʼ�϶��ľ���ִ��performDrag����
			if (distance >= QApplication::startDragDistance())
				performDrag();
		}
	}
	QListWidget::mouseMoveEvent(event);//������ϵͳ�Դ��ĺ��������¼�
}
void BaseListWidget::dragEnterEvent(QDragEnterEvent *event){
	BaseListWidget *source = qobject_cast<BaseListWidget *>(event->source());
	// event->source()��ȡ����
	if (source && source != this) {//����ͬһӦ�ó����򷵻���ָ��
		event->setDropAction(Qt::MoveAction);//Ȼ�����ý��� Qt::MoveAction ���͵��϶�
		event->accept();
	}
}
void BaseListWidget::dragMoveEvent(QDragMoveEvent *event){
	BaseListWidget *source = qobject_cast<BaseListWidget *>(event->source());
	// event->source()��ȡ����
	if (source && source != this) {//����ͬһӦ�ó����򷵻���ָ��
		event->setDropAction(Qt::MoveAction);//Ȼ�����ý��� Qt::MoveAction ���͵��϶�
		event->accept();
	}
}
void BaseListWidget::dropEvent(QDropEvent *event){
	BaseListWidget *source = qobject_cast<BaseListWidget *>(event->source());
	if (source && source != this) {
		QString name = event->mimeData()->text();
		auto items = findItems(name, Qt::MatchFixedString);
		QListWidgetItem* item = nullptr;
		if (items.empty()){
			addItem(name);
			item = this->item(this->count());
		}
		else{
			item = items.at(0);
		}
		setCurrentItem(item);
		//����ȡ�� QDrag �е� mimeData ���ݣ����� addItem() ��ӵ���ǰ���б���
		event->setDropAction(Qt::MoveAction);
		event->accept();
	}
}
void BaseListWidget::performDrag(){
	QListWidgetItem *item = currentItem();
	//������QListWidget����ͼ����Ŀ,currentItem���ص�ǰ��
	if (item) {
		//�ṩ�����ڼ�¼��MIME������Ϣ��������
		QMimeData *mimeData = new QMimeData;
		mimeData->setText(item->text());

		QDrag *drag = new QDrag(this);//��QDrag���ṩ��ͨ���Ϸ����ݴ���MIME֧��
		drag->setMimeData(mimeData);//��������
		drag->setPixmap(QPixmap(":/Lemon01/shop"));//����ͼƬ
		if (drag->exec(Qt::MoveAction) == Qt::MoveAction)
		{
			//delete item;
		}
	}
}