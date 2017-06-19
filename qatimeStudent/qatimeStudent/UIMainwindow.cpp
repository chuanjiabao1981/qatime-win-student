#include "UIMainwindow.h"
#include "course.h"
#include "lesson.h"
#include <QDir>
#include <QDesktopWidget>
#include "nim_tools_http_cpp.h"
#include "HttpRequest.h"

#define MAINWINDOW_X_MARGIN 2
#define MAINWINDOW_Y_MARGIN 2
#define MAINWINDOW_TITLE_HEIGHT 100

extern bool g_environmentType;	// 环境类型		true为生产环境		false为测试环境  默认为true
extern QString g_remeberToken;

UIMainWindow* m_This = NULL;
UIMainWindow::UIMainWindow(QWidget *parent)
	: QWidget(parent)
	, m_LoginWindow(NULL)
	, m_WindowSet(NULL)
	, m_AuxiliaryWnd(NULL)
	, m_hBoardWnd(NULL)
	, m_hCameraWnd(NULL)
	, m_BoardTimer(NULL)
	, m_CameraTimer(NULL)
{
	ui.setupUi(this);
	m_This = this;
	setWindowTitle("StudentWindow");

	m_BoardTimer = new QTimer(this);
	connect(m_BoardTimer, SIGNAL(timeout()), this, SLOT(slot_BoardTimeout()));

	m_CameraTimer = new QTimer(this);
	connect(m_CameraTimer, SIGNAL(timeout()), this, SLOT(slot_CameraTimeout()));

	m_WindowSet = new UIWindowSet();
	m_WindowSet->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinMaxButtonsHint);
	m_WindowSet->setMainWindow(this);
	m_WindowSet->hide();

	m_AuxiliaryWnd = new UIAuxiliaryWnd();
	m_AuxiliaryWnd->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
	m_AuxiliaryWnd->setMainWindow(this);
	m_AuxiliaryWnd->show();
	SetWindowPos((HWND)m_AuxiliaryWnd->winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}

UIMainWindow::~UIMainWindow()
{
	if (m_WindowSet)
	{
		delete m_WindowSet;
		m_WindowSet = NULL;
	}

	if (m_AuxiliaryWnd)
	{
		delete m_AuxiliaryWnd;
		m_AuxiliaryWnd = NULL;
	}
}

void UIMainWindow::setLoginWindow(LoginWindow* parent)
{
	m_LoginWindow = parent;
}

void UIMainWindow::setSudentInfo(QJsonObject &data)
{
	int iID = data["id"].toInt();
	m_studentID = QString::number(iID);
	m_WindowSet->setStudent(m_studentID);

 	// 设置学生名字
	m_studentName = data["name"].toString();
	m_AuxiliaryWnd->setStudentName(m_studentName);
 
	// 设置老师头像
	QString studentPhoto_url = data["avatar_url"].toString();
	m_studentPix = m_AuxiliaryWnd->setStudentUrl(studentPhoto_url);

	// 学生云信信息
	QJsonObject obj = data["chat_account"].toObject();
	m_accid = obj["accid"].toString();
	m_accidPassword = obj["token"].toString();
 
	m_WindowSet->setAccid(m_accid);
}

void UIMainWindow::setAutoSudentInfo(QString studentID, QString studentName, QString studentUrl, QString accid, QString token)
{
	m_studentID = studentID;
	m_studentName = studentName;
	m_WindowSet->setStudent(m_studentID);
	m_AuxiliaryWnd->setStudentName(m_studentName);

	// 设置头像
	m_studentPix = m_AuxiliaryWnd->setStudentUrl(studentUrl);

	// 学生云信信息
	m_accid= accid;
	m_accidPassword = token;

	m_WindowSet->setAccid(m_accid);
}

void UIMainWindow::setVersion(QString version)
{
	if (m_AuxiliaryWnd)
		m_AuxiliaryWnd->setVersion(version);
}

void UIMainWindow::ShowLesson()
{
	QString strUrl;
	if (g_environmentType)
	{
		strUrl = "https://qatime.cn/api/v1/live_studio/students/{student_id}/schedule";
		strUrl.replace("{student_id}", m_studentID);
	}
	else
	{
		strUrl = "http://testing.qatime.cn/api/v1/live_studio/students/{student_id}/schedule";
		strUrl.replace("{student_id}", m_studentID);
	}

	QUrl url = QUrl(strUrl);
	QNetworkRequest request(url);

	request.setRawHeader("Remember-Token", g_remeberToken.toUtf8());
	reply = manager.get(request);
	connect(reply, &QNetworkReply::finished, this, &UIMainWindow::LessonRequestFinished);
}

void UIMainWindow::LessonRequestFinished()
{
	int iCount = 0;
	QByteArray result = reply->readAll();
	QJsonDocument document(QJsonDocument::fromJson(result));
	QJsonObject obj = document.object();
	QJsonArray courses = obj["data"].toArray();
	foreach(const QJsonValue & value, courses)
	{
		QJsonObject obj = value.toObject();
		QJsonArray lessons = obj["lessons"].toArray();
		foreach(const QJsonValue & value, lessons)
		{
			Lesson *lesson = new Lesson();
			lesson->readJson(value.toObject());

			QString curTime = QDateTime::currentDateTime().toString("yyyy-MM-dd");
//			curTime = "2017-03-01";
			if (lesson->Date() == curTime)
			{
				if (m_AuxiliaryWnd)
					m_AuxiliaryWnd->AddTodayAuxiliary(lesson->name(), lesson->CourseID(), lesson->CourseName(), lesson->LessonTime(), lesson->ChinaLessonStatus(),lesson->Is1v1());

				iCount++;
			}
			
			delete lesson;
		}
	}

	if (iCount == 0)
	{
		if (m_AuxiliaryWnd)
			m_AuxiliaryWnd->AddTodayNoLesson();
	}

	ShowAuxiliary();
}

void UIMainWindow::ShowOneToOneAuxiliary()
{
	QString strUrl;
	if (g_environmentType)
	{
		strUrl = "https://qatime.cn/api/v1/live_studio/students/{student_id}/interactive_courses";
		strUrl.replace("{student_id}", m_studentID);
	}
	else
	{
		strUrl = "http://testing.qatime.cn/api/v1/live_studio/students/{student_id}/interactive_courses";
		strUrl.replace("{student_id}", m_studentID);
	}

	QByteArray append("?per_page=");
	append += "100";

	strUrl += append;
	QUrl url = QUrl(strUrl);
	QNetworkRequest request(url);

	request.setRawHeader("Remember-Token", g_remeberToken.toUtf8());
	reply = manager.get(request);
	connect(reply, &QNetworkReply::finished, this, &UIMainWindow::OneToOneAuxiliaryRequestFinished);
}

void UIMainWindow::OneToOneAuxiliaryRequestFinished()
{
	int i = 0;
	QByteArray result = reply->readAll();
	QJsonDocument document(QJsonDocument::fromJson(result));
	QJsonObject obj = document.object();

	OTO_INFO info = Course::getOneToOneInfoFromJson(obj);

	foreach(const OTO_DATA &data, info.data)
	{
		if (m_AuxiliaryWnd)
		{
			//数据中包含多个老师信息，如何确定当前一对一直播为哪个老师？
			QString teacherName;
			QString teacherId;
			if (!data.teachers.isEmpty())
			{
				teacherName = data.teachers.first().name;
				teacherId = data.teachers.first().id;
			}
			m_AuxiliaryWnd->AddOneToOneAuxiliary(data.publicize_info_url, data.name, data.grade, teacherName, data.chat_team_id, QString::number(data.id),
				teacherId, m_studentName, m_AudioPath, data.status);
		}

		i++;
	}

	qDebug() << QString::number(i);
	if (m_AuxiliaryWnd)
		m_AuxiliaryWnd->LoadPic();
}

void UIMainWindow::ShowAuxiliary()
{
	QString strUrl;
	if (g_environmentType)
	{
		strUrl = "https://qatime.cn/api/v1/live_studio/students/{student_id}/courses";
		strUrl.replace("{student_id}", m_studentID);
	}
	else
	{
		strUrl = "http://testing.qatime.cn/api/v1/live_studio/students/{student_id}/courses";
		strUrl.replace("{student_id}", m_studentID);
	}

	QByteArray append("?per_page=");
	append += "100";

	strUrl += append;
	QUrl url = QUrl(strUrl);
	QNetworkRequest request(url);

	request.setRawHeader("Remember-Token", g_remeberToken.toUtf8());
	reply = manager.get(request);
	connect(reply, &QNetworkReply::finished, this, &UIMainWindow::AuxiliaryRequestFinished);
}

void UIMainWindow::AuxiliaryRequestFinished()
{
	int i = 0;
	QByteArray result = reply->readAll();
	QJsonDocument document(QJsonDocument::fromJson(result));
	QJsonObject obj = document.object();
	QJsonArray courses = obj["data"].toArray();
	foreach(const QJsonValue & value, courses)
	{
		QJsonObject obj = value.toObject();
		Course *course = new Course();
		course->readJson(obj);

 		if (m_AuxiliaryWnd)
 			m_AuxiliaryWnd->AddAuxiliary(course->PicUrl(), course->name(), course->Grade(), course->TeacherName(), course->ChatId(), course->id(), course->OwnerId(),
				m_studentName, m_AudioPath, course->status());

		i++;

		delete course;
	}

	qDebug() << QString::number(i);
	if (m_AuxiliaryWnd)
		m_AuxiliaryWnd->LoadPic();

	ShowOneToOneAuxiliary();
}

void UIMainWindow::RequestKey()
{
	QString strUrl;
	if (g_environmentType)
		strUrl = "https://qatime.cn/api/v1/app_constant/im_app_key";
	else
		strUrl = "http://testing.qatime.cn/api/v1/app_constant/im_app_key";

	HttpRequest http;
	http.setRawHeader("Remember-Token", g_remeberToken.toUtf8());
	
	QByteArray result = http.httpGet(strUrl);
	QJsonDocument document(QJsonDocument::fromJson(result));
	QJsonObject obj = document.object();
	QJsonObject data = obj["data"].toObject();
	QJsonObject error = obj["error"].toObject();
	if (obj["status"].toInt() == 1 && data.contains("im_app_key"))
	{
		QString key = data["im_app_key"].toString();
		setKeyAndLogin(key);
	}
	else if (obj["status"].toInt() == 0)
	{
		//		RequestError(error);
	}
}

void UIMainWindow::setKeyAndLogin(QString key)
{
	// 登陆
	nim::SDKConfig config;
	config.database_encrypt_key_ = "Netease";
	//sdk能力参数（必填）
	nim::Client::Init(key.toStdString(),"Netease", "", config);
	nim_http::Init(); // 初始化云信http
	InitAudio();	  // 初始化语音
	m_WindowSet->initCallBack();

	auto Mcb = std::bind(OnMultispotLoginCallback, std::placeholders::_1);
	nim::Client::RegMultispotLoginCb(Mcb);

	auto cb = std::bind(OnLoginCallback, std::placeholders::_1, nullptr);
	nim::Client::Login(key.toStdString(), m_accid.toStdString(), m_accidPassword.toStdString(), cb);

	auto Kcb = std::bind(OnKickoutCallback, std::placeholders::_1);
	nim::Client::RegKickoutCb(Kcb);

	ShowLesson();
}

void UIMainWindow::OnLoginCallback(const nim::LoginRes& login_res, const void* user_data)
{
	m_This->m_WindowSet->ReceiverLoginMsg(login_res);
}
//多端
void UIMainWindow::OnMultispotLoginCallback(const nim::MultiSpotLoginRes& res)
{
	bool online = res.notify_type_ == nim::kNIMMultiSpotNotifyTypeImIn;
	if (!res.other_clients_.empty())
	{
	}
}

void UIMainWindow::OnKickoutCallback(const nim::KickoutRes& res)
{
	int i = 0;
	if (res.client_type_ == nim::kNIMClientTypePCWindows)
	{
		i++;
	}
}

bool UIMainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
	if ("windows_generic_MSG" == eventType)
	{
		PMSG pMsg = static_cast<PMSG>(message);
		switch (pMsg->message)
		{
		case MSG_VIDEO_HWND:
		{
			MSG* Msg = pMsg;
			m_hBoardWnd = (HWND)Msg->wParam;
			m_WindowSet->setBoardHwnd(m_hBoardWnd);

			
			m_BoardTimer->start(400);
		}
		break;
		case MSG_VIDEO_CAMERA:
		{
			MSG* Msg = pMsg;
			m_hCameraWnd = (HWND)Msg->wParam;
			m_WindowSet->setCameraHwnd(m_hCameraWnd);
			
			m_CameraTimer->start(200);
		}
		break;
		case MSG_UPDATE_MSG:
		{
			CloseDialog();
		}
		break;
		case MSG_CLIENT_RECEIVE:  // 接收聊天消息
		{
			MSG* Msg = pMsg;
			nim::IMMessage* pIMsg = (nim::IMMessage*)Msg->wParam;

			if (m_WindowSet)
			{
				if (!m_WindowSet->ReceiverMsg(pIMsg))
				{
					if (m_AuxiliaryWnd)
					{
						m_AuxiliaryWnd->ReceiverNumber(QString::fromStdString(pIMsg->local_talk_id_));
					}
				}
			}

			delete pIMsg;
			pIMsg = NULL;
		}
		break;
		case MSG_CLIENT_RECORD:  // 接收历史消息
		{
			
			qInfo("接收到历史消息");
			MSG* Msg = pMsg;
			nim::QueryMsglogResult* pIMsg = (nim::QueryMsglogResult*)Msg->wParam;
			int iTyle = Msg->lParam;

			if (m_WindowSet)
			{
				if (iTyle == 1)
				{
					std::vector<nim::IMMessage> vec;
					for (auto& msg : pIMsg->msglogs_)
					{
						vec.push_back(msg);
					}
					
					//msg倒序排列
					size_t len = vec.size();
					for (size_t i = len; i > 0; i--)
					{
						m_WindowSet->ReceiverChatMsg(&vec[i-1]);
					}
				}
				else
				{
					m_WindowSet->ReceiverRecordMsg(pIMsg);
				}
			}
			
			delete pIMsg;
			pIMsg = NULL;
		}
		break;
		case MSG_SEND_AUDIO_MSG: // 语音播放结束
		{
			MSG* Msg = pMsg;
			char* msgid = (char*)Msg->lParam;
			int   sid = Msg->wParam;
			std::string strSid = QString::number(sid).toStdString();

			if (m_WindowSet)
				m_WindowSet->OnStopPlayAudio(strSid,msgid);
		}
		break;
		case MSG_MEMBERS_INFO:  // 接收群成员信息
		{
			MSG* Msg = pMsg;
			std::list<nim::TeamMemberProperty>* pMemberMsg = (std::list<nim::TeamMemberProperty>*)Msg->wParam;
			if (pMemberMsg->size() == 0)
			{
				delete pMemberMsg;
				return false;
			}
			
			std::string sid = pMemberMsg->front().GetTeamID();
		
			if (m_WindowSet)
				m_WindowSet->ReceiverMemberMsg(sid, pMemberMsg);

			delete pMemberMsg;
			pMemberMsg = NULL;
		}
		break;
		case WM_NCLBUTTONDBLCLK:
		{
			int y = GET_Y_LPARAM(pMsg->lParam) - this->frameGeometry().y();
			if (y > MAINWINDOW_Y_MARGIN && y <= MAINWINDOW_TITLE_HEIGHT)
			{
				// titleDoubleClick();
				return true;
			}
		}
		break;
		case MSG_PICTURE_PROCESS:
		{
			double iUpSize = (double)pMsg->wParam;
			double iFileSize = (double)pMsg->lParam;
			double diff = iUpSize / iFileSize;
// 			if (m_charRoom)
// 			{
// 				m_charRoom->UpLoadPicProcess(diff);
// 			}
		}
		break;
		case  MSG_SEND_MSG_STATUS:
		{
			nim::SendMessageArc* arcNew = (nim::SendMessageArc*)pMsg->wParam;
			if (arcNew)
			{
				if (m_WindowSet)
					m_WindowSet->SendStatus(arcNew);

				delete arcNew;
				arcNew = NULL;
			}
		}
		break;
		case MSG_SEND_AUDIO_FINISH_MSG:
		{
			MyAudioStruct* audio = (MyAudioStruct*)pMsg->wParam;
			if (audio)
			{
				if (m_WindowSet)
					m_WindowSet->SendAudio(audio->sSessionID, audio->sMsgID, audio->sFilePath, audio->fileSize, audio->duration, audio->sFileEx);

				delete audio;
				audio = NULL;
			}
		}
		break;
		case MSG_LOAD_AUDIO_SUC_MSG:
		{
			char* chatid = (char*)pMsg->wParam;
			char* msgid = (char*)pMsg->lParam;

			if (m_WindowSet)
				m_WindowSet->ReceiverAudioStatus(chatid, msgid,true);
		}
		break;
		case MSG_LOAD_AUDIO_FAIL_MSG:
		{
			char* chatid = (char*)pMsg->wParam;
			char* msgid = (char*)pMsg->lParam;

			if (m_WindowSet)
				m_WindowSet->ReceiverAudioStatus(chatid, msgid, false);
		}
		break;
		default:
			return false;
			break;
		}
	}
	return false;
}

//////////////////////////////添加云信功能////////////////////////////////
void UIMainWindow::setNetworkPic(const QString &szUrl)
{
	QUrl url(szUrl);
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
	QPixmap scaledPixmap = pixmap.scaled(QSize(30, 30), Qt::KeepAspectRatio);
	m_studentPix = scaledPixmap;
}

QPixmap UIMainWindow::StudentPhotoPixmap()
{
	return m_studentPix;
}

void UIMainWindow::InitAudio()
{
	// 语音消息初始化 audio
	std::string acc = m_accid.toStdString();
	std::string app_data_audio_path = nim::Tool::GetUserAppdataDir(acc.c_str());
	QString audio_path = QString::fromStdString(app_data_audio_path);
	QDir Dir(audio_path);
	if (!Dir.exists())
		Dir.mkdir(audio_path);
	std::string res_audio_path = audio_path.toStdString();
	bool ret = nim_audio::Audio::Init(res_audio_path);

	nim_audio::Audio::RegStopPlayCb(&UIMainWindow::OnStopAudioCallback);

	audio_path.append("audio\\");
	m_AudioPath = audio_path.toStdString();
}

void UIMainWindow::OnStopAudioCallback(int code, const char* file_path, const char* sid, const char* cid)
{
	QString strSid = QString(QLatin1String(sid));
	char* pData = new char[strlen(cid)];
	memcpy(pData, cid, strlen(cid));
	HWND hWnd = FindWindow(L"Qt5QWindowIcon", L"StudentWindow");
	if (hWnd == NULL)
		hWnd = FindWindow(L"Qt5QWindowToolSaveBits", L"StudentWindow");
	PostMessage(hWnd, MSG_SEND_AUDIO_MSG, (WPARAM)(int)strSid.toInt(), (LPARAM)pData);
}

void UIMainWindow::returnClick()
{
	if (m_LoginWindow)
		m_LoginWindow->ReturnLogin();
}

void UIMainWindow::CreateRoom(QString chatID, QString courseID, QString teacherID, QString studentName, std::string audioPath, QString courseName, int UnreadCount, QString status, bool b1v1Lesson)
{
	if (m_WindowSet)
		m_WindowSet->AddChatRoom(chatID, courseID, teacherID, studentName, audioPath, courseName, UnreadCount, status, b1v1Lesson);
}

void UIMainWindow::CreateRoomForCourseID(QString courseID)
{
	QString strUrl;
	if (g_environmentType)
		strUrl = "https://qatime.cn/api/v1/live_studio/students/{studentid}/courses/{id}";
	else
		strUrl = "http://testing.qatime.cn/api/v1/live_studio/students/{studentid}/courses/{id}";

	strUrl.replace("{id}", courseID);
	strUrl.replace("{studentid}", m_studentID);

	HttpRequest http;
	http.setRawHeader("Remember-Token", g_remeberToken.toUtf8());

	QByteArray result = http.httpGet(strUrl);
	QJsonDocument document(QJsonDocument::fromJson(result));
	QJsonObject obj = document.object();
	QJsonObject data = obj["data"].toObject();
	QJsonObject error = obj["error"].toObject();
	if (obj["status"].toInt() == 1 )
	{
 		QString chatID, teacherID, courseName, status;
		chatID = data["chat_team_id"].toString();
		teacherID = data["chat_team_owner"].toString();
 		courseName = data["name"].toString();
		status = data["status"].toString();
		
		if (m_WindowSet)
			m_WindowSet->AddChatRoom(chatID, courseID, teacherID, m_studentName, m_AudioPath, courseName, 0, status, false);
	}
	else if (obj["status"].toInt() == 0)
	{
		//		RequestError(error);
	}
}

void UIMainWindow::ShowCourse()
{
	if (m_AuxiliaryWnd)
	{
		if (m_AuxiliaryWnd->isVisible())
		{
			m_AuxiliaryWnd->showMinimized();
			m_AuxiliaryWnd->hide();
			return;
		}
		m_AuxiliaryWnd->showNormal();
	}
}

void UIMainWindow::CloseDialog()
{
	::PostMessage(m_hBoardWnd, WM_QUIT, 0, 0);
	::PostMessage(m_hCameraWnd, WM_QUIT, 0, 0);

	if (m_LoginWindow)
		m_LoginWindow->CloseTray();

	nim_audio::Audio::Cleanup();
	nim::Client::Cleanup();
	exit(0);
}

void UIMainWindow::slot_BoardTimeout()
{
	m_BoardTimer->stop();
	MoveWindow(m_hBoardWnd, 0, 0, 680, 525, true);//QApplication::desktop()->width() QApplication::desktop()->height()
}

void UIMainWindow::slot_CameraTimeout()
{
	m_CameraTimer->stop();
	MoveWindow(m_hCameraWnd, 0, 0, 300, 169, true);
}

void UIMainWindow::SetEnvironmental(bool bType)
{
	g_environmentType = bType;
}