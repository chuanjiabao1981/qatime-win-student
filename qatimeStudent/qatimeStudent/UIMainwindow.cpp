#include "UIMainwindow.h"
#include "course.h"
#include "lesson.h"
#include <QDir>
#include <QDesktopWidget>

#include "UIBoardview.h"

typedef bool(*nim_client_init)(const char *app_data_dir, const char *app_install_dir, const char *json_extension);
typedef void(*nim_client_cleanup)(const char *json_extension);
typedef void(*nim_client_login)(const char *app_token, const char *account, const char *password, const char *json_extension, nim_json_transport_cb_func cb, const void* user_data);
typedef void(*nim_client_relogin)(const char *json_extension);
typedef void(*nim_client_logout)(NIMLogoutType logout_type, const char *json_extension, nim_json_transport_cb_func cb, const void* user_data);
typedef void(*nim_client_kick_other_client)(const char *json_extension);
typedef void(*nim_client_reg_auto_relogin_cb)(const char *json_extension, nim_json_transport_cb_func cb, const void* user_data);
typedef void(*nim_client_reg_kickout_cb)(const char *json_extension, nim_json_transport_cb_func cb, const void* user_data);
typedef void(*nim_client_reg_disconnect_cb)(const char *json_extension, nim_json_transport_cb_func cb, const void* user_data);
typedef void(*nim_client_reg_multispot_login_notify_cb)(const char *json_extension, nim_json_transport_cb_func cb, const void *user_data);
typedef void(*nim_client_reg_kickout_other_client_cb)(const char *json_extension, nim_json_transport_cb_func cb, const void *user_data);
typedef void(*nim_client_reg_sync_multiport_push_config_cb)(const char *json_extension, nim_client_multiport_push_config_cb_func cb, const void *user_data);
typedef void(*nim_client_set_multiport_push_config)(const char *switch_content, const char *json_extension, nim_client_multiport_push_config_cb_func cb, const void *user_data);
typedef void(*nim_client_get_multiport_push_config)(const char *json_extension, nim_client_multiport_push_config_cb_func cb, const void *user_data);

typedef	const wchar_t * (*nim_tool_get_user_appdata_dir)(const char * app_account);
typedef	void(*nim_global_free_buf)(void *data);


#define MAINWINDOW_X_MARGIN 2
#define MAINWINDOW_Y_MARGIN 2
#define MAINWINDOW_TITLE_HEIGHT 100

#ifdef TEST
#define _DEBUG
#else
#endif
UIMainWindow::UIMainWindow(QWidget *parent)
	: QWidget(parent)
	, m_LoginWindow(NULL)
	, m_WindowSet(NULL)
	, m_AuxiliaryWnd(NULL)
	, m_hBoardWnd(NULL)
	, m_hCameraWnd(NULL)
	, m_BoardTimer(NULL)
	, m_CameraTimer(NULL)
	, m_EnvironmentalTyle(true)
{
	ui.setupUi(this);
	setWindowTitle("StudentWindow");
	initSDK();

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

// 	UIBoardView* vew = new UIBoardView();
// 	vew->show();
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

void UIMainWindow::setRemeberToken(const QString &token)
{
	mRemeberToken = token;
	m_WindowSet->SetToken(mRemeberToken);
	m_AuxiliaryWnd->SetToken(mRemeberToken);
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
	m_token = obj["token"].toString();
 
	m_WindowSet->setAccid(m_accid);

	InitAudio();
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
	m_token = token;

	m_WindowSet->setAccid(m_accid);

	InitAudio();
}

void UIMainWindow::setVersion(QString version)
{
	if (m_AuxiliaryWnd)
		m_AuxiliaryWnd->setVersion(version);
}

void UIMainWindow::ShowLesson()
{
	QString strUrl;
	if (m_EnvironmentalTyle)
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
	QString str = this->mRemeberToken;

	request.setRawHeader("Remember-Token", this->mRemeberToken.toUtf8());
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
					m_AuxiliaryWnd->AddTodayAuxiliary(lesson->name(), lesson->CourseID(), lesson->CourseName(), lesson->LessonTime(), lesson->ChinaLessonStatus());

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

void UIMainWindow::ShowAuxiliary()
{
	QString strUrl;
	if (m_EnvironmentalTyle)
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
	QString str = this->mRemeberToken;

	request.setRawHeader("Remember-Token", this->mRemeberToken.toUtf8());
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
		course->readJson(value.toObject());

 		if (m_AuxiliaryWnd)
 			m_AuxiliaryWnd->AddAuxiliary(course->PicUrl(), course->name(), course->Grade(), course->TeacherName(), course->ChatId(), course->id(), course->OwnerId(),
				mRemeberToken, m_studentName, m_AudioPath, course->status());

		i++;

		delete course;
	}

	qDebug() << QString::number(i);
	if (m_AuxiliaryWnd)
		m_AuxiliaryWnd->LoadPic();
	RequestKey();
}

void UIMainWindow::RequestKey()
{
	QString strUrl;
	if (m_EnvironmentalTyle)
		strUrl = "https://qatime.cn/api/v1/app_constant/im_app_key";
	else
		strUrl = "http://testing.qatime.cn/api/v1/app_constant/im_app_key";

	QUrl url = QUrl(strUrl);
	QNetworkRequest request(url);
	QString str = this->mRemeberToken;

	request.setRawHeader("Remember-Token", this->mRemeberToken.toUtf8());
	reply = manager.get(request);
	connect(reply, &QNetworkReply::finished, this, &UIMainWindow::returnKey);
}

void UIMainWindow::returnKey()
{
	QByteArray result = reply->readAll();
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
	auto cb = std::bind(OnLoginCallback, std::placeholders::_1, nullptr);
	bool bFail = nim::Client::Login(key.toStdString(), m_accid.toStdString(), m_token.toStdString(), cb);
	if (!bFail)
	{
		CMessageBox::showMessage(QString("答疑时间"), QString("失败！"), QString("确定"), QString("取消"));
		return;
	}

//	m_bLogin = true;
}

void UIMainWindow::OnLoginCallback(const nim::LoginRes& login_res, const void* user_data)
{
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
// 				*result = HTCAPTION;
// 			}
// 			return true;
// 		}
// 		break;
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
		case MSG_LOGIN:  // 接收登录返回结果
		{
			MSG* Msg = pMsg;
			nim::LoginRes* pLogMsg = (nim::LoginRes*)Msg->wParam;

			if (m_WindowSet)
				m_WindowSet->ReceiverLoginMsg(pLogMsg);

			delete pLogMsg;
			pLogMsg;
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
void UIMainWindow::initSDK()
{
	nim::SDKConfig config;

	//sdk能力参数（必填）
	config.database_encrypt_key_ = "Netease"; //string（db key必填，目前只支持最多32个字符的加密密钥！建议使用32个字符）
	bool ret = false;
	ret = nim::SDKFunction::LoadSdkDll();
	if (!ret)
		CMessageBox::showMessage(QString("答疑时间"), QString("加载云信SDK失败！"), QString("确定"), QString("取消"));

	ret = LoadConfig("Netease", "", config);
	if (!ret)
		CMessageBox::showMessage(QString("答疑时间"), QString("加载云信Config失败！"), QString("确定"), QString("取消"));

	nim_http::Init(); // 初始化云信http

	// 接受消息回调
	nim::Talk::RegReceiveCb(&nim_comp::TalkCallback::OnReceiveMsgCallback);
	// 发送消息状态回调
	nim::Talk::RegSendMsgCb(&nim_comp::TalkCallback::OnSendMsgCallback);
}

bool UIMainWindow::LoadConfig(const std::string& app_data_dir, const std::string& app_install_dir, nim::SDKConfig &config)
{
	QJsonObject config_root;
	QJsonObject config_values;
	config_values[kNIMDataBaseEncryptKey] = config.database_encrypt_key_.c_str();//string（db key必填，目前只支持最多32个字符的加密密钥！建议使用32个字符）
	config_values[kNIMPreloadAttach] = config.preload_attach_;        //bool （选填，是否需要预下载附件缩略图， sdk默认预下载）
	config_values[kNIMSDKLogLevel] = config.sdk_log_level_;
	config_root[kNIMGlobalConfig] = config_values;

	return NIM_SDK_GET_FUNC(nim_client_init)(app_data_dir.c_str(), app_install_dir.c_str(), GetJsonStringWithNoStyled(config_root).c_str());
}

std::string UIMainWindow::GetJsonStringWithNoStyled(const QJsonObject& values)
{
	QJsonDocument document;
	document.setObject(values);

	QByteArray byte_array = document.toJson(QJsonDocument::Compact);
	QString json_str(byte_array);
	return json_str.toStdString();
}

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
//	ui.teacherPhoto_Label->setPixmap(scaledPixmap);
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
	const wchar_t *dir = NIM_SDK_GET_FUNC(nim_tool_get_user_appdata_dir)(acc.c_str());
	std::wstring app_data_audio_path = (std::wstring)dir;
	NIM_SDK_GET_FUNC(nim_global_free_buf)((void*)dir);
	QString audio_path = QString::fromStdWString(app_data_audio_path);
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
	PostMessage(hWnd, MSG_SEND_AUDIO_MSG, (WPARAM)(int)strSid.toInt(), (LPARAM)pData);
}

void UIMainWindow::returnClick()
{
	if (m_LoginWindow)
		m_LoginWindow->ReturnLogin();
}

void UIMainWindow::CreateRoom(QString chatID, QString courseID, QString teacherID, QString token, QString studentName, std::string audioPath, QString courseName, int UnreadCount, QString status)
{
	if (m_WindowSet)
		m_WindowSet->AddChatRoom(chatID, courseID, teacherID, token, studentName, audioPath, courseName, UnreadCount, status);
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
	MoveWindow(m_hBoardWnd, 0, 0, 656, 519, true);//QApplication::desktop()->width() QApplication::desktop()->height()
}

void UIMainWindow::slot_CameraTimeout()
{
	m_CameraTimer->stop();
	MoveWindow(m_hCameraWnd, 0, 0, 300, 169, true);
}

void UIMainWindow::SetEnvironmental(bool bType)
{
	m_EnvironmentalTyle = bType;
	m_WindowSet->SetEnvironmental(m_EnvironmentalTyle);
	m_AuxiliaryWnd->SetEnvironmental(m_EnvironmentalTyle);
}