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

protected:
	virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result); // ����caption
	virtual void resizeEvent(QResizeEvent *);											// ���ô���Բ��
	virtual void paintEvent(QPaintEvent *event);
	virtual bool eventFilter(QObject *target, QEvent *event);

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
	UITags* AddTag(QString chatID, QString name, QString ID, bool sel, UIChatRoom* room, QString status);		// ���ӱ�ǩ����, ����sel�Ƿ�ѡ�д˱�ǩ
	void DeleleTag(UITags* tag);				// �ر�Tag
	void SetToken(QString token);
	void AddChatRoom(QString chatID, QString courseid, QString teacherid, QString token, QString studentName, std::string strCurAudioPath, QString name, int UnreadCount, QString status);// �������촰
	bool ReceiverMsg(nim::IMMessage* pIMsg);				// ������Ϣ
	void ReceiverChatMsg(nim::IMMessage* pIMsg);			// ������Ϣ
	void ReceiverRecordMsg(nim::QueryMsglogResult* pIMsg);  // ������ʷ��Ϣ
	void OnStopPlayAudio(std::string sid, char* msgid);		// �������Ž���
	void ReceiverLoginMsg(nim::LoginRes* pLogMsg);			// ���ص�¼���
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
};

#endif // UIWINDOWSET_H