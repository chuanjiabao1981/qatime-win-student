#ifndef UIMAINWINDOW_H
#define UIMAINWINDOW_H

#include <QWidget>
#include "ui_UIMainwindow.h"
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "loginwindow.h"
#include "UIMessageBox.h"
#include "define.h"
#include <vector>
#include "UIWindowSet.h"
#include "UIAuxiliarywnd.h"

//---云信
#include "assert.h"
#include <string>
#include "nim_cpp_api.h"

// 语音录制完成的参数
struct MyAudioStruct
{
	std::string sSessionID;
	std::string sMsgID;
	std::string sFilePath;
	std::string sFileEx;
	long	fileSize;
	int		duration;
};

class LoginWindow;
class UIWindowSet;
class UIAuxiliaryWnd;
class UIMainWindow : public QWidget
{
	Q_OBJECT

public:
	UIMainWindow(QWidget *parent = 0);
	~UIMainWindow();

private:
	Ui::UIMainWindow ui;

	QString							mRemeberToken;
	QNetworkAccessManager			manager;
	QNetworkReply*					reply;

	LoginWindow*					m_LoginWindow;
	QString							m_studentID;			// 学生ID
	QPixmap							m_studentPix;			// 学生头像
	QString							m_studentName;			// 学生名字

	QString							m_accid;				// 云信自己的ID
	QString							m_token;				// 云信密码
	std::string						m_AudioPath;			// 语音消息路径
	UIWindowSet*					m_WindowSet;			// 窗口集
	UIAuxiliaryWnd*					m_AuxiliaryWnd;			// 辅导班窗口		
	HWND							m_hBoardWnd;			// 白板窗口
	HWND							m_hCameraWnd;			// 摄像头窗口
	QTimer*							m_BoardTimer;			// 白板延时
	QTimer*							m_CameraTimer;			// 摄像头延时
	bool						    m_EnvironmentalTyle;	// 环境类型
protected slots:
	void slot_BoardTimeout();
	void slot_CameraTimeout();
public:
	void	setLoginWindow(LoginWindow* parent);			// 登录窗口
	void	setRemeberToken(const QString &token);			// 设置token
	void	ShowAuxiliary();								// 查询学生辅导班信息
	void	AuxiliaryRequestFinished();						// 辅导班http请求
	void    ShowLesson();									// 查询今日课程信息
	void	LessonRequestFinished();						// 今日课程http请求
	void	ShowOneToOneAuxiliary();						// 查询一对一互动信息
	void	OneToOneAuxiliaryRequestFinished();				// 一对一互动http请求
	void	setSudentInfo(QJsonObject &data);				// 学生信息
	void	setAutoSudentInfo(QString teacherID, QString teacherName, QString teacherUrl, QString accid, QString token); // 自动登录的学生信息
	void    setVersion(QString version);
	QPixmap StudentPhotoPixmap();							// 本人头像
	void	setNetworkPic(const QString &szUrl);			// 从网络上获取头像
	void    returnClick();									// 重新登录
	void	ShowCourse();									// 显示辅导班
	void    CreateRoom(QString chatID, QString courseID, QString teacherID, QString token, QString studentName, std::string audioPath, QString courseName, int UnreadCount, QString status);		// 创建聊天室
	void    CloseDialog();

	/*添加云信功能*/
	void    InitAudio();									// 初始化语音
	void    RequestKey();									// 请求Key
	void    returnKey();									// 返回Key
	void	setKeyAndLogin(QString key);					// 获取Key							
	static void OnLoginCallback(const nim::LoginRes& login_res, const void* user_data);

	/**
	* 停止播放语音消息的回调
	* @param[in] code 返回码
	* @param[in] file_path 语音消息文件路径
	* @param[in] sid 会话id
	* @param[in] cid 消息id
	* @return void 无返回值
	*/
	static void OnStopAudioCallback(int code, const char* file_path, const char* sid, const char* cid);
	void SetEnvironmental(bool bType);	// 设置环境
protected:
	virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result); // 添加caption
};

#endif // UIMAINWINDOW_H
