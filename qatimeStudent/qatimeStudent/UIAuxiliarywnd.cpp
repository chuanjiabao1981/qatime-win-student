#include "UIAuxiliarywnd.h"
#include <QPainter>
#include <QScrollBar>
#include <QBitmap>
#include "UIBtnPic.h"
#include <QDesktopWidget>

#define MAINWINDOW_X_MARGIN 4
#define MAINWINDOW_Y_MARGIN 4
#define MAINWINDOW_TITLE_HEIGHT 0

UIAuxiliaryWnd::UIAuxiliaryWnd(QWidget *parent)
	: QWidget(parent)
	, m_parent(NULL)
	, m_VerAll(NULL)
	, m_VerOneToOne(NULL)
	, m_VerToday(NULL)
	, m_mainAllView(NULL)
	, m_oneToOneView(NULL)
	, m_mainTodayView(NULL)
	, m_vecAuxiliaryList(NULL)
	, m_spacerAll(NULL)
	, m_spacerToday(NULL)
	, m_spacerOneToOne(NULL)
	, m_pWorker(NULL)
	, m_menu(NULL)
	, m_UIMenu(NULL)
{
	ui.setupUi(this);
	setAutoFillBackground(true);;
	QPalette p = palette();
	p.setColor(QPalette::Window, QColor(255, 255, 255));
	setPalette(p);
	setFocusPolicy(Qt::StrongFocus);
	setMouseTracking(true);

	// 设置全部辅导垂直布局
	m_mainAllView = new QWidget();
	ui.scrollArea->setWidget(m_mainAllView);
	m_VerAll = new QVBoxLayout(m_mainAllView);
	m_VerAll->setSpacing(0);
	m_VerAll->setContentsMargins(5,5, 0, 0);
	ui.scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	style(ui.scrollArea);

	// 设置今日课程垂直布局
	m_mainTodayView = new QWidget();
	ui.today_scrollArea->setWidget(m_mainTodayView);
	m_VerToday = new QVBoxLayout(m_mainTodayView);
	m_VerToday->setSpacing(0);
	m_VerToday->setContentsMargins(5, 5, 0, 0);
	ui.today_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	style(ui.today_scrollArea);

	// 设置一对一互动垂直布局
	m_oneToOneView = new QWidget();
	ui.oneToOne_scrollArea->setWidget(m_oneToOneView);
	m_VerOneToOne = new QVBoxLayout(m_oneToOneView);
	m_VerOneToOne->setSpacing(0);
	m_VerOneToOne->setContentsMargins(5, 5, 0, 0);
	ui.oneToOne_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	style(ui.oneToOne_scrollArea);

	ui.scrollArea->installEventFilter(this);
	ui.today_scrollArea->installEventFilter(this);
	ui.title_pushButton->installEventFilter(this);
	ui.all_widget->setVisible(false);
	ui.oneToOne_widget->setVisible(false);

	connect(ui.today_radioButton, SIGNAL(clicked()), this, SLOT(clickToday()));
	connect(ui.all_radioButton, SIGNAL(clicked()), this, SLOT(clickAll()));
	connect(ui.oneToOne_radioButton, SIGNAL(clicked()), this, SLOT(clickOneToOne())); 
	connect(ui.min_pushButton, SIGNAL(clicked()), this, SLOT(MinDialog()));
	connect(ui.close_pushButton, SIGNAL(clicked()), this, SLOT(CloseDialog()));
	connect(ui.pic_btn, SIGNAL(clicked()), this, SLOT(clickPic()));

	m_pWorker = new Worker();
	connect(this, SIGNAL(sig_StartLoading()), m_pWorker, SLOT(slot_StartLoading()));
	connect(this, SIGNAL(sig_Close()), m_pWorker, SLOT(slot_Close()));

	int iWidth = QApplication::desktop()->width();
	int iHeight = QApplication::desktop()->height();

	move(iWidth - width() - 100, 30);

	QAction* returnAction = new QAction(tr("切换账号"), this);
	connect(returnAction, SIGNAL(triggered()), this, SLOT(ReturnAccount()));
	QAction* closeAction = new QAction(tr("关闭"), this);
	connect(closeAction, SIGNAL(triggered()), this, SLOT(CloseWindow()));

	m_menu = new QMenu;
	m_menu->addAction(returnAction);
	m_menu->addAction(closeAction);

	m_UIMenu = new UIMenu(this);
	m_UIMenu->SetMainParent(this);
	m_UIMenu->hide();
}

UIAuxiliaryWnd::~UIAuxiliaryWnd()
{

}

void UIAuxiliaryWnd::setMainWindow(UIMainWindow* parent)
{
	m_parent = parent;
}

void UIAuxiliaryWnd::AddAuxiliary(QString picUrl, QString courseName, QString grade, QString teacherName , QString chatID, QString courseID, QString teacherID, QString token,
	QString studentName, std::string AudioPath, QString status)
{
	UIAuxiliaryList* auxiliary = new UIAuxiliaryList(ui.all_widget);
	QLabel* pic = auxiliary->AddCourse(picUrl, courseName, grade, teacherName, chatID, courseID, teacherID, token, studentName, AudioPath, status);
	connect(auxiliary, SIGNAL(clickAuxiliary(UIAuxiliaryList*)), this, SLOT(clickAuxiliary(UIAuxiliaryList*)));
	m_VerAll->addWidget(auxiliary);

	QLabel* line = new QLabel();
	line->setFixedHeight(1);
	line->setStyleSheet("background-color: rgb(229, 229, 229);");
	m_VerAll->addWidget(line);

	m_mapAuxiliaryChatID.insert(chatID, auxiliary);
	m_mapAuxiliaryCourseID.insert(courseID, auxiliary);
	m_pWorker->m_mapUrl.insert(pic, picUrl);

	// 添加到布局里
	if (m_spacerAll == NULL)
	{
		m_spacerAll = new QSpacerItem(5, 5, QSizePolicy::Minimum, QSizePolicy::Expanding);
		m_VerAll->addSpacerItem(m_spacerAll);
	}
	else
	{
		m_VerAll->removeItem(m_spacerAll);
		m_spacerAll = NULL;
		m_spacerAll = new QSpacerItem(5, 5, QSizePolicy::Minimum, QSizePolicy::Expanding);
		m_VerAll->addSpacerItem(m_spacerAll);
	}
}

void UIAuxiliaryWnd::AddOneToOneAuxiliary(QString picUrl, QString courseName, QString grade, QString teacherName, QString chatID, QString courseID,
	QString teacherID, QString token, QString studentName, std::string AudioPath, QString status)
{
	UIAuxiliaryList* auxiliary = new UIAuxiliaryList(ui.all_widget);
	QLabel* pic = auxiliary->AddCourse(picUrl, courseName, grade, teacherName, chatID, courseID, teacherID, token, studentName, AudioPath, status,true);
	connect(auxiliary, SIGNAL(clickAuxiliary(UIAuxiliaryList*)), this, SLOT(clickAuxiliary(UIAuxiliaryList*)));//TODO
	m_VerOneToOne->addWidget(auxiliary);

	QLabel* line = new QLabel();
	line->setFixedHeight(1);
	line->setStyleSheet("background-color: rgb(229, 229, 229);");
	m_VerOneToOne->addWidget(line);

	m_mapAuxiliaryChatID.insert(chatID, auxiliary);
	m_mapAuxiliaryCourseID.insert(courseID, auxiliary);
	m_pWorker->m_mapUrl.insert(pic, picUrl);

	// 添加到布局里
	if (m_spacerOneToOne == NULL)
	{
		m_spacerOneToOne = new QSpacerItem(5, 5, QSizePolicy::Minimum, QSizePolicy::Expanding);
		m_VerOneToOne->addSpacerItem(m_spacerOneToOne);
	}
	else
	{
		m_VerOneToOne->removeItem(m_spacerOneToOne);
		m_spacerOneToOne = NULL;
		m_spacerOneToOne = new QSpacerItem(5, 5, QSizePolicy::Minimum, QSizePolicy::Expanding);
		m_VerOneToOne->addSpacerItem(m_spacerOneToOne);
	}
}

void UIAuxiliaryWnd::AddTodayAuxiliary(QString lessonName, QString courseID, QString courseName, QString time, QString status)
{
	UIAuxiliaryToday* auxiliary = new UIAuxiliaryToday(ui.today_widget);
	auxiliary->AddLesson(lessonName, courseID, courseName, time, status);
	connect(auxiliary, SIGNAL(clickAuxiliaryToday(UIAuxiliaryToday*)), this, SLOT(clickAuxiliaryToday(UIAuxiliaryToday*)));
	m_VerToday->addWidget(auxiliary);

	// 添加到布局里
	if (m_spacerToday == NULL)
	{
		m_spacerToday = new QSpacerItem(5, 5, QSizePolicy::Minimum, QSizePolicy::Expanding);
		m_VerToday->addSpacerItem(m_spacerToday);
	}
	else
	{
		m_VerToday->removeItem(m_spacerToday);
		m_spacerToday = NULL;
		m_spacerToday = new QSpacerItem(5, 5, QSizePolicy::Minimum, QSizePolicy::Expanding);
		m_VerToday->addSpacerItem(m_spacerToday);
	}
}

void UIAuxiliaryWnd::paintEvent(QPaintEvent *event)
{
	QPainterPath path;
	QPainter painter(this);

	QColor color(5, 157, 213);
	for (int i = 0; i < 45; i++)
		path.addRect(0, i, this->width() - 1, this->height() - i - 1);

	painter.setPen(color);
	painter.drawPath(path);
}

// 拖动标题做的处理
bool UIAuxiliaryWnd::eventFilter(QObject *target, QEvent *event)
{
	if (target == ui.title_pushButton)
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
	}
	else if (target == ui.scrollArea || target == ui.today_scrollArea)
	{
		QMouseEvent* pMe = static_cast<QMouseEvent*>(event);
		if (event->type() == QEvent::Resize)
		{
			m_mainAllView->setMaximumWidth(ui.scrollArea->width());
			m_mainTodayView->setMaximumWidth(ui.today_scrollArea->width());
		}
	}
	return false;
}

void UIAuxiliaryWnd::clickAuxiliary(UIAuxiliaryList* auxiliary)
{
	if (m_parent)
	{
		m_parent->CreateRoom(auxiliary->ChatID(), auxiliary->CourseID(), auxiliary->TeacherID(), auxiliary->Token(), auxiliary->StudentName(), 
			auxiliary->AudioPath(), auxiliary->CourseName(), auxiliary->UnreadMsgCount(), auxiliary->Status(), auxiliary->Is1v1Lesson());
		auxiliary->ClearMsgNumber();
	}
}

void UIAuxiliaryWnd::clickAuxiliaryOneToOne(UIAuxiliaryList*)
{

}

void UIAuxiliaryWnd::clickAuxiliaryToday(UIAuxiliaryToday* auxiliaryToday)
{
	QMap<QString, UIAuxiliaryList*>::iterator it;
	UIAuxiliaryList* auxiliary = NULL;
	it = m_mapAuxiliaryCourseID.find(auxiliaryToday->GetCourseID());
	if (it != m_mapAuxiliaryCourseID.end())
	{
		auxiliary = *it;
		if (m_parent)
		{
			m_parent->CreateRoom(auxiliary->ChatID(), auxiliary->CourseID(), auxiliary->TeacherID(), auxiliary->Token(), auxiliary->StudentName(), auxiliary->AudioPath(), auxiliary->CourseName(), auxiliary->UnreadMsgCount(), auxiliary->Status(),auxiliary->Is1v1Lesson());
			auxiliary->ClearMsgNumber();
		}
	}
}

void UIAuxiliaryWnd::style(QScrollArea *style)
{
	if (style)
	{
		// 设置滚动条样式
		style->verticalScrollBar()->setStyleSheet("QScrollBar:vertical"
			"{"
			"width:8px;"
			"background:rgba(0,0,0,0%);"
			"margin:0px,0px,0px,0px;"
			"padding-top:9px;"
			"padding-bottom:9px;"
			"}"
			"QScrollBar::handle:vertical"
			"{"
			"width:8px;"
			"background:rgba(0,0,0,25%);"
			" border-radius:4px;"
			"min-height:20;"
			"}"
			"QScrollBar::handle:vertical:hover"
			"{"
			"width:8px;"
			"background:rgba(0,0,0,50%);"
			" border-radius:4px;"
			"min-height:20;"
			"}"
			"QScrollBar::add-line:vertical"
			"{"
			"height:9px;width:8px;"
			"border-image:url(:/images/a/3.png);"
			"subcontrol-position:bottom;"
			"}"
			"QScrollBar::sub-line:vertical"
			"{"
			"height:9px;width:8px;"
			"border-image:url(:/images/a/1.png);"
			"subcontrol-position:top;"
			"}"
			"QScrollBar::add-line:vertical:hover"
			"{"
			"height:9px;width:8px;"
			"border-image:url(:/images/a/4.png);"
			"subcontrol-position:bottom;"
			"}"
			"QScrollBar::sub-line:vertical:hover"
			"{"
			"height:9px;width:8px;"
			"border-image:url(:/images/a/2.png);"
			"subcontrol-position:top;"
			"}"
			"QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical"
			"{"
			"background:rgba(0,0,0,10%);"
			"border-radius:4px;"
			"}"
			);
	}
}

void UIAuxiliaryWnd::clickToday()
{
	ui.today_widget->setVisible(true);
	ui.all_widget->setVisible(false);
	ui.oneToOne_widget->setVisible(false);
}

void UIAuxiliaryWnd::clickAll()
{
	ui.today_widget->setVisible(false);
	ui.all_widget->setVisible(true);
	ui.oneToOne_widget->setVisible(false);
}

void UIAuxiliaryWnd::clickOneToOne()
{
	ui.oneToOne_widget->setVisible(true);
	ui.today_widget->setVisible(false);
	ui.all_widget->setVisible(false);
}

void UIAuxiliaryWnd::AddTodayNoLesson()
{
	QHBoxLayout* layout = new QHBoxLayout();
	QPixmap pix = QPixmap("./images/nolesson.png");
	QLabel* noLesson = new QLabel();
	noLesson->setFixedSize(180, 180);
	noLesson->setPixmap(pix);
	layout->addWidget(noLesson);
	m_VerToday->addLayout(layout);
}

QPixmap UIAuxiliaryWnd::setStudentUrl(QString Url)
{
	QUrl url(Url);
	QNetworkAccessManager manager;
	QEventLoop loop;

	QNetworkReply *reply = manager.get(QNetworkRequest(url));
	//请求结束并下载完成后，退出子事件循环 
	QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
	//开启子事件循环 
	loop.exec();

	QByteArray jpegData = reply->readAll();
	QPixmap pixmap;
	pixmap.loadFromData(jpegData);
	QPixmap scaledPixmap = pixmap.scaled(QSize(30, 30));
//	ui.pic_label->setPixmap(scaledPixmap);

	ui.pic_widget->setAutoFillBackground(true);
	QPalette palette;
	palette.setBrush(QPalette::Background, QBrush(scaledPixmap));
	ui.pic_widget->setPalette(palette);

	m_UIMenu->setPic(scaledPixmap);
	return scaledPixmap;
}

void UIAuxiliaryWnd::MinDialog()
{
	showMinimized();
	hide();
}

void UIAuxiliaryWnd::CloseDialog()
{
	emit sig_Close();

	if (m_parent)
		m_parent->CloseDialog();
}

// bool UIAuxiliaryWnd::nativeEvent(const QByteArray &eventType, void *message, long *result)
// {
// 	if ("windows_generic_MSG" == eventType)
// 	{
// 		PMSG pMsg = static_cast<PMSG>(message);
// 		switch (pMsg->message)
// 		{
// 		case WM_NCHITTEST:
// 		{
// 			int x = GET_X_LPARAM(pMsg->lParam) - this->frameGeometry().x();
// 			int y = GET_Y_LPARAM(pMsg->lParam) - this->frameGeometry().y();
// 
// 			int xflag = (x <= MAINWINDOW_X_MARGIN) ? -1 : ((x < this->width() - MAINWINDOW_X_MARGIN) ? 0 : 1);
// 			int yflag = (y <= MAINWINDOW_Y_MARGIN) ? -1 : ((y < this->height() - MAINWINDOW_Y_MARGIN) ? 0 : 1);
// 
// 			if (-1 == xflag && -1 == yflag)
// 			{
// 				*result = HTTOPLEFT;
// 			}
// 			else if (-1 == xflag && 0 == yflag)
// 			{
// 				*result = HTLEFT;
// 			}
// 			else if (-1 == xflag && 1 == yflag)
// 			{
// 				*result = HTBOTTOMLEFT;
// 			}
// 			else if (0 == xflag && -1 == yflag)
// 			{
// 				*result = HTTOP;
// 			}
// 			else if (0 == xflag && 0 == yflag)
// 			{
// 				*result = HTCLIENT;
// 			}
// 			else if (0 == xflag && 1 == yflag)
// 			{
// 				*result = HTBOTTOM;
// 			}
// 			else if (1 == xflag && -1 == yflag)
// 			{
// 				*result = HTTOPRIGHT;
// 			}
// 			else if (1 == xflag && 0 == yflag)
// 			{
// 				*result = HTRIGHT;
// 			}
// 			else if (1 == xflag && 1 == yflag)
// 			{
// 				*result = HTBOTTOMRIGHT;
// 			}
// 			if (0 == xflag && y > MAINWINDOW_Y_MARGIN && y <= MAINWINDOW_TITLE_HEIGHT)
// 			{
// 				qInfo("激活窗口");
// 				*result = HTCAPTION;
// 			}
// 			return true;
// 		}
// 		break;
// 		default:
// 			return false;
// 			break;
// 		}
// 	}
// 	return false;
// }

void UIAuxiliaryWnd::ReceiverNumber(QString chatID)
{
	QMap<QString, UIAuxiliaryList*>::iterator it;
	UIAuxiliaryList* auxiliary = NULL;
	it = m_mapAuxiliaryChatID.find(chatID);
	if (it != m_mapAuxiliaryChatID.end())
	{
		auxiliary = *it;
		auxiliary->AddMsgCount();
	}
}

void UIAuxiliaryWnd::LoadPic()
{
	emit sig_StartLoading();
}

void UIAuxiliaryWnd::clickPic()
{
//	m_menu->exec(QCursor::pos());//在当前鼠标位置显示
	if (m_UIMenu)
	{
		m_UIMenu->move(165,0);
		m_UIMenu->show();
		m_UIMenu->setFocus();
	}
}

void UIAuxiliaryWnd::ReturnAccount()
{
	emit sig_Close();

	if (m_parent)
		m_parent->returnClick();
}

void UIAuxiliaryWnd::setStudentName(QString studentName)
{
	m_strStudentName = studentName;
	m_UIMenu->setName(m_strStudentName);
}

void UIAuxiliaryWnd::setVersion(QString version)
{
	m_UIMenu->setVersion(version);
}

void UIAuxiliaryWnd::SetToken(QString mRemeberToken)
{
	m_UIMenu->setToken(mRemeberToken);
}