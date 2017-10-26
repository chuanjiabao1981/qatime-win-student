#ifndef UITALK_H
#define UITALK_H

#include <QWidget>
#include "ui_UITalk.h"
#include <QScrollArea>
#include "UIBtnPix.h"
#include "UIBtnAudio.h"
#include "animatedtextbrowserA.h"
#include "UIChatRoom.h"

#pragma execution_character_set("utf-8")

class UIChatRoom;
class AnimatedTextBrowserA;
class UITalk : public QWidget
{
	Q_OBJECT

public:
	UITalk(QWidget *parent = 0);
	~UITalk();

protected Q_SLOTS:
	void slot_btnclicked(QString, QPixmap, bool);
	void slot_faildclicked(CBtnPix*);
	void slot_scrollDown();
	void slot_Audioclicked(std::string, std::string, std::string, bool);
	void slot_AudioLoadFail(nim::IMMessage);
	void slot_ScrollDownBottom(int, int);
	void slot_Delay();
private:
	Ui::UITalk ui;
	QSpacerItem*		m_spacer;
	QScrollArea*		m_view;
	QWidget*			m_mainView;
	QVBoxLayout*		m_Ver;
	QVBoxLayout*		m_VerAll;
	UIChatRoom*			m_parent;
	std::vector<CBtnPix*>	m_vecImgProcess;	// ������е�ͼƬ�ؼ�

	std::vector<CBtnAudio*> m_vecAudio;			// ���е�audio��ť
	QScrollBar*			m_bar;					// ������

	int					m_maxValue;				// ������߶�
private:
	bool isDigitStr(QString str);
	void style(QScrollArea *style);
public:
	void InsertChat(QPixmap* head, QString name, QString time, QString text, bool bTeacher=true);  // bTeacher ����������ɫ
	void InsertAudioChat(QPixmap* head, QString name, QString time, QString text, std::string path,
		std::string sid, std::string msgid, nim::IMMessage msg, bool bTeacher = true, bool bRead = false);  // ������������
	void InsertNotice(QString text);												// ��Ϣ֪ͨ
	void InsertOneNotice(QString mText);											// ���뵥�е���Ϣ��Ӧ��ֱ�������ر�
	void InsertNewNotice(QString name, QString text);								// ���빫��
	void InsertPic(QPixmap* head, QString name, QString time, QString URL, QString sMsgID="", bool bTeacher = true);	// ����ͼƬ
	void InsertPicUrl(QPixmap* head, QString name, QString time, QString URL, QString sMsgID = "", bool bTeacher = true);	// ����ͼƬ
	void InsertEmoji(QPixmap* head, QString name, QString time, QString text, bool bTeacher=true);		// �������
	void ParseFace(AnimatedTextBrowserA* Anim, QString qContect);					// ��������
	QString BuildFaceToUrl(QString qFace);											// ��������url
	void stringToHtml(QString &str, QColor crl);									// QStringתhtlm����ɫ
	void ScrollDown();																// ��������ײ�
	void PicProcess(double iProcess);												// ����ͼƬ����
	void SendStatus(QString msgid);													// ����״̬
	void setMainWindow(UIChatRoom* parent);											// ���ø�����
	void ClearAll();																// ���
	void stopAudio(char* msgid);													// ֹͣ��������
	void SetAudioStatus(char* msgid, bool bSuc = true);								// ������������״̬
	void InsertSpacer();
};

#endif // UITALK_H
