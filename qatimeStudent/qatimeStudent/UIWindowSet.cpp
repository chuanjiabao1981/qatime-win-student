#include "UIWindowSet.h"
#include <QPainter>
#include <windowsx.h>
#include "wtypes.h"
#include <QMouseEvent>
#include <QVideoWidget>
#include "member.h"
#include <QToolTip>
#include <iosfwd>
#include <sstream>
#include <tlhelp32.h>

#include "1v1/IMInterface.h"

//1对1
#include "1v1/palette.h"
#include "1v1/UICamera1v1.h"
#include "1v1/UICameraS1v1.h"
#include "1v1/UIVideochange1v1.h"
#include "1v1/UIAudiochange1v1.h"
#include "1v1/UIAudioOutchange1v1.h"
#include "1v1/UIWhiteBoardTool.h"
#include "1v1/UIVideo1v1.h"
#include "1v1/UIAppWnd.h"
#include "1v1/UIAppWndTool.h"

#define MAINWINDOW_X_MARGIN 10
#define MAINWINDOW_Y_MARGIN 10
#define MAINWINDOW_TITLE_HEIGHT 0
#define LIVE_BUTTON_NAME	"选课直播"
#define LESSON_LABEL		"暂无直播"

#define Audio 0		//音频设备
#define AudioOut 2	//扬声器
#define Video 3		//视频设备
#define VIDEO_FPS 50  

UIWindowSet* m_This = NULL;
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
	m_This = this;
	setAutoFillBackground(true);
	QPalette p = palette();
	p.setColor(QPalette::Window, QColor(255, 255, 255));
	setPalette(p);
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

void UIWindowSet::ReturnDialog()
{
	if (m_parent)
		m_parent->returnClick();
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

void UIWindowSet::ReceiverLoginMsg(nim::LoginRes pLogMsg)
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
		ui.label_space->setVisible(false);
		ui.chatcamera_widget->setMaximumWidth(3000);

		if (m_curTags)
			m_curTags->setModle(false);
	}
	else
	{
		ui.line_label->setVisible(true);
		ui.camera_widget->setVisible(true);
		ui.whiteboard_widget->setVisible(true);
		ui.label_space->setVisible(true);
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
		ui.label_space->setVisible(false);
		ui.chatcamera_widget->setMaximumWidth(3000);
		PlayLive("","");
	}
	else
	{
		ui.line_label->setVisible(true);
		ui.camera_widget->setVisible(true);
		ui.whiteboard_widget->setVisible(true);
		ui.label_space->setVisible(true);
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

void UIWindowSet::joinRtsRoom(const std::string &roomName)
{
	IMInterface::getInstance()->joinRtsRoom(roomName, false);
}

void UIWindowSet::joinRoomSuccessfully(const std::string &session_id, __int64 channel_id, const std::string &custom_info)
{
	IMInterface::getInstance()->joinVChatRoom(2, m_curTags->ChatID().toStdString());//TODO  是否学生端只加入房间不创建房间？
}

// 加入音视频成功后，开始直播流程
void UIWindowSet::joinVChatSuccessfully()
{
	/*m_bLiving1v1 = true;
	m_LiveStatusManager->SendStart1v1LiveHttpMsg(m_lessonid, m_curTags->ChatID(), m_Token);*/
	//TODO  加入直播房间后应接受音视频流
}

void UIWindowSet::errorInfo(const QString & error)
{
	m_bLiving1v1 = false;
	CMessageBox::showMessage(
		QString("答疑时间"),
		QString("加入房间出现错误！代码：") + error,
		QString("确定"),
		QString("取消"));
}

void UIWindowSet::PicData(QString str)
{
	std::string picData = str.toStdString();
	IMInterface::getInstance()->SendData("", 1, picData);
}

void UIWindowSet::setDeviceInfos(int type)
{
	switch (type)
	{
	case Audio:
	{
		foreach(const DevInfo &info, IMInterface::getInstance()->getDeviceInfos())
		{
			if (info.type == type)
			{
				m_AudioChangeInfo1v1->setAudioParam(info.name, info.path);
			}
		}
	}
	break;
	case AudioOut:
	{
		foreach(const DevInfo &info, IMInterface::getInstance()->getDeviceInfos())
		{
			if (info.type == type)
			{
				m_AudioOutChangeInfo1v1->setAudioParam(info.name, info.path);
			}
		}
	}
	break;
	case Video:
		foreach(const DevInfo &info, IMInterface::getInstance()->getDeviceInfos())
		{
			if (info.type == type)
			{
				m_VideoChangeInfo1v1->setVideoParam(info.name, info.path);
			}
		}
		break;
	default:
		break;
	}
}

void UIWindowSet::clickVideo1v1Param()
{
	m_VideoChangeInfo1v1->show();
	m_VideoChangeInfo1v1->move(width() / 2 - m_VideoChangeInfo1v1->width() / 2, height() / 2 - m_VideoChangeInfo1v1->height() / 2);
	m_VideoChangeInfo1v1->setFocus();
}

void UIWindowSet::clickAudio1v1Param()
{
	m_AudioChangeInfo1v1->show();
	m_AudioChangeInfo1v1->move(width() / 2 - m_AudioChangeInfo1v1->width() / 2, height() / 2 - m_AudioChangeInfo1v1->height() / 2);
	m_AudioChangeInfo1v1->setFocus();
}

void UIWindowSet::clickAudioOut1v1Param()
{
	m_AudioOutChangeInfo1v1->show();
	m_AudioOutChangeInfo1v1->move(width() / 2 - m_AudioOutChangeInfo1v1->width() / 2, height() / 2 - m_AudioOutChangeInfo1v1->height() / 2);
	m_AudioOutChangeInfo1v1->setFocus();
}

void UIWindowSet::Audio1v1Status(int iStatus)
{
	if (iStatus)
	{
		IMInterface::getInstance()->endDevice(Audio);
	}
	else
	{
		//开启云信的摄像头采集
		QString dPath = m_AudioChangeInfo1v1->GetCurPath();
		if (!dPath.isNull())
			IMInterface::getInstance()->startDevice(Audio, dPath.toStdString(), 0, 0, 0);
	}
}

void UIWindowSet::Video1v1Status(int iStatus)
{
	if (iStatus)
	{
		//关闭云信的摄像头采集
		IMInterface::getInstance()->endDevice(Video);
		m_Camera1v1Info->StartEndVideo(true);
	}
	else
	{
		//开启云信的摄像头采集
		QString dPath = m_VideoChangeInfo1v1->GetCurPath();
		if (!dPath.isNull())
			IMInterface::getInstance()->startDevice(Video, dPath.toStdString(), VIDEO_FPS, 0, 0);
		m_Camera1v1Info->StartEndVideo(false);
	}
}

void UIWindowSet::AudioOut1v1Status(int iSatus)
{
	if (iSatus)
	{
		IMInterface::getInstance()->endDevice(AudioOut);
	}
	else
	{
		//开启云信的摄像头采集
		QString dPath = m_AudioOutChangeInfo1v1->GetCurPath();
		if (!dPath.isNull())
			IMInterface::getInstance()->startDevice(AudioOut, dPath.toStdString(), 0, 0, 0);
	}
}

//void UIWindowSet::clickLive1v1()
//{
//	if (m_bLiving1v1)
//	{
//		int iStatus = CMessageBox::showMessage(
//			QString("答疑时间"),
//			QString("是否关闭当前直播！"),
//			QString("确定"),
//			QString("取消"));
//		if (iStatus == 1)
//		{
//			// 结束白板和音视频
//			// 实现结束白板和音视频功能......待写
//
//			m_LiveStatusManager->SendStopLiveHttpMsg1v1();
//
//			if (m_CountTimer->isActive())
//			{
//				m_CountTimer->stop();					// 停止计时
//				m_iTimerCount = 0;						// 重置秒数
//				ui.time1v1_label->setVisible(false);	// 隐藏
//				ui.time1v1_label->setText("00:00:00");	// 重置时间
//			}
//
//			m_bLiving1v1 = false;
//		}
//		else
//			return;
//	}
//	else
//	{
//		if (m_curTags == NULL)
//			return;
//
//		m_LiveLesson->DeleteItem();
//		m_LiveLesson->setCourseID(m_curTags->CourseID(), true);
//		m_LiveLesson->move(width() / 2 - m_LiveLesson->width() / 2, height() / 2 - m_LiveLesson->height() / 2);
//		m_LiveLesson->show();
//	}
//}

void UIWindowSet::selectColor(QColor& color)
{
	if (mWhiteBoard)
	{
		mWhiteBoard->setPenColor(color);
	}
}

void UIWindowSet::returnClick()
{
	if (mWhiteBoard)
		mWhiteBoard->revocation();
}

void UIWindowSet::deleteClick()
{
	if (mWhiteBoard)
		mWhiteBoard->cleanUp();
}

void UIWindowSet::laserClick()
{
	if (mWhiteBoard)
		mWhiteBoard->setIsDraw(false);
}

void UIWindowSet::drawClick()
{
	if (mWhiteBoard)
		mWhiteBoard->setIsDraw(true);
}

void UIWindowSet::rtsDataReceived(const std::string& data)
{
	if (mWhiteBoard)
		mWhiteBoard->RecData(data);
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

// 接收消息回调
void CallbackReceiveMsg(const nim::IMMessage& msg)
{
	nim::IMMessage* pMsg = new nim::IMMessage;
	pMsg->session_type_ = msg.session_type_;
	pMsg->receiver_accid_ = msg.receiver_accid_;
	pMsg->sender_accid_ = msg.sender_accid_;
	pMsg->readonly_sender_client_type_ = msg.readonly_sender_client_type_;
	pMsg->readonly_sender_device_id_ = msg.readonly_sender_device_id_;
	pMsg->readonly_sender_nickname_ = msg.readonly_sender_nickname_;
	pMsg->timetag_ = msg.timetag_;

	pMsg->type_ = msg.type_;
	pMsg->content_ = msg.content_;
	pMsg->attach_ = msg.attach_;
	pMsg->client_msg_id_ = msg.client_msg_id_;
	pMsg->readonly_server_id_ = msg.readonly_server_id_;

	pMsg->local_res_path_ = msg.local_res_path_;
	pMsg->local_talk_id_ = msg.local_talk_id_;
	pMsg->local_res_id_ = msg.local_res_id_;
	pMsg->status_ = msg.status_;
	pMsg->sub_status_ = msg.sub_status_;

	PostMessage(m_This->GetParentWnd(), MSG_CLIENT_RECEIVE, (WPARAM)pMsg, 0);
}

// 发送消息回调
void CallbackSendMsgArc(const nim::SendMessageArc& arc)
{
	nim::SendMessageArc* arcNew = new nim::SendMessageArc;
	arcNew->msg_id_ = arc.msg_id_;
	arcNew->msg_timetag_ = arc.msg_timetag_;
	arcNew->rescode_ = arc.rescode_;
	arcNew->talk_id_ = arc.talk_id_;

	PostMessage(m_This->GetParentWnd(), MSG_SEND_MSG_STATUS, (WPARAM)arcNew, 0);
}

HWND UIWindowSet::GetParentWnd()
{
	return (HWND)m_parent->winId();
}

/***************************************************************************/
/*																		   */
/*					  初始化云信、白板、及回调							   */
/*																		   */
/***************************************************************************/
void UIWindowSet::initCallBack()
{
// 	IMInterface::getInstance()->initVChat();
// 	IMInterface::getInstance()->initVChatCallback();
// 	IMInterface::getInstance()->initRtsCallback();
// 	// 初始化白板
// 	initWhiteBoardWidget();
// 	initConnection();
// 
// 	IMInterface::getInstance()->EnumDeviceDevpath(Audio);
// 	IMInterface::getInstance()->EnumDeviceDevpath(Video);
// 	IMInterface::getInstance()->EnumDeviceDevpath(AudioOut);

	// 接受消息回调
	nim::Talk::RegReceiveCb(&CallbackReceiveMsg);
	// 发送消息状态回调
	nim::Talk::RegSendMsgCb(&CallbackSendMsgArc);
}

// 查询历史记录回调
void UIWindowSet::QueryMsgOnlineCb(nim::NIMResCode code, const std::string& id, nim::NIMSessionType type, const nim::QueryMsglogResult& result)
{
	nim::QueryMsglogResult* pRes = new nim::QueryMsglogResult;
	pRes->msglogs_ = result.msglogs_;
	pRes->count_ = result.count_;

	PostMessage(m_This->GetParentWnd(), MSG_CLIENT_RECORD, (WPARAM)pRes, 0);
}

// 第一次查询历史记录回调
void UIWindowSet::QueryFirstMsgOnlineCb(nim::NIMResCode code, const std::string& id, nim::NIMSessionType type, const nim::QueryMsglogResult& result)
{
	nim::QueryMsglogResult* pRes = new nim::QueryMsglogResult;
	pRes->msglogs_ = result.msglogs_;
	pRes->count_ = result.count_;

	PostMessage(m_This->GetParentWnd(), MSG_CLIENT_RECORD, (WPARAM)pRes, 1);
}

// 获取成员回调
void UIWindowSet::OnGetTeamMemberCallback(const std::string& tid, int count, const std::list<nim::TeamMemberProperty>& team_member_info_list)
{
	std::list<nim::TeamMemberProperty> *pTeamList = new std::list<nim::TeamMemberProperty>;

	for (const auto& member : team_member_info_list)
		pTeamList->push_back(member);

	PostMessage(m_This->GetParentWnd(), MSG_MEMBERS_INFO, (WPARAM)pTeamList, 0);
}

// 打开直播课
void UIWindowSet::OpenCourse(QString chatID, QString courseid, QString teacherid, QString token, QString studentName, std::string strCurAudioPath,
	QString courseName, int UnreadCount, QString status, QString boardurl, QString cameraUrl, bool b1v1Lesson)
{
	UIChatRoom* chatRoom = IsHasRoom(chatID);
	if (chatRoom == NULL)
	{
		chatRoom = new UIChatRoom(ui.chat_widget);
		chatRoom->setWindowFlags(Qt::FramelessWindowHint);
		chatRoom->setMainWindow(this);
		chatRoom->SetEnvironmental(m_EnvironmentalTyle);
		chatRoom->setCurChatID(chatID, courseid, teacherid, token, studentName, m_accid, UnreadCount);
		chatRoom->SetCurAudioPath(strCurAudioPath);
		ui.horizontalLayout_6->addWidget(chatRoom);
		m_vecChatRoom.push_back(chatRoom);
		m_mapChatRoom.insert(chatID, chatRoom);
		chatRoom->show();
	}

	AddTag(chatID, courseName, courseid, true, chatRoom, status, b1v1Lesson, boardurl, cameraUrl);
}

// 打开互动直播
void UIWindowSet::OpenCourse1v1(QString chatID, QString courseid, QString teacherid, QString token, QString studentName, std::string strCurAudioPath,
	QString courseName, int UnreadCount, QString status, QString boardurl, QString cameraUrl, bool b1v1Lesson)
{
	UIChatRoom* chatRoom = IsHasRoom(chatID);
	if (chatRoom == NULL)
	{
		chatRoom = new UIChatRoom(ui.chat1v1_widget);
		chatRoom->setWindowFlags(Qt::FramelessWindowHint);
		chatRoom->setMainWindow(this);
		chatRoom->SetEnvironmental(m_EnvironmentalTyle);
		chatRoom->setCurChatID(chatID, courseid, teacherid, token, studentName, m_accid, UnreadCount, true);
		chatRoom->SetCurAudioPath(strCurAudioPath);
		ui.horizontalLayout_18->addWidget(chatRoom);
		m_vecChatRoom.push_back(chatRoom);
		m_mapChatRoom.insert(chatID, chatRoom);
		chatRoom->show();
	}

	AddTag(chatID, courseName, courseid, true, chatRoom, status, b1v1Lesson, boardurl, cameraUrl);
}

void UIWindowSet::InitSetParamWnds()
{
	m_AudioChangeInfo1v1 = new UIAudioChange1v1(this);
	m_AudioChangeInfo1v1->setWindowFlags(Qt::FramelessWindowHint);
	m_AudioChangeInfo1v1->setAudioChange(this);
	m_AudioChangeInfo1v1->hide();

	m_AudioOutChangeInfo1v1 = new UIAudioOutChange1v1(this);
	m_AudioOutChangeInfo1v1->setWindowFlags(Qt::FramelessWindowHint);
	m_AudioOutChangeInfo1v1->setAudioChange(this);
	m_AudioOutChangeInfo1v1->hide();

	m_VideoChangeInfo1v1 = new UIVideoChange1v1(this);
	m_VideoChangeInfo1v1->setWindowFlags(Qt::FramelessWindowHint);
	m_VideoChangeInfo1v1->setVideoChange(this);
	m_VideoChangeInfo1v1->hide();

	// 设置麦克风样式
	ui.Audio1v1_checkBox->setStyleSheet("QCheckBox::indicator{width: 24px;height: 19px;}"
		"QCheckBox::indicator:unchecked{image: url(./images/mic_open.png);}"
		"QCheckBox::indicator:checked{image: url(./images/mic_close.png);}");
	ui.Audio1v1_pushButton->setStyleSheet("QPushButton{border-image:url(./images/Arrow.png);}");

	// 设置摄像头视频源样式
	ui.video1v1_checkBox->setStyleSheet("QCheckBox::indicator{width: 29px;height: 19px;}"
		"QCheckBox::indicator:unchecked{image: url(./images/camera_open.png);}"
		"QCheckBox::indicator:checked{image: url(./images/camera_close.png);}");
	ui.video1v1_pushButton->setStyleSheet("QPushButton{border-image:url(./images/Arrow.png);}");

	// 设置弹幕样式
	ui.Bullet1v1_checkBox->setStyleSheet("QCheckBox::indicator{width: 26px;height: 19px;}"
		"QCheckBox::indicator:unchecked{image: url(./images/bullet_close.png);}"
		"QCheckBox::indicator:checked{image: url(./images/bullet_open.png);}");
	ui.Bullet1v1_pushButton->setStyleSheet("QPushButton{border-image:url(./images/Arrow.png);}");

	// 设置设置扬声器样式
	ui.AudioOut_checkBox->setStyleSheet("QCheckBox::indicator{width: 29px;height: 19px;}"
		"QCheckBox::indicator:unchecked{image: url(./images/audioout_open.png);}"
		"QCheckBox::indicator:checked{image: url(./images/audioout_close.png);}");
	ui.AudioOut_pushButton->setStyleSheet("QPushButton{border-image:url(./images/Arrow.png);}");

	ui.shapeScreen_pushButton->setStyleSheet("QPushButton{border-image:url(./images/shapeScreen.png);}");
	ui.doc_pushButton->setStyleSheet("QPushButton{border-image:url(./images/docBtn.png);}");

	connect(ui.Audio1v1_checkBox, SIGNAL(stateChanged(int)), this, SLOT(Audio1v1Status(int)));
	connect(ui.Audio1v1_pushButton, SIGNAL(clicked()), this, SLOT(clickAudio1v1Param()));
	connect(ui.video1v1_checkBox, SIGNAL(stateChanged(int)), this, SLOT(Video1v1Status(int)));
	connect(ui.video1v1_pushButton, SIGNAL(clicked()), this, SLOT(clickVideo1v1Param()));
	connect(ui.Bullet1v1_checkBox, SIGNAL(stateChanged(int)), this, SLOT(BulletStatus(int)));
	connect(ui.Bullet1v1_pushButton, SIGNAL(clicked()), this, SLOT(clickBulletParam()));
	connect(ui.AudioOut_checkBox, SIGNAL(stateChanged(int)), this, SLOT(AudioOut1v1Status(int)));
	connect(ui.AudioOut_pushButton, SIGNAL(clicked()), this, SLOT(clickAudioOut1v1Param()));
	connect(ui.Live1v1_pushBtn, SIGNAL(clicked()), this, SLOT(clickLive1v1()));
	connect(ui.time1v1_label, SIGNAL(clicked()), this, SLOT(clickLive1v1()));
	connect(ui.shapeScreen_pushButton, SIGNAL(clicked()), this, SLOT(clickShapeScreen1v1()));
}

//初始化白板
void UIWindowSet::initWhiteBoardWidget()
{
	mWhiteBoard = new Palette();
	ui.horizontalLayout_21->addWidget(mWhiteBoard);
	mWhiteBoard->setIsDraw(true);
	connect(mWhiteBoard, SIGNAL(PicData(QString)), this, SLOT(PicData(QString)));

	m_WhiteBoardTool = new UIWhiteBoardTool(ui.live1v1_widget);
	m_WhiteBoardTool->hide();
	connect(m_WhiteBoardTool, SIGNAL(selectColor(QColor&)), this, SLOT(selectColor(QColor&)));
	connect(m_WhiteBoardTool, SIGNAL(returnClick()), this, SLOT(returnClick()));
	connect(m_WhiteBoardTool, SIGNAL(deleteClick()), this, SLOT(deleteClick()));
	connect(m_WhiteBoardTool, SIGNAL(laserClick()), this, SLOT(laserClick()));
	connect(m_WhiteBoardTool, SIGNAL(drawClick()), this, SLOT(drawClick()));

	// 老师自己的摄像头
	m_Camera1v1Info = new UICamera1v1(ui.cameraT_widget);
	m_Camera1v1Info->setWindowFlags(Qt::FramelessWindowHint);
	ui.horizontalLayout_22->addWidget(m_Camera1v1Info);
	m_Camera1v1Info->show();

	// 学生摄像头
	m_CameraS1v1Info = new UICameraS1v1(ui.cameraS_widget);
	m_CameraS1v1Info->setWindowFlags(Qt::FramelessWindowHint);
	ui.horizontalLayout_23->addWidget(m_CameraS1v1Info);
	m_CameraS1v1Info->show();

	// 1v1全屏桌面
	m_VideoInfo1v1 = new UIVideo1v1(ui.fullS1v1_widget);
	m_VideoInfo1v1->setWindowFlags(Qt::FramelessWindowHint);
	ui.horizontalLayout_21->addWidget(m_VideoInfo1v1);
	m_VideoInfo1v1->hide();
	connect(m_VideoInfo1v1, SIGNAL(sig_refreshWnd()), this, SLOT(slot_refreshWnd()));
	connect(m_VideoInfo1v1, SIGNAL(sig_CustomVideoData(__int64, const char*, int, int, int)), this, SLOT(slot_CustomVideoData(__int64, const char*, int, int, int)));

	m_AppWndTool1v1 = new UIAppWndTool(ui.live1v1_widget);
	m_AppWndTool1v1->hide();
	connect(m_AppWndTool1v1, SIGNAL(sig_shiftWnd()), this, SLOT(slot_shiftWnd()));
	connect(m_AppWndTool1v1, SIGNAL(sig_CloseWnd()), this, SLOT(slot_CloseWnd()));

	// 1v1选择窗口界面
	m_AppWnd1v1 = new UIAppWnd();
	m_AppWnd1v1->setWindowFlags(Qt::FramelessWindowHint);
	m_AppWnd1v1->hide();
	connect(m_AppWnd1v1, SIGNAL(sig_selectWnd(HWND)), this, SLOT(slot_selectWnd(HWND)));
}

void UIWindowSet::initConnection()
{
	IMInterface *instance = IMInterface::getInstance();
	if (NULL == instance)
	{
		return;
	}

	connect(instance, SIGNAL(createRtsRoomSuccessfully(const std::string&)), this, SLOT(joinRtsRoom(const std::string&)));
	connect(instance, SIGNAL(joinRtsRoomSuccessfully(const std::string&, __int64, const std::string&)),
		this, SLOT(joinRoomSuccessfully(const std::string&, __int64, const std::string&)));
	connect(instance, SIGNAL(createVChatRoomSuccessfully()), this, SLOT(joinVChatRoom()));
	connect(instance, SIGNAL(joinVChatSuccessfully()), this, SLOT(joinVChatSuccessfully()));
	connect(instance, SIGNAL(hasError(const QString &)), this, SLOT(errorInfo(const QString &)));
	connect(instance, SIGNAL(deviceInfos(int)), this, SLOT(setDeviceInfos(int)));
	connect(instance, SIGNAL(startDeviceSuccessfully(int)), this, SLOT(startDeviceSuccessfully(int)));
	connect(instance, SIGNAL(VideoCapture(const char*, unsigned int, unsigned int, unsigned int)), m_Camera1v1Info, SLOT(VideoCapture(const char*, unsigned int, unsigned int, unsigned int)));
	connect(instance, SIGNAL(RecVideoCapture(const char*, unsigned int, unsigned int, unsigned int)), m_CameraS1v1Info, SLOT(VideoCapture(const char*, unsigned int, unsigned int, unsigned int)));

	connect(instance, SIGNAL(rtsDataReceived(const std::string&)), this, SLOT(rtsDataReceived(const std::string&)));
}

void UIWindowSet::setAudioChange1v1(QString path)
{
	if (!ui.Audio1v1_checkBox->isChecked())
	{
		if (!path.isNull())
		{
			IMInterface::getInstance()->endDevice(Audio);
			IMInterface::getInstance()->startDevice(Audio, path.toStdString(), 0, 0, 0);
		}
	}
}

void UIWindowSet::setValueChange1v1(int iVolumn, bool capturn)
{
	IMInterface::getInstance()->setAudioChange(iVolumn, capturn);
}

void UIWindowSet::setVideoChange1v1(QString path)
{
	if (!ui.video1v1_checkBox->isChecked())
	{
		if (!path.isNull())
		{
			IMInterface::getInstance()->endDevice(Video);
			IMInterface::getInstance()->startDevice(Video, path.toStdString(), VIDEO_FPS, 0, 0);
		}
	}
}

void UIWindowSet::show1v1ErrorTip(QString sError)
{
	m_bLiving1v1 = false;
	CMessageBox::showMessage(
		QString("答疑时间"),
		QString(sError),
		QString("确定"));
}