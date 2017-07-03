#include "loginwindow.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
//#include "user.h"
#include "windows.h"
#include "ShellApi.h"
#include <QHBoxLayout>
#include "QShortcut"
#include <QDesktopWidget>
#include <QApplication>

extern QString g_remeberToken;
TCHAR m_pathHomePage[MAX_PATH] = {0};
TCHAR m_pathUserName[MAX_PATH] = { 0 };
TCHAR m_pathTeacherName[MAX_PATH] = { 0 };
TCHAR m_pathTeacherID[MAX_PATH] = { 0 };
TCHAR m_pathTeacherToken[MAX_PATH] = { 0 };
TCHAR m_pathTeacherUrl[MAX_PATH] = { 0 };
TCHAR m_pathAccid[MAX_PATH] = { 0 };
TCHAR m_pathAccidToken[MAX_PATH] = { 0 };
TCHAR m_pathVersion[MAX_PATH] = { 0 };
TCHAR m_pathUserPass[MAX_PATH] = { 0 };
int	  m_iRemeber = 0;

#define MAINWINDOW_MAXHEIGHT 832	//�ͻ������߶�
LoginWindow::LoginWindow(QWidget *parent)
	: QMainWindow(parent)
	, mainWin(NULL)
	, trayIcon(NULL)
	, m_EnvironmentalFormally(true)
{
	ui.setupUi(this);
	setAutoFillBackground(true);;
	QPalette p = palette();
	p.setColor(QPalette::Window, QColor(255,255,255));
	setPalette(p);

	SetEnvironmental();

	connect(ui.login_pushBtn, SIGNAL(clicked()), this, SLOT(OnLogIn()));
	connect(ui.homepage_pushBtn, SIGNAL(clicked()), this, SLOT(BrowseHomePage()));
	connect(ui.register_pushBtn, SIGNAL(clicked()), this, SLOT(RegisterAccount()));
	connect(ui.findPassword_pushBtn, SIGNAL(clicked()), this, SLOT(FindPassword()));
	connect(ui.min_pushBtn, SIGNAL(clicked()), this, SLOT(MinDialog()));
	connect(ui.close_pushBtn, SIGNAL(clicked()), this, SLOT(CloseDialog()));
	connect(ui.remember_checkBox, SIGNAL(stateChanged(int)), this, SLOT(changedRemeber(int)));

	ui.UserName_Edit->setTextMargins(45, 3, 20, 3); 
	ui.UserName_Edit->setStyleSheet("QLineEdit:hover{ border: 1px solid #57cff8 }"
		"QLineEdit{ border: 1px solid #cccccc }");
	ui.UserPass_Edit->setTextMargins(45, 3, 20, 3);
	ui.UserPass_Edit->setStyleSheet("QLineEdit:hover{ border: 1px solid #57cff8 }"
		"QLineEdit{ border: 1px solid #cccccc }");

	ReadSetting();
	InitUserName();

	QShortcut *key = new QShortcut(QKeySequence(Qt::Key_Return | Qt::Key_Enter), this);//����һ����ݼ�"Key_Return"��
	connect(key, SIGNAL(activated()), this, SLOT(OnLogIn()));//���ӵ�ָ���ۺ���
	setTabOrder(ui.UserName_Edit, ui.UserPass_Edit);
	setTabOrder(ui.UserPass_Edit, ui.login_pushBtn);

	QPixmap pixmap(QCoreApplication::applicationDirPath() + "/images/login_min.png");
	QPixmap pixmap1(QCoreApplication::applicationDirPath() + "/images/login_close.png");
	ui.min_pushBtn->setPixmap(pixmap, 1);
	ui.close_pushBtn->setPixmap(pixmap1, 1);
	ui.homepage_pushBtn->installEventFilter(this);	

	ui.remember_checkBox->setStyleSheet("QCheckBox::indicator{width: 65px;height: 13px;}"
		"QCheckBox::indicator:unchecked{image: url(./images/remenber_uncheck.png);}"
		"QCheckBox::indicator:checked{image: url(./images/remenber_check.png);}");

	ui.back_label->setStyleSheet("border: 1px solid #059ed5;");
	CreateTray();

	QFont font = ui.title_label->font();
	font.setPixelSize(13);
	ui.title_label->setFont(font);

	ui.UserPass_Edit->installEventFilter(this);
}

LoginWindow::~LoginWindow()
{
	if (mainWin)
	{
		delete mainWin;
		mainWin = NULL;
	}

	if (trayIcon)
	{
		delete trayIcon;
		trayIcon = NULL;
	}
}

void LoginWindow::mousePressEvent(QMouseEvent *e)
{
	last = e->globalPos();
}

void LoginWindow::mouseMoveEvent(QMouseEvent *e)
{
	int dx = e->globalX() - last.x();
	int dy = e->globalY() - last.y();

	last = e->globalPos();
	move(x() + dx, y() + dy);
}

void LoginWindow::mouseReleaseEvent(QMouseEvent *e)
{
	int dx = e->globalX() - last.x();
	int dy = e->globalY() - last.y();

	move(x() + dx, y() + dy);
}

// ��ʼ��½
void LoginWindow::OnLogIn()
{
	Logining(true);
	if (ui.UserName_Edit->text().isEmpty())
	{
		ui.ErrorTip_Label->setText(QString("�������û���"));
		ui.UserName_Edit->setText(QString(""));
		ui.UserPass_Edit->setText(QString(""));
		return;
	}
	else if (ui.UserPass_Edit->text().isEmpty())
	{
		ui.ErrorTip_Label->setText(QString("����������"));
		ui.UserName_Edit->setText(QString(""));
		ui.UserPass_Edit->setText(QString(""));
		return;
	}

	QString str = ui.UserPass_Edit->text();
	str = parse(str);
	if (m_EnvironmentalFormally)
		url = QUrl("https://qatime.cn/api/v1/sessions"); 
	else
		url = QUrl("http://testing.qatime.cn/api/v1/sessions");

	QByteArray append("client_type=pc");
	append.append("&login_account=");
	append += ui.UserName_Edit->text();
	append.append("&password=");
	append += str;
	append.append("&client_cate=student_client");
	qInfo(append);
	
	QNetworkRequest request(url);
	reply = manager.post(request, append);
	connect(reply, &QNetworkReply::finished, this, &LoginWindow::loginFinished);
}

// ���ص�½���
void LoginWindow::loginFinished()
{
	QString strError;
	QByteArray result = reply->readAll();
	QJsonDocument document(QJsonDocument::fromJson(result));
	QJsonObject obj = document.object();
	QJsonObject data = obj["data"].toObject();
	QJsonObject error = obj["error"].toObject();

	// ��ס��ʦ��Ϣ�������Զ���¼
	if (obj["status"].toInt() == 1 && data.contains("remember_token"))
	{
		this->hide();
		// ��ס��ʦ��Ϣ�������Զ���¼
		g_remeberToken= data["remember_token"].toString();
		QJsonObject objInfo = data["user"].toObject();
		m_studentID = QString::number(objInfo["id"].toInt());
		m_studentName = objInfo["name"].toString();
		m_studentUrl = objInfo["avatar_url"].toString();
		m_accid = objInfo["chat_account"].toObject()["accid"].toString();
		m_accidPassword = objInfo["chat_account"].toObject()["token"].toString();

		mainWin = new UIMainWindow();
		mainWin->setWindowFlags(Qt::FramelessWindowHint|Qt::Tool);
		mainWin->setAttribute(Qt::WA_DeleteOnClose, false);
		mainWin->SetEnvironmental(m_EnvironmentalFormally);
		mainWin->setSudentInfo(data["user"].toObject());
		mainWin->setVersion(m_version);
		mainWin->RequestKey();
		mainWin->setLoginWindow(this);		
		mainWin->resize(1, 1);
		mainWin->show();
	}
	else if (error["code"].toInt() == 1001)
		strError = QString("�û�δ��¼");
	else if (error["code"].toInt() == 1002)
		strError = QString("��Ȩ����");
	else if (error["code"].toInt() == 1003)
		strError = QString("û��Ȩ�޷���");
	else if (error["code"].toInt() == 1004)
		strError = QString("��Ȩʧ��");
	else if (error["code"].toInt() == 2001)
		strError = QString("�ͻ��˰汾����");
	else if (error["code"].toInt() == 2002)
		strError = QString("��֧�ֵĿͻ���");
	else if (error["code"].toInt() == 3001)
		strError = QString("��������");
	else if (error["code"].toInt() == 3002)
		strError = QString("���ݲ��Ϸ�");
	else if (error["code"].toInt() == 4001)
		strError = QString("�Ҳ�����Դ");
	else if (error["code"].toInt() == 9999)
		strError = QString("����������");
	else
		strError = QString("�û��������벻��ȷ");

	ui.ErrorTip_Label->setText(strError);

	RemeberPassword();
	Logining(false);
}

void LoginWindow::BrowseHomePage()
{
	ShellExecute(NULL, L"open", m_pathHomePage, NULL, NULL, SW_SHOW);
}

void LoginWindow::RegisterAccount()
{
	TCHAR m_pathRegister[MAX_PATH] = { 0 };
	lstrcat(m_pathRegister, m_pathHomePage);
	lstrcat(m_pathRegister, L"//students//new");
	ShellExecute(NULL, L"open", m_pathRegister, NULL, NULL, SW_SHOW);
}

void LoginWindow::FindPassword()
{
	TCHAR m_pathPass[MAX_PATH] = { 0 };
	lstrcat(m_pathPass, m_pathHomePage);
	lstrcat(m_pathPass, L"//passwords//new");
	ShellExecute(NULL, L"open", m_pathPass, NULL, NULL, SW_SHOW);
}

void LoginWindow::MinDialog()
{
	showMinimized();
}

void LoginWindow::CloseDialog()
{
	close();
}

void LoginWindow::ReadSetting()
{
	TCHAR szTempPath[MAX_PATH] = { 0 };

	GetCurrentDirectory(MAX_PATH, szTempPath);
	lstrcat(szTempPath, L"\\config.ini");

	GetPrivateProfileString(L"CONFIG_PATH", L"HOMEPAGE", L"", m_pathHomePage, MAX_PATH, szTempPath);				//������ҳ·��
	GetPrivateProfileString(L"CONFIG_PATH", L"USERNAME", L"",    m_pathUserName, MAX_PATH, szTempPath);
	GetPrivateProfileString(L"CONFIG_PATH", L"TEACHERNAME", L"", m_pathTeacherName, MAX_PATH, szTempPath);
	GetPrivateProfileString(L"CONFIG_PATH", L"TEACHERID", L"", m_pathTeacherID, MAX_PATH, szTempPath);
	GetPrivateProfileString(L"CONFIG_PATH", L"TOKEN", L"", m_pathTeacherToken, MAX_PATH, szTempPath);
	GetPrivateProfileString(L"CONFIG_PATH", L"TEACHERURL", L"", m_pathTeacherUrl, MAX_PATH, szTempPath);
	GetPrivateProfileString(L"CONFIG_PATH", L"ACCID", L"", m_pathAccid, MAX_PATH, szTempPath);
	GetPrivateProfileString(L"CONFIG_PATH", L"ACCIDTOKEN", L"", m_pathAccidToken, MAX_PATH, szTempPath);
	GetPrivateProfileString(L"CONFIG_PATH", L"VERSION", L"", m_pathVersion, MAX_PATH, szTempPath);
	GetPrivateProfileString(L"CONFIG_PATH", L"USERPASSWORD", L"", m_pathUserPass, MAX_PATH, szTempPath);
	m_iRemeber = GetPrivateProfileInt(L"CONFIG_PATH", L"REMEBER", 0, szTempPath);

	m_studentName = QString::fromStdWString(m_pathTeacherName);
	m_studentID = QString::fromStdWString(m_pathTeacherID);
	g_remeberToken= QString::fromStdWString(m_pathTeacherToken);
	m_studentUrl = QString::fromStdWString(m_pathTeacherUrl);
	m_accid = QString::fromStdWString(m_pathAccid);
	m_accidPassword = QString::fromStdWString(m_pathAccidToken);
	m_version = QString::fromStdWString(m_pathVersion);
	m_password = QString::fromStdWString(m_pathUserPass);

	QString sVersion = "  ����ʱ��ѧ��������{version}";
	sVersion.replace("{version}", m_version);
	ui.title_label->setText(sVersion);
}

void LoginWindow::changedRemeber(int i)
{
	TCHAR szTempPath[MAX_PATH] = { 0 };

	GetCurrentDirectory(MAX_PATH, szTempPath);
	lstrcat(szTempPath, L"\\config.ini");

	if (i == 0)
	{
		WritePrivateProfileString(L"CONFIG_PATH", L"REMEBER", L"0", szTempPath);
	}
	else
	{
		WritePrivateProfileString(L"CONFIG_PATH", L"REMEBER", L"1", szTempPath);
	}
}

void LoginWindow::RemeberPassword()
{
	TCHAR szTempPath[MAX_PATH] = { 0 };

	GetCurrentDirectory(MAX_PATH, szTempPath);
	lstrcat(szTempPath, L"\\config.ini");

	QString strName = ui.UserName_Edit->text();
	WritePrivateProfileString(L"CONFIG_PATH", L"USERNAME", (LPCTSTR)strName.utf16(), szTempPath);

	QString strPassWord = ui.UserPass_Edit->text();
	QByteArray bPassWord = strPassWord.toLatin1().toBase64();
	m_password = bPassWord;

	if (ui.remember_checkBox->isChecked())
	{
		// TOKEN
		WritePrivateProfileString(L"CONFIG_PATH", L"TOKEN", (LPCTSTR)g_remeberToken.utf16(), szTempPath);
		// ��ʦID
		WritePrivateProfileString(L"CONFIG_PATH", L"TEACHERID", (LPCTSTR)m_studentID.utf16(), szTempPath);
		// ��ʦ����
		WritePrivateProfileString(L"CONFIG_PATH", L"TEACHERNAME", (LPCTSTR)m_studentName.utf16(), szTempPath);
		// ��ʦͷ��URL
		WritePrivateProfileString(L"CONFIG_PATH", L"TEACHERURL", (LPCTSTR)m_studentUrl.utf16(), szTempPath);
		// ��ʦaccid
		WritePrivateProfileString(L"CONFIG_PATH", L"ACCID", (LPCTSTR)m_accid.utf16(), szTempPath);
		// ��ʦaccidToken
		WritePrivateProfileString(L"CONFIG_PATH", L"ACCIDTOKEN", (LPCTSTR)m_accidPassword.utf16(), szTempPath);
		// ��ʦ����
		WritePrivateProfileString(L"CONFIG_PATH", L"USERPASSWORD", (LPCTSTR)m_password.utf16(), szTempPath);
	}
	ui.UserPass_Edit->setText(QString(""));
}

void LoginWindow::InitUserName()
{
	ui.UserName_Edit->setText(QString::fromStdWString(m_pathUserName));

	if (m_iRemeber == 1)
	{
		ui.remember_checkBox->setCheckState(Qt::Checked);

		QByteArray bPassword = m_password.toLatin1();
		QString password = QByteArray::fromBase64(bPassword);
		ui.UserPass_Edit->setText(password);
	}

	this->show();
}

void LoginWindow::ReturnLogin()
{
	if (mainWin)
	{
		delete mainWin;
		mainWin = NULL;
	}
	this->show();
}

bool LoginWindow::IsAutoLogin()
{
	return (bool)m_iRemeber;
}

// �϶��������Ĵ���
bool LoginWindow::eventFilter(QObject *target, QEvent *event)
{
	if (target == ui.homepage_pushBtn)
	{
		QMouseEvent* pMe = static_cast<QMouseEvent*>(event);
		if (event->type() == QEvent::MouseButtonPress)
		{
			m_startPos = pMe->globalPos();
			m_WndCurPos = this->pos();
		}
		else if (event->type() == QEvent::MouseMove)
		{
			m_clickPos = pMe->globalPos();
			this->move(m_WndCurPos + (m_clickPos - m_startPos));
		}
		return false;
	}
	else if (target == ui.UserPass_Edit)
	{
		if (event->type() == QEvent::KeyPress) 
		{
			QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
			if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
			{
				OnLogIn();
				return false;
			}
		}
	}
	return false;
}

void LoginWindow::CreateTray()
{
	QAction* returnAction = new QAction(tr("�л��˺�"), this);
	connect(returnAction, SIGNAL(triggered()), this, SLOT(ReturnAccount()));
	QAction* closeAction = new QAction(tr("�ر�"), this);
	connect(closeAction, SIGNAL(triggered()), this, SLOT(CloseWindow()));


	menu = new QMenu;
	menu->addAction(returnAction);
	menu->addAction(closeAction);

	QIcon icon = QIcon("./images/favicon.png");
	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setContextMenu(menu);
	trayIcon->setIcon(icon);
	trayIcon->setToolTip("����ʱ��");
	trayIcon->show();

// 	// ��ӵ� / ˫�������Ӧ
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this, SLOT(trayiconActivated(QSystemTrayIcon::ActivationReason)));
}

void LoginWindow::trayiconActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason)
	{
	case QSystemTrayIcon::Unknown:
		{
			int i = 0;
			i++;
		}
		break;
	case QSystemTrayIcon::Context:
		{
		}
		break;
	case QSystemTrayIcon::DoubleClick:
		{
			if (mainWin)
				mainWin->ShowCourse();
		}
		break;
	case QSystemTrayIcon::Trigger:
		{
		}
		break;
	case QSystemTrayIcon::MiddleClick:
		{
			int i = 0;
			i++;
		}
		break;
	default:
		break;
	}
}

void LoginWindow::CloseTray()
{
	if (trayIcon)
	{
		delete trayIcon;
		trayIcon = NULL;
	}
}

void LoginWindow::ShowMain()
{
// 	if (mainWin)
// 		mainWin->ShowMain();
}

void LoginWindow::CloseWindow()
{
	if (mainWin)
		mainWin->CloseDialog();
	
	exit(0);
}

void LoginWindow::ReturnAccount()
{
	if (mainWin)
		mainWin->returnClick();
}

QString LoginWindow::parse(QString str)
{
	QString encode;
	int iCount = str.count();
	for (int i = 0; i < iCount;i++ )
	{
		QString s = str.at(i);
		if (s == "~")
			s = "%7E";
		else if(s == "`")
			s = "%60";
		else if (s == "!")
			s = "%EFBC81";
		else if (s == "��")
			s = "%21";
		else if (s == "@")
			s = "%40";
		else if (s == "#")
			s = "%23";
		else if (s == "$")
			s = "%24";
		else if (s == "%")
			s = "%25";
		else if (s == "^")
			s = "%5E";
		else if (s == "&")
			s = "%26";
		else if (s == "*")
			s = "%2A";
		else if (s == "(")
			s = "%28";
		else if (s == ")")
			s = "%29";
		else if (s == "��")
			s = "%EFBC88";
		else if (s == "��")
			s = "%EFBC89";
		else if (s == "_")
			s = "%5F";
		else if (s == "��")
			s = "%E28094";
		else if (s == "+")
			s = "%2B";
		else if (s == "=")
			s = "%3D";
		else if (s == "\\")
			s = "%5C";
		else if (s == "|")
			s = "%7C";
		else if (s == "[")
			s = "%5B";
		else if (s == "]")
			s = "%5D";
		else if (s == "��")
			s = "%EFBC9B";
		else if (s == "��")
			s = "%EFBC9A";
		else if (s == ":")
			s = "%3A";
		else if (s == ";")
			s = "%3B";
		else if (s == "'")
			s = "%27";
		else if (s == "\"")
			s = "%22";
		else if (s == "��")
			s = "%E28098";
		else if (s == "��")
			s = "%E2809C";
		else if (s == ",")
			s = "%2C";
		else if (s == "��")
			s = "%EFBC8C";
		else if (s == "��")
			s = "%E3808A";
		else if (s == "<")
			s = "%3C";
		else if (s == ".")
			s = "%2E";
		else if (s == ">")
			s = "%3E";
		else if (s == "��")
			s = "%E38082";
		else if (s == "��")
			s = "%E3808B";
		else if (s == "/")
			s = "%2F";
		else if (s == "?")
			s = "%3F";
		else if (s == "��")
			s = "%EFBC9F";
		else if (s == "��")
			s = "%E38090";
		else if (s == "��")
			s = "%E38091";

		encode += s;
	}
	return encode;
}

void LoginWindow::SetEnvironmental()
{
	QFile file("./EnvironmentalTest.txt");
	if (file.exists())
		m_EnvironmentalFormally = false;
	else
		m_EnvironmentalFormally = true;
}

void LoginWindow::Logining(bool bLogining)
{
	if (bLogining)
	{
		ui.login_pushBtn->setStyleSheet("color: rgb(255, 255, 255);background-color: rgb(190, 190, 190); ");
		ui.login_pushBtn->setText("���ڵ�¼��...");
		ui.login_pushBtn->setEnabled(false);
		ui.UserName_Edit->setEnabled(false);
		ui.UserPass_Edit->setEnabled(false);
	}
	else
	{
		ui.login_pushBtn->setStyleSheet("color: rgb(255, 255, 255);background-color: rgb(87, 207, 248); ");
		ui.login_pushBtn->setText("��¼");
		ui.login_pushBtn->setEnabled(true);
		ui.UserName_Edit->setEnabled(true);
		ui.UserPass_Edit->setEnabled(true);
	}
}