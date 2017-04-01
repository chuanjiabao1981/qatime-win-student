/*
*	Author       hzzhuling15
*	Date         2015/09/11
*	Copyright    Hangzhou, Netease Inc.
*	Brief        
*   Notice       
*/
#include "UIWorkThread.h"
#include <QUrl>
#include <QNetworkAccessManager>
#include <QObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QPixmap>

Worker::Worker() {

	workerThread.start();
	this->moveToThread(&workerThread);
	m_url = "";
	m_bClose = false;
}

Worker::~Worker()
{
	
}
void Worker::SetUrl(QLabel* pic, QString url)
{
	m_pic = pic;
	m_url = url;
}

void Worker::slot_StartLoading()
{
	for (int i = 0; i < m_mapUrl.size(); i++)
	{
		if (m_mapUrl.keys().at(i))
		{
			QLabel* pic = m_mapUrl.keys().at(i);
			QString url_ = m_mapUrl.values().at(i);

			QUrl url(url_);
			QNetworkAccessManager manager;
			QEventLoop loop;

			QNetworkReply *reply = manager.get(QNetworkRequest(url));
			//���������������ɺ��˳����¼�ѭ�� 
			QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
			//�������¼�ѭ�� 
			loop.exec();

			if (m_bClose)
				return;

			QByteArray jpegData = reply->readAll();
			QPixmap pixmap;
			QPixmap scaledPixmap;
			QSize pixSize(pic->width(), pic->height());
			// ���سɹ�����ʾ
			if (pixmap.loadFromData(jpegData))
			{
				scaledPixmap = pixmap.scaled(pixSize, Qt::IgnoreAspectRatio);
			}
			else // ������ʾ����ͼƬ
			{
				QString sUrl = "./images/teacherPhoto.png";
				pixmap = QPixmap(sUrl);
				scaledPixmap = pixmap.scaled(pixSize, Qt::IgnoreAspectRatio);
			}
			pic->setPixmap(scaledPixmap);
		}
	}

	m_mapUrl.clear();
}

void Worker::slot_Close()
{
	m_bClose = true;
}