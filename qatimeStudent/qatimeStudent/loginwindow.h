#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QMouseEvent>
#include <QNetworkAccessManager>
#include <QUrl>
#include "ui_loginwindow.h"
#include "UIButton.h"
#include <string.h>
#include <wtypes.h>
#include "UIMainwindow.h"
#include <QSystemTrayIcon>
#include <QMenu>
#include "define.h"

#pragma execution_character_set("utf-8")

class UIMainWindow;
class UITrayMenu;


class LoginWindow : public QMainWindow
{
	Q_OBJECT

public:
	LoginWindow(QWidget *parent = 0);
	~LoginWindow();

	// ��갴���϶��Ĳ���
	QPoint							m_startPos;
	QPoint							m_clickPos;
	QPoint							m_WndCurPos;
	bool							m_EnvironmentalFormally;	// ��������
protected:
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	virtual bool eventFilter(QObject *target, QEvent *event);

private slots :
	void OnLogIn();				// ������½
	void BrowseHomePage();		// ������ҳ
	void RegisterAccount();		// ע���û�
	void FindPassword();		// �����һ�����
	void MinDialog();			// ��С���Ի���
	void CloseDialog();			// �رնԻ���
	void changedRemeber(int i);	// ��ס�˺�
	void trayiconActivated(QSystemTrayIcon::ActivationReason);
	void ShowMain();
	void CloseWindow();
	void ReturnAccount();

private:
	void loginFinished();		// ��½��ɵķ����¼�

protected:
	void RemeberPassword();
	void InitUserName();
	void AutoLogin();			// �Զ���¼����ס���빦�ܣ�

private:
	CButton* m_MinBtn;
	CButton* m_CloseBtn;

private:
	Ui::LoginWindowClass ui;
	QPoint last;
	QUrl url;
	QNetworkAccessManager manager;
	QNetworkReply *reply;
	UIMainWindow* mainWin;
	UITrayMenu*	  m_TrayMenu;

	// �Զ���¼ʱ��Ҫ
	QString m_studentID;
	QString m_studentName;
	QString m_studentUrl;
	QString m_accid;
	QString m_accidPassword;
	QString m_version;
	QSystemTrayIcon *trayIcon;
	QMenu * menu;

public:
	void ReturnLogin();			// ���µ�½
	void ReadSetting();			// ��ȡ�����ļ�·����Ϣ
	bool IsAutoLogin();			// �Ƿ��Զ���¼
	void Checking();			// ��֤��ǰtoken�Ƿ����
	void CheckingFinished();
	void CreateTray();			// ��������
	void CloseTray();
	QString parse(QString str);	// ��������
	void SetEnvironmental();	// ���õ�ǰ����
};

#endif // LOGINWINDOW_H
