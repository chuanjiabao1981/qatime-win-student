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
#include "windows.h"
#include <QFile>

Worker::Worker() {

	workerThread.start();
	this->moveToThread(&workerThread);
	m_url = "";
	m_bClose = false;
}

Worker::~Worker()
{
	// 请求停止 add by zbc 20171016
	workerThread.requestInterruption();
	workerThread.quit();
	workerThread.wait();
}
void Worker::SetUrl(QLabel* pic, QString url)
{
	m_pic = pic;
	m_url = url;
}

void Worker::slot_StartLoading()
{
	bool bExists = false;
	for (int i = 0; i < m_mapUrl.size(); i++)
	{
		if (m_mapUrl.keys().at(i))
		{
			if (m_bClose)
				return;

			QLabel* pic = m_mapUrl.keys().at(i);
			QString url_ = m_mapUrl.values().at(i);

			if (!url_.isEmpty())
			{
				// 获取图片本地路径
				QStringList arr = url_.split("/");
				QString urlName = arr.last();
				QString picPath = "\\catch\\" + urlName;

				TCHAR szTempPath[MAX_PATH] = { 0 };
				GetCurrentDirectory(MAX_PATH, szTempPath);
				lstrcat(szTempPath, (LPCTSTR)picPath.utf16());

				QString path = QString::fromStdWString(szTempPath);
				QFile file(path);
				bExists = (file.exists() == true ? true : false);

				// 如果存在，则直接显示
				if (bExists)
				{
					QPixmap pixmap;
					QPixmap scaledPixmap;
					pixmap = QPixmap(path);

					QSize pixSize(pic->width(), pic->height());
					scaledPixmap = pixmap.scaled(pixSize, Qt::IgnoreAspectRatio);
					pic->setPixmap(scaledPixmap);
				}
				else
				{
					QUrl url(url_);
					QNetworkAccessManager manager;
					QEventLoop loop;

					QNetworkReply *reply = manager.get(QNetworkRequest(url));
					//请求结束并下载完成后，退出子事件循环 
					QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
					//开启子事件循环 
					loop.exec();

					QByteArray jpegData = reply->readAll();
					QPixmap pixmap;
					QPixmap scaledPixmap;
					QSize pixSize(pic->width(), pic->height());
					// 加载成功则显示
					if (pixmap.loadFromData(jpegData))
					{	
						if (!jpegData.isEmpty())
						{
							scaledPixmap = pixmap.scaled(pixSize, Qt::IgnoreAspectRatio);
							if (!pixmap.save(path))
							{
								qDebug() << __FILE__ << __LINE__ << "pic save fail path:" << path;
							}
						}
						else
						{
							qDebug() << __FILE__ << __LINE__ << "pic path:" << path;
						}
					}
					else // 否则显示备用图片
					{
						QString sUrl = "./images/teacherPhoto.png";
						pixmap = QPixmap(sUrl);
						scaledPixmap = pixmap.scaled(pixSize, Qt::IgnoreAspectRatio);
					}

					pic->setPixmap(scaledPixmap);
				}
			}
			else
			{
				QString sUrl = "./images/teacherPhoto.png";
				QPixmap pixmap;
				QPixmap scaledPixmap;
				pixmap = QPixmap(sUrl);
				QSize pixSize(pic->width(), pic->height());
				scaledPixmap = pixmap.scaled(pixSize, Qt::IgnoreAspectRatio);
	
				pic->setPixmap(scaledPixmap);
			}
		}
	}

	m_mapUrl.clear();
}

void Worker::slot_Close()
{
	m_bClose = true;
}