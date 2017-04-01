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
 	ui.name_label->setText(m_name);                                       //设置label上的字符串

	int fontSize = fontMetrics().width(m_name);//获取之前设置的字符串的像素大小
	if (fontSize >= ui.name_label->width()) //与label自身相比较
	{
		ui.ellipsis_label->setVisible(true);
	}
	else
	{
		ui.ellipsis_label->setVisible(false);
	}

	if (m_bSelect)
	{
		ui.widget->setStyleSheet("background-color: rgb(0, 63, 86);border-radius:10px;");
		ui.name_label->setStyleSheet("color: rgb(255, 255, 255)");
		ui.ellipsis_label->setStyleSheet("color: rgb(255, 255, 255)");
		ui.close_pushButton->setStyleSheet("border-image: url(:/LoginWindow/images/select_close.png)");
	}
	else
	{
		ui.widget->setStyleSheet("background-color: rgb(255, 255, 255);border-radius:10px;");		 
		ui.name_label->setStyleSheet("color: rgb(0, 0, 0)");
		ui.ellipsis_label->setStyleSheet("color: rgb(0, 0, 0)");
		ui.close_pushButton->setStyleSheet("border-image: url(:/LoginWindow/images/noselect_close.png)");
	}
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
// 	if (bSel)
// 	{
// 		ui.widget->setStyleSheet("background-color: rgb(0, 63, 86);border-radius:10px;");
// // 		ui.left_label->setStyleSheet("border-image: url(:/LoginWindow/images/select_left.png)");
// // 		ui.right_label->setStyleSheet("border-image: url(:/LoginWindow/images/select_right.png)");
// // 		ui.widget->setStyleSheet("border-image: url(:/LoginWindow/images/select_center.png)");
// 		ui.name_label->setStyleSheet("color: rgb(255, 255, 255)");
// 		ui.ellipsis_label->setStyleSheet("color: rgb(255, 255, 255)");
// 		ui.close_pushButton->setStyleSheet("border-image: url(:/LoginWindow/images/select_close.png)");
// 	}
// 	else
// 	{
// 		ui.widget->setStyleSheet("background-color: rgb(255, 255, 255);border-radius:10px;");
// // 		ui.left_label->setStyleSheet("border-image: url(:/LoginWindow/images/noselect_left.png)");
// // 		ui.right_label->setStyleSheet("border-image: url(:/LoginWindow/images/noselect_right.png)");
// // 		ui.widget->setStyleSheet("border-image: url(:/LoginWindow/images/noselect_center.png)");				 
// 		ui.name_label->setStyleSheet("color: rgb(0, 0, 0)");
// 		ui.ellipsis_label->setStyleSheet("color: rgb(0, 0, 0)");
// 		ui.close_pushButton->setStyleSheet("border-image: url(:/LoginWindow/images/noselect_close.png)");
// 	}

	m_bSelect = bSel;
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
}