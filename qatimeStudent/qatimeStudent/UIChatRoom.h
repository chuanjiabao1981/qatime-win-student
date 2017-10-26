#ifndef UICHATROOM_H
#define UICHATROOM_H

#include <QWidget>
#include "ui_UIChatRoom.h"
#include "myemotionwidget.h"
#include <QLabel>
#include "uimainwindow.h"
#include "UITalk.h"
#include "UITalkRecord.h"
#include <vector>
#include "UIAudioBar.h"
#include "UIWindowSet.h"
#include "UIWorkThread.h"
#include "calendar/DefDateTimeEdit.h"

#include "assert.h"
#include <string>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>

//---����
#include "nim_cpp_api.h"
#include "nim_cpp_client.h"
#include "nim_cpp_tool.h"
#include "nim_audio_cpp.h"

class UIWindowSet;
class UITalk;
class UITalkRecord;
class UIAudioBar;

struct MyImageInfo
{
	QPixmap			PhotoImg;		//����ͷ��
	QString			name;			//�ǳ�
	QString			time;			//ʱ��
	QString			ReceiverImg;	//���յ�ͼƬ·��
	std::string		chatID;			//�Ựid
};

class UIChatRoom : public QWidget
{
	Q_OBJECT

public:
	UIChatRoom(QWidget *parent = 0);
	~UIChatRoom();
	void stringToHtmlFilter(QString &str);					//QStringתhtlm
	void stringToHtml(QString &str, QColor crl);			//QStringתhtlm����ɫ
	void stringToHtmlPos(QString &str, QColor crl);			//QStringתhtml����ɫ��λ������
	void imgPathToHtml(QString &path);						//edit ����ͼƬ
	void setMainWindow(UIWindowSet* parent);
public slots:
	void setBrow(QString path);

private:
	Ui::UIChatRoom ui;
	Worker*							m_pWorker;
	static UIChatRoom*				m_pThis;
	QNetworkAccessManager			manager;
	QNetworkReply*					reply;
	UITalk*							m_uitalk;			// ���촰���Զ�������ؼ�
	UITalkRecord*					m_uitalkRecord;		// �����¼

	//����
	DefDateTimeEdit*				m_defDateTimeEdit;
	//�����	
	MyEmotionWidget*				m_smallEmotionWidget;
	MyEmotionWidget*				m_normalEmotionWidget;
	QLabel*						    m_lableTitle;
	QStringList						m_borw;//��ǰѡ�еı���
	bool							m_isBorw;//�Ƿ��б���
	std::string						m_CurChatID;		// ��ǰ�Ự����ID 
	QString							m_CurCourseID;		// ��ǰ������ID
	QString							m_CurTeacherID;		// ��ʦ����ID
	QString							m_accid;			// �����û�ID
	QString							m_token;			// �����û�����
	QString							m_appKey;			// ����key
	bool							m_bLogin;			// �Ƿ��¼
	bool                            m_switchTime;		// �л�ʱ��
	QDateTime						m_RecordTime;		// ��Ϣ��¼ʱ�䣨����ʷ��¼ʱ�����Աȣ�������ʾ���죩
	QDateTime						m_ReceiveTime;		// �����¼ʱ�䣨��֮ǰ��¼���Աȣ�������ʾ���죩
	/************************************************************************/
	/*								�����¼����							*/
	long long						m_farst_msg_time;	// ��Զ����Ϣʱ��
	int								kMsgLogNumberShow;	// һ�λ�ȡ������
	/************************************************************************/
	QTextCursor*					m_TextCursor;
	QMap<QString, QString>			m_StudentInfo;		// key ��accid 
	std::string						m_AddMemberID;		// �¼���ĳ�ԱID
	UIWindowSet*					m_parent;			// �����ڶ���
	int								m_studentNum;		// ��ǰѧ������
	int								m_proclamationHeight;// ����߶�
	std::vector<MyImageInfo>		m_VerReceiveImg;	// ���յ�ͼƬ��Ϣ����
	QTimer*							m_LoadImgTimer;		// ��ʱ������ͼƬ
	int								m_drawingWidth;		// �����ı�Ŀ��
	bool							m_bClickPic;		// ��ǰ�Ƿ���ͼƬ
	std::string						m_AudioPath;		// ������Ϣ·��
	int								m_LessonType;		// ��ǰ����ֱ������
public:
	/****************����������***************************************/
	UIAudioBar*						m_AudioBar;			// ��������������
	bool							m_bSendAudio;		// Ĭ�Ϸ�������

	bool							m_bPerson;			// �Ƿ��������Ա
	int								m_UnreadCount;		// δ����Ϣ��
	void initEmotion();
	QTimer*							m_DisSendMsgTimer;	// ����Ϣ��Ҫ2����
	bool							m_bCanSend;			// trueΪ�ɷ�����Ϣ
	bool							m_bMute;			// trueΪ������
	int								m_DisCount;			// 

	QTimer*							m_AudioBarTimer;	// ��������ʾ��ʱ��
	
public:
	QString							m_StudentName;		// ѧ������
protected:
	virtual bool eventFilter(QObject *watched, QEvent *event);
	void mousePressEvent(QMouseEvent *event);
signals:
	void sig_StartLoading();
public slots :
	void clickTalk();									// ���������
	void clickStudentList();							// ����ѧ���б�
	void clickProclamation();							// ��������
	void clickCleanText();								// ����
	void clickBrow();									// ���鰴ť
	void clickNotes();									// ��Ϣ��¼
	void closeNotes();									// �ر���Ϣ��¼
	void clickSendMseeage();							// ������Ϣ
	void chickChoseTime();								// ѡ��ʱ��ۿ���Ϣ��¼
	void choseTime(QDate);								// ���ĳһʱ��Ĳۺ���
	void forwardTime();									// �����ǰһ��Ĳۺ���
	void afterTime();									// �������һ��Ĳۺ���
	void announce();									// ������������桿��ť
	void putTalk();										// �������������ť
	void putTalkCancel();								// �����ȡ����������ť
	void proclamationTextChage();						// �����༭�������������÷�����ť�Ƿ���Ա������
	void colseCalendar();								// �ر������ۺ���
	void colseBrow();									// �رձ���ۺ���
	void calendaCurrentPageChanged(int year, int month);// ������ǰҳ�汻�ı�
	void clickPic();									// ѡ��ͼƬ
	void LoadImgTimeout();								// ����ͼƬ��ʱ��
	void DisSendMsgTimeout();							// ��ֹ������Ϣ��2����
	void clickAudio();									// �������
	void AudioBarTimer();								// �ı䴰�ں�����������ʾλ��
	void slot_CalendarClick(QDate date);				// �������
private:
	void		PackageMsg(nim::IMMessage &msg);
	void        style(QTextBrowser *style,QTextEdit* pEidt=NULL);

	/**
	* Ⱥ�¼��ص�
	* @param[in] rescode ������
	* @param[in] notification_id Ⱥ�¼�����
	* @param[in] tid Ⱥid
	* @param[in] result �¼���Ϣ���ݣ� Json����ת���ɵ��ַ���
	* @return void	�޷���ֵ
	*/
	static void OnTeamEventCallback(const nim::TeamEvent& result);

	static void OnGetTeamInfoCb(const nim::TeamEvent& team_event);

	QString		UserAppdataPath();
	void		GenerateUploadImage(const std::wstring src, const std::wstring dest);
	std::string GetFileMD5(QString path);
	long		GetFileSize(QString path);
	void		RequestMember();	// �ӷ������ϻ�ȡ��Ա��Ϣ
	void		Request1v1Member();	// �ӷ������ϻ�ȡ1��1��Ա��Ϣ
	void        returnMember();		// ������Ա��Ϣ

public:
	void	SendImage(const std::wstring src, QString &filename, QString msgid="");
	bool	ReceiverMsg(nim::IMMessage* pMsg);					// ���շ��������͹�������Ϣ
	void	ReceiverRecordMsg(nim::QueryMsglogResult* pMsg);	// ������ʷ��Ϣ��¼
	void	ReceiverLoginMsg(nim::LoginRes pRes);				// ���յ�¼���
	void	ReceiverMemberMsg(std::list<nim::TeamMemberProperty>* pMemberMsg); //����Ⱥ��Ա��Ϣ
	void	setCurChatID(QString chatID, QString courseid, QString teacherid, QString studentName, QString accid, int UnreadCount, bool b1v1 = false);		// ���õ�ǰ���ڻỰID,���ڽ�����Ϣʱ�Ƚ�
	bool	IsLogin();											// �Ƿ��¼
	bool	IsCurChatRoom(QString chatID);						// �Ƿ��ǵ�ǰ�ỰID

	void	ShowMsgs(const std::vector<nim::IMMessage> &msg);
	void	ShowMsg(nim::IMMessage pMsg);
	void	ParseFace(QString qContect, QString name, QString time);						// �������յ�����Ϣ
	bool	IsHasFace(QString qContect);						// �ж��Ƿ��б���
	QString BuildFaceToUrl(QString qFace);						// ͨ�����鷵��url·�������磺����[em_1]����./images/em_1.gif��
	void	setAdaptHeight(int iHeight);						// ����Ӧ�߶�
	bool	IsFous();
	void	UpLoadPicProcess(double iProcess);					// �ϴ�ͼƬ����
	void	SendStatus(nim::SendMessageArc* arcNew);			// ������Ϣ����״̬
	bool    IsClickPic();										// ��ǰ�Ƿ���ͼƬ
	void    OnPlayAudio(std::string path, std::string sid, std::string msgid, bool isPlay);
	void	OnStopPlayAudio(char* msgid);
	std::string    GetCurChatID();								// ��ǰ�ỰID
	QString	GetCourseID();										// ��ǰ������ID
	QString GetTeacherID();										// ��ǰ��ʦID
	QString GetAccid();											// ��ȡ�û�ID
	void    SetCurAudioPath(std::string path);					// ���õ�ǰ����·��
	void    InitAudioCallBack();								// ¼��������ʼ���ص�
	void    finishAudio();										// ���¼��,Ĭ�Ϸ�������
	void    AddAudioMsg(nim::IMMessage pMsg, nim::IMAudio audio);	// �����������������Ϣ
	bool	IsCaptrueAudio();									// ��ǰ�Ƿ���¼������
	void    SendAudio(QString msgid, QString path, long size, int audio_duration, std::string file_ex);		// ��������

	/**
	* ֹͣ����������Ϣ�Ļص�
	* @param[in] code ������
	* @param[in] file_path ������Ϣ�ļ�·��
	* @param[in] sid �Ựid
	* @param[in] cid ��Ϣid
	* @return void �޷���ֵ
	*/
	static void OnStopAudioCallback(int code, const char* file_path, const char* sid, const char* cid);
// 	static void OnStartCaptureCallback(int code);				// ��ʼ�ɼ��ص�
// 	static void OnStopCaptureCallback(int rescode, const char* sid, const char* cid, const char* file_path, const char* file_ext, long file_size, int audio_duration);				// ֹͣ�ɼ��ص�
// 	static void OnCancelCaptureCallback(int code);				// ȡ���ɼ��ص�

	std::vector<personListBuddy*>  GetBuddy();
	bool		IsPerson();	
	void		ResultMsg();									// ��һ�ε������
	void        ShowChatMsg(nim::IMMessage pMsg);				// ��ʾ��һ�ν����������Ϣ
	void		RecordMoved();									// ��Ϣ��¼������
	void        setEditFocus();
	void        SetAudioStatus(char* msgid, bool bSuc = true);	// ������Ϣ״̬
public slots:
	void chickChage(int, QString, QString);
	bool AddStudent(QString iconUrl, QString name, QString accid);		//��ӳ�Ա
	void AddStudentNumbers(int num);									//��ӳ�Ա����
	void AddAnnouncement(QString announcement, QString time);			//��ӹ���
	void QueryGroup();													//��ѯȺ��Ա��Ϣ
	void QueryRecord(QString dtstr);
	void stepDays(QDateTime date);										//��ʷ��¼����
	void stepMsgDays(QDateTime dateTime);								//�����¼����
	void clearAll();													//��������¼�����桢Ⱥ��Ա
	void OnSendAnnouncements(QString Announcements);					//����Ⱥ����
	void ReturnAnnouncements();											//���ع���״̬
	void RequestError(QJsonObject& error);								//����
	void QueryMember();
	void returnAllMember();
private:
	QToolButton* pPreMonthButton1;
	QToolButton* pPreMonthButton;
	bool		 m_bZoom;	// ����ֱ���������ֺ��Ƿ����Ź� 
};

#endif // UICHATROOM_H
