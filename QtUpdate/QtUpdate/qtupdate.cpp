#include "qtupdate.h"
#include <QFile>
#include "quazip/JlCompress.h"
#include <windows.h>
#include <QPainter>
#include <QFile>
#include "windows.h"
#include "string.h"
#include <TCHAR.H>

static BOOL WCharToMByte(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD dwSize)
{
	DWORD dwMinSize;
	dwMinSize = WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, NULL, 0, NULL, FALSE);
	if (dwSize < dwMinSize)
	{
		return FALSE;
	}
	WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, lpszStr, dwSize, NULL, FALSE);
	return TRUE;
}

static std::string WTOMB(const std::wstring & astr)
{
	std::string lRet;
	int liLen = astr.size() * 2 + 10;
	char * buff = new char[liLen];
	if (buff == 0)
		return "";
	memset(buff, 0, liLen);
	WCharToMByte(astr.c_str(), buff, liLen - 1);
	lRet = buff;
	delete[] buff;
	return lRet;
}

#define MSG_UPDATE_MSG					5000	+ 12	//���͸�����Ϣ
QtUpdate::QtUpdate(QWidget *parent)
	: QMainWindow(parent)
	, m_worker(NULL)
	, m_name("")
{
	ui.setupUi(this);
	setAutoFillBackground(true);
	QPalette p = palette();
	p.setColor(QPalette::Window, QColor(255, 255, 255));
	setPalette(p);

	ui.close_pushButton->setStyleSheet("QPushButton{border-image:url(./images/login_close.png);}");
	connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(clickBtn()));
	connect(ui.close_pushButton, SIGNAL(clicked()), this, SLOT(clickDialog()));

//	JlCompress::compressDir("456.zip", "E:\\qatimeStudent\\qatimeStudent\\qatimeStudent\\Bin\\Release"); //ѹ��
//	JlCompress::extractDir("456.zip", "456");//��ѹ��

	m_worker = new Worker();
	connect(this, SIGNAL(sig_StartStep()), m_worker, SLOT(slot_StartStep()));
	connect(this, SIGNAL(sig_Close()), m_worker, SLOT(slot_Close()));
	connect(m_worker, SIGNAL(sig_ResultReady()), this, SLOT(slot_ResultReady()));
	connect(m_worker, SIGNAL(sig_Progress(QString, qint64, qint64)), this, SLOT(slot_Progress(QString, qint64, qint64)));
}

QtUpdate::~QtUpdate()
{

}

void QtUpdate::httpDownload()
{
	m_url = QUrl("http://qatime-production.oss-cn-beijing.aliyuncs.com/download/software/windows/qatime-teacher-release-0.7.0.5-win.zip");
	m_url = QUrl(m_loadPath);

	m_name = m_url.fileName();
	m_file = new QFile(m_name);
	m_file->open(QIODevice::WriteOnly);//ֻ����ʽ���ļ�
	QNetworkRequest request(m_url);
	manager = new QNetworkAccessManager(this);
	reply = manager->get(request);//ͨ���������ݣ�����ֵ������replyָ����.
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));//finishΪmanager�Դ����źţ�replyFinished���Զ����
	connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(onDownloadProgress(qint64, qint64)));//download�ļ�ʱ����
	connect((QObject *)reply, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

void QtUpdate::replyFinished(QNetworkReply *reply)
{
	//��ȡ��Ӧ����Ϣ��״̬��Ϊ200��ʾ����
	QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	QByteArray bytes = reply->readAll();  //��ȡ�ֽ�
	QString result(bytes);  //ת��Ϊ�ַ���
	qDebug() << result;

	//�޴��󷵻�
	if (reply->error() == QNetworkReply::NoError)
	{
		QByteArray bytes = reply->readAll();  //��ȡ�ֽ�
		QString result(bytes);  //ת��Ϊ�ַ���
		qDebug() << result;
		m_file->close();
		ui.progress_label->setText("�������");
		ui.pushButton->setEnabled(true);
	}
	else
	{
		//�������
		qDebug() << "fail";
		ui.progress_label->setText("����ʧ�ܣ����������أ�");
		ui.pushButton->setStyleSheet("QPushButton{border-image:url(./images/update_fail_nor.png);border-radius:2px;}"
			"QPushButton:disabled{border-image:url(./images/update_fail_dis.png);border-radius:2px;}"
			"QPushButton:pressed{border-image:url(./images/update_fail_hover.png);border-radius:2px;}");
		ui.pushButton->setEnabled(true);
	}

	reply->deleteLater();//Ҫɾ��reply�����ǲ�����repyfinished��ֱ��delete��Ҫ����deletelater;
}

void QtUpdate::onDownloadProgress(qint64 bytesSent, qint64 bytesTotal)
{
	float total = (float)bytesTotal / 1024 / 1024;
	float sent = (float)bytesSent / 1024 / 1024;
	QString strProgress;
	strProgress = QString("������(%1MB/%2MB)").arg(QString::number(sent, 'f', 2)).arg(QString::number(total, 'f', 2));
	ui.progress_label->setText(strProgress);
	ui.progressBar->setMaximum(bytesTotal);
	ui.progressBar->setValue(bytesSent);
}

void QtUpdate::onReadyRead()
{
	m_file->write(reply->readAll());
}

// �϶�����
bool QtUpdate::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
	if ("windows_generic_MSG" == eventType)
	{
		PMSG pMsg = static_cast<PMSG>(message);
		switch (pMsg->message)
		{
		case WM_NCHITTEST:
		{
			int x = GET_X_LPARAM(pMsg->lParam) - this->frameGeometry().x();
			int y = GET_Y_LPARAM(pMsg->lParam) - this->frameGeometry().y();

			int xflag = (x <= MAINWINDOW_X_MARGIN) ? -1 : ((x < this->width() - MAINWINDOW_X_MARGIN) ? 0 : 1);
			int yflag = (y <= MAINWINDOW_Y_MARGIN) ? -1 : ((y < this->height() - MAINWINDOW_Y_MARGIN) ? 0 : 1);

			if (-1 == xflag && -1 == yflag)
			{
				*result = HTTOPLEFT;
			}
			else if (-1 == xflag && 0 == yflag)
			{
				*result = HTLEFT;
			}
			else if (-1 == xflag && 1 == yflag)
			{
				*result = HTBOTTOMLEFT;
			}
			else if (0 == xflag && -1 == yflag)
			{
				*result = HTTOP;
			}
			else if (0 == xflag && 0 == yflag)
			{
				*result = HTCLIENT;
			}
			else if (0 == xflag && 1 == yflag)
			{
				*result = HTBOTTOM;
			}
			else if (1 == xflag && -1 == yflag)
			{
				*result = HTTOPRIGHT;
			}
			else if (1 == xflag && 0 == yflag)
			{
				*result = HTRIGHT;
			}
			else if (1 == xflag && 1 == yflag)
			{
				*result = HTBOTTOMRIGHT;
			}
			if (0 == xflag && y > MAINWINDOW_Y_MARGIN && y <= MAINWINDOW_TITLE_HEIGHT)
			{
				if (ui.close_pushButton)
				{
					QRect Rect = ui.close_pushButton->geometry();
					if (x > Rect.left() && x < Rect.right() && y > Rect.top() && y < Rect.bottom())
					{
						return false;
					}
				}
				*result = HTCAPTION;
			}
			return true;
		}
		break;
		default:
			return false;
			break;
		}
	}
	return false;
}

void QtUpdate::paintEvent(QPaintEvent *event)
{
	QPainterPath path;
	QPainter painter(this);

	QColor color(5, 157, 213);
	for (int i = 0; i < 30; i++)
		path.addRect(0, i, this->width() - 1, this->height() - i - 1);

	painter.setPen(color);
	painter.drawPath(path);
}

void QtUpdate::clickDialog()
{
	exit(0);
}

void QtUpdate::clickBtn()
{
	QString strStyle = ui.pushButton->styleSheet();
	if (strStyle.indexOf("btn_nor") != -1 || strStyle.indexOf("fail_nor") != -1)			//����
	{
		HWND hWnd = FindWindow(L"Qt5QWindowIcon", L"StudentWindow");
		PostMessage(hWnd, MSG_UPDATE_MSG, 0, 0);

		ui.progress_label->setText("������...");
		ui.page2_widget->setVisible(false);
		ui.page3_widget->setVisible(true);
		ui.pushButton->setStyleSheet("QPushButton{border-image:url(./images/update_step_nor.png);border-radius:2px;}"
			"QPushButton:disabled{border-image:url(./images/update_step_dis.png);border-radius:2px;}"
			"QPushButton:pressed{border-image:url(./images/update_step_hover.png);border-radius:2px;}");

		ui.pushButton->setEnabled(false);

		// ��ʼ����
		httpDownload();
	}
	else if (strStyle.indexOf("step_nor") != -1)	//��װ
	{
		ui.pushButton->setStyleSheet("QPushButton{border-image:url(./images/update_ok_nor.png);border-radius:2px;}"
			"QPushButton:disabled{border-image:url(./images/update_ok_dis.png);border-radius:2px;}"
			"QPushButton:pressed{border-image:url(./images/update_ok_hover.png);border-radius:2px;}");
		ui.pushButton->setEnabled(false);

		m_worker->setCompressName(m_name);
		emit sig_StartStep();
	}
	else if (strStyle.indexOf("ok") != -1)		//ȷ��
	{
		if (ui.page1_widget->isVisible())
		{
			// ֱ���˳�
			exit(0);
		}
		else if (ui.page3_widget->isVisible())
		{
			// ����ѧ��������
			TCHAR szTempPath[MAX_PATH] = { 0 };
			GetCurrentDirectory(MAX_PATH, szTempPath);
			lstrcat(szTempPath, L"\\qatimeStudent.exe");
			/************************************************************************/
			/* 	�������½���														*/
			/************************************************************************/
			std::wstring wszCmdLine;
			STARTUPINFOW	si = { 0 };
			si.cb = sizeof(si);

			PROCESS_INFORMATION	pi;
			BOOL bIsSuc = CreateProcess(szTempPath, (LPWSTR)wszCmdLine.c_str(), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);

			// ɾ�����ص�ѹ����
			TCHAR szFilename[MAX_PATH] = { 0 };
			GetCurrentDirectory(MAX_PATH, szFilename);
			QString fileName = QString::fromStdString((WTOMB)(szFilename));
			fileName.append("\\"+ m_name);
			QFile file;
			file.remove(fileName);
			exit(0);
		}
	}
}

// ��װ����
void QtUpdate::slot_Progress(QString text, qint64 sent, qint64 total)
{
	ui.progress_label->setText(text);
	ui.progressBar->setMaximum(total);
	ui.progressBar->setValue(sent);
}

// ��װ���
void QtUpdate::slot_ResultReady()
{
	ui.progress_label->setText("��װ���");
	ui.pushButton->setEnabled(true);
}

void QtUpdate::setLoadPath(QString path)
{
	m_loadPath = path;
}

void QtUpdate::setVersion(QString version)
{
	if (version == "")
	{
		ui.pushButton->setStyleSheet("QPushButton{border-image:url(./images/update_ok_nor.png);border-radius:2px;}"
			"QPushButton:pressed{border-image:url(./images/update_ok_hover.png);border-radius:2px;}");

		ui.newversion_label->setText("��ǰ�汾Ϊ���°汾��");
		ui.page2_widget->setVisible(false);
		ui.page3_widget->setVisible(false);
	}
	else
	{
		m_version = version;
		QString strVersion = "�����°汾V";
		strVersion.append(m_version);
		ui.version_label->setText(strVersion);
		ui.page1_widget->setVisible(false);
		ui.page3_widget->setVisible(false);
		ui.pushButton->setStyleSheet("QPushButton{border-image:url(./images/update_btn_nor.png);border-radius:2px;}"
			"QPushButton:pressed{border-image:url(./images/update_btn_hover.png);border-radius:2px;}");
	}
}

void QtUpdate::setUpdate(QString update)
{
	if (update == "0")
	{
		ui.pushButton->setStyleSheet("QPushButton{border-image:url(./images/update_ok_nor.png);border-radius:2px;}"
			"QPushButton:pressed{border-image:url(./images/update_ok_hover.png);border-radius:2px;}");

		ui.newversion_label->setText("��ǰ�汾Ϊ���°汾��");
		ui.page2_widget->setVisible(false);
		ui.page3_widget->setVisible(false);
	}
	else if (update == "1")
	{
		ui.pushButton->setStyleSheet("QPushButton{border-image:url(./images/update_btn_nor.png);border-radius:2px;}"
			"QPushButton:pressed{border-image:url(./images/update_btn_hover.png);border-radius:2px;}");
	}
}