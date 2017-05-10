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

//---����
#include "nim_client_def.h"
#include "assert.h"
#include <string>
//IM SDK�ӿڶ���ͷ�ļ�
#include "nim_cpp_api.h"
#include "nim_cpp_client.h"
//#include "nim_tools_http_cpp.h"

//ǰ������������Ϊ�յ�������Ϣ֪ͨ������������Ϣ���֪ͨ������
void CallbackReceiveMsg(const nim::IMMessage& msg);
void CallbackSendMsgArc(const nim::SendMessageArc& arc);

class UIMainWindow;
class UIChatRoom;
class UITags;
class UIPersonWnd;
class UINoticeWnd;
class UICourseWnd;
class UILessonList;

//����ֱ��
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

	// ��갴���϶��Ĳ���
	QPoint							m_startPos;
	QPoint							m_clickPos;
	QPoint							m_WndCurPos;

	QString							m_studentID;	//	ѧ���Լ���ID
	QString							m_accid;		//  ����ID

	QString							m_CameraPath;	// ����ͷֱ��·��
	QString							m_BoardPath;	// �װ�ֱ��·��

	HWND							m_hBoardWnd;	// �װ崰�ھ��
	HWND							m_hCameraWnd;	// ����ͷ���ھ��
	bool							m_EnvironmentalTyle; // ��������
private:
	bool							m_bInitLive;	// ��ʼ��ֱ������
	Ui::UIWindowSet ui;
	QNetworkReply*					reply;
	QNetworkAccessManager			manager;
	UIMainWindow*					m_parent;
	QSpacerItem*				 	m_spacer;
	std::vector<UITags*>			m_vecTags;			// ���ڱ�ǩ��
	std::vector<UIChatRoom*>		m_vecChatRoom;		// ���еĸ��������촰
	QString							m_Token;			// token
	
	QMap<QString, UITags*>			m_mapTags;			// ��ǩmap
	QMap<QString, UIChatRoom*>		m_mapChatRoom;		// ���촰��map

	UITags*							m_curTags;			// ��ǰ��ǩ
	UIChatRoom*						m_curChatRoom;		// ��ǰ���촰

	UINoticeWnd*					m_NoticeWnd;		// ���洰��
	UICourseWnd*					m_CourseWnd;		// ��������
	UIPersonWnd*					m_PersonWnd;		// ��Ա�б�
	UILessonList*					m_LessonWnd;		// �γ��б�
	QTimer*							m_LiveTimer;		// ֱ����ʱ��

	/***************************����ֱ��*****************************/
	bool							m_bLiving1v1;			// 1��1ֱ����ʶ
	Palette*						mWhiteBoard;			// ����
	UICamera1v1*					m_Camera1v1Info;		// 1��1����ͷ
	UICameraS1v1*					m_CameraS1v1Info;		// 1��1��ʦ����ͷ
	UIVideoChange1v1*				m_VideoChangeInfo1v1;	// ����ͷѡ�񴰿�
	UIAudioChange1v1*				m_AudioChangeInfo1v1;	// ��˷�ѡ�񴰿�
	UIAudioOutChange1v1*			m_AudioOutChangeInfo1v1;// ������ѡ�񴰿�
	UIWhiteBoardTool*				m_WhiteBoardTool;		// �װ����
	UIVideo1v1*						m_VideoInfo1v1;			// 1v1ȫ������
signals:
	void sig_Modle(bool bModle);

private slots :
	void MaxDialog();							// ���
	void MinDialog();							// ��С��
	void CloseDialog();							// �رնԻ���
	void clickTag(UITags*);						// �����ǩ
	void ReturnDialog();						// ���µ�¼
	void clickNotice();							// ����
	void clickCourse();							// ����������
	void clickPerson();							// ��Ա
	void clickLesson();							// �γ��б�
	void clickChange(bool);						// �ı�ģʽ
	void slots_Modle(bool bModle);				// �ı�ģʽ
	void slot_onTimeout();						// ʱ��

	/*����ֱ��*/
	void joinRtsRoom(const std::string &);					// ����װ巿��
	void joinRoomSuccessfully(const std::string &, __int64, const std::string &);// ����װ巿��ɹ�
	//void joinVChatRoom();									// ��������Ƶ
	void joinVChatSuccessfully();							// ��������Ƶ����ɹ�
	void errorInfo(const QString &);						// ����ʧ�ܴ�����Ϣ
	void PicData(QString);									// �װ�����
	void setDeviceInfos(int);								// �豸����
	void clickVideo1v1Param();								// ����ͷ����
	void clickAudio1v1Param();								// ��˷����
	void clickAudioOut1v1Param();							// ����������
	void Audio1v1Status(int iStatus);						// �����ر���˷�
	void Video1v1Status(int iStatus);						// �����ر�����ͷ
	void AudioOut1v1Status(int);							// �����ر�������
	//void clickLive1v1();									// ����1v1ֱ��
	void selectColor(QColor&);								// ��ɫ��
	void returnClick();										// ������һ��
	void deleteClick();										// ��հװ�
	void laserClick();										// �����
	void drawClick();										// �滭��
	void rtsDataReceived(const std::string& data);			// ���հװ�����
	//void clickShapeScreen1v1();								// ���������Ļ
	//void slot_selectWnd(HWND);								// ѡ�񴰿�
	//void slot_refreshWnd();									// ˢ�´���
	//void slot_CustomVideoData(__int64, const char*, int, int, int);	// �����Զ�������

protected:
	virtual void paintEvent(QPaintEvent *event);
	virtual bool eventFilter(QObject *target, QEvent *event);
	virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);
private:
	void titleButtonWidth();
	void AgainSelectTag();
	bool IsHasTag(QString chatID, QString status);		// �жϵ�ǰ�Ƿ��д˱�ǩ
	UIChatRoom* IsHasRoom(QString chatID);				// �жϵ�ǰ�Ƿ��д����촰	
	void InitBoardView();								// ��ʼ���װ岥����
	void PlayLive(QString sBoard, QString sCamera);		// ����ֱ��
public:
	void setMainWindow(UIMainWindow* parent);	// ���ô���
	void setStudent(QString id);				// ѧ��ID
	UITags* AddTag(QString chatID, QString name, QString ID, bool sel, UIChatRoom* room, QString status);		// ��ӱ�ǩ����, ����sel�Ƿ�ѡ�д˱�ǩ
	void DeleleTag(UITags* tag);				// �ر�Tag
	void SetToken(QString token);
	void AddChatRoom(QString chatID, QString courseid, QString teacherid, QString token, QString studentName, std::string strCurAudioPath, QString name, int UnreadCount, QString status);// �������촰
	bool ReceiverMsg(nim::IMMessage* pIMsg);				// ������Ϣ
	void ReceiverChatMsg(nim::IMMessage* pIMsg);			// ������Ϣ
	void ReceiverRecordMsg(nim::QueryMsglogResult* pIMsg);  // ������ʷ��Ϣ
	void OnStopPlayAudio(std::string sid, char* msgid);		// �������Ž���
	void ReceiverLoginMsg(nim::LoginRes pLogMsg);			// ���ص�¼���
	void ReceiverMemberMsg(std::string sid, std::list<nim::TeamMemberProperty>* pMemberMsg);	// ���س�Ա
	void SendStatus(nim::SendMessageArc* arcNew);			// ������Ϣ״̬
	void SendAudio(std::string sid, std::string msgid, std::string path, long size, int dur, std::string fileEx);		// ������ɲ�����
	QPixmap StudentPhotoPixmap();							// ѧ��ͷ��
	bool IsCaptrueAudio();									// ��ǰ�Ƿ���¼������
	void QueryNotice();										// ��ѯ��ǰ����
	void returnNotice();									// ���ع���
	void QueryCourse();										// ��ѯ�������
	void returnCourse();									// ���ؼ��
	void QueryPerson();										// ��ѯ��Ա
	void QueryLesson();										// ��ѯ�γ�
	void returnLesson();									// ���ؿγ�
	void setAccid(QString accid);							// ����ID
	void QueryLiveInfo();									// ��ѯֱ����Ϣ��ַ
	void returnLiveInfo();									// ����ֱ����ַ
	void setBoardHwnd(HWND hwnd);							// ���ðװ崰�ھ��
	void setCameraHwnd(HWND hwnd);							// ��������ͷ���ھ��
	bool IsHasFoucs(BTN_TYPE type);						// ��ǰ����ĵ��Ƿ��ڿؼ���
	QPushButton*	GetNoticeBtn();
	QPushButton*	GetLessonBtn();
	QPushButton*	GetPersonBtn();
	QPushButton*	GetCourseBtn();
	void ChangeBtnStyle(bool bLive);						// ��ǰģʽ
	void SetEnvironmental(bool bType);						// ��������
	void ReceiverAudioStatus(std::string sid, char* msgid, bool bSuc=true); // ��������״̬��Ϣ

	// ��������
	void	initCallBack();	 
	HWND	GetParentWnd();
	static void QueryFirstMsgOnlineCb(nim::NIMResCode code, const std::string& id, nim::NIMSessionType type, const nim::QueryMsglogResult& result);	// ��һ������
	static void QueryMsgOnlineCb(nim::NIMResCode code, const std::string& id, nim::NIMSessionType type, const nim::QueryMsglogResult& result);		// ������ʷ��¼����
	static void OnGetTeamMemberCallback(const std::string& tid, int count, const std::list<nim::TeamMemberProperty>& team_member_info_list);		// ��ȡ��Ա�ص�

	/***************************����ֱ��*****************************/
	void	OpenCourse(QString chatID, QString courseid, QString teacherid, QString token, QString studentName,
		std::string strCurAudioPath, QString courseName, int UnreadCount, QString status,
		QString boardurl, QString cameraUrl, bool b1v1Lesson);// �򿪸�����
	void	OpenCourse1v1(QString chatID, QString courseid, QString teacherid, QString token, QString studentName,
		std::string strCurAudioPath, QString courseName, int UnreadCount, QString status,
		QString boardurl, QString cameraUrl, bool b1v1Lesson);// �򿪻���ֱ��
	void	createRtsRoom(const QString &roomName, const QString &roomInfo = "");// �����װ�����
	void	InitSetParamWnds();										  // ������ͷ��������˷�ȴ���
	void	initWhiteBoardWidget();									  // ��ʼ���װ�
	void	initConnection();										  // ��ʼ���װ�ӿ�
	void	setAudioChange1v1(QString path);						  // ����һ��һ��˷�
	void	setValueChange1v1(int iVolumn, bool capturn);			  // ������˷�����
	void	setVideoChange1v1(QString path);						  // ����һ��һ����ͷ
	//void	start1v1LiveStream();
	void	show1v1ErrorTip(QString sError);
};

#endif // UIWINDOWSET_H
