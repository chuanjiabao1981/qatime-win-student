#include "ZWeb.h"
#include <QUrl>
#include "UIMessageBox.h"


ZWeb::ZWeb(QWidget *parent)

{
	
}

//post ����
bool ZWeb::PostURLInfo(QString strUrl, QJsonObject &urlValue, QString mToken)
{
	QUrl url = QUrl(strUrl);
	QNetworkRequest request(url);
	QString str = mToken;
	QByteArray append("");
	QEventLoop mloop;
	request.setRawHeader("Remember-Token", mToken.toUtf8());

	z_reply = z_manager.post(request, append);
	//���������ɺ��˳����¼�ѭ�� 
	QObject::connect(z_reply, SIGNAL(finished()), &mloop, SLOT(quit()));
	//�������¼�ѭ��
	mloop.exec();
	//���������źŲ۵ķ�ʽҲ��
	//connect(z_reply, &QNetworkReply::finished, this, &ZWeb::FinishedPost11);
	QByteArray mResult = z_reply->readAll();
	//QJsonObject urlValue;
	int mStatus = QJsonDocument::fromJson(mResult).object()["status"].toInt();
	//�����������ж�
	if (mStatus == 0)
	{
		RequestError(QJsonDocument::fromJson(mResult).object()["error"].toObject());
		return false;
	}
	QJsonDocument document(QJsonDocument::fromJson(mResult));
	urlValue = document.object();
	return true;
}




//get����
bool ZWeb::GetURLInfo(QString strUrl, QJsonObject &urlValue, QString mToken)
{
	QUrl url = QUrl(strUrl);
	QNetworkRequest request(url);
	QString str = mToken;
	QEventLoop mloop;
	request.setRawHeader("Remember-Token", mToken.toUtf8());
	z_reply = z_manager.get(request);

	
	//���������ɺ��˳����¼�ѭ�� 
	QObject::connect(z_reply, SIGNAL(finished()), &mloop, SLOT(quit()));
	//�������¼�ѭ��
	mloop.exec();


	QByteArray mResult = z_reply->readAll();
	int mStatus = QJsonDocument::fromJson(mResult).object()["status"].toInt();
	//�����������ж�
	if (mStatus == 0)
	{
		RequestError(QJsonDocument::fromJson(mResult).object()["error"].toObject());
		return false;
	}
	QJsonDocument mDocument(QJsonDocument::fromJson(mResult));
	urlValue = mDocument.object();
	return true;
}

void ZWeb::slot_FinishedGet()
{
	QByteArray mResult = z_reply->readAll();
	int mStatus = QJsonDocument::fromJson(mResult).object()["status"].toInt();
	//�����������ж�
	if (mStatus == 0)
	{
		RequestError(QJsonDocument::fromJson(mResult).object()["error"].toObject());
		return;
	}
	QJsonDocument mDocument(QJsonDocument::fromJson(mResult));
	m_JsonObj = mDocument.object();
}

void ZWeb::RequestError(QJsonObject &error)
{
	QString strError;
	int mErrorCode = 0;
	mErrorCode = error["code"].toInt();
	if (mErrorCode == 1002)
	{
		strError = QString("��Ȩ����,�����µ�¼��");
	}
	else if (mErrorCode == 1003)
		strError = QString("û��Ȩ�޷��ʣ�");
	else if (mErrorCode == 1004)
		strError = QString("��Ȩʧ��,�����µ�¼��");
	else if (mErrorCode == 3001)
		strError = QString("��������,�����µ�¼��");
	else if (mErrorCode == 3002)
		strError = QString("���ݲ��Ϸ�,�����µ�¼��");
	else if (mErrorCode == 4001)
		strError = QString("�Ҳ�����Դ,�����µ�¼��");
	else if (mErrorCode == 9999)
		strError = QString("����������,�����µ�¼��");
	else
		strError = QString("δ֪����");


	int iStatus = CMessageBox::showMessage(
		QString("����ʱ��"),
		QString(strError),
		QString("ȷ��"),
		QString());
}

ZWeb::~ZWeb()
{

}