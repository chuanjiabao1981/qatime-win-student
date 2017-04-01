#ifndef QTUPDATE_H
#define QTUPDATE_H

#include <QtWidgets/QMainWindow>
#include "ui_qtupdate.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QFile>
#include <windowsx.h>
#include "UIWorkThread.h"

#define MAINWINDOW_X_MARGIN 0
#define MAINWINDOW_Y_MARGIN 0
#define MAINWINDOW_TITLE_HEIGHT 30

#pragma execution_character_set("utf-8")

class QtUpdate : public QMainWindow
{
	Q_OBJECT

public:
	QtUpdate(QWidget *parent = 0);
	~QtUpdate();

private:
	Ui::QtUpdateClass ui;

private:
	// 下载文件
	QFile*				m_file;
	QUrl				m_url;
	QNetworkAccessManager* manager;
	QNetworkReply *reply;

	Worker*				m_worker;
	QString				m_name;
	QString				m_loadPath;
	QString				m_version;
protected:
	virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);
	virtual void paintEvent(QPaintEvent *event);

signals:
	void sig_Close();
	void sig_StartStep();

private slots:
	void replyFinished(QNetworkReply*);
	void onDownloadProgress(qint64, qint64);
	void onReadyRead();
	void clickBtn();
	void clickDialog();
	void slot_ResultReady();						//安装完成
	void slot_Progress(QString, qint64, qint64);	//安装进度
private:
	void httpDownload();

public:
	void setLoadPath(QString path);
	void setUpdate(QString update);
	void setVersion(QString version);
};

#endif // QTUPDATE_H
