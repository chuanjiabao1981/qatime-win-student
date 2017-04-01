/*
*	Author       hzzhuling15
*	Date         2015/09/11
*	Copyright    Hangzhou, Netease Inc.
*	Brief        
*   Notice       
*/
#ifndef _LS_UI_BUTTON_AUDIO_H_
#define _LS_UI_BUTTON_AUDIO_H_
#include <QtWidgets/QPushButton>
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
#pragma execution_character_set("utf-8")

class CBtnAudio : public QPushButton
{
	Q_OBJECT

public:
	CBtnAudio(std::string path, std::string sid, std::string msgid, QWidget *parent = 0, bool bRead = false); //bRead Ĭ��Ϊδ��
	~CBtnAudio();

Q_SIGNALS:
	void sig_Audioclicked(std::string, std::string, std::string, bool);
	void sig_AudioLoadFail(nim::IMMessage);

public Q_SLOTS:
	void onClicked(bool);
	void slot_onCountTimeout();		// ��ʱ������
	void slot_onDownTimeout();		// ������Դ

protected:

private:
	std::string m_path;			// ����·��
	std::string m_sid;			// �ỰID
	std::string m_msgid;		// ��ϢID
	bool m_bIsPlay;				// �����Ƿ��ڲ���
	bool m_bDownEnd;			// �����Ƿ��������
	bool m_bRead;				// �����Ѷ�δ��
	int  m_ImageCount;			// ����ͼƬ��˳��

	QTimer*	m_Timer;			// ������ʱ��
	QTimer* m_TimerDown;		// ����������ʱ��
	int			m_iCount;		// ѭ��60�� Ҳ����3�뻹���ز��꣬����������
	nim::IMMessage m_msg;		// ��Ϣ�ṹ
	bool m_LoadStatus;			// ����״̬

public:
	QString GetMsgID();							// ��ȡ��Ϣ
	void stopPlay();							// ֹͣ����
	bool GetPlayStatus();						// ��ȡ����״̬
	void setMsg(nim::IMMessage msg);
	void LoadFail();							// ����ʧ��
	void LoadSuc();								// ���سɹ�

};

#endif // _LS_MEDIACAPTURE_UI_BUTTON_H_