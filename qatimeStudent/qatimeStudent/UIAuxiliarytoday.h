#ifndef UIAUXILIARYTODAY_H
#define UIAUXILIARYTODAY_H

#include <QWidget>
#include "ui_UIAuxiliarytoday.h"

#pragma execution_character_set("utf-8")
class UIAuxiliaryToday : public QWidget
{
	Q_OBJECT

public:
	UIAuxiliaryToday(QWidget *parent = 0);
	~UIAuxiliaryToday();

	QString			m_courseID;		 // ������ID
	QString			GetCourseID();	 // ��ȡ������ID
	QString			m_status;		 // ������״̬
	QString			GetStatus();	 // ��ȡ������״̬
	bool			m_b1v1;
private:
	Ui::UIAuxiliaryToday ui;

signals:
	void clickAuxiliaryToday(UIAuxiliaryToday*);

private slots :
	void clickedEnter();
public:
	void AddLesson(QString lessonName, QString courseID, QString courseName, QString time, QString status, bool b1v1);
	bool Is1v1();
};

#endif // UIAUXILIARYTODAY_H
