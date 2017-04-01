#ifndef UITalkRecord_H
#define UITalkRecord_H

#include <QWidget>
#include "ui_UITalkRecord.h"
#include <QScrollArea>
#include "UIBtnPix.h"
#include "animatedtextbrowserA.h"
#include "UIChatRoom.h"
#include "UIBtnAudio.h"

#pragma execution_character_set("utf-8")

class UIChatRoom;
class AnimatedTextBrowserA;
class UITalkRecord : public QWidget
{
	Q_OBJECT

public:
	UITalkRecord(QWidget *parent = 0);
	~UITalkRecord();

protected:
	void mousePressEvent(QMouseEvent *e);

signals:
	void colseCalendar();
	void colseBrow();

protected Q_SLOTS:
	void slot_btnclicked(QString, QPixmap, bool);
	void slot_faildclicked(CBtnPix*);
	void slot_scrollDown();
	void slot_Audioclicked(std::string, std::string, std::string, bool);
	void slot_Btnclicked();

private:
	Ui::UITalkRecord ui;
	QSpacerItem*		m_spacer;
	QScrollArea*		m_view;
	QWidget*			m_mainView;
	QVBoxLayout*		m_VerAll;
	UIChatRoom*			m_parent;

	std::vector<CBtnPix*>	m_vecImgProcess;	//������е�ͼƬ�ؼ�
	std::vector<CBtnAudio*> m_vecAudio;			//���е�audio��ť
public:
	QVBoxLayout*		m_Ver;

	QHBoxLayout*		m_btnLayout;
	QPushButton*		m_btn;
private:
	bool isDigitStr(QString str);
	void style(QScrollArea *style);
public:
	void InsertChat(QPixmap* head, QString name, QString time, QString text, bool bTeacher=true);  // bTeacher ����������ɫ
	void InsertAudioChat(QPixmap* pixmap, QString name, QString time, QString text, std::string path, std::string sid, std::string msgid, bool bTeacher); // ��������
	void InsertNotice(QString text);												// ��Ϣ֪ͨ
	void InsertPic(QPixmap* head, QString name, QString time, QString URL, QString sMsgID="", bool bTeacher = true);	// ����ͼƬ
	void InsertEmoji(QPixmap* head, QString name, QString time, QString text, bool bTeacher=true);		// �������
	void ParseFace(AnimatedTextBrowserA* Anim, QString qContect);					// ��������
	QString BuildFaceToUrl(QString qFace);											// ��������url
	void stringToHtml(QString &str, QColor crl);									// QStringתhtlm����ɫ
	void ScrollDown();																// ��������ײ�
	void PicProcess(double iProcess);												// ����ͼƬ����
	void SendStatus(QString msgid);													// ����״̬
	void setMainWindow(UIChatRoom* parent);											// ���ø�����
	void ClearAll();																// ���
	void DelaySrcoll();
	void stopAudio(char* msgid);													// ֹͣ����
	void InsertBtn();
};

#endif // UITalkRecord_H
