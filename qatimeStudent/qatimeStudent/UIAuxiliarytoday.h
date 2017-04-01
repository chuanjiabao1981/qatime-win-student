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
private:
	Ui::UIAuxiliaryToday ui;

protected:
	virtual void enterEvent(QEvent *e);
	virtual void leaveEvent(QEvent* e);

signals:
	void clickAuxiliaryToday(UIAuxiliaryToday*);

private slots :
	void clickedEnter();
public:
	void AddLesson(QString lessonName, QString courseID, QString courseName, QString time, QString status);
};

#endif // UIAUXILIARYTODAY_H
