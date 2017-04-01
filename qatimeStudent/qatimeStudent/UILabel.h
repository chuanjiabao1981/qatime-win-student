/*
*	Author       hzzhuling15
*	Date         2015/09/11
*	Copyright    Hangzhou, Netease Inc.
*	Brief        
*   Notice       
*/
#ifndef _LS_UI_LABEL_H_
#define _LS_UI_LABEL_H_
#include <QtWidgets/QLabel>
#pragma execution_character_set("utf-8")

class CLabel : public QLabel
{
	Q_OBJECT

public:
	CLabel(QWidget *parent = 0);
	~CLabel();

	QPixmap			m_pixmap;
	
protected:
	virtual void paintEvent(QPaintEvent *event);
public:
	void setImage(QPixmap pixmap);
};

#endif // _LS_MEDIACAPTURE_UI_BUTTON_H_