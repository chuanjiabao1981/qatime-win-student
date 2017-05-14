#ifndef UIAUXILIARYLIST_H
#define UIAUXILIARYLIST_H

#include <QWidget>
#include "ui_UIAuxiliarylist.h"
#include "UIWorkThread.h"

class UIAuxiliaryList : public QWidget
{
	Q_OBJECT

public:
	UIAuxiliaryList(QWidget *parent = 0);
	~UIAuxiliaryList();

private:
	Ui::UIAuxiliaryList ui;
	QString				m_picUrl;
	QString				m_courseName;
	QString				m_teacherName;
	QString				m_chatID;
	QString				m_courseID;
	QString				m_teacherID;
	QString				m_studentName;
	std::string			m_AudioPath;
	QString				m_status;
	int					m_iMsgCount;	// 消息个数

	Worker*				m_pWorker;
	bool				m_b1v1Lesson;	// 是否是1v1辅导班
public:
	QString				PicUrl();
	QString				CourseName();
	QString				CourseID();
	QString				ChatID();
	QString				TeacherID();
	QString				TeacherName();
	std::string			AudioPath();
	QString				StudentName();
	QString				Status();
	int					UnreadMsgCount();
	bool				Is1v1Lesson();

signals:
	void clickAuxiliary(UIAuxiliaryList*);
	void sig_StartLoading();

protected:
	virtual void mousePressEvent(QMouseEvent *e);       //--鼠标按下事件 
	virtual void enterEvent(QEvent *e);
	virtual void leaveEvent(QEvent* e);

private:
	void setNetworkPic(const QString &szUrl);

public:
	QLabel* AddCourse(QString picUrl, QString courseName, QString grade, QString teacherName, QString chatID, QString courseID, QString teacherID,
		QString studentName, std::string AudioPath, QString status, bool b1v1Lesson = false); // 新建辅导班

	void AddMsgCount();			// 改变消息个数
	void ClearMsgNumber();		// 清空消息数
};

#endif // UIAUXILIARYLIST_H
