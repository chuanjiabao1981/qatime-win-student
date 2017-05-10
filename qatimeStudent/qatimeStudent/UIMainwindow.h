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

//---����
#include "assert.h"
#include <string>
#include "nim_cpp_api.h"

// ����¼����ɵĲ���
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
	QString							m_studentID;			// ѧ��ID
	QPixmap							m_studentPix;			// ѧ��ͷ��
	QString							m_studentName;			// ѧ������

	QString							m_accid;				// �����Լ���ID
	QString							m_token;				// ��������
	std::string						m_AudioPath;			// ������Ϣ·��
	UIWindowSet*					m_WindowSet;			// ���ڼ�
	UIAuxiliaryWnd*					m_AuxiliaryWnd;			// �����ര��		
	HWND							m_hBoardWnd;			// �װ崰��
	HWND							m_hCameraWnd;			// ����ͷ����
	QTimer*							m_BoardTimer;			// �װ���ʱ
	QTimer*							m_CameraTimer;			// ����ͷ��ʱ
	bool						    m_EnvironmentalTyle;	// ��������
protected slots:
	void slot_BoardTimeout();
	void slot_CameraTimeout();
public:
	void	setLoginWindow(LoginWindow* parent);			// ��¼����
	void	setRemeberToken(const QString &token);			// ����token
	void	ShowAuxiliary();								// ��ѯѧ����������Ϣ
	void	AuxiliaryRequestFinished();						// ������http����
	void    ShowLesson();									// ��ѯ���տγ���Ϣ
	void	LessonRequestFinished();						// ���տγ�http����
	void	ShowOneToOneAuxiliary();						// ��ѯһ��һ������Ϣ
	void	OneToOneAuxiliaryRequestFinished();				// һ��һ����http����
	void	setSudentInfo(QJsonObject &data);				// ѧ����Ϣ
	void	setAutoSudentInfo(QString teacherID, QString teacherName, QString teacherUrl, QString accid, QString token); // �Զ���¼��ѧ����Ϣ
	void    setVersion(QString version);
	QPixmap StudentPhotoPixmap();							// ����ͷ��
	void	setNetworkPic(const QString &szUrl);			// �������ϻ�ȡͷ��
	void    returnClick();									// ���µ�¼
	void	ShowCourse();									// ��ʾ������
	void    CreateRoom(QString chatID, QString courseID, QString teacherID, QString token, QString studentName, std::string audioPath, QString courseName, int UnreadCount, QString status);		// ����������
	void    CloseDialog();

	/*������Ź���*/
	void    InitAudio();									// ��ʼ������
	void    RequestKey();									// ����Key
	void    returnKey();									// ����Key
	void	setKeyAndLogin(QString key);					// ��ȡKey							
	static void OnLoginCallback(const nim::LoginRes& login_res, const void* user_data);

	/**
	* ֹͣ����������Ϣ�Ļص�
	* @param[in] code ������
	* @param[in] file_path ������Ϣ�ļ�·��
	* @param[in] sid �Ựid
	* @param[in] cid ��Ϣid
	* @return void �޷���ֵ
	*/
	static void OnStopAudioCallback(int code, const char* file_path, const char* sid, const char* cid);
	void SetEnvironmental(bool bType);	// ���û���
protected:
	virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result); // ���caption
};

#endif // UIMAINWINDOW_H
