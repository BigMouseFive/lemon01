#include "HttpClient.h"
#include "qnetworkrequest.h"
#include "qnetworkreply.h"
#include "qdebug.h"
#include "QJsonParseError"
#include "QJsonObject"
#include "QJsonArray"
#include "QAuthenticator"
#include "QHttpMultiPart"

/* reply handle demo  
if (reply->error() != QNetworkReply::NoError){
	//处理中的错误信息
	qDebug() << "reply error:" << reply->errorString();
}
else{
	//获取响应信息
	const QByteArray reply_data = reply->readAll();
	qDebug() << "read all:" << reply_data;

	//解析json
	QJsonParseError json_error;
	QJsonDocument doucment = QJsonDocument::fromJson(reply_data, &json_error);
	if (json_error.error == QJsonParseError::NoError) {
		if (doucment.isObject()) {
			const QJsonObject obj = doucment.object();
			qDebug() << obj;
			if (obj.contains("args")) {
				QJsonValue value = obj.value("args");
				qDebug() << value;
			}
		}
	}
	else{
		qDebug() << "json error:" << json_error.errorString();
	}
}
reply->deleteLater();
*/

HttpClient::HttpClient(QObject *parent)
	: QNetworkAccessManager(parent)
{
	// finished
	connect(this, &QNetworkAccessManager::finished, [=](QNetworkReply* reply){
		auto method = reply->operation();
		auto url = reply->url();
		if (url.toString().contains(url_payment)){
			if (method == QNetworkAccessManager::GetOperation)
				Q_EMIT getPriceInfoReply(reply);
			else if (method == QNetworkAccessManager::PostOperation)
				Q_EMIT requestPaymentReply(reply);
		}
		else if (url.toString().contains(url_payment_order)){
			if (method == QNetworkAccessManager::GetOperation)
				Q_EMIT getOrderInfoReply(reply);
			else if (method == QNetworkAccessManager::PostOperation)
				Q_EMIT closeOrderReply(reply);
		}
		else if (url.toString().contains(url_update)){
			if (method == QNetworkAccessManager::PostOperation){
				QString update_data_path = "";
				bool valid = true;
				QString msg = "";
				if (reply->error() != QNetworkReply::NoError){
					//处理中的错误信息
					qDebug() << "[url_update] reply error:" << reply->errorString();
				}
				else{
					//获取响应信息
					const QByteArray reply_data = reply->readAll();
					qDebug() << "[url_update] read all : " << reply_data;

					//解析json
					QJsonParseError json_error;
					QJsonDocument document = QJsonDocument::fromJson(reply_data, &json_error);
					if (json_error.error == QJsonParseError::NoError) {
						if (document.isObject()) {
							valid = false;
							const QJsonObject obj = document.object();
							if (obj.contains("data")) {
								QJsonObject data_obj = obj.value("data").toObject();
								if (data_obj.contains("update_data_path"))
									update_data_path = data_obj.value("update_data_path").toString();
								if (data_obj.contains("valid"))
									valid = data_obj.value("valid").toBool();
								if (data_obj.contains("msg"))
									msg = data_obj.value("msg").toString();
							}
						}
					}
					else{
						qDebug() << "[url_update] json error:" << json_error.errorString();
					}
				}
				Q_EMIT getUpdateInfoReply(update_data_path, valid, msg);
			}
			return;
		}
		else if (url.toString().contains(url_network_time)){
			if (method == QNetworkAccessManager::GetOperation){
				int network_time = 0;
				if (reply->error() != QNetworkReply::NoError){
					//处理中的错误信息
					qDebug() << "[url_network_time] reply error:" << reply->errorString();
				}
				else{
					//获取响应信息
					const QByteArray reply_data = reply->readAll();
					qDebug() << "[url_network_time] read all : " << reply_data;

					//解析json
					QJsonParseError json_error;
					QJsonDocument document = QJsonDocument::fromJson(reply_data, &json_error);
					if (json_error.error == QJsonParseError::NoError) {
						if (document.isObject()) {
							const QJsonObject obj = document.object();
							if (obj.contains("data")) {
								QJsonObject data_obj = obj.value("data").toObject();
								if (data_obj.contains("t")){
									network_time = data_obj.value("t").toString().toDouble() / 1000;
								}
							}
						}
					}
					else{
						qDebug() << "[url_network_time] json error:" << json_error.errorString();
					}
				}
				Q_EMIT getNetworkTimeReply(network_time);
			}
			return;
		}
		else if (url.toString().contains(url_auth)){
			bool is_shop_valid = true;
			if (method == QNetworkAccessManager::PostOperation){
				if (reply->error() != QNetworkReply::NoError){
					//处理中的错误信息
					qDebug() << "[url_auth] reply error:" << reply->errorString();
				}
				else{
					//获取响应信息
					const QByteArray reply_data = reply->readAll();
					qDebug() << "[url_auth] read all : " << reply_data;

					//解析json
					QJsonParseError json_error;
					QJsonDocument document = QJsonDocument::fromJson(reply_data, &json_error);
					if (json_error.error == QJsonParseError::NoError) {
						if (document.isObject()) {
							const QJsonObject obj = document.object();
							if (obj.contains("data")) {
								QJsonObject data_obj = obj.value("data").toObject();
								if (data_obj.contains("valid"))
									is_shop_valid = data_obj.value("valid").toBool();
							}
						}
					}
					else{
						qDebug() << "[url_auth] json error:" << json_error.errorString();
					}
				}
				Q_EMIT isShopValidReply(is_shop_valid);
			}
			return;
		}


		if (reply->error() != QNetworkReply::NoError){
			//处理中的错误信息
			qDebug() << "reply error:" << reply->errorString();
		}
		else{
			//获取响应信息
			const QByteArray reply_data = reply->readAll();
			qDebug() << "read all:" << reply_data;

			//解析json
			QJsonParseError json_error;
			QJsonDocument doucment = QJsonDocument::fromJson(reply_data, &json_error);
			if (json_error.error == QJsonParseError::NoError) {
				if (doucment.isObject()) {
					const QJsonObject obj = doucment.object();
					qDebug() << obj;
					if (obj.contains("args")) {
						QJsonValue value = obj.value("args");
						qDebug() << value;
					}
				}
			}
			else{
				qDebug() << "json error:" << json_error.errorString();
			}
		}
		reply->deleteLater();
	});

	// authenticationRequired
	connect(this, &QNetworkAccessManager::authenticationRequired, [=](QNetworkReply *reply, QAuthenticator *authenticator){
		qDebug() << "authenticationRequired";
	});

	// encrypted
	connect(this, &QNetworkAccessManager::encrypted, [=](QNetworkReply *reply){
		qDebug() << "encrypted";
	});

	// sslErrors
	connect(this, &QNetworkAccessManager::sslErrors, [=](QNetworkReply *reply, const QList<QSslError> &errors){
		qDebug() << "sslErrors";
	});
}
// 获取更新信息
int HttpClient::getUpdateInfo(const std::string& register_code, const std::vector<std::string>& shop_names){
	QJsonObject j_data{
		{ "time", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") },
		{ "register_code", QString::fromStdString(register_code) }
	};
	QJsonArray j_shop_names;
	for (auto& shop_name : shop_names)
		j_shop_names.append(QString::fromStdString(shop_name));
	j_data["shop_names"] = j_shop_names;
	QNetworkRequest request(QUrl(QString(host_name) + url_update));
	request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
	post(request, QJsonDocument(j_data).toJson());
	qDebug() << "[HttpClient] getUpdateInfo post: " << j_data;
	return 0;
}
// 获取更新数据
int HttpClient::getUpdateData(const QString& update_data_url){

	return 0;
}
// 账户是否可用
int HttpClient::isAcountValid(){
	return 0;
}
// 店铺是否可用
int HttpClient::isShopValid(const std::string& shop_name){
	QJsonObject j_data{
		{ "time", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") },
		{ "shop_name", QString::fromStdString(shop_name) },
	};
	QNetworkRequest request(QUrl(QString(host_name) + url_auth));
	request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
	post(request, QJsonDocument(j_data).toJson());
	qDebug() << "[HttpClient] isShopValid post: " << j_data;
	return 0;
}
// 获取定价信息
int HttpClient::getPriceInfo(){
	get(QNetworkRequest(QUrl(QString(host_name) + url_payment)));
	return 0;
}
// 请求付款
int HttpClient::requestPayment(const int& shop_num, const QString& mac, const int& price){
	QJsonObject j_data{
		{ "time", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") },
		{ "shop_num", shop_num },
		{ "mac", mac },
		{ "price", price }
	};
	QNetworkRequest request(QUrl(QString(host_name) + url_payment));
	request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
	post(request, QJsonDocument(j_data).toJson());
	return 0;
}
// 获取付款二维码
int HttpClient::requestQRCode(){
	return 0;
}
// 获取付款结果
int HttpClient::requestPaymentResult(){
	return 0;
}
// 获取订单信息
int HttpClient::getOrderInfo(const QString& payjs_order_id){
	QUrl url(QString(host_name) + url_payment_order);
	QUrlQuery url_query;
	url_query.addQueryItem("payjs_order_id", payjs_order_id);
	url.setQuery(url_query);
	QNetworkRequest request(url);
	get(request);
	return 0;
}
// 关闭订单
int HttpClient::closeOrder(const QString& payjs_order_id){
	QHttpPart text_part1;
	text_part1.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"payjs_order_id\""));
	text_part1.setBody(payjs_order_id.toUtf8());

	QHttpPart text_part2;
	text_part2.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"method\""));
	text_part2.setBody("close");

	QHttpMultiPart *multi_part = new QHttpMultiPart(QHttpMultiPart::FormDataType);
	multi_part->append(text_part1);
	multi_part->append(text_part2);

	QNetworkRequest request(QUrl(QString(host_name) + url_payment_order));
	post(request, multi_part);
	return 0;
}