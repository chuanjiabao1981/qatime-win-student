/*
*	Author       hzzhuling15
*	Date         2015/09/11
*	Copyright    Hangzhou, Netease Inc.
*	Brief        
*   Notice       
*/
#include "UILabel.h"
#include <QtGui/QPainter>
#include <QtGui/QEvent.h>

CLabel::CLabel(QWidget *parent /*= 0*/)
: QLabel(parent)
{
}

CLabel::~CLabel()
{
}

void CLabel::setImage(QPixmap pixmap)
{
	this->setPixmap(m_pixmap);
}

void CLabel::paintEvent(QPaintEvent *event)
{
	
//	this->setStyleSheet("border-image: url(./images/t_photo.png);");
}

