﻿#include "IMInterface.h"

#include "windows.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QImage>
#include "json.h"

#define ERROR_INFO(code, error_string) QString("%1	错误代码：%2").arg(error_string).arg(code)

using namespace nim;

IMInterface *IMInterface::mInstace = NULL;

IMInterface *IMInterface::getInstance()
{
	if (!mInstace)
	{
		mInstace = new IMInterface();
	}

	return mInstace;
}

void IMInterface::destroy()
{
	if (mInstace)
	{
		delete mInstace;
		mInstace = NULL;
	}
}

IMInterface::IMInterface(QObject *parent)
	: QObject(parent)
{
	mVChatInitSuccess = false;
	m_bFullStatus = false;
	mLastError = "无错误";
	
}

IMInterface::~IMInterface()
{
	destroyRts();

	destroyVChat();
}

void IMInterface::initWhiteBoard()
{
	initRtsCallback();
}

void IMInterface::destroyRts()
{
	if (!mRtsSession_id.empty())
	{
		Rts::Hangup(mRtsSession_id, &CallbackHangup);
		mRtsSession_id = "";
	}	
}

void IMInterface::initVChat()
{
	mVChatInitSuccess = VChat::Init("");
//	initVChatCallback();
}

void IMInterface::destroyVChat()
{
	if (mVChatInitSuccess)
	{
		VChat::Cleanup();
	}	
}

void IMInterface::initRtsCallback()
{
	Rts::SetStartNotifyCb(&CallbackStartNotify);
	Rts::SetConnectNotifyCb(&CallbackConnectNotify);
	Rts::SetAckNotifyCb(&CallbackAckNotify);
	Rts::SetMemberChangeCb(&CallbackMemberNotify);
	Rts::SetHangupNotifyCb(&CallbackHangupNotify);
	Rts::SetRecDataCb(&CallbackRecData);
	Rts::SetControlNotifyCb(&CallbackControlNotify);
	Rts::SetSyncAckNotifyCb(&CallbackSyncAckNotify);
	
}

void IMInterface::createRtsRoom(const std::string &name, const std::string &custom_info/* = ""*/)
{
	mRtsRoomName = name;
	Rts::CreateConf(mRtsRoomName, custom_info, &CallbackCreateConf);
}

void IMInterface::joinRtsRoom(const std::string &name, bool record/* = false*/)
{
	mRtsRecord = record;
	Rts::JoinConf(name, record, &CallbackJoinConf);
}

void IMInterface::SendData(const std::string& session_id, int channel_type, const std::string& data, const std::string& uid /* = "" */)
{
	Rts::SendData(IMInterface::getInstance()->getSessionID(), kNIMRtsChannelTypeTcp, data);
}

void IMInterface::setSessionID(const std::string &sessionID)
{
	mRtsSession_id = sessionID;
}

const std::string &IMInterface::getSessionID()
{
	return mRtsSession_id;
}

void IMInterface::setRtsRoomName(const std::string &roomName)
{
	mRtsRoomName = roomName;
}

const std::string &IMInterface::getRtsRoomName()
{
	return mRtsRoomName;
}

void IMInterface::setRtsData(const std::string & data)
{
	mRtsData = data;
}

const std::string &IMInterface::getData()
{
	return mRtsData;
}

void IMInterface::setLastError(const QString &error)
{
	mLastError = error;
	emit hasError(error);
}

const QString &IMInterface::getLastError()
{
	return mLastError;
}

void IMInterface::initVChatCallback()
{
	VChat::NetDetect(&CallbackNetDetect);
	//注册音视频回调
	VChat::SetVideoDataCb(true, &CallbackVideoCaptureData);
	VChat::SetVideoDataCb(false, &CallbackVideoRecData);
	VChat::SetAudioDataCb(true, &CallbackAudioCaptureData);
	VChat::SetAudioDataCb(false, &CallbackAudioRecData);
	VChat::SetCbFunc(&CallbackVChatCb);
	nim::SystemMsg::RegSysmsgCb(std::bind(&OnReceiveSysmsgCallback, std::placeholders::_1));
}

void IMInterface::createVChatRoom(const std::string &name, const std::string &custom_info /* = "" */)
{
	mVChatRoomName = name;
	VChat::CreateRoom(mVChatRoomName, custom_info, "", &CallbackOpt2CreateCall);
}

void IMInterface::joinVChatRoom(int chatMode, const std::string &name, const std::string &json_extension /* = "" */)
{
	std::string json_value = json_extension;
	//if (m_LessonType == d_1V1Lesson)
	{
		Json::FastWriter fs;
		Json::Value value;
		value[nim::kNIMVChatSessionId] = "";
		value[nim::kNIMVChatRtmpUrl] = "";
		value[nim::kNIMVChatBypassRtmp] = 1;	// 开启互动直播功能
		value[nim::kNIMVChatRtmpRecord] = 1;
		value[nim::kNIMVChatAudioHighRate] = 1;
		value[nim::kNIMVChatSplitMode] = "";
		json_value = fs.write(value);
	}
	switch (chatMode)
	{
	case 1:
		VChat::JoinRoom(kNIMVideoChatModeAudio, name, json_value, &CallbackOpt2Call);
		break;
	case 2:
		VChat::JoinRoom(kNIMVideoChatModeVideo, name, json_value, &CallbackOpt2Call);
		break;
	default:
		break;
	}
}

void IMInterface::EnumDeviceDevpath(int deviceType)
{
	switch (deviceType)
	{
	case 0:
		VChat::EnumDeviceDevpath(kNIMDeviceTypeAudioIn, &CallbackDeviceDevpath);
		break;
	case 2:
		VChat::EnumDeviceDevpath(kNIMDeviceTypeAudioOutChat, &CallbackDeviceDevpath);
		break;
	case 3:
		VChat::EnumDeviceDevpath(kNIMDeviceTypeVideo, &CallbackDeviceDevpath);
		break;
	default:
		break;
	}
}

void IMInterface::startDevice(int type, const std::string& device_path, unsigned fps, int width, int height)
{
// 	if (device_path.empty())
// 	{
// 		emit IMInterface::getInstance()->hasError("启动设备失败，设备路径为空");
// 	}
	switch (type)
	{
	case 0:
		VChat::StartDevice(kNIMDeviceTypeAudioIn, device_path, fps, width, height, &CallbackStartDevice);
		break;
	case 2:
		VChat::StartDevice(kNIMDeviceTypeAudioOutChat, device_path, fps, width, height, &CallbackStartDevice);
		break;
	case 3:
		VChat::StartDevice(kNIMDeviceTypeVideo, device_path, fps, width, height, &CallbackStartDevice);
		break;
	default:
		break;
	}
}

void IMInterface::endDevice(int type)
{
	switch (type)
	{
	case 0:
		VChat::EndDevice(kNIMDeviceTypeAudioIn);
		break;
	case 2:
		VChat::EndDevice(kNIMDeviceTypeAudioOutChat);
		break;
	case 3:
		VChat::EndDevice(kNIMDeviceTypeVideo);
		break;
	default:
		break;
	}
}

void IMInterface::addDeviceInfo(const DevInfo &dInfo)
{
	mDeviceInfoMap.insert(dInfo.path, dInfo);
}

void IMInterface::addDeviceInfo(int type, const char *json_string)
{
	QJsonDocument document = QJsonDocument::fromJson(json_string);
	QJsonArray deviceArray = document.array();
	foreach (const QJsonValue &value, deviceArray)
	{
		QJsonObject obj = value.toObject();
		DevInfo info;
		info.type = type;
		info.name = obj["name"].toString();
		info.path = obj["path"].toString();
		mDeviceInfoMap.insert(info.path, info);
	}
}

const DeviceInfoMap &IMInterface::getDeviceInfos()
{
	return mDeviceInfoMap;
}

void IMInterface::setAudioChange(int volumn, bool captrue)
{
	VChat::SetAudioVolumn(volumn, captrue);
}

void IMInterface::SetCustomData(bool bVideo)
{
	VChat::SetCustomData(false,bVideo);
}

void IMInterface::CustomVideoData(__int64 time, const char* data, int size, int width, int height)
{
	VChat::CustomVideoData(time, data, size, width, height, nullptr);
}

void IMInterface::EndLive()
{
	Rts::Hangup(IMInterface::getInstance()->getSessionID(), &CallbackHangup);
	VChat::End("");
}

void CallbackStartChannel(nim::NIMResCode res_code, const std::string& session_id, int channel_type, const std::string& uid)
{
	if (nim::kNIMResSuccess == res_code)
	{
		IMInterface::getInstance()->setSessionID(session_id);
	}
}

void CallbackStartNotify(const std::string& session_id, int channel_type, const std::string& uid, const std::string& custom_info)
{
	if (IMInterface::getInstance()->getSessionID().empty())
	{
		IMInterface::getInstance()->setSessionID(session_id);
		Rts::Ack(session_id, channel_type, true, false, false, &CallbackAck);
	}
	else
	{
		Rts::Ack(session_id, channel_type, false, false, false, &CallbackAck);
	}
}

void CallbackConnectNotify(const std::string& session_id, int channel_type, int code, const std::string& json)
{
	if (session_id == IMInterface::getInstance()->getSessionID())
	{
		if (code != 200)//连接异常，挂断
		{
			// 白板连接出现异常
// 			Rts::Hangup(session_id.c_str(), &CallbackHangup);
// 			IMInterface::getInstance()->setSessionID("");
// 			VChat::End("");

			qDebug() << __FILE__ << __LINE__ << "rts fail server discontect errorcode：" << code;

			// 等同于成员离开，重新再次进入
			emit IMInterface::getInstance()->PeopleStatus(true);
		}
	}
}

void CallbackHangup(nim::NIMResCode res_code, const std::string& session_id)
{
}

void CallbackCreateConf(nim::NIMResCode res_code)
{
	IMInterface *instance = IMInterface::getInstance();
	if (NULL == instance)
	{
		return;
	}

	switch (res_code)
	{
	case nim::kNIMResSuccess://房间创建成功
		{
			std::string roomid = IMInterface::getInstance()->getRtsRoomName();
			emit IMInterface::getInstance()->createRtsRoomSuccessfully(roomid);
		}
		break;
	case nim::kNIMResExist://房间已存在
		//		emit instance->rtsRoomExistence();
		{
			std::string roomid = IMInterface::getInstance()->getRtsRoomName();
			emit IMInterface::getInstance()->createRtsRoomSuccessfully(roomid);
		}
		break;
	case nim::kNIMResForbidden://没有权限，禁止操作
		instance->setLastError(ERROR_INFO(res_code, "没有权限，禁止操作"));
		break;
	default:
		break;
	}
	instance = NULL;
}

void CallbackJoinConf(nim::NIMResCode res_code, const std::string& session_id, __int64 channel_id, const std::string& custom_info)
{
	if (nim::kNIMResSuccess == res_code || nim::kNIMResExist == res_code)
	{
		IMInterface::getInstance()->setSessionID(session_id);
		emit IMInterface::getInstance()->joinRtsRoomSuccessfully(session_id, channel_id, custom_info);
		qDebug() << __FILE__ << __LINE__ << "白板加入成功！";
	}
	else
	{
		emit IMInterface::getInstance()->joinVChatSuccessfully(false);
		qDebug() << __FILE__ << __LINE__ << "白板加入失败,code:"<<res_code;
	}
}

void CallbackAck(nim::NIMResCode res_code, const std::string& session_id, int channel_type, bool accept)
{

}

void CallbackAckNotify(const std::string& session_id, int channel_type, bool accept, const std::string& uid)
{
	if (session_id == IMInterface::getInstance()->getSessionID())
	{
		if (accept)
		{
			//对方同意，sdk底层开始连接
		}
		else
		{
			IMInterface::getInstance()->setSessionID("");
		}
	}
}

void CallbackSyncAckNotify(const std::string& session_id, int channel_type, bool accept)
{

}

void CallbackMemberNotify(const std::string& session_id, int channel_type, const std::string& uid, int code)
{
	if (session_id == IMInterface::getInstance()->getSessionID())
	{
		if (code == kNIMRtsMemberStatusJoined)
		{
			//成员进入，此时可以在tcp通道发送数据
			qDebug() << __FILE__ << __LINE__ << QString::fromStdString(uid) << code << "成员进入";
		}
		else if (code == kNIMRtsMemberStatusLeaved)
		{
			qDebug() << __FILE__ << __LINE__ << QString::fromStdString(uid) << code << "成员离开";
			// 成员离开时，返回false
			emit IMInterface::getInstance()->PeopleStatus(false);
		}
	}
}

void CallbackHangupNotify(const std::string& session_id, const std::string& uid)
{
	if (session_id == IMInterface::getInstance()->getSessionID())
	{
		IMInterface::getInstance()->setSessionID("");
	}
}

void CallbackControl(nim::NIMResCode res_code, const std::string& session_id, const std::string& info)
{

}

void CallbackControlNotify(const std::string& session_id, const std::string& info, const std::string& uid)
{

}

void CallbackRecData(const std::string& session_id, int channel_type, const std::string& uid, const std::string& data)
{
	if (session_id == IMInterface::getInstance()->getSessionID())
	{
		int draw_op_type_;

		QString strData = QString::fromStdString(data);
		if (strData.mid(0, 2).toInt() == 16)
		{
			qDebug() << __FILE__ << __LINE__ << "切屏消息：" << QString::fromStdString(data);
			QStringList list = strData.split(";");
			foreach(const QString& p, list)
			{
				QStringList param_list = p.split(":");
				draw_op_type_ = param_list.first().toInt();
				QStringList pointInfo = param_list.last().split(",");

				if (pointInfo.size() >= 4)
				{
					// 暂时去掉白板接收切换消息 add by zbc 20170826
					QString status = pointInfo.at(3);
					/*
					if (status == "desktop")
						IMInterface::getInstance()->setFullScreenStatus(true);
					else
						IMInterface::getInstance()->setFullScreenStatus(false);

					emit IMInterface::getInstance()->sig_SendFullScreen(IMInterface::getInstance()->IsFullScreen());
					*/
				}
			}
		}

		emit IMInterface::getInstance()->rtsDataReceived(data);
	}	
}

void IMInterface::OnReceiveSysmsgCallback(const nim::SysMessage& msg)
{
	int i = 0;
	
	if (msg.type_ == nim::kNIMSysMsgTypeCustomTeamMsg)
	{
		Json::Value json;
		Json::Reader reader;
		reader.parse(msg.attach_, json);
		if (json.isObject())
		{
			if (json.isMember("SwitchVedioObject"))
			{
				std::string mShowObject = json["SwitchVedioObject"].asString();
				qDebug() << __FILE__ << __LINE__ << "IM接收切屏消息：" << QString::fromStdString(mShowObject);
				
				if (mShowObject == "Board")
				{
					IMInterface::getInstance()->setFullScreenStatus(false);
				}
				else
				{
					IMInterface::getInstance()->setFullScreenStatus(true);
				}
				emit IMInterface::getInstance()->sig_SendFullScreen(IMInterface::getInstance()->IsFullScreen());
			}
			return;
		}
	}
}

void IMInterface::setFullScreenStatus(bool bopen)
{
	m_bFullStatus = bopen;
}

bool IMInterface::IsFullScreen()
{
	return m_bFullStatus;
}




void CallbackNetDetect(int code, nim::NetDetectCbInfo info)
{
	if (kNIMResSuccess == code)
	{

	}
}

// 音视频加入回调
void CallbackOpt2Call(int code, __int64 channel_id, const std::string& json_extension)
{
	qDebug() << __FILE__ << __LINE__ << code << channel_id << json_extension.c_str();
	if (kNIMResSuccess == code || nim::kNIMResExist == code)
	{
		emit IMInterface::getInstance()->joinVChatSuccessfully(true);
		qDebug() << __FILE__ << __LINE__ << "音视频互动加入成功！";
	}
	else
	{
		emit IMInterface::getInstance()->joinVChatSuccessfully(false);
		qDebug() << __FILE__ << __LINE__ << "音视频互动加入失败，code:" << code;
	}
}

// 音视频创建回调
void CallbackOpt2CreateCall(int code, __int64 channel_id, const std::string& json_extension)
{
	qDebug() << __FILE__ << __LINE__ << code << channel_id << json_extension.c_str();
	if (kNIMResSuccess == code || kNIMResExist == code)
	{
		emit IMInterface::getInstance()->createVChatRoomSuccessfully();
	}
}

void CallbackVideoCaptureData(uint64_t time, const char *data, unsigned int size, unsigned int width, unsigned int height, const char *json_extension, const void *user_data)
{
	char* copydata = new char[size];
	memcpy(copydata, data, size);
	emit IMInterface::getInstance()->VideoCapture(copydata, width, height, size);
}

void CallbackVideoRecData(uint64_t time, const char *data, unsigned int size, unsigned int width, unsigned int height, const char *json_extension, const void *user_data)
{
	char* copydata = new char[size];
	memcpy(copydata, data, size);
	if (IMInterface::getInstance()->IsFullScreen())
	{
		emit IMInterface::getInstance()->RecFullScreen(copydata, width, height, size);
	}
	else
		emit IMInterface::getInstance()->RecVideoCapture(copydata, width, height, size);

	emit IMInterface::getInstance()->sig_SendFullScreen(IMInterface::getInstance()->IsFullScreen());
}

void CallbackAudioCaptureData(uint64_t time, const char *data, unsigned int size, const char *json_extension, const void *user_data)
{

}

void CallbackAudioRecData(uint64_t time, const char *data, unsigned int size, const char *json_extension, const void *user_data)
{

}

void CallbackVChatCb(nim::NIMVideoChatSessionType type, __int64 channel_id, int code, const char *json, const void*)
{
	switch (type)
	{
	case nim::kNIMVideoChatSessionTypeStartRes:{
	}break;
	case nim::kNIMVideoChatSessionTypeInviteNotify:{
	}break;
	case nim::kNIMVideoChatSessionTypeCalleeAckRes:{
	}break;
	case nim::kNIMVideoChatSessionTypeCalleeAckNotify:{
	}break;
	case nim::kNIMVideoChatSessionTypeControlRes:{
		//
	}break;
	case nim::kNIMVideoChatSessionTypeControlNotify:{
	}break;
	case nim::kNIMVideoChatSessionTypeConnect:{
	}break;
	case nim::kNIMVideoChatSessionTypePeopleStatus:{
		if (code == nim::kNIMVideoChatSessionStatusJoined)
		{
			// 修改为成员进入时，传入true	modify by zbc 20170823
			emit IMInterface::getInstance()->PeopleStatus(true);
			return;
		}
		else if (code == nim::kNIMVideoChatSessionStatusLeaved)
		{
			// 修改为成员离开时，传入false	modify by zbc 20170823
			emit IMInterface::getInstance()->PeopleStatus(false);
			return;
		}
	}break;
	// 网络实时状况反馈
	case nim::kNIMVideoChatSessionTypeNetStatus:
	{
		emit IMInterface::getInstance()->FunctionGetNetState(code);
	}break;
	case nim::kNIMVideoChatSessionTypeHangupRes:{
	}break;
	case nim::kNIMVideoChatSessionTypeHangupNotify:{
	}break;
	case nim::kNIMVideoChatSessionTypeSyncAckNotify:{
	}break;
	case nim::kNIMVideoChatSessionTypeMp4Notify:{
	}break;
	case nim::kNIMVideoChatSessionTypeVolumeNotify:{
	}break;
	}
}

void CallbackDeviceDevpath(bool ret, nim::NIMDeviceType type, const char *json_extension, const void *user_data)
{
	if (!ret)
	{
		return;
	}

	IMInterface::getInstance()->addDeviceInfo(type, json_extension);
	switch (type)
	{
	case nim::kNIMDeviceTypeAudioIn:
		qDebug() << __FILE__ << __LINE__ << json_extension;
		break;
	case nim::kNIMDeviceTypeAudioOut:
		break;
	case nim::kNIMDeviceTypeAudioOutChat:
		break;
	case nim::kNIMDeviceTypeVideo:
		qDebug() << __FILE__ << __LINE__ << json_extension;
		break;
	case nim::kNIMDeviceTypeSoundcardCapturer:
		break;
	case nim::kNIMDeviceTypeAudioHook:
		break;
	default:
		break;
	}

	emit IMInterface::getInstance()->deviceInfos(type);
}

void CallbackStartDevice(nim::NIMDeviceType type, bool ret, const char *json_extension, const void *user_data)
{
	if (ret)
	{
		emit IMInterface::getInstance()->startDeviceSuccessfully(type);
	}
}