#include "ui1v1.h"

#include "IMInterface.h"

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

#define Audio 0		//音频设备
#define AudioOut 2	//扬声器
#define Video 3		//视频设备
#define VIDEO_FPS 50  

UI1v1::UI1v1(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	initWhiteBoardWidget();
	InitSetParamWnds();
	initConnection();
}

UI1v1::~UI1v1()
{

}

void UI1v1::InitSetParamWnds()
{
	m_AudioChangeInfo1v1 = new UIAudioChange1v1(this);
	m_AudioChangeInfo1v1->setWindowFlags(Qt::FramelessWindowHint);
	m_AudioChangeInfo1v1->hide();
	connect(m_AudioChangeInfo1v1, SIGNAL(sig_setAudioChange1v1(QString)), this, SLOT(setAudioChange1v1(QString)));
	connect(m_AudioChangeInfo1v1, SIGNAL(sig_setValueChange1v1(int, bool)), this, SLOT(setValueChange1v1(int, bool)));

	m_AudioOutChangeInfo1v1 = new UIAudioOutChange1v1(this);
	m_AudioOutChangeInfo1v1->setWindowFlags(Qt::FramelessWindowHint);
	m_AudioOutChangeInfo1v1->hide();
	connect(m_AudioOutChangeInfo1v1, SIGNAL(sig_setAudioOutChange1v1(QString)), this, SLOT(setAudioOutChange1v1(QString)));
	connect(m_AudioChangeInfo1v1, SIGNAL(sig_setValueChange1v1(int, bool)), this, SLOT(setValueChange1v1(int, bool)));

	m_VideoChangeInfo1v1 = new UIVideoChange1v1(this);
	m_VideoChangeInfo1v1->setWindowFlags(Qt::FramelessWindowHint);
	m_VideoChangeInfo1v1->hide();
	connect(m_VideoChangeInfo1v1, SIGNAL(sig_setVideoChange1v1(QString)), this, SLOT(setVideoChange1v1(QString)));

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
	 
	 	// 设置设置扬声器样式
	 	ui.AudioOut_checkBox->setStyleSheet("QCheckBox::indicator{width: 29px;height: 19px;}"
	 		"QCheckBox::indicator:unchecked{image: url(./images/audioout_open.png);}"
	 		"QCheckBox::indicator:checked{image: url(./images/audioout_close.png);}");
	 	ui.AudioOut_pushButton->setStyleSheet("QPushButton{border-image:url(./images/Arrow.png);}");
	 
	 	connect(ui.Audio1v1_checkBox, SIGNAL(stateChanged(int)), this, SLOT(Audio1v1Status(int)));
	 	connect(ui.Audio1v1_pushButton, SIGNAL(clicked()), this, SLOT(clickAudio1v1Param()));
	 	connect(ui.video1v1_checkBox, SIGNAL(stateChanged(int)), this, SLOT(Video1v1Status(int)));
	 	connect(ui.video1v1_pushButton, SIGNAL(clicked()), this, SLOT(clickVideo1v1Param()));
	 	connect(ui.AudioOut_checkBox, SIGNAL(stateChanged(int)), this, SLOT(AudioOut1v1Status(int)));
	 	connect(ui.AudioOut_pushButton, SIGNAL(clicked()), this, SLOT(clickAudioOut1v1Param()));
}

//初始化白板
void UI1v1::initWhiteBoardWidget()
{
	mWhiteBoard = new Palette();
	ui.horizontalLayout_21->addWidget(mWhiteBoard);
	mWhiteBoard->setIsDraw(true);
	connect(mWhiteBoard, SIGNAL(PicData(QString)), this, SLOT(PicData(QString)));

	m_WhiteBoardTool = new UIWhiteBoardTool(ui.live1v1_widget);
	m_WhiteBoardTool->hide();
	connect(m_WhiteBoardTool, SIGNAL(selectColor(QColor&)), this, SLOT(selectColor(QColor&)));
	connect(m_WhiteBoardTool, SIGNAL(returnClick()), this, SLOT(returnClick()));

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

	//1v1全屏桌面
	m_VideoInfo1v1 = new UIVideo1v1(ui.fullS1v1_widget);
	m_VideoInfo1v1->setWindowFlags(Qt::FramelessWindowHint);
	ui.horizontalLayout_21->addWidget(m_VideoInfo1v1);
	m_VideoInfo1v1->hide();
	connect(m_VideoInfo1v1, SIGNAL(sig_refreshWnd()), this, SLOT(slot_refreshWnd()));
	connect(m_VideoInfo1v1, SIGNAL(sig_CustomVideoData(__int64, const char*, int, int, int)), this, SLOT(slot_CustomVideoData(__int64, const char*, int, int, int)));
}

void UI1v1::initConnection()
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

void UI1v1::setAudioChange1v1(QString path)
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

void UI1v1::setValueChange1v1(int iVolumn, bool capturn)
{
	IMInterface::getInstance()->setAudioChange(iVolumn, capturn);
}

void UI1v1::setVideoChange1v1(QString path)
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

void UI1v1::setAudioOutChange1v1(QString path)
{
	if (!ui.AudioOut_pushButton->isChecked())
	{
		if (!path.isNull())
		{
			IMInterface::getInstance()->endDevice(AudioOut);
			IMInterface::getInstance()->startDevice(AudioOut, path.toStdString(), 0, 0, 0);
		}
	}
}

void UI1v1::show1v1ErrorTip(QString sError)
{
	m_bLiving1v1 = false;
	CMessageBox::showMessage(
		QString("答疑时间"),
		QString(sError),
		QString("确定"));
}

void UI1v1::joinRtsRoom(const std::string &roomName)
{
	m_RoomName = roomName;
	IMInterface::getInstance()->joinRtsRoom(roomName, false);
}

void UI1v1::joinRoomSuccessfully(const std::string &session_id, __int64 channel_id, const std::string &custom_info)
{
	IMInterface::getInstance()->joinVChatRoom(2, m_RoomName);//TODO  是否学生端只加入房间不创建房间？
}

// 加入音视频成功后，开始直播流程
void UI1v1::joinVChatSuccessfully()
{
	/*m_bLiving1v1 = true;
	m_LiveStatusManager->SendStart1v1LiveHttpMsg(m_lessonid, m_curTags->ChatID(), m_Token);*/
	//TODO  加入直播房间后应接受音视频流
}

void UI1v1::errorInfo(const QString & error)
{
	m_bLiving1v1 = false;
	CMessageBox::showMessage(
		QString("答疑时间"),
		QString("加入房间出现错误！代码：") + error,
		QString("确定"),
		QString("取消"));
}

void UI1v1::PicData(QString str)
{
	std::string picData = str.toStdString();
	IMInterface::getInstance()->SendData("", 1, picData);
}

void UI1v1::setDeviceInfos(int type)
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

void UI1v1::clickVideo1v1Param()
{
	m_VideoChangeInfo1v1->show();
	m_VideoChangeInfo1v1->move(width() / 2 - m_VideoChangeInfo1v1->width() / 2, height() / 2 - m_VideoChangeInfo1v1->height() / 2);
	m_VideoChangeInfo1v1->setFocus();
}

void UI1v1::clickAudio1v1Param()
{
	m_AudioChangeInfo1v1->show();
	m_AudioChangeInfo1v1->move(width() / 2 - m_AudioChangeInfo1v1->width() / 2, height() / 2 - m_AudioChangeInfo1v1->height() / 2);
	m_AudioChangeInfo1v1->setFocus();
}

void UI1v1::clickAudioOut1v1Param()
{
	m_AudioOutChangeInfo1v1->show();
	m_AudioOutChangeInfo1v1->move(width() / 2 - m_AudioOutChangeInfo1v1->width() / 2, height() / 2 - m_AudioOutChangeInfo1v1->height() / 2);
	m_AudioOutChangeInfo1v1->setFocus();
}

void UI1v1::Audio1v1Status(int iStatus)
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

void UI1v1::Video1v1Status(int iStatus)
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

void UI1v1::AudioOut1v1Status(int iSatus)
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

void UI1v1::selectColor(QColor& color)
{
	if (mWhiteBoard)
	{
		mWhiteBoard->setPenColor(color);
	}
}

void UI1v1::returnClick()
{
	if (mWhiteBoard)
		mWhiteBoard->revocation();
}

void UI1v1::rtsDataReceived(const std::string& data)
{
	if (mWhiteBoard)
		mWhiteBoard->RecData(data);
}

QLayout* UI1v1::chat1v1Widget()
{
	return ui.horizontalLayout_18;
}

void UI1v1::initDevice()
{
	IMInterface::getInstance()->initVChat();
	IMInterface::getInstance()->initVChatCallback();
	IMInterface::getInstance()->initRtsCallback();

	IMInterface::getInstance()->EnumDeviceDevpath(Audio);
	IMInterface::getInstance()->EnumDeviceDevpath(Video);
	IMInterface::getInstance()->EnumDeviceDevpath(AudioOut);
}