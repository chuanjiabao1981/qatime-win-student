#ifndef UIPERSONWND_H
#define UIPERSONWND_H

#include <QWidget>
#include "ui_UIPersonwnd.h"
#include "personListBuddy.h"
#include <QPushButton>
#include "UIWindowSet.h"

class UIWindowSet;
#pragma execution_character_set("utf-8")

class UIPersonWnd : public QWidget
{
	Q_OBJECT

public:
	UIPersonWnd(QWidget *parent = 0);
	~UIPersonWnd();

private:
	Ui::UIPersonWnd ui;
	QSpacerItem*				m_spacer;

	std::vector<QLabel*>		m_vecLayout;
	QPushButton*				m_btn;
	UIWindowSet*				 m_parent;
protected:
	virtual void paintEvent(QPaintEvent *event);
	virtual void focusOutEvent(QFocusEvent *);

public:
	void AddPerson(std::vector<personListBuddy*> vecBuddy, QString teacherID);
	void DeletePerson();
	void style(QScrollArea *style);
	void setParentBtn(UIWindowSet* parent);
};

#endif // UIPERSONWND_H
