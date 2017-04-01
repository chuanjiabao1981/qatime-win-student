/*
*	Author       hzzhuling15
*	Date         2015/09/11
*	Copyright    Hangzhou, Netease Inc.
*	Brief        
*   Notice       
*/
#ifndef _UI_WORKTHREAD_H_
#define _UI_WORKTHREAD_H_

#include <QLabel>
#include <Qthread>
#include <QProgressBar>

#pragma execution_character_set("utf-8")

class Worker :public QObject
{
	Q_OBJECT
public:
	Worker();
	~Worker();

	bool					m_bClose;
	QString					m_name;
Q_SIGNALS:
	void sig_ResultReady();
	void sig_Progress(QString, qint64, qint64);
protected Q_SLOTS :
	void slot_StartStep();
	void slot_Close();

private:
	QThread				workerThread;
	QStringList			m_FilesName;		// 不需要重复安装的文件名列表

public:
	void initNoupdateFile();
	void onStepProgress(qint64 bytesSent, qint64 bytesTotal);
	void setCompressName(QString name);
};
#endif//_LS_MEDIACAPTURE_UI_WORKTHREAD_H_

