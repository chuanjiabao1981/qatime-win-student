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
//IM SDK接口定义头文件
#include "nim_cpp_api.h"
#include "nim_cpp_client.h"
//#include "nim_tools_http_cpp.h"

//前置声明，依次为收到在线消息通知函数、发送消息结果通知函数。
void CallbackReceiveMsg(const nim::IMMessage& msg);
void CallbackSendMsgArc(const nim::SendMessageArc& arc);

class UIMainWindow;
class UIChatRoom;
class UITags;
class UIPersonWnd;
class UINoticeWnd;
class UICourseWnd;
class UILessonList;

//互动直播
class Palette;
class UICamera1v1;
class UICameraS1v1;
class UIVideoChange1v1;
class UIAudioChange1v1;
class UIAudioOutChange1v1;
class UIWhiteBoardTool;
class UIVideo1v1;
class UIAppWnd;
class UIAppWndTool;
class UI1v1;

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

	/***************************互动直播*****************************/
	UI1v1*							m_Ui1v1;
	QTimer*							m_timer;			//轮询1对1互动直播定时器
	QString							m_course_id1v1;		//课程ID
	QString							m_status;			//1v1直播状态
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

	void status1v1();							// 获取1v1直播状态

//	void slot_refreshWnd();									// 刷新窗口

protected:
	virtual void paintEvent(QPaintEvent *event);
	virtual bool eventFilter(QObject *target, QEvent *event);
	virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);
private:
	void titleButtonWidth();
	void AgainSelectTag();
	bool IsHasTag(QString chatID, QString status);		// 判断当前是否有此标签
	UIChatRoom* IsHasRoom(QString chatID);				// 判断当前是否有此聊天窗	
	void InitBoardView();								// 初始化白板播放器
	void PlayLive(QString sBoard, QString sCamera);		// 播放直播

	void init1v1();
	void init1v1Timer();
public:
	void setMainWindow(UIMainWindow* parent);	// 设置窗口
	void setStudent(QString id);				// 学生ID
	UITags* AddTag(QString chatID, QString name, QString ID, bool sel, UIChatRoom* room, QString status, bool b1v1Lesson);		// 添加标签窗口, 参数sel是否选中此标签
	void DeleleTag(UITags* tag);				// 关闭Tag
	void SetToken(QString token);
	void AddChatRoom(QString chatID, QString courseid, QString teacherid, QString token, QString studentName, std::string strCurAudioPath, QString name, int UnreadCount, QString status, bool b1v1Lesson);// 创建聊天窗
	bool ReceiverMsg(nim::IMMessage* pIMsg);				// 接收消息
	void ReceiverChatMsg(nim::IMMessage* pIMsg);			// 接收消息
	void ReceiverRecordMsg(nim::QueryMsglogResult* pIMsg);  // 接收历史消息
	void OnStopPlayAudio(std::string sid, char* msgid);		// 语音播放结束
	void ReceiverLoginMsg(nim::LoginRes pLogMsg);			// 返回登录结果
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
	void ReceiverAudioStatus(std::string sid, char* msgid, bool bSuc=true); // 下载语音状态消息

	// 云信聊天
	void	initCallBack();	 
	HWND	GetParentWnd();
	static void QueryFirstMsgOnlineCb(nim::NIMResCode code, const std::string& id, nim::NIMSessionType type, const nim::QueryMsglogResult& result);	// 第一次请求
	static void QueryMsgOnlineCb(nim::NIMResCode code, const std::string& id, nim::NIMSessionType type, const nim::QueryMsglogResult& result);		// 正常历史记录请求
	static void OnGetTeamMemberCallback(const std::string& tid, int count, const std::list<nim::TeamMemberProperty>& team_member_info_list);		// 获取成员回调

	/***************************互动直播*****************************/
	void	OpenCourse(QString chatID, QString courseid, QString teacherid, QString token, QString studentName,
		std::string strCurAudioPath, QString courseName, int UnreadCount, QString status,bool b1v1Lesson);// 打开辅导班
	void	OpenCourse1v1(QString chatID, QString courseid, QString teacherid, QString token, QString studentName,
		std::string strCurAudioPath, QString courseName, int UnreadCount, QString status,bool b1v1Lesson);// 打开互动直播

	void start1v1Status(int msec);		//开始轮询1v1直播状态
	void stop1v1Status();				//停止轮询1v1直播状态
};

#endif // UIWINDOWSET_H
