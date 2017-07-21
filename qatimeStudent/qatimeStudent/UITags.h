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
	bool				m_bSelect;			// 当前是否被选中
	QString				m_boardStream;		// 白板直播流地址
	QString				m_cameraStream;		// 摄像头直播流地址
	bool				m_bModle;			// 当前窗口模式

	bool				m_b1v1Lesson;
	bool				m_bLiving;
protected:
	virtual void paintEvent(QPaintEvent *event);
	virtual void mousePressEvent(QMouseEvent *e);       //--鼠标按下事件 

signals:
	void clickTag(UITags* tag);

private slots :
	void CloseDialog();			// 最大化

public:
	void SetCourseNameAndID(QString sName, QString id, QString chatID);
	void SetMainWindow(UIWindowSet* parent);
	UIChatRoom* GetRoom();			// 获取当前的聊天窗口
	void SetRoom(UIChatRoom* room);	// 设置关联的聊天窗口
	void setStyle(bool bSel);		// 选中标签样式
	bool IsSelect();				// 当前是否被选中
	QString ChatID();
	QString CourseID();
	QString BoardStream();
	QString CameraStream();
	void setBoardStream(QString strBoard);			
	void setCameraStream(QString strCamera);
	void setModle(bool bModle);		// 设置模式 true为直播状态 false为聊天状态，默认都为聊天状态
	bool IsModle();
	void set1v1Lesson(bool b1v1Lesson);
	bool Is1v1Lesson();
	void setLiving(bool bLiving);	// 当前直播状态
	bool IsLiving();		
};

#endif // UITAGS_H
