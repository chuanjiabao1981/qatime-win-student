#include "UIWindowSet.h"
#include <QPainter>
#include <windowsx.h>
#include "wtypes.h"
#include <QMouseEvent>
//#include <QMediaPlayer>
//#include <QMediaPlaylist>
#include <QVideoWidget>
#include "member.h"
#include <QToolTip>
#include <iosfwd>
#include <sstream>
#include <tlhelp32.h>

#define MAINWINDOW_X_MARGIN 10
#define MAINWINDOW_Y_MARGIN 10
#define MAINWINDOW_TITLE_HEIGHT 0

#ifdef TEST
#define _DEBUG
#else
#endif

UIWindowSet::UIWindowSet(QWidget *parent)
	: QWidget(parent)
	, m_spacer(NULL)
	, m_vecTags(NULL)
	, m_vecChatRoom(NULL)
	, m_NoticeWnd(NULL)
	, m_CourseWnd(NULL)
	, m_LessonWnd(NULL)
	, m_curTags(NULL)
	, m_curChatRoom(NULL)
	, m_hBoardWnd(NULL)
	, m_hCameraWnd(NULL)
	, m_bInitLive(false)
	, m_EnvironmentalTyle(true)
{
	ui.setupUi(this);
	setAutoFillBackground(true);
	QPalette p = palette();
	p.setColor(QPalette::Window, QColor(255, 255, 255));
	setPalette(p);
//	setAttribute(Qt::WA_TranslucentBackground, true);
//	setMouseTracking(true);
	ui.logo_pushButton->installEventFilter(this);
	ui.title_pushButton->installEventFilter(this);
	ui.whiteboard_widget->installEventFilter(this);

	connect(ui.min_pushButton, SIGNAL(clicked()), this, SLOT(MinDialog()));
	connect(ui.max_pushButton, SIGNAL(clicked()), this, SLOT(MaxDialog()));
	connect(ui.close_pushButton, SIGNAL(clicked()), this, SLOT(CloseDialog()));
	connect(ui.return_pushButton, SIGNAL(clicked()), this, SLOT(ReturnDialog()));
	connect(ui.notice_pushButton, SIGNAL(clicked()), this, SLOT(clickNotice()));
	connect(ui.course_pushButton, SIGNAL(clicked()), this, SLOT(clickCourse()));
	connect(ui.person_pushButton, SIGNAL(clicked()), this, SLOT(clickPerson()));
	connect(ui.lesson_pushButton, SIGNAL(clicked()), this, SLOT(clickLesson()));
	connect(ui.change_pushButton, SIGNAL(clicked(bool)), this, SLOT(clickChange(bool)));
	connect(this, SIGNAL(sig_Modle(bool)), this, SLOT(slots_Modle(bool)));

	m_NoticeWnd = new UINoticeWnd(this);
	m_NoticeWnd->setParentBtn(this);
	m_NoticeWnd->hide();

	m_CourseWnd = new UICourseWnd(this);
	m_CourseWnd->setParentBtn(this);
	m_CourseWnd->hide();

	m_PersonWnd = new UIPersonWnd(this);
	m_PersonWnd->setParentBtn(this);
	m_PersonWnd->hide();

	m_LessonWnd = new UILessonList(this);
	m_LessonWnd->setParentBtn(this);
	m_LessonWnd->hide();

	QFont font = ui.notice_pushButton->font();
	font.setPixelSize(15);
	ui.notice_pushButton->setFont(font);

	font = ui.course_pushButton->font();
	font.setPixelSize(15);
	ui.course_pushButton->setFont(font);

	font = ui.lesson_pushButton->font();
	font.setPixelSize(15);
	ui.lesson_pushButton->setFont(font);

	font = ui.person_pushButton->font();
	font.setPixelSize(15);
	ui.person_pushButton->setFont(font);

	font = ui.change_pushButton->font();
	font.setPixelSize(15);
	ui.change_pushButton->setFont(font);

	font = ui.return_pushButton->font();
	font.setPixelSize(15);
	ui.return_pushButton->setFont(font);

	ui.camera_widget->setVisible(false);
	ui.whiteboard_widget->setVisible(false);

	ui.line_label->setVisible(false);
	ui.return_pushButton->setVisible(false);

	m_LiveTimer = new QTimer(this);
	connect(m_LiveTimer, SIGNAL(timeout()), this, SLOT(slot_onTimeout()));

	ChangeBtnStyle(false);
}

UIWindowSet::~UIWindowSet()
{

}

void UIWindowSet::MinDialog()
{
	showMinimized();
}

void UIWindowSet::CloseDialog()
{
	if (m_vecTags.size() > 0)
	{
		std::vector<UITags*>::iterator it;
		for (it = m_vecTags.begin(); it != m_vecTags.end(); it++)
		{
			UITags* tags = *it;
			tags->GetRoom()->setVisible(false);
			tags->close();
			delete tags;
		}
	}
	m_vecTags.clear();
	m_mapTags.clear();
	m_curTags = NULL;
	m_curChatRoom = NULL;
	PlayLive("", "");

	hide();
}

void UIWindowSet::MaxDialog()
{
	if (this->isMaximized())
	{
		showNormal();
		return;
	}
	showMaximized();
}

void UIWindowSet::ReturnDialog()
{
	if (m_parent)
		m_parent->returnClick();
}

bool UIWindowSet::nativeEvent(const QByteArray &eventType, void *message, long *result)
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

void UIWindowSet::paintEvent(QPaintEvent *event)
{
	QPainterPath path;
	QPainter painter(this);

 	QColor color(5, 157, 213);
	for (int i = 0; i < 45;i++)
		path.addRect(0, i, this->width()-1, this->height()-i-1);

	painter.setPen(color);
	painter.drawPath(path);
}

void UIWindowSet::resizeEvent(QResizeEvent *e)
{
}

// 拖动标题做的处理
bool UIWindowSet::eventFilter(QObject *target, QEvent *event)
{
	if (target == ui.logo_pushButton)
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
	else if (target == ui.title_pushButton)
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
	else if (target == ui.whiteboard_widget)
	{
		if (event->type() == QEvent::Resize)
		{
			int iWidth = ui.whiteboard_widget->width();
			int iHeight = ui.whiteboard_widget->height();
			
			if (m_curTags && m_curTags->IsModle())
			{
				QString url;
				if (m_EnvironmentalTyle)
					url = "https://qatime.cn/play.html?address={rtmp}&width={width}&height={height}";//&width={width}&height={height}
				else
					url = "http://testing.qatime.cn/play.html?address={rtmp}&width={width}&height={height}";//&width={width}&height={height}
				
				url.replace("{rtmp}", m_curTags->BoardStream()); //"rtmp://v1.one-tv.com/live/mpegts.stream" 
				url.replace("{width}", QString::number(iWidth));
				url.replace("{height}", QString::number(iHeight));

				COPYDATASTRUCT sendData;
				char result[MAX_PATH];
				QByteArray chPath = url.toLatin1();

				strcpy(result, chPath.data());

				ZeroMemory(&sendData, sizeof(sendData));
				sendData.lpData = result;
				sendData.cbData = MAX_PATH;

				if (IsWindow(m_hBoardWnd))
					::SendMessage(m_hBoardWnd, WM_COPYDATA, 0, (LPARAM)&sendData);

				MoveWindow(m_hBoardWnd, 0, 0, iWidth, iHeight, true);
			}
		}
		return false;
	}
	return false;
}

void UIWindowSet::setMainWindow(UIMainWindow* parent)
{
	m_parent = parent;
}

void UIWindowSet::setStudent(QString id)
{
	m_studentID = id;
}

UITags* UIWindowSet::AddTag(QString chatID, QString name, QString ID, bool sel, UIChatRoom* room, QString status)
{
	UITags* tag = new UITags(ui.tags_widget);
	tag->setMaximumWidth(200);
	tag->SetCourseNameAndID(name,ID, chatID);
	tag->SetMainWindow(this);
	tag->SetRoom(room);
	tag->setStyle(sel);
	tag->show();
	connect(tag, SIGNAL(clickTag(UITags*)), this, SLOT(clickTag(UITags*)));
	ui.horizontalLayout_3->addWidget(tag);

	m_vecTags.push_back(tag);
	m_mapTags.insert(tag->ChatID(), tag);
	titleButtonWidth();
	room->show();

	m_curTags = tag;
	m_curChatRoom = room;
	m_curTags->update();

	if (status == "teaching")
	{
		m_curTags->setModle(true);
		emit sig_Modle(true);
	}
	else
	{
		m_curTags->setModle(false);
		emit sig_Modle(false);
	}
	return tag;
}

void UIWindowSet::DeleleTag(UITags* tag)
{
	QMap<QString, UITags*>::iterator iter;
	UITags* item = NULL;
	iter = m_mapTags.find(tag->ChatID());
	if (iter != m_mapTags.end())
	{
		m_mapTags.erase(iter);
	}

	if (m_vecTags.size() > 0)
	{
		std::vector<UITags*>::iterator it;
		for (it = m_vecTags.begin(); it != m_vecTags.end(); it++)
		{
			UITags* tags = *it;
			bool bSel = false;
			// 判断是当前会话窗口
			if (tag == tags)
			{
				bSel = tags->IsSelect();
				tags->GetRoom()->setVisible(false);

				m_vecTags.erase(it);
				tags->close();
				delete tags;
				titleButtonWidth();

				// 关闭的窗口，是否是当前显示的
				if (bSel)
					AgainSelectTag();
				
				return;
			}
		}
	}
}

void UIWindowSet::AddChatRoom(QString chatID, QString courseid, QString teacherid, QString token, QString studentName, std::string strCurAudioPath, QString name, int UnreadCount, QString status)
{
	show();
	if (isMinimized())
		showNormal();
	SetWindowPos((HWND)winId(), HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	QMap<QString, UITags*>::iterator it;
	it = m_mapTags.find(chatID);
	if (it == m_mapTags.end())
	{
		// 先做下限制
		if (m_vecTags.size() == 10)
		{
			CMessageBox::showMessage(QString("答疑时间"), QString("最多能添加10个辅导班！！"), QString("确定"), QString());
			return;
		}
	}

	if (IsHasTag(chatID, status))
		return;

	UIChatRoom* chatRoom = IsHasRoom(chatID);
	if (chatRoom == NULL)
	{
		chatRoom = new UIChatRoom(ui.chat_widget);
		chatRoom->setWindowFlags(Qt::FramelessWindowHint);
		chatRoom->setMainWindow(this);
		chatRoom->SetEnvironmental(m_EnvironmentalTyle);
		chatRoom->setCurChatID(chatID, courseid, teacherid, token, studentName, m_accid, UnreadCount);
		chatRoom->SetCurAudioPath(strCurAudioPath);
		chatRoom->InitAudioCallBack();
		ui.horizontalLayout_6->addWidget(chatRoom);
		m_vecChatRoom.push_back(chatRoom);
		m_mapChatRoom.insert(chatID, chatRoom);
		chatRoom->show();
		chatRoom->setEditFocus();
	}

	UITags* tag = AddTag(chatID, name, courseid, true, chatRoom, status);
}

bool UIWindowSet::ReceiverMsg(nim::IMMessage* pIMsg)
{
	QString strChatID = QString::fromStdString(pIMsg->local_talk_id_.c_str());
	QMap<QString, UIChatRoom*>::Iterator it;
	UIChatRoom* room=NULL;
	it = m_mapChatRoom.find(strChatID);
	if (it != m_mapChatRoom.end())
	{
		room = *it;
		room->ReceiverMsg(pIMsg);
	}

	QMap<QString, UITags*>::Iterator itTag;
	itTag = m_mapTags.find(strChatID);
	if (itTag != m_mapTags.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

void UIWindowSet::ReceiverRecordMsg(nim::QueryMsglogResult* pIMsg)
{
	if (m_vecChatRoom.size() > 0)
	{
		std::vector<UIChatRoom*>::iterator it;
		for (it = m_vecChatRoom.begin(); it != m_vecChatRoom.end(); it++)
		{
			UIChatRoom* room = *it;

			// 判断历史消息>0， 且是当前会话窗口
			if (pIMsg->msglogs_.size() > 0 && strcmp(pIMsg->msglogs_.front().local_talk_id_.c_str(), room->GetCurChatID().c_str()) == 0)
			{
				room->ReceiverRecordMsg(pIMsg);
				return;
			}
		}
	}
}

void UIWindowSet::OnStopPlayAudio(std::string sid, char* msgid)
{
	if (m_vecChatRoom.size() > 0)
	{
		std::vector<UIChatRoom*>::iterator it;
		for (it = m_vecChatRoom.begin(); it != m_vecChatRoom.end(); it++)
		{
			UIChatRoom* room = *it;
			// 判断是当前会话窗口
			if (strcmp(sid.c_str(), room->GetCurChatID().c_str()) == 0)
			{
				room->OnStopPlayAudio(msgid);
				return;
			}
		}
	}
}

void UIWindowSet::ReceiverLoginMsg(nim::LoginRes* pLogMsg)
{
	if (m_vecChatRoom.size() > 0)
	{
		std::vector<UIChatRoom*>::iterator it;
		for (it = m_vecChatRoom.begin(); it != m_vecChatRoom.end(); it++)
		{
			UIChatRoom* room = *it;
			room->ReceiverLoginMsg(pLogMsg);
		}
	}
}

void UIWindowSet::ReceiverMemberMsg(std::string sid, std::list<nim::TeamMemberProperty>* pMemberMsg)
{
	if (m_vecChatRoom.size() > 0)
	{
		std::vector<UIChatRoom*>::iterator it;
		for (it = m_vecChatRoom.begin(); it != m_vecChatRoom.end(); it++)
		{
			UIChatRoom* room = *it;
			// 判断是当前会话窗口
			if (strcmp(sid.c_str(), room->GetCurChatID().c_str()) == 0)
			{
				room->ReceiverMemberMsg(pMemberMsg);
				return;
			}
		}
	}
}

void UIWindowSet::SendStatus(nim::SendMessageArc* arcNew)
{
	if (m_vecChatRoom.size() > 0)
	{
		std::vector<UIChatRoom*>::iterator it;
		for (it = m_vecChatRoom.begin(); it != m_vecChatRoom.end(); it++)
		{
			UIChatRoom* room = *it;
			// 判断是当前会话窗口
			if (strcmp(arcNew->talk_id_.c_str(), room->GetCurChatID().c_str()) == 0 && arcNew->rescode_ != 200)
			{
				room->SendStatus(arcNew);
				return;
			}
		}
	}
}

void UIWindowSet::SendAudio(std::string sid, std::string msgid, std::string path, long size, int dur, std::string fileEx)
{
	if (m_vecChatRoom.size() > 0)
	{
		std::vector<UIChatRoom*>::iterator it;
		for (it = m_vecChatRoom.begin(); it != m_vecChatRoom.end(); it++)
		{
			UIChatRoom* room = *it;
			// 判断是当前会话窗口
			if (strcmp(sid.c_str(), room->GetCurChatID().c_str()) == 0)
			{
				room->SendAudio(QString::fromStdString(msgid), QString::fromStdString(path), size, dur, fileEx);
				return;
			}
		}
	}
}

QPixmap UIWindowSet::StudentPhotoPixmap()
{
	if (m_parent)
		return m_parent->StudentPhotoPixmap();

	return NULL;
}

bool UIWindowSet::IsCaptrueAudio()
{
	if (m_vecChatRoom.size() > 0)
	{
		std::vector<UIChatRoom*>::iterator it;
		for (it = m_vecChatRoom.begin(); it != m_vecChatRoom.end(); it++)
		{
			UIChatRoom* room = *it;
			if (room->IsCaptrueAudio())
			{
				return true;
			}
		}
	}
	return false;
}

void UIWindowSet::clickTag(UITags* tag)
{
	if (m_vecTags.size() > 0)
	{
		std::vector<UITags*>::iterator it;
		for (it = m_vecTags.begin(); it != m_vecTags.end(); it++)
		{
			UITags* tags = *it;
			if (tags == tag)
			{
				tags->setStyle(true);
				tags->GetRoom()->setVisible(true);
				m_curTags = tags;
				m_curChatRoom = tags->GetRoom();
				tags->update();
			}
			else
			{
				// 隐藏没选中的
				tags->setStyle(false);
				tags->GetRoom()->setVisible(false);
				tags->update();
			}
		}
	}

	if (m_NoticeWnd)
	{
		ui.notice_pushButton->setStyleSheet("color:rgb(0,0,0);background-color: rgb(255, 255, 255);");
		m_NoticeWnd->hide();
	}
		
	if (m_CourseWnd)
	{
		ui.course_pushButton->setStyleSheet("color:rgb(0,0,0);background-color: rgb(255, 255, 255);");
		m_CourseWnd->hide();
	}

	if (m_PersonWnd)
	{
		ui.person_pushButton->setStyleSheet("color:rgb(0,0,0);background-color: rgb(255, 255, 255);");
		m_PersonWnd->hide();
	}
		
	if (m_LessonWnd)
	{
		ui.lesson_pushButton->setStyleSheet("color:rgb(0,0,0);background-color: rgb(255, 255, 255);");
		m_LessonWnd->hide();
	}
		

	if (m_curTags->IsModle())
	{
		emit sig_Modle(true);
	}
	else
	{
		emit sig_Modle(false);
	}
}

// 可拖动标题的宽度
void UIWindowSet::titleButtonWidth()
{
	int iCount = m_vecTags.size();
	int iWidth = iCount * 200 + iCount * 6;
	int iTagWidth = width() - ui.logo_pushButton->width() - ui.widget->width() - 48;
	if (iWidth >= iTagWidth)
	{
		ui.tags_widget->setMaximumWidth(iTagWidth);
	}
	else
	{
		ui.tags_widget->setMaximumWidth(iWidth);
	}

}

void UIWindowSet::AgainSelectTag()
{
	if (m_vecTags.size() == 0)
	{
		m_curTags = NULL;
		m_curChatRoom = NULL;
		PlayLive("","");
		hide();
	}
	else
	{
		UITags* tag = m_vecTags.front();
		clickTag(tag);
	}
}

bool UIWindowSet::IsHasTag(QString chatID, QString status)
{
	bool bHas = false;
	if (m_vecTags.size() > 0)
	{
		std::vector<UITags*>::iterator it;
		for (it = m_vecTags.begin(); it != m_vecTags.end(); it++)
		{
			UITags* tags = *it;
			if (tags->ChatID() == chatID)
			{
				tags->setStyle(true);
				tags->GetRoom()->setVisible(true);
				m_curTags = tags;
				m_curChatRoom = tags->GetRoom();
				m_curChatRoom->setEditFocus();
				bHas = true;
				tags->update();

				if (m_curTags->IsModle())
					emit sig_Modle(true);
				else
					emit sig_Modle(false);
			}
			else
			{
				// 隐藏没选中的
				tags->setStyle(false);
				tags->GetRoom()->setVisible(false);
				tags->update();
			}
		}
	}

	return bHas;
}

UIChatRoom* UIWindowSet::IsHasRoom(QString chatID)
{
	if (m_vecChatRoom.size() > 0)
	{
		std::vector<UIChatRoom*>::iterator it;
		for (it = m_vecChatRoom.begin(); it != m_vecChatRoom.end(); it++)
		{
			UIChatRoom* room = *it;
			if (QString::fromStdString(room->GetCurChatID()) == chatID)
			{
				return room;
			}
		}
	}

	return NULL;
}

void UIWindowSet::SetToken(QString token)
{
	m_Token = token;
}

void UIWindowSet::clickNotice()
{
	if (m_NoticeWnd)
	{
		if (m_NoticeWnd->isVisible())
		{
			ui.notice_pushButton->setStyleSheet("color:rgb(0,0,0);background-color: rgb(255, 255, 255);");
			m_NoticeWnd->setVisible(false);
			return;
		}

		m_NoticeWnd->initDate();
		ui.notice_pushButton->setStyleSheet("color:rgb(255, 255, 255);background-color: rgb(5,157,213);");
		m_NoticeWnd->move(4, 74);
		m_NoticeWnd->DeleteNotice();
		m_NoticeWnd->show();
		m_NoticeWnd->setFocus();
		QueryNotice();
	}
}

void UIWindowSet::QueryNotice()
{
	QString strCourseID = "";
	if (m_curTags)
		strCourseID = m_curTags->CourseID();
	else
		return;

	QString strUrl;
// #ifdef _DEBUG
// 	strUrl = "http://testing.qatime.cn/api/v1/live_studio/courses/{id}/realtime";
// 	strUrl.replace("{id}", strCourseID);
// #else
// 	strUrl = "https://qatime.cn/api/v1/live_studio/courses/{id}/realtime";
// 	strUrl.replace("{id}", strCourseID);
// #endif
	if (m_EnvironmentalTyle)
	{
		strUrl = "https://qatime.cn/api/v1/live_studio/courses/{id}/realtime";
		strUrl.replace("{id}", strCourseID);
	}
	else
	{
		strUrl = "http://testing.qatime.cn/api/v1/live_studio/courses/{id}/realtime";
		strUrl.replace("{id}", strCourseID);
	}

	QUrl url = QUrl(strUrl);
	QNetworkRequest request(url);

	request.setRawHeader("Remember-Token", m_Token.toUtf8());
	reply = manager.get(request);
	connect(reply, &QNetworkReply::finished, this, &UIWindowSet::returnNotice);
}


void UIWindowSet::returnNotice()
{
	QByteArray result = reply->readAll();
	QJsonDocument document(QJsonDocument::fromJson(result));
	QJsonObject obj = document.object();
	QJsonObject data = obj["data"].toObject();
	QJsonObject error = obj["error"].toObject();
	if (obj["status"].toInt() == 1 && data.contains("members"))
	{
		// 群公告信息
		QJsonArray announcements = data["announcements"].toArray();
		foreach(const QJsonValue & value, announcements)
		{
			QJsonObject obj = value.toObject();
			YXMember *announcements = new YXMember();
			announcements->readJsonToAnnouncement(value.toObject());

			if (m_NoticeWnd)
				m_NoticeWnd->AddNotic(announcements->announcement(), announcements->announcementTime());

			//用完之后删除
			delete announcements;
		}
	}
	else if (obj["status"].toInt() == 0)
	{
		return;
	}
}

void UIWindowSet::clickCourse()
{
	if (m_CourseWnd)
	{
		if (m_CourseWnd->isVisible())
		{
			ui.course_pushButton->setStyleSheet("color:rgb(0,0,0);background-color: rgb(255, 255, 255);");
			m_CourseWnd->setVisible(false);
			return;
		}

		ui.course_pushButton->setStyleSheet("color:rgb(255, 255, 255);background-color: rgb(5,157,213);");
		m_CourseWnd->move(4, 74);
		m_CourseWnd->show();
		m_CourseWnd->setFocus();
		QueryCourse();
	}
}

void UIWindowSet::QueryCourse()
{
	QString strCourseID = "";
	if (m_curTags)
		strCourseID = m_curTags->CourseID();
	else
		return;

	QString strUrl;
// #ifdef _DEBUG
// 	strUrl = "http://testing.qatime.cn/api/v1/live_studio/students/{studentid}/courses/{id}";
// 	strUrl.replace("{id}", strCourseID);
// 	strUrl.replace("{studentid}", m_studentID);
// #else
// 	strUrl = "https://qatime.cn/api/v1/live_studio/students/{studentid}/courses/{id}";
// 	strUrl.replace("{id}", strCourseID);
// 	strUrl.replace("{studentid}", m_studentID);
// #endif
	if (m_EnvironmentalTyle)
	{
		strUrl = "https://qatime.cn/api/v1/live_studio/students/{studentid}/courses/{id}";
		strUrl.replace("{id}", strCourseID);
		strUrl.replace("{studentid}", m_studentID);
	}
	else
	{
		strUrl = "http://testing.qatime.cn/api/v1/live_studio/students/{studentid}/courses/{id}";
		strUrl.replace("{id}", strCourseID);
		strUrl.replace("{studentid}", m_studentID);
	}

	QUrl url = QUrl(strUrl);
	QNetworkRequest request(url);

	request.setRawHeader("Remember-Token", m_Token.toUtf8());
	reply = manager.get(request);
	connect(reply, &QNetworkReply::finished, this, &UIWindowSet::returnCourse);
}


void UIWindowSet::returnCourse()
{
	QByteArray result = reply->readAll();
	QJsonDocument document(QJsonDocument::fromJson(result));
	QJsonObject obj = document.object();
	QJsonObject data = obj["data"].toObject();
	QJsonObject error = obj["error"].toObject();
	if (obj["status"].toInt() == 1)
	{
		// 辅导班信息
		QString coursePic = data["publicize"].toString();
		QString courseName = data["name"].toString();
		QString courseGrade = data["grade"].toString();
		QString courseGrade1 = data["subject"].toString();
		QString teacherName = data["teacher_name"].toString();
		QString coursePross = QString::number(data["completed_lesson_count"].toInt());
		QString courseProsses = QString::number(data["preset_lesson_count"].toInt());

		QString courseStart = data["live_start_time"].toString();
		QString courseEnd = data["live_end_time"].toString();
		QString courseDesc = data["description"].toString();
	
		// 年级信息
		courseGrade = courseGrade + courseGrade1 + " | 主讲:" + teacherName;
		coursePross = "课程进度: " + coursePross +"/"+ courseProsses;
		courseStart = "时      间: " + courseStart + "至" + courseEnd;
		if (m_CourseWnd)
			m_CourseWnd->AddInfo(coursePic, courseName, courseGrade, coursePross, courseStart, courseDesc);
	}
	else if (obj["status"].toInt() == 0)
	{
		return;
	}
}

void UIWindowSet::clickPerson()
{
	if (m_PersonWnd)
	{
		if (m_PersonWnd->isVisible())
		{
			ui.person_pushButton->setStyleSheet("color:rgb(0,0,0);background-color: rgb(255, 255, 255);");
			m_PersonWnd->setVisible(false);
			return;
		}

		ui.person_pushButton->setStyleSheet("color:rgb(255, 255, 255);background-color: rgb(5,157,213);");
		m_PersonWnd->move(280, 74);
		m_PersonWnd->DeletePerson();
		m_PersonWnd->show();
		m_PersonWnd->setFocus();
		QueryPerson();
	}
}

void UIWindowSet::QueryPerson()
{
	if (m_curChatRoom)
	{
		std::vector<personListBuddy*> vecBuddy;
		if (!m_curChatRoom->IsPerson())
		{
			QToolTip::showText(QCursor::pos(), "成员初始化中,请稍后查询！");
			m_PersonWnd->hide();
			return;
		}

		vecBuddy = m_curChatRoom->GetBuddy();
		if (m_PersonWnd)
			m_PersonWnd->AddPerson(vecBuddy, m_curChatRoom->GetTeacherID());
	}
}

void UIWindowSet::clickLesson()
{
	if (m_LessonWnd)
	{
		if (m_LessonWnd->isVisible())
		{
			ui.lesson_pushButton->setStyleSheet("color:rgb(0,0,0);background-color: rgb(255, 255, 255);");
			m_LessonWnd->setVisible(false);
			return;
		}

		ui.lesson_pushButton->setStyleSheet("color:rgb(255, 255, 255);background-color: rgb(5,157,213);");
		m_LessonWnd->move(4, 74);
		m_LessonWnd->DeleteLesson();
		m_LessonWnd->show();
		m_LessonWnd->setFocus();
		QueryLesson();
	}
}

void UIWindowSet::QueryLesson()
{
	QString strCourseID = "";
	if (m_curTags)
		strCourseID = m_curTags->CourseID();
	else
		return;

	QString strUrl;
// #ifdef _DEBUG
// 	strUrl = "http://testing.qatime.cn/api/v1/live_studio/students/{studentid}/courses/{id}";
// 	strUrl.replace("{id}", strCourseID);
// 	strUrl.replace("{studentid}", m_studentID);
// #else
// 	strUrl = "https://qatime.cn/api/v1/live_studio/students/{studentid}/courses/{id}";
// 	strUrl.replace("{id}", strCourseID);
// 	strUrl.replace("{studentid}", m_studentID);
// #endif
	if (m_EnvironmentalTyle)
	{
		strUrl = "https://qatime.cn/api/v1/live_studio/students/{studentid}/courses/{id}";
		strUrl.replace("{id}", strCourseID);
		strUrl.replace("{studentid}", m_studentID);
	}
	else
	{
		strUrl = "http://testing.qatime.cn/api/v1/live_studio/students/{studentid}/courses/{id}";
		strUrl.replace("{id}", strCourseID);
		strUrl.replace("{studentid}", m_studentID);
	}

	QUrl url = QUrl(strUrl);
	QNetworkRequest request(url);

	request.setRawHeader("Remember-Token", m_Token.toUtf8());
	reply = manager.get(request);
	connect(reply, &QNetworkReply::finished, this, &UIWindowSet::returnLesson);
}

void UIWindowSet::returnLesson()
{
	int iCount = 0;
	QByteArray result = reply->readAll();
	QJsonDocument document(QJsonDocument::fromJson(result));
	QJsonObject obj = document.object();
	QJsonObject data = obj["data"].toObject();
	QJsonObject error = obj["error"].toObject();
	if (obj["status"].toInt() == 1)
	{
		// 群公告信息
		QJsonArray lesson = data["lessons"].toArray();
		foreach(const QJsonValue & value, lesson)
		{
			iCount++;
			QJsonObject obj = value.toObject();
			QString strClassDate = obj["class_date"].toString();
			QString strLiveTime = obj["live_time"].toString();
			QString strStatus = obj["status"].toString();
			QString strName = obj["name"].toString();

			strClassDate = strClassDate + "  " + strLiveTime;
			if (m_LessonWnd)
				m_LessonWnd->AddLesson(QString().sprintf("%02d", iCount), strClassDate, strName, strStatus);
		}
	}
	else if (obj["status"].toInt() == 0)
	{
		return;
	}
}

void UIWindowSet::ReceiverChatMsg(nim::IMMessage* pIMsg)
{
	QString strChatID = QString::fromStdString(pIMsg->local_talk_id_.c_str());
	QMap<QString, UIChatRoom*>::Iterator it;
	UIChatRoom* room = NULL;
	it = m_mapChatRoom.find(strChatID);
	if (it != m_mapChatRoom.end())
	{
		room = *it;
		room->ShowChatMsg(*pIMsg);
	}
}

void UIWindowSet::setAccid(QString accid)
{
	m_accid = accid;
}

void UIWindowSet::clickChange(bool checked)
{
	if (ui.camera_widget->isVisible())
	{
		ui.line_label->setVisible(false);
		ui.camera_widget->setVisible(false);
		ui.whiteboard_widget->setVisible(false);
		ui.chatcamera_widget->setMaximumWidth(3000);

		if (m_curTags)
			m_curTags->setModle(false);
	}
	else
	{
		ui.line_label->setVisible(true);
		ui.camera_widget->setVisible(true);
		ui.whiteboard_widget->setVisible(true);
		ui.chatcamera_widget->setMaximumWidth(300);

		InitBoardView();

		QueryLiveInfo();

		if (m_curTags)
			m_curTags->setModle(true);
	}
}

void UIWindowSet::InitBoardView()
{
	if (!m_bInitLive)
	{
		// 启动直播视频
		TCHAR szTempPath[MAX_PATH] = { 0 };
		GetCurrentDirectory(MAX_PATH, szTempPath);
		lstrcat(szTempPath, L"\\CMSVideo.exe");
//		lstrcat(szTempPath, L"F:\\迅雷下载\\duilibcefdemo\\out\\Release\\CMSVideo.exe");
		std::wstring wszCmdLine;
		std::wstringstream wstream;
		HWND hwnd = (HWND)ui.whiteboard_widget->winId();
		wstream << L" -parenthwnd " << hwnd
			<< std::endl;
		wszCmdLine = wstream.str();

		STARTUPINFOW	si = { 0 };
		si.cb = sizeof(si);

		PROCESS_INFORMATION	pi;

		BOOL bIsSuc = CreateProcess(szTempPath, (LPWSTR)wszCmdLine.c_str(), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);

		// 启动直播视频
		TCHAR szCameraPath[MAX_PATH] = { 0 };
		GetCurrentDirectory(MAX_PATH, szCameraPath);
		lstrcat(szCameraPath, L"\\CVideo.exe");
//		lstrcat(szCameraPath, L"F:\\迅雷下载\\duilibcefdemo1\\out\\Release\\CVideo.exe");

		std::wstring wszCmdLineCamera;
		std::wstringstream wstreamCamera;
		HWND chwnd = (HWND)ui.camera_widget->winId();
		wstreamCamera << L" -parenthwnd " << chwnd
			<< std::endl;
		wszCmdLineCamera = wstreamCamera.str();

		STARTUPINFOW csi = { 0 };
		csi.cb = sizeof(csi);

		bIsSuc = CreateProcess(szCameraPath, (LPWSTR)wszCmdLineCamera.c_str(), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &csi, &pi);

		m_bInitLive = true;
	}
}

void UIWindowSet::QueryLiveInfo()
{
	QString strCourseID = "";
	if (m_curTags)
	{
		if (m_curTags->BoardStream().isEmpty() || m_curTags->CameraStream().isEmpty())
		{
			strCourseID = m_curTags->CourseID();
		}
		else
		{
			PlayLive(m_curTags->BoardStream(), m_curTags->CameraStream());
			return;
		}
	}
	else
	{
		return;
	}

	QString strUrl;
	if (m_EnvironmentalTyle)
	{
		strUrl = "https://qatime.cn/api/v1/live_studio/courses/{id}/play_info";
		strUrl.replace("{id}", strCourseID);
	}
	else
	{
		strUrl = "http://testing.qatime.cn/api/v1/live_studio/courses/{id}/play_info";
		strUrl.replace("{id}", strCourseID);
	}

	QUrl url = QUrl(strUrl);
	QNetworkRequest request(url);

	request.setRawHeader("Remember-Token", m_Token.toUtf8());
	reply = manager.get(request);
	connect(reply, &QNetworkReply::finished, this, &UIWindowSet::returnLiveInfo);
}

void UIWindowSet::returnLiveInfo()
{
	QByteArray result = reply->readAll();
	QJsonDocument document(QJsonDocument::fromJson(result));
	QJsonObject obj = document.object();
	QJsonObject data = obj["data"].toObject();
	QJsonObject error = obj["error"].toObject();
	if (obj["status"].toInt() == 1)
	{
		// 拉流信息
		int iCourseId = data["id"].toInt();
		QJsonArray lesson = data["lessons"].toArray();
		foreach(const QJsonValue & value, lesson)
		{
			QJsonObject obj = value.toObject();
			QString strboardS = obj["board_pull_stream"].toString();
			QString strcameraS = obj["camera_pull_stream"].toString();
			
			if (!strboardS.isEmpty() && !strcameraS.isEmpty())
			{
				if (m_curTags->CourseID().toInt() == iCourseId)
				{
					m_curTags->setBoardStream(strboardS);
					m_curTags->setCameraStream(strcameraS);
				}
				PlayLive(strboardS, strcameraS);
				return;
			}
		}
	}
	else if (obj["status"].toInt() == 0)
	{
		return;
	}
}

void UIWindowSet::PlayLive(QString sBoard, QString sCamera)
{
	m_BoardPath = sBoard;
	m_CameraPath = sCamera;
//	 sBoard = "rtmp://live.hkstv.hk.lxdns.com/live/hks";
//	 sCamera= "rtmp://v1.one-tv.com/live/mpegts.stream";
	int iWidth = ui.whiteboard_widget->width();
	int iHeight = ui.whiteboard_widget->height();
	QString url;
	if (m_EnvironmentalTyle)
		url = "https://qatime.cn/play.html?address={rtmp}&width={width}&height={height}";//&width={width}&height={height}
	else
		url = "http://testing.qatime.cn/play.html?address={rtmp}&width={width}&height={height}";//&width={width}&height={height}
	url.replace("{rtmp}", sBoard);//"rtmp://live.hkstv.hk.lxdns.com/live/hks"
	url.replace("{width}", QString::number(iWidth));
	url.replace("{height}", QString::number(iHeight));

	COPYDATASTRUCT sendData;
	char result[MAX_PATH];
	QByteArray chPath = url.toLatin1();

	strcpy(result, chPath.data());

	ZeroMemory(&sendData, sizeof(sendData));
	sendData.lpData = result;
	sendData.cbData = MAX_PATH;

	if (IsWindow(m_hBoardWnd))
		::SendMessage(m_hBoardWnd, WM_COPYDATA, 3, (LPARAM)&sendData);
	else
	{
		m_LiveTimer->stop();
		m_LiveTimer->start(200);
	}

	iWidth = ui.camera_widget->width();
	iHeight = ui.camera_widget->height();
	if (m_EnvironmentalTyle)
		url = "https://qatime.cn/play.html?address={rtmp}&width={width}&height={height}";//&width={width}&height={height}
	else
		url = "http://testing.qatime.cn/play.html?address={rtmp}&width={width}&height={height}";//&width={width}&height={height}
	url.replace("{rtmp}", sCamera); //"rtmp://v1.one-tv.com/live/mpegts.stream"
	url.replace("{width}", QString::number(iWidth));
	url.replace("{height}", QString::number(iHeight));

	COPYDATASTRUCT sendCameraData;
	char resultCamera[MAX_PATH];
	QByteArray chCameraPath = url.toLatin1();

	strcpy(resultCamera, chCameraPath.data());

	ZeroMemory(&sendCameraData, sizeof(sendCameraData));
	sendCameraData.lpData = resultCamera;
	sendCameraData.cbData = MAX_PATH;

	if (IsWindow(m_hCameraWnd))
	{
		::SendMessage(m_hCameraWnd, WM_COPYDATA, 3, (LPARAM)&sendCameraData);
	}	
	else
	{
		m_LiveTimer->stop();
		m_LiveTimer->start(200);
	}
}

void UIWindowSet::slots_Modle(bool bModle)
{
	if (!bModle)
	{
		ui.line_label->setVisible(false);
		ui.camera_widget->setVisible(false);
		ui.whiteboard_widget->setVisible(false);
		ui.chatcamera_widget->setMaximumWidth(3000);
		PlayLive("","");
	}
	else
	{
		ui.line_label->setVisible(true);
		ui.camera_widget->setVisible(true);
		ui.whiteboard_widget->setVisible(true);
		ui.chatcamera_widget->setMaximumWidth(300);

		InitBoardView();
		
		QueryLiveInfo();
	}
}

void UIWindowSet::setBoardHwnd(HWND hwnd)
{
	m_hBoardWnd = hwnd;
}

void UIWindowSet::setCameraHwnd(HWND hwnd)
{
	m_hCameraWnd = hwnd;
}

bool UIWindowSet::IsHasFoucs(BTN_TYPE type)
{
	QPoint pt = mapFromGlobal(QCursor::pos());
	QPushButton* btn = NULL;
	switch (type)
	{
		case UIWindowSet::NoticeWnd:
		{
			btn = ui.notice_pushButton;
			break;
		}
		case UIWindowSet::CourseWnd:
		{
			btn = ui.course_pushButton;
			break;
		}
		case UIWindowSet::PersonWnd:
		{
			btn = ui.person_pushButton;
			break;
		}
		case UIWindowSet::LessonWnd:
		{
			btn = ui.lesson_pushButton;
			break;
		}
		default:
			break;
	}

	QRect rc = btn->geometry();
	rc.setY(rc.y() + ui.title_widget->geometry().height()+5);
	rc.setHeight(rc.height() + ui.title_widget->geometry().height()+5);
	if (rc.contains(pt))
		return true;
	else
		return false;
}

QPushButton*	 UIWindowSet::GetNoticeBtn()
{
	return ui.notice_pushButton;
}
QPushButton*	 UIWindowSet::GetLessonBtn()
{
	return ui.lesson_pushButton;
}
QPushButton*	 UIWindowSet::GetPersonBtn()
{
	return ui.person_pushButton;
}
QPushButton*	 UIWindowSet::GetCourseBtn()
{
	return ui.course_pushButton;
}

void UIWindowSet::slot_onTimeout()
{
	if (m_hBoardWnd != NULL && m_hCameraWnd != NULL)
	{
		m_LiveTimer->stop();
		PlayLive(m_BoardPath, m_CameraPath);
	}
}

void UIWindowSet::ChangeBtnStyle(bool bLive)
{
	if (bLive)
	{
		ui.change_pushButton->setStyleSheet("QPushButton{border-image:url(./images/liveChange_nor.png);}"
			"QPushButton:pressed{border-image:url(./images/liveChange_hover.png);}");
	}
	else
	{
		ui.change_pushButton->setStyleSheet("QPushButton{border-image:url(./images/noliveChange_nor.png);}"
				"QPushButton:pressed{border-image:url(./images/noliveChange_hover.png);}");
	}
}

void UIWindowSet::SetEnvironmental(bool bType)
{
	m_EnvironmentalTyle = bType;
}

void UIWindowSet::ReceiverAudioStatus(std::string sid, char* msgid, bool bSuc)
{
	if (m_vecChatRoom.size() > 0)
	{
		std::vector<UIChatRoom*>::iterator it;
		for (it = m_vecChatRoom.begin(); it != m_vecChatRoom.end(); it++)
		{
			UIChatRoom* room = *it;
			// 判断是当前会话窗口
			if (strcmp(sid.c_str(), room->GetCurChatID().c_str()) == 0)
			{
				room->SetAudioStatus(msgid,bSuc);
				return;
			}
		}
	}
}