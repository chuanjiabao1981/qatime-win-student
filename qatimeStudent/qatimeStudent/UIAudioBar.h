#ifndef UIAUDIOBAR_H
#define UIAUDIOBAR_H

#include <QWidget>
#include "ui_UIAudioBar.h"
#include "UIChatRoom.h"

class UIChatRoom;
class UIAudioBar : public QWidget
{
	Q_OBJECT

public:
	UIAudioBar(QWidget *parent = 0);
	~UIAudioBar();

private:
	Ui::UIAudioBar ui;
	UIChatRoom*			m_parent;
	QTimer*				m_timer;	// ������ʱ��

	int					m_iSec;		// ����
	bool				m_bSend;	// �Ƿ���

	QTimer*				m_FailTimer;// ��ʱ5����ʧ��
	bool				m_bCapturing;	//�Ƿ���¼����
private slots:
	void clickSend();									// ��������
	void clickCancel();									// ȡ������
	void slot_onTimeout();								// ������ʱ��
	void slot_onFailTimeout();							// ʧ��

public:
	void setMainWindow(UIChatRoom* parent);
	void CaptureAudio();								// ��ʼ�ɼ�����
	bool IsSend();										// �Ƿ���
	void MonitorFail();									// ����Ƿ�ʧ��
	bool IsCapturing();									// �Ƿ���¼���ɼ��� false��¼���� true��δ¼��
};

#endif // UIAUDIOBAR_H
