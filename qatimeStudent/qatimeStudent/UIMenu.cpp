#include "UIMenu.h"
#include <QPainter>
#include <iosfwd>
#include <sstream>
#include <tlhelp32.h>
#include <string.h>

extern bool g_environmentType;	// ��������		trueΪ��������		falseΪ���Ի���  Ĭ��Ϊtrue
extern QString g_remeberToken;

UIMenu::UIMenu(QWidget *parent)
	: QWidget(parent)
	, m_parent(NULL)
{
	ui.setupUi(this);
	ui.label->setStyleSheet("border-image: url(./images/t_photo.png);");
	connect(ui.label, SIGNAL(clicked()), this, SLOT(clickPic()));
	connect(ui.return_pushButton, SIGNAL(clicked()), this, SLOT(clickReturn()));
	connect(ui.check_pushButton, SIGNAL(clicked()), this, SLOT(clickCheck()));

	ui.return_pushButton->setStyleSheet("QPushButton{border-image:url(./images/return_n.png);}"
						"QPushButton:hover{border-image:url(./images/return_h.png);}");
	ui.check_pushButton->setStyleSheet("QPushButton{border-image:url(./images/check_nor.png);}"
		"QPushButton:hover{border-image:url(./images/check_hover.png);}");
	ui.label->setEnabled(false);
}

UIMenu::~UIMenu()
{

}

void UIMenu::focusOutEvent(QFocusEvent* e)
{
	QPoint pt = mapFromGlobal(QCursor::pos());
	pt.setY(pt.y() + geometry().y());
	QRect rc = this->geometry();
	if (rc.contains(pt))
	{
		setFocus();
		return;
	}

	this->hide();
}

void UIMenu::SetMainParent(UIAuxiliaryWnd* pWnd)
{
	m_parent = pWnd;
}

void UIMenu::setPic(QPixmap pix)
{
	ui.pic_label->setPixmap(pix);
}

void UIMenu::setName(QString name)
{
	ui.name_label->setText(name);

	int fontSize = ui.name_label->fontMetrics().width(name);//��ȡ֮ǰ���õ��ַ��������ش�С
	if (fontSize >= ui.name_label->width()) //��label������Ƚ�
	{
		QString  tmpStr = ui.name_label->fontMetrics().elidedText(name, Qt::ElideRight, ui.name_label->width());
		ui.name_label->setText(tmpStr);
		ui.name_label->setToolTip(name);
	}
}

void UIMenu::clickPic()
{
	hide();
}

void UIMenu::clickReturn()
{
	if (m_parent)
	{
		m_parent->ReturnAccount();
	}
}

void UIMenu::paintEvent(QPaintEvent *event)
{
	QPainterPath path;
	path.setFillRule(Qt::WindingFill);
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);

	QColor color(255, 255, 255);
	for (int i = 0; i < 4; i++)
	{
		path.setFillRule(Qt::WindingFill);

		path.addRect(0, 0, this->width(), this->height());

		painter.setPen(color);
		painter.drawPath(path);
	}
}

void UIMenu::clickCheck()
{
	checkVersion();
}
void UIMenu::StartCheck(QString enforce, QString version, QString downpath)
{
	// �������³���
	TCHAR szTempPath[MAX_PATH] = { 0 };
	GetCurrentDirectory(MAX_PATH, szTempPath);
	lstrcat(szTempPath, L"\\QtUpdate.exe");
//	lstrcat(szTempPath, L"E:\\qatimeStudent\\QtUpdate\\QtUpdate\\Bin\\Release\\QtUpdate.exe");
	TCHAR szDownPath[_MAX_PATH] = { 0 }; //	����·��
	TCHAR szVersion[_MAX_PATH] = { 0 };	 //	�汾��
	TCHAR szUpdate[_MAX_PATH] = { 0 };	 //	����

	lstrcat(szDownPath, (LPCWSTR)downpath.toStdWString().c_str());
	lstrcat(szVersion, (LPCWSTR)version.toStdWString().c_str());

	if (enforce == "")
		lstrcat(szUpdate, L"0");
	else
		lstrcat(szUpdate, L"1");
	
	/************************************************************************/
	/* 	�������½���														*/
	/*	����˵����															*/
	/*	-downpath		����·��											*/
	/*	-version		�汾��												*/
	/*	-update			�Ƿ���£�0�����£�1����							*/
	/************************************************************************/
	std::wstring wszCmdLine;
	std::wstringstream wstream;
	wstream << L" -downpath " << szDownPath
		<< L" -version " << szVersion
		<< L" -update " << szUpdate
		<< std::endl;
	wszCmdLine = wstream.str();

	STARTUPINFOW	si = { 0 };
	si.cb = sizeof(si);

	PROCESS_INFORMATION	pi;

	BOOL bIsSuc = CreateProcess(szTempPath, (LPWSTR)wszCmdLine.c_str(), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
}

void UIMenu::setVersion(QString version)
{
	m_version = version;
}

void UIMenu::checkVersion()
{
	QString strUrl;
	if (g_environmentType){
		strUrl = "https://qatime.cn/api/v1/system/check_update?=category=student_client&version={version}&platform=windows";
		strUrl.replace("{version}", m_version);
	}
	else{		  
		strUrl = "http://testing.qatime.cn/api/v1/system/check_update?category=student_client&version={version}&platform=windows";
		strUrl.replace("{version}", m_version);
	}

	QUrl url = QUrl(strUrl);
	QNetworkRequest request(url);

	request.setRawHeader("Remember-Token", g_remeberToken.toUtf8());
	reply = manager.get(request);
	connect(reply, &QNetworkReply::finished, this, &UIMenu::returnVersion);
}

void UIMenu::returnVersion()
{
	QString strError;
	QByteArray result = reply->readAll();
	QJsonDocument document(QJsonDocument::fromJson(result));
	QJsonObject obj = document.object();
	QJsonObject data = obj["data"].toObject();
	QJsonObject error = obj["error"].toObject();

	if (obj["status"].toInt() == 1)
	{
		// ��ס��ʦ��Ϣ�������Զ���¼
		QString version = data["version"].toString();
		QString down_links = data["cdn_url"].toString();
		bool bEnforce = data["enforce"].toBool();
		
		if (version == "")
			StartCheck("", version,down_links);
		else 
			StartCheck("true", version, down_links);
	}
}

void UIMenu::leaveEvent(QEvent *e)
{
	hide();
}