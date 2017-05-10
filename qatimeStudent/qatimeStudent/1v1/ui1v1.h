#ifndef UI1V1_H
#define UI1V1_H

#include <QWidget>
#include "ui_ui1v1.h"

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

class UI1v1 : public QWidget
{
	Q_OBJECT

public:
	UI1v1(QWidget *parent = 0);
	~UI1v1();

	void	createRtsRoom(const QString &roomName, const QString &roomInfo = "");// �����װ�����
	void	InitSetParamWnds();										  // ������ͷ��������˷�ȴ���
	void	initWhiteBoardWidget();									  // ��ʼ���װ�
	void	initConnection();										  // ��ʼ���װ�ӿ�
	void	show1v1ErrorTip(QString sError);
	QLayout* chat1v1Widget();
	void	initDevice();

private slots:
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
	void selectColor(QColor&);								// ��ɫ��
	void returnClick();										// ������һ��
	void rtsDataReceived(const std::string& data);			// ���հװ�����

	void setVideoChange1v1(QString path);				    // ����һ��һ����ͷ
	void setAudioChange1v1(QString path);				    // ����һ��һ��˷�
	void setAudioOutChange1v1(QString path);				// ����һ��һ������
	void setValueChange1v1(int iVolumn, bool capturn);		// ������˷硢����������
private:
	Ui::UI1v1 ui;

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

	std::string						m_RoomName;				//
};

#endif // UI1V1_H
