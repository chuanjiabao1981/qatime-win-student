/*
*	Author       hzzhuling15
*	Date         2015/09/11
*	Copyright    Hangzhou, Netease Inc.
*	Brief        
*   Notice       
*/
#include "UIBtnPic.h"
#include <QtGui/QPainter>
#include <QtGui/QEvent.h>

CBtnPic::CBtnPic(QWidget *parent /*= 0*/)
: QPushButton(parent)
, m_qsImage("")
{
}

CBtnPic::CBtnPic(QString url, QPixmap pixmap, QWidget *parent/* = 0*/)
: QPushButton(parent)
, m_qsImage("")
, m_nImageWidth(0)
, m_nImageHeight(0)
, m_pImgProcess(NULL)
, m_MsgID("")
, m_pFailBtn(NULL)
{
//	setCursor(Qt::PointingHandCursor);
	this->setIcon(QIcon(pixmap.scaled(QSize(30, 30))));
	this->setIconSize(QSize(30, 30));
	this->setFlat(true);

	connect(this, SIGNAL(clicked(bool)), this, SLOT(onClicked(bool)));

	m_pFailBtn = new QPushButton(this);
	m_pFailBtn->resize(30, 30);
	m_pFailBtn->setStyleSheet("QPushButton{background-color:rgba(255, 255, 255, 0);border-image:url(./images/t_photo.png);}");
}

CBtnPic::CBtnPic(QPixmap pixmap, QWidget *parent/* = 0*/)
	: QPushButton(parent)
{
	this->setIcon(QIcon(pixmap.scaled(QSize(24, 24))));
	this->setIconSize(QSize(24, 24));
	this->setFlat(true);
}

CBtnPic::~CBtnPic()
{
}

void CBtnPic::onClicked(bool bChecked)
{
	emit sig_idclicked(m_qsImage, m_pixmap, bChecked);
}

void CBtnPic::SetImg(QPixmap map)
{
	this->setIcon(QIcon(map.scaled(QSize(30, 30))));
	this->setIconSize(QSize(30, 30));
	this->setFlat(true);

	connect(this, SIGNAL(clicked(bool)), this, SLOT(onClicked(bool)));

	m_pFailBtn = new QPushButton(this);
	m_pFailBtn->resize(30, 30);
	m_pFailBtn->setStyleSheet("QPushButton{background-color:rgba(255, 255, 255, 0);border-image:url(./images/t_photo.png);}");
}

QPushButton* CBtnPic::GetImgProcess()
{
	if (m_pImgProcess)
		return m_pImgProcess;

	return NULL;
}

void CBtnPic::SetMsgID(QString msgid)
{
	m_MsgID = msgid;
}

QString CBtnPic::GetMsgID()
{
	return m_MsgID;
}

QString CBtnPic::GetUrl()
{
	return m_qsImage;
}

void CBtnPic::AddFailBtn()
{
	m_pFailBtn->setStyleSheet("QPushButton{background-color:rgba(255, 255, 255, 0);border-image:url(./images/retry.png);}");
}

void CBtnPic::DeleteFailBtn()
{
	m_pFailBtn->setStyleSheet("QPushButton{background-color:rgba(255, 255, 255, 0);border-image:url(./images/alpha.png);}}");
}

void CBtnPic::onClickedFail(bool bChecked)
{
	QString strSheet = m_pFailBtn->styleSheet();
	if (strSheet.indexOf("./images/alpha.png") > -1)
		emit sig_idclicked(m_qsImage, m_pixmap, bChecked); //正常点击
	else
		emit sig_faildclicked(this);					   //失败点击
}

// 中心裁切
QPixmap CBtnPic::CenterRect(QPixmap pix)
{
	QPixmap AfterPix;
	int h = pix.height();
	int w = pix.width();
	if (w>h)
	{
		int x = (w - h) / 2;
		AfterPix = pix.copy(x, 0, h, h);
	}
	else
	{
		int y = (h - w) / 2;
		AfterPix = pix.copy(0, y, w, w);
	}

	return AfterPix;
}