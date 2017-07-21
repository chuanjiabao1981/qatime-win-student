#include "UITags.h"
#include <QMouseEvent>
#include <windows.h>

UITags::UITags(QWidget *parent)
	: QWidget(parent)
	, m_parent(NULL)
	, m_room(NULL)
	, m_bSelect(false)
	, m_boardStream("")
	, m_cameraStream("")
	, m_bModle(false)
	, m_b1v1Lesson(false)
	, m_bLiving(false)
{
	ui.setupUi(this);
	connect(ui.close_pushButton, SIGNAL(clicked()), this, SLOT(CloseDialog()));
	ui.name_label->setCursor(Qt::PointingHandCursor);

	QFont font = ui.name_label->font();
	font.setPixelSize(13);
	ui.name_label->setFont(font);

	font = ui.ellipsis_label->font();
	font.setPixelSize(13);
	ui.ellipsis_label->setFont(font);
}

UITags::~UITags()
{

}


void UITags::SetCourseNameAndID(QString sName, QString id, QString chatID)
{
	m_name = sName;
	m_id = id;
	m_chatID = chatID;
	ui.name_label->setToolTip(m_name);
}

QString UITags::ChatID()
{
	return m_chatID;
}

QString UITags::CourseID()
{
	return m_id;
}

void UITags::paintEvent(QPaintEvent *event)
{
 	ui.name_label->setText(m_name);            //����label�ϵ��ַ���

	int fontSize = fontMetrics().width(m_name);//��ȡ֮ǰ���õ��ַ��������ش�С
	if (fontSize >= ui.name_label->width()) //��label������Ƚ�
		ui.ellipsis_label->setVisible(true);
	else
		ui.ellipsis_label->setVisible(false);
}

void UITags::CloseDialog()
{
	m_parent->DeleleTag(this);
}

void UITags::SetMainWindow(UIWindowSet* parent)
{
	m_parent = parent;
}

void UITags::SetRoom(UIChatRoom* room)
{
	m_room = room;
}

void UITags::setStyle(bool bSel)
{
	m_bSelect = bSel;
	if (m_bSelect)
	{
		ui.widget->setStyleSheet("background-color: rgb(0, 63, 86);border-radius:10px;");
		ui.name_label->setStyleSheet("color: rgb(255, 255, 255)");
		ui.ellipsis_label->setStyleSheet("color: rgb(255, 255, 255)");
		ui.close_pushButton->setStyleSheet("border-image: url(:/LoginWindow/images/select_close.png)");

		if (m_bModle)
			ui.pic_label->setStyleSheet("border-image: url(./images/tags_video_hover.png);");
		else
			ui.pic_label->setStyleSheet("border-image: url(./images/tags_chat_hover.png);");
	}
	else
	{
		ui.widget->setStyleSheet("background-color: rgb(255, 255, 255);border-radius:10px;");		 
		ui.name_label->setStyleSheet("color: rgb(0, 0, 0)");
		ui.ellipsis_label->setStyleSheet("color: rgb(0, 0, 0)");
		ui.close_pushButton->setStyleSheet("border-image: url(:/LoginWindow/images/noselect_close.png)");

		if (m_bModle)
			ui.pic_label->setStyleSheet("border-image: url(./images/tags_video_nor.png);");
		else
			ui.pic_label->setStyleSheet("border-image: url(./images/tags_chat_nor.png);");
	}
}

bool UITags::IsSelect()
{
	return m_bSelect;
}

void UITags::mousePressEvent(QMouseEvent *e)
{
	if (Qt::LeftButton == e->button())
	{
		if (IsSelect())
			return;

		emit clickTag(this);
	}
}

UIChatRoom* UITags::GetRoom()
{
	return m_room;
}

void UITags::setBoardStream(QString strBoard)
{
	m_boardStream = strBoard;
}

void UITags::setCameraStream(QString strCamera)
{
	m_cameraStream = strCamera;
}

QString UITags::BoardStream()
{
	return m_boardStream;
}

QString UITags::CameraStream()
{
	return m_cameraStream;
}

bool UITags::IsModle()
{
	return m_bModle;
}

void UITags::setModle(bool bModle)
{
	m_bModle = bModle;

	if (m_bSelect)
	{
		if (m_bModle)
			ui.pic_label->setStyleSheet("border-image: url(./images/tags_video_hover.png);");
		else
			ui.pic_label->setStyleSheet("border-image: url(./images/tags_chat_hover.png);");
	}
	else
	{
		if (m_bModle)
			ui.pic_label->setStyleSheet("border-image: url(./images/tags_video_nor.png);");
		else
			ui.pic_label->setStyleSheet("border-image: url(./images/tags_chat_nor.png);");
	}
}

void UITags::set1v1Lesson(bool b1v1Lesson)
{
	m_b1v1Lesson = b1v1Lesson;
}

bool UITags::Is1v1Lesson()
{
	return m_b1v1Lesson;
}

void UITags::setLiving(bool bLiving)
{
	m_bLiving = bLiving;
}

bool UITags::IsLiving()
{
	return m_bLiving;
}