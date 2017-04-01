#ifndef UIWINDOWSET_H
#define UIWINDOWSET_H

#include <QWidget>
#include "ui_UIWindowSet.h"
#include "UIMainwindow.h"
#include "UITags.h"
#include "UIChatRoom.h"
#include "UINoticewnd.h"
#include "UICoursewnd.h"
#include "UIPersonwnd.h"
#include "UILessonlist.h"
#include <QTimer>

//---云信
#include "nim_client_def.h"
#include "assert.h"
#include <string>
#include "YxChat/nim_tools_http_cpp_wrapper.h"
#include "YxChat/nim_client_helper.h"
#include "YxChat/nim_cpp_talk.h"
#include "YxChat/nim_cpp_team.h"
#include "YxChat/nim_cpp_msglog.h"
#include "YxChat/nim_cpp_nos.h"
#include "YxChat/nim_tools_audio_cpp_wrapper.h"
#include "YxChat/nim_sdk_helper.h"
#include "YxChat/session_callback.h"
#include "YxChat/nim_cpp_client.h"

class UIMainWindow;
class UIChatRoom;
class UITags;
class UIPersonWnd;
class UINoticeWnd;
class UICourseWnd;
class UILessonList;

class UIWindowSet : public QWidget
{
	Q_OBJECT

public:
	enum BTN_TYPE {
		NoticeWnd,
		CourseWnd,
		PersonWnd,
		LessonWnd
	};

	UIWindowSet(QWidget *parent = 0);
	~UIWindowSet();

	// 鼠标按下拖动的操作
	QPoint							m_startPos;
	QPoint							m_clickPos;
	QPoint							m_WndCurPos;

	QString							m_studentID;	//	学生自己的ID
	QString							m_accid;		//  云信ID

	QString							m_CameraPath;	// 摄像头直播路径
	QString							m_BoardPath;	// 白板直播路径

	HWND							m_hBoardWnd;	// 白板窗口句柄
	HWND							m_hCameraWnd;	// 摄像头窗口句柄
	bool							m_EnvironmentalTyle; // 环境变量
private:
	bool							m_bInitLive;	// 初始化直播窗口
	Ui::UIWindowSet ui;
	QNetworkReply*					reply;
	QNetworkAccessManager			manager;
	UIMainWindow*					m_parent;
	QSpacerItem*				 	m_spacer;
	std::vector<UITags*>			m_vecTags;			// 窗口标签集
	std::vector<UIChatRoom*>		m_vecChatRoom;		// 所有的辅导班聊天窗
	QString							m_Token;			// token
	
	QMap<QString, UITags*>			m_mapTags;			// 标签map
	QMap<QString, UIChatRoom*>		m_mapChatRoom;		// 聊天窗口map

	UITags*							m_curTags;			// 当前标签
	UIChatRoom*						m_curChatRoom;		// 当前聊天窗

	UINoticeWnd*					m_NoticeWnd;		// 公告窗口
	UICourseWnd*					m_CourseWnd;		// 辅导班简介
	UIPersonWnd*					m_PersonWnd;		// 成员列表
	UILessonList*					m_LessonWnd;		// 课程列表
	QTimer*							m_LiveTimer;		// 直播定时器
signals:
	void sig_Modle(bool bModle);

private slots :
	void MaxDialog();							// 最大化
	void MinDialog();							// 最小化
	void CloseDialog();							// 关闭对话框
	void clickTag(UITags*);						// 点击标签
	void ReturnDialog();						// 重新登录
	void clickNotice();							// 公告
	void clickCourse();							// 辅导班详情
	void clickPerson();							// 成员
	void clickLesson();							// 课程列表
	void clickChange(bool);						// 改变模式
	void slots_Modle(bool bModle);				// 改变模式
	void slot_onTimeout();						// 时间

protected:
	virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result); // 添加caption
	virtual void resizeEvent(QResizeEvent *);											// 设置窗口圆角
	virtual void paintEvent(QPaintEvent *event);
	virtual bool eventFilter(QObject *target, QEvent *event);

private:
	void titleButtonWidth();
	void AgainSelectTag();
	bool IsHasTag(QString chatID, QString status);		// 判断当前是否有此标签
	UIChatRoom* IsHasRoom(QString chatID);				// 判断当前是否有此聊天窗	
	void InitBoardView();								// 初始化白板播放器
	void PlayLive(QString sBoard, QString sCamera);		// 播放直播
public:
	void setMainWindow(UIMainWindow* parent);	// 设置窗口
	void setStudent(QString id);				// 学生ID
	UITags* AddTag(QString chatID, QString name, QString ID, bool sel, UIChatRoom* room, QString status);		// 添加标签窗口, 参数sel是否选中此标签
	void DeleleTag(UITags* tag);				// 关闭Tag
	void SetToken(QString token);
	void AddChatRoom(QString chatID, QString courseid, QString teacherid, QString token, QString studentName, std::string strCurAudioPath, QString name, int UnreadCount, QString status);// 创建聊天窗
	bool ReceiverMsg(nim::IMMessage* pIMsg);				// 接收消息
	void ReceiverChatMsg(nim::IMMessage* pIMsg);			// 接收消息
	void ReceiverRecordMsg(nim::QueryMsglogResult* pIMsg);  // 接收历史消息
	void OnStopPlayAudio(std::string sid, char* msgid);		// 语音播放结束
	void ReceiverLoginMsg(nim::LoginRes* pLogMsg);			// 返回登录结果
	void ReceiverMemberMsg(std::string sid, std::list<nim::TeamMemberProperty>* pMemberMsg);	// 返回成员
	void SendStatus(nim::SendMessageArc* arcNew);			// 接收消息状态
	void SendAudio(std::string sid, std::string msgid, std::string path, long size, int dur, std::string fileEx);		// 语音完成并发送
	QPixmap StudentPhotoPixmap();							// 学生头像
	bool IsCaptrueAudio();									// 当前是否有录制语音
	void QueryNotice();										// 查询当前公告
	void returnNotice();									// 返回公告
	void QueryCourse();										// 查询辅导简介
	void returnCourse();									// 返回简介
	void QueryPerson();										// 查询成员
	void QueryLesson();										// 查询课程
	void returnLesson();									// 返回课程
	void setAccid(QString accid);							// 设置ID
	void QueryLiveInfo();									// 查询直播信息地址
	void returnLiveInfo();									// 返回直播地址
	void setBoardHwnd(HWND hwnd);							// 设置白板窗口句柄
	void setCameraHwnd(HWND hwnd);							// 设置摄像头窗口句柄
	bool IsHasFoucs(BTN_TYPE type);						// 当前点击的点是否在控件内
	QPushButton*	GetNoticeBtn();
	QPushButton*	GetLessonBtn();
	QPushButton*	GetPersonBtn();
	QPushButton*	GetCourseBtn();
	void ChangeBtnStyle(bool bLive);						// 当前模式
	void SetEnvironmental(bool bType);						// 环境变量
	void ReceiverAudioStatus(std::string sid, char* msgid, bool bSuc=true); // 下载语音状态消息
};

#endif // UIWINDOWSET_H
