#ifndef UITAGS_H
#define UITAGS_H

#include <QWidget>
#include "ui_UITags.h"
#include "UIWindowSet.h"
#include "UIChatRoom.h"

#pragma execution_character_set("utf-8")

class UIWindowSet;
class UIChatRoom;
class UITags : public QWidget
{
	Q_OBJECT

public:
	UITags(QWidget *parent = 0);
	~UITags();

	QString			m_name;
	QString			m_id;
	QString			m_chatID;
private:
	Ui::UITags ui;
	UIWindowSet*		m_parent;
	UIChatRoom*			m_room;
	bool				m_bSelect;			// ��ǰ�Ƿ�ѡ��
	QString				m_boardStream;		// �װ�ֱ������ַ
	QString				m_cameraStream;		// ����ͷֱ������ַ
	bool				m_bModle;			// ��ǰ����ģʽ

	bool				m_b1v1Lesson;
	bool				m_bLiving;
protected:
	virtual void paintEvent(QPaintEvent *event);
	virtual void mousePressEvent(QMouseEvent *e);       //--��갴���¼� 

signals:
	void clickTag(UITags* tag);

private slots :
	void CloseDialog();			// ���

public:
	void SetCourseNameAndID(QString sName, QString id, QString chatID);
	void SetMainWindow(UIWindowSet* parent);
	UIChatRoom* GetRoom();			// ��ȡ��ǰ�����촰��
	void SetRoom(UIChatRoom* room);	// ���ù��������촰��
	void setStyle(bool bSel);		// ѡ�б�ǩ��ʽ
	bool IsSelect();				// ��ǰ�Ƿ�ѡ��
	QString ChatID();
	QString CourseID();
	QString BoardStream();
	QString CameraStream();
	void setBoardStream(QString strBoard);			
	void setCameraStream(QString strCamera);
	void setModle(bool bModle);		// ����ģʽ trueΪֱ��״̬ falseΪ����״̬��Ĭ�϶�Ϊ����״̬
	bool IsModle();
	void set1v1Lesson(bool b1v1Lesson);
	bool Is1v1Lesson();
	void setLiving(bool bLiving);	// ��ǰֱ��״̬
	bool IsLiving();		
};

#endif // UITAGS_H
