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

#include "HttpRequest.h"
#include "course.h"

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
#include "1v1/ui1v1.h"

#include "IMInterface.h"

#define MAINWINDOW_X_MARGIN 10
#define MAINWINDOW_Y_MARGIN 10
#define MAINWINDOW_TITLE_HEIGHT 0
#define LIVE_BUTTON_NAME	"选课直播"
#define LESSON_LABEL		"暂无直播"
#define STATUS_TIME			10000

extern bool g_environmentType;	// 环境类型		true为生产环境		false为测试环境  默认为true
extern QString g_remeberToken;
extern QString g_homePage;
int m_PublicCameraStatus;	// 1v1直播时，摄像头状态（1打开，0关闭），用于判断教师是否进行了白板和桌面的切换

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
	, m_Ui1v1(NULL)
	, m_timer(NULL)
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

	font = ui.lesson_label->font();
	font.setPixelSize(15);
	ui.lesson_label->setFont(font);

	ui.lesson_widget->setVisible(false);
	ui.camera_widget->setVisible(false);
	ui.whiteboard_widget->setVisible(false);
	ui.label_space->setVisible(false);
	ui.line_label->setVisible(false);
	ui.return_pushButton->setVisible(false);
	ui.live1v1_widget->setVisible(false);

	m_LiveTimer = new QTimer(this);
	connect(m_LiveTimer, SIGNAL(timeout()), this, SLOT(slot_onTimeout()));

	ChangeBtnStyle(false);

	init1v1();

	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(status1v1()));
	m_timer->start(3000);

	ui.notice_pushButton->setStyleSheet("border-image: url(./images/notice_nor.png);");
	ui.lesson_pushButton->setStyleSheet("border-image: url(./images/lessonBtn_nor.png);");
	ui.course_pushButton->setStyleSheet("border-image: url(./images/courseBtn_nor.png);");
	ui.person_pushButton->setStyleSheet("border-image: url(./images/personBtn_nor.png);");
	ui.pic_label->setStyleSheet("border-image: url(./images/online.png);");


	m_PublicCameraStatus = 1; // 默认1v1直播时，教师端摄像头为开启状态
	connect(m_Ui1v1, SIGNAL(m_Ui1v1->ui.btn_GetFrameState->clicked()), this, SLOT(slot_onTimeout()));
}

UIWindowSet::~UIWindowSet()
{
	if (m_timer)
	{
		m_timer->stop();
		delete m_timer;
		m_timer = NULL;
	}
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

	if (m_curTags->Is1v1Lesson())
	{
		m_Ui1v1->setMuteBoard(true);
		// 结束设备
		m_Ui1v1->ModleChange(false);
	}

	m_vecTags.clear();
	m_mapTags.clear();
	m_curTags = NULL;
	m_curChatRoom = NULL;
	PlayLive("", "");
	// 课程名置空
	ui.lesson_label->setText(LESSON_LABEL);
	hide();

}

void UIWindowSet::MaxDialog()
{
	if (this->isMaximized())
	{
		ui.max_pushButton->setStyleSheet("border-image: url(./images/login_max.png);");
		showNormal();
		return;
	}
	ui.max_pushButton->setStyleSheet("border-image: url(./images/login_max1.png);");
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
		if (this->isMaximized())//最大化不让拖动
			return false;
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
		if (this->isMaximized())//最大化不让拖动
			return false;
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
				if (g_environmentType)
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
		else if (event->type() == QEvent::MouseButtonPress)
		{
			ui.whiteboard_widget->setFocus();
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

UITags* UIWindowSet::AddTag(QString chatID, QString name, QString ID, bool sel, UIChatRoom* room, QString status, bool b1v1Lesson)
{
	UITags* tag = new UITags(ui.tags_widget);
	tag->setMaximumWidth(200);
	tag->SetCourseNameAndID(name,ID, chatID);
	tag->SetMainWindow(this);
	tag->SetRoom(room);
	tag->setStyle(sel);
	tag->set1v1Lesson(b1v1Lesson);
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

	// 隐藏与显示互动直播
	ui.live_widget->setVisible(!b1v1Lesson);
	ui.live1v1_widget->setVisible(b1v1Lesson);

	if (status == "teaching")
	{
		m_curTags->setModle(true);
		emit sig_Modle(true);

		// 开启轮询
		if (m_curTags)
			m_curTags->setLiving(false);

		start1v1Status(STATUS_TIME);
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

				if (tags->Is1v1Lesson())
				{
					m_Ui1v1->setMuteBoard(true);
				}

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

void UIWindowSet::AddChatRoom(QString chatID, QString courseid, QString teacherid, QString studentName, std::string strCurAudioPath, QString name, int UnreadCount, QString status, bool b1v1Lesson)
{
	show();
	if (isMinimized())
		showNormal();
	SetWindowPos((HWND)winId(), HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	m_course_id1v1 = courseid;

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

	if (b1v1Lesson)
		OpenCourse1v1(chatID, courseid, teacherid, studentName, strCurAudioPath, name, UnreadCount, status, b1v1Lesson);
	else
		OpenCourse(chatID, courseid, teacherid,studentName, strCurAudioPath, name, UnreadCount, status, b1v1Lesson);
}

bool UIWindowSet::ReceiverMsg(nim::IMMessage* pIMsg)
{
	QString strChatID = QString::fromStdString(pIMsg->local_talk_id_.c_str());
	QMap<QString, UIChatRoom*>::Iterator it;
	UIChatRoom* room=NULL;
	room = m_mapChatRoom.value(strChatID,NULL);
	if (room)
	{
		room->ReceiverMsg(pIMsg);
	}

	UITags* tag = NULL;
	tag = m_mapTags.value(strChatID,NULL);
	if (tag)
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

	if (m_NoticeWnd){
		ui.notice_pushButton->setStyleSheet("border-image: url(./images/notice_nor.png);");//background-color: rgb(255, 255, 255);
		m_NoticeWnd->hide();
	}

	if (m_CourseWnd){
		ui.course_pushButton->setStyleSheet("border-image: url(./images/courseBtn_nor.png);");
		m_CourseWnd->hide();
	}

	if (m_PersonWnd){
		ui.person_pushButton->setStyleSheet("border-image: url(./images/personBtn_nor.png);");
		m_PersonWnd->hide();
	}

	if (m_LessonWnd){
		ui.lesson_pushButton->setStyleSheet("border-image: url(./images/lessonBtn_nor.png);");
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

	start1v1Status(STATUS_TIME);
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
		// 课程名置空
		ui.lesson_label->setText(LESSON_LABEL);
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

void UIWindowSet::clickNotice()
{
	if (m_NoticeWnd)
	{
		if (m_NoticeWnd->isVisible())
		{
			ui.notice_pushButton->setStyleSheet("color:rgb(0,0,0);border-image: url(./images/notice_nor.png);");
			m_NoticeWnd->setVisible(false);
			return;
		}

		m_NoticeWnd->initDate();
		ui.notice_pushButton->setStyleSheet("color:rgb(255, 255, 255);border-image: url(./images/notice_hor.png);");
		m_NoticeWnd->move(0, 69);
		m_NoticeWnd->DeleteNotice();
		m_NoticeWnd->show();
		m_NoticeWnd->setFocus();
		m_NoticeWnd->setFixedHeight(500);
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
	if (m_curTags->Is1v1Lesson())
	{
		strUrl += g_homePage;
		strUrl += "/api/v1/live_studio/interactive_courses/{id}/realtime";
		strUrl.replace("{id}", strCourseID);
	}
	else
	{
		strUrl += g_homePage;
		strUrl += "/api/v1/live_studio/courses/{id}/realtime";
		strUrl.replace("{id}", strCourseID);
	}

	QUrl url = QUrl(strUrl);
	QNetworkRequest request(url);

	request.setRawHeader("Remember-Token", g_remeberToken.toUtf8());
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

			// 只取一条
			return;
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
			ui.course_pushButton->setStyleSheet("border-image: url(./images/courseBtn_nor.png);");
			m_CourseWnd->setVisible(false);
			return;
		}

		ui.course_pushButton->setStyleSheet("border-image: url(./images/courseBtn_hor.png);");
		m_CourseWnd->move(0, 69);
		m_CourseWnd->show();
		m_CourseWnd->setFocus();
		m_CourseWnd->setFixedHeight(500);
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
	strUrl += g_homePage;
	if (m_curTags->Is1v1Lesson())
	{
		strUrl += "/api/v1/live_studio/interactive_courses/{id}";
		strUrl.replace("{id}", strCourseID);
	}
	else
	{
		strUrl += "/api/v1/live_studio/students/{studentid}/courses/{id}";
		strUrl.replace("{id}", strCourseID);
		strUrl.replace("{studentid}", m_studentID);
	}

	QUrl url = QUrl(strUrl);
	QNetworkRequest request(url);

	request.setRawHeader("Remember-Token", g_remeberToken.toUtf8());
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
		QString coursePic;
		if (m_curTags->Is1v1Lesson())
		{
			coursePic = data["publicize_list_url"].toString();
		}
		else
		{
			coursePic = data["publicize"].toString();
		}

		QString courseName = data["name"].toString();
		QString courseGrade = data["grade"].toString();
		QString courseGrade1 = data["subject"].toString();
		QString courseProsses = QString::number(data["lessons_count"].toInt());
		QString coursePross = QString::number(data["closed_lessons_count"].toInt());
		QString courseStart = data["live_start_time"].toString();
		QString courseEnd = data["live_end_time"].toString();
		QString courseDesc = data["description"].toString();
	
		QString teacherName;
		if (m_curTags->Is1v1Lesson())
		{
			QJsonArray lesson1v1 = data["interactive_lessons"].toArray();
			foreach(const QJsonValue & value, lesson1v1)
			{
				QJsonObject teacherObj = value.toObject()["teacher"].toObject();
				teacherName = teacherObj["name"].toString();
				break;
			}	
		}
		else
			teacherName = data["teacher_name"].toString();

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
			ui.person_pushButton->setStyleSheet("color:rgb(0,0,0);border-image: url(./images/personBtn_nor.png);");
			m_PersonWnd->setVisible(false);
			return;
		}

		ui.person_pushButton->setStyleSheet("color:rgb(255, 255, 255);border-image: url(./images/personBtn_hor.png);");
		m_PersonWnd->move(300, 69);
		m_PersonWnd->DeletePerson();
		m_PersonWnd->show();
		m_PersonWnd->setFocus();
		m_PersonWnd->setFixedHeight(500);
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

		QString strID;
		if (m_curTags->Is1v1Lesson())
			strID = m_curChatRoom->GetAccid();
		else
			strID = m_curChatRoom->GetTeacherID();

		vecBuddy = m_curChatRoom->GetBuddy();
		if (m_PersonWnd)
			m_PersonWnd->AddPerson(vecBuddy, strID, m_curTags->Is1v1Lesson());
	}
}

void UIWindowSet::clickLesson()
{
	if (m_LessonWnd)
	{
		if (m_LessonWnd->isVisible())
		{
			ui.lesson_pushButton->setStyleSheet("border-image: url(./images/lessonBtn_nor.png);");
			m_LessonWnd->setVisible(false);
			return;
		}

		ui.lesson_pushButton->setStyleSheet("border-image: url(./images/lessonBtn_hor.png);");
		m_LessonWnd->move(0, 69);
		m_LessonWnd->DeleteLesson();
		m_LessonWnd->show();
		m_LessonWnd->setFocus();
		m_LessonWnd->setFixedHeight(500);
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
	strUrl += g_homePage;
	if (m_curTags->Is1v1Lesson())
	{
		strUrl += "/api/v1/live_studio/interactive_courses/{id}";
		strUrl.replace("{id}", strCourseID);
	}
	else
	{
		strUrl += "/api/v1/live_studio/students/{studentid}/courses/{id}";
		strUrl.replace("{id}", strCourseID);
		strUrl.replace("{studentid}", m_studentID);
	}


	QUrl url = QUrl(strUrl);
	QNetworkRequest request(url);

	request.setRawHeader("Remember-Token", g_remeberToken.toUtf8());
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
		QJsonArray lesson;
		if (m_curTags->Is1v1Lesson())
		{
			lesson = data["interactive_lessons"].toArray();
			foreach(const QJsonValue & value, lesson)
			{
				iCount++;
				QJsonObject obj = value.toObject();
				QString strClassDate = obj["class_date"].toString();
				QString strStartTime = obj["start_time"].toString();
				QString strEndTime = obj["end_time"].toString();
				QString strStatus = obj["status"].toString();
				QString strName = obj["name"].toString();

				strClassDate = strClassDate + "  " + strStartTime + "-" + strEndTime;
				if (m_LessonWnd)
					m_LessonWnd->AddLesson(QString().sprintf("%02d", iCount), strClassDate, strName, strStatus);
			}
		}
		else
		{
			lesson = data["lessons"].toArray();
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

		if (iCount != 0 && (iCount*40 < 500))
			m_LessonWnd->setFixedHeight(iCount * 40+5);
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
	if (m_curTags->Is1v1Lesson())
	{
		EndOpendTag1v1();

		ui.live_widget->setVisible(false);
		ui.live1v1_widget->setVisible(true);

		m_Ui1v1->ModleChange(!m_curTags->IsModle());

		if (m_curTags)
			m_curTags->setModle(!m_curTags->IsModle());

		if (m_curTags->IsModle())
		{
			if (m_curTags)
				m_curTags->setLiving(false);
			start1v1Status(STATUS_TIME);//轮询直播状态
		}
		else
		{
			m_Ui1v1->setMuteBoard(true);
			stop1v1Status();
		}
	}
	else
	{
		ui.live_widget->setVisible(true);
		ui.live1v1_widget->setVisible(false);

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

			if (m_curTags)
				m_curTags->setLiving(false);
			start1v1Status(STATUS_TIME);//轮询直播状态
		}
	}

	ui.lesson_widget->setVisible(m_curTags->IsModle());
	ChangeBtnStyle(m_curTags->IsModle());
}

void UIWindowSet::InitBoardView()
{
	if (!m_bInitLive)
	{
		// 启动直播视频
		TCHAR szTempPath[MAX_PATH] = { 0 }; 
		GetCurrentDirectory(MAX_PATH, szTempPath);
		lstrcat(szTempPath, L"\\CMSVideo.exe");
//		lstrcat(szTempPath, L"E:\\qatimeStudent\\qatimeStudent\\qatimeStudent\\Bin\\Release\\CMSVideo.exe");
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
	strUrl += g_homePage;
	strUrl += "/api/v1/live_studio/courses/{id}/play_info";
	strUrl.replace("{id}", strCourseID);

	QUrl url = QUrl(strUrl);
	QNetworkRequest request(url);

	request.setRawHeader("Remember-Token", g_remeberToken.toUtf8());
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
	if (g_environmentType)
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
	if (g_environmentType)
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

void UIWindowSet::init1v1()
{
	if (!m_Ui1v1)
	{
		delete m_Ui1v1;
	}
	m_Ui1v1 = new UI1v1(this);
	
	ui.horizontalLayout_8->addWidget(m_Ui1v1);
	connect(m_Ui1v1, SIGNAL(teacherStatus(bool)), this, SLOT(teacherStatus(bool)));
	connect(m_Ui1v1, SIGNAL(exitVChat()), this, SLOT(exitVChat()));
	connect(m_Ui1v1, SIGNAL(sig_joinRoomFail()), this, SLOT(slot_joinRoomFail()));
}

void UIWindowSet::teacherStatus(bool bEnd)
{
	
	if (m_curTags->IsLiving() == false)
		return;
	// 修改为如果bEnd = false，让学生重新连入房间 modify by zbc 20170823
	if (!bEnd)
	{
		m_Ui1v1->setMuteBoard(true);

		if (m_curTags)
			m_curTags->setLiving(false);
		start1v1Status(STATUS_TIME);
	}
}

void UIWindowSet::exitVChat()
{
	clickChange(true);
}

void UIWindowSet::slots_Modle(bool bModle)
{
	if (!bModle)
	{
		if (m_curTags && m_curTags->Is1v1Lesson())
		{
			ui.live_widget->setVisible(false);
			ui.live1v1_widget->setVisible(true);
			m_Ui1v1->ModleChange(false);
		}
		else
		{
			ui.live_widget->setVisible(true);
			ui.live1v1_widget->setVisible(false);

			ui.line_label->setVisible(false);
			ui.camera_widget->setVisible(false);
			ui.whiteboard_widget->setVisible(false);
			ui.label_space->setVisible(false);
			ui.chatcamera_widget->setMaximumWidth(3000);
			PlayLive("", "");
		}
	}
	else
	{
		if (m_curTags && m_curTags->Is1v1Lesson())
		{
			EndOpendTag1v1();
			ui.live_widget->setVisible(false);
			ui.live1v1_widget->setVisible(true);

			m_Ui1v1->ModleChange(true);
		}
		else
		{
			ui.live_widget->setVisible(true);
			ui.live1v1_widget->setVisible(false);

			ui.line_label->setVisible(true);
			ui.camera_widget->setVisible(true);
			ui.whiteboard_widget->setVisible(true);
			ui.label_space->setVisible(true);
			ui.chatcamera_widget->setMaximumWidth(300);

			InitBoardView();
			QueryLiveInfo();
		}
	}

	ui.lesson_widget->setVisible(bModle);
	ChangeBtnStyle(bModle);
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

void UIWindowSet::status1v1()
{	
	// 如果当前页面不是在直播室模式，则不查询
	if (m_curTags == NULL || m_curChatRoom==NULL || !m_curTags->IsModle())
		return;

	// 设置当前界面辅导班ID
	m_course_id1v1 = m_curChatRoom->GetCourseID();

	if (m_curTags->Is1v1Lesson()) // 查询1对1辅导班
	{
		QString strUrl;
		strUrl += g_homePage;
		strUrl += "/api/v1/live_studio/interactive_courses/{interactive_course_id}/live_status";
		strUrl.replace("{interactive_course_id}", m_course_id1v1);

		HttpRequest httpRequest;
		httpRequest.setRawHeader("Remember-Token", g_remeberToken.toUtf8());

		QByteArray byteArray = httpRequest.httpGet(strUrl);

		if (m_curChatRoom && m_course_id1v1 != m_curChatRoom->GetCourseID())
			return;

		QJsonDocument document(QJsonDocument::fromJson(byteArray));
		QJsonObject obj = document.object();

		ONETOONE_STATUS otoStatus = Course::getOneToOneStatusFromJson(obj); 

		if (m_Ui1v1 && !otoStatus.data.room_id.isEmpty() && otoStatus.data.status=="teaching")
		{
			ui.lesson_label->setText(otoStatus.data.name);
			ui.pic_label->setVisible(false);
			ui.online_label->setVisible(false);
//			ui.online_label->setText(QString::number(otoStatus.data.online_users.size()));

			if (m_curTags && !m_curTags->IsLiving())
			{
//				stop1v1Status();
				m_curTags->setLiving(true);
				m_Ui1v1->joinRtsRoom(otoStatus.data.room_id.toStdString());
			}
		}
		else
		{
			ui.lesson_label->setText(LESSON_LABEL);
			ui.pic_label->setVisible(false);
			ui.online_label->setVisible(false);
		}
		qDebug() << __FILE__ << __LINE__ << "1对1课程状态：" << otoStatus.data.status;
	}
	else  // 查询1对多直播课辅导班
	{
		QString strUrl;
		strUrl += g_homePage;
		strUrl += "/api/v1/live_studio/courses/{course_id}/status";
		strUrl.replace("{course_id}", m_course_id1v1);

		HttpRequest httpRequest;
		httpRequest.setRawHeader("Remember-Token", g_remeberToken.toUtf8());

		QByteArray byteArray = httpRequest.httpGet(strUrl);

		if (m_curChatRoom && m_course_id1v1 != m_curChatRoom->GetCourseID())
			return;

		QJsonDocument document(QJsonDocument::fromJson(byteArray));
		QJsonObject obj = document.object();

		ONETOONE_STATUS otoStatus = Course::getOneToOneStatusFromJson(obj);

		if (m_Ui1v1 && otoStatus.data.status == "teaching")
		{
			ui.lesson_label->setText(otoStatus.data.name);
			ui.pic_label->setVisible(true);
			ui.online_label->setVisible(true);
			ui.online_label->setText(QString::number(otoStatus.data.online_users.size()));
			
		}
		else
		{
			ui.lesson_label->setText(LESSON_LABEL);
			ui.pic_label->setVisible(false);
			ui.online_label->setVisible(false);
		}
		
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
	// 接受消息回调
	nim::Talk::RegReceiveCb(&CallbackReceiveMsg);
	// 发送消息状态回调
	nim::Talk::RegSendMsgCb(&CallbackSendMsgArc);

	// 语音录制回调
	InitAudioCallBack();

	m_Ui1v1->initDevice();
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
void UIWindowSet::OpenCourse(QString chatID, QString courseid, QString teacherid, QString studentName, std::string strCurAudioPath,
	QString courseName, int UnreadCount, QString status, bool b1v1Lesson)
{
	UIChatRoom* chatRoom = IsHasRoom(chatID);
	if (chatRoom == NULL)
	{
		chatRoom = new UIChatRoom(ui.chat_widget);
		chatRoom->setWindowFlags(Qt::FramelessWindowHint);
		chatRoom->setMainWindow(this);
		chatRoom->setCurChatID(chatID, courseid, teacherid, studentName, m_accid, UnreadCount);
		chatRoom->SetCurAudioPath(strCurAudioPath);
		ui.horizontalLayout_6->addWidget(chatRoom);
		m_vecChatRoom.push_back(chatRoom);
		m_mapChatRoom.insert(chatID, chatRoom);
		chatRoom->show();
	}

	AddTag(chatID, courseName, courseid, true, chatRoom, status, b1v1Lesson);
}

// 打开互动直播
void UIWindowSet::OpenCourse1v1(QString chatID, QString courseid, QString teacherid, QString studentName, std::string strCurAudioPath,
	QString courseName, int UnreadCount, QString status, bool b1v1Lesson)
{
	UIChatRoom* chatRoom = IsHasRoom(chatID);
	if (chatRoom == NULL)
	{
		chatRoom = new UIChatRoom();
		chatRoom->setWindowFlags(Qt::FramelessWindowHint);
		chatRoom->setMainWindow(this);
		chatRoom->setCurChatID(chatID, courseid, teacherid, studentName, m_accid, UnreadCount, true);
		chatRoom->SetCurAudioPath(strCurAudioPath);
		m_Ui1v1->chat1v1Widget()->addWidget(chatRoom);
		m_vecChatRoom.push_back(chatRoom);
		m_mapChatRoom.insert(chatID, chatRoom);
		chatRoom->show();
	}

	AddTag(chatID, courseName, courseid, true, chatRoom, status, b1v1Lesson);
}

void UIWindowSet::start1v1Status(int msec)
{
	m_timer->stop();
	status1v1();
	m_timer->start(msec);
}

void UIWindowSet::stop1v1Status()
{
	if (m_timer)
	{
		m_timer->stop();
	}
}

void UIWindowSet::shapeScreen(bool bType)
{
	m_Ui1v1->SetShapeScreen(bType);
}

void UIWindowSet::slot_joinRoomFail()
{
	// 加入失败，重新轮询
	if (m_curTags)
		m_curTags->setLiving(false);
	start1v1Status(STATUS_TIME);
}

// 关闭进入直播室的1对1辅导班
void UIWindowSet::EndOpendTag1v1()
{
	if (m_vecTags.size() > 0)
	{
		std::vector<UITags*>::iterator it;
		for (it = m_vecTags.begin(); it != m_vecTags.end(); it++)
		{
			UITags * tag = *it;
			if (m_curTags && m_curTags == tag)
				continue;
			
			if (tag->Is1v1Lesson() && tag->IsModle())
			{
				tag->setModle(false);

				//关闭白板
				m_Ui1v1->setMuteBoard(true);
			}
		}
	}
}

// 语音录制功能
void UIWindowSet::InitAudioCallBack()
{
	nim_audio::Audio::RegStartCaptureCb(&OnStartCaptureCallback);
	nim_audio::Audio::RegStopCaptureCb(&OnStopCaptureCallback);
}

void UIWindowSet::RecordingVoice(std::string chatID, std::string msgID)
{
	nim_audio::Audio::StartCapture(chatID.c_str(), msgID.c_str(), nim_audio::AMR);
}

void UIWindowSet::StopRecord()
{
	nim_audio::Audio::StopCapture();
}

void UIWindowSet::OnStartCaptureCallback(int code)
{
	if (code != 200)
	{
		QToolTip::showText(QCursor::pos(), "录制语音失败！");
	}
	else
	{
		if (m_This)
			m_This->StartCaptureAudio();
	}
}

void UIWindowSet::StartCaptureAudio()
{
	if (m_curChatRoom)
	{
		m_curChatRoom->m_AudioBar->CaptureAudio();
	}
}

void UIWindowSet::OnStopCaptureCallback(int rescode, const char* sid, const char* cid, const char* file_path, const char* file_ext, long file_size, int audio_duration)
{
	if (rescode == 200 && m_This)
	{
		MyAudioStruct* audio = new MyAudioStruct;
		audio->sSessionID = sid;
		audio->sMsgID = cid;
		audio->sFilePath = file_path;
		audio->sFileEx = file_ext;
		audio->fileSize = file_size;
		audio->duration = audio_duration;
		
		if (m_This->m_parent)
			PostMessage((HWND)m_This->m_parent->winId(), MSG_SEND_AUDIO_FINISH_MSG, (WPARAM)audio, 0);
	}
}




// 手动获取当前直播画面
void UIWindowSet::RefreshFrameState()
{
	// 如果当前页面不是在直播室模式，则不查询
	if (m_curTags == NULL || m_curChatRoom == NULL || !m_curTags->IsModle())
		return;
	// 设置当前界面辅导班ID
	m_course_id1v1 = m_curChatRoom->GetCourseID();
	QString strUrl;
	strUrl += g_homePage;
	strUrl += "/api/v1/live_studio/interactive_courses/{interactive_course_id}/live_status";
	strUrl.replace("{interactive_course_id}", m_course_id1v1);

	
	HttpRequest httpRequest;
	httpRequest.setRawHeader("Remember-Token", g_remeberToken.toUtf8());
	QByteArray byteArray = httpRequest.httpGet(strUrl);
	if (m_curChatRoom && m_course_id1v1 != m_curChatRoom->GetCourseID())
			return;

	QJsonDocument document(QJsonDocument::fromJson(byteArray));
	QJsonObject obj = document.object();
	ONETOONE_STATUS otoStatus = Course::getOneToOneStatusFromJson(obj);
	if (m_Ui1v1 && !otoStatus.data.room_id.isEmpty() && otoStatus.data.status == "teaching")
	{
		m_PublicCameraStatus = otoStatus.data.camera;
	}
	else
	{
		return;
	}
	if (m_PublicCameraStatus == 0)
	{
		IMInterface::getInstance()->setFullScreenStatus(true);
	}
	else
	if (m_PublicCameraStatus == 1)
	{
		IMInterface::getInstance()->setFullScreenStatus(false);
	}
	emit IMInterface::getInstance()->sig_SendFullScreen(IMInterface::getInstance()->IsFullScreen());
}