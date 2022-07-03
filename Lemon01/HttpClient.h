#pragma once
#include "QNetworkAccessManager"
#include "QUrlQuery"
#include "QSslConfiguration"

#define host_name "https://www.echizenryoma.top"
#define url_payment "/api/pay_server/payment"
#define url_payment_order "/api/pay_server/payment_order"
#define url_update "/api/pay_server/update"
#define url_auth "/api/pay_server/auth"
#define url_payjz "/api/pay_server/payjz"
#define url_network_time "http://api.m.taobao.com/rest/api3.do?api=mtop.common.getTimestamp"

class HttpClient : public QNetworkAccessManager {
	Q_OBJECT
public:
	HttpClient(QObject *parent = Q_NULLPTR);

	// ��ȡ������Ϣ
	int getUpdateInfo(const std::string& register_code, const std::vector<std::string>& shop_names);
	// ��ȡ��������
	int getUpdateData(const QString& update_data_url);
	// �˻��Ƿ����
	int isAcountValid();
	// �����Ƿ����
	int isShopValid(const std::string& shop_name);
	// ��ȡ������Ϣ
	int getPriceInfo();
	// ���󸶿�
	int requestPayment(const int& shop_num, const QString& mac, const int& price);
	// ��ȡ�����ά��
	int requestQRCode();
	// ��ȡ������
	int requestPaymentResult();
	// ��ȡ������Ϣ
	int getOrderInfo(const QString& payjs_order_id);
	// �رն���
	int closeOrder(const QString& payjs_order_id);

signals:
	void getUpdateInfoReply(QString, bool, QString);
	void getUpdateDataReply(QNetworkReply* reply);
	void isAcountValidReply(QNetworkReply* reply);
	void isShopValidReply(bool);
	void getPriceInfoReply(QNetworkReply* reply);
	void requestPaymentReply(QNetworkReply* reply);
	void requestQRCodeReply(QNetworkReply* reply);
	void requestPaymentResultReply(QNetworkReply* reply);
	void getOrderInfoReply(QNetworkReply* reply);
	void closeOrderReply(QNetworkReply* reply);
	void getNetworkTimeReply(int network_time);

private:
	QSslConfiguration m_config;
};