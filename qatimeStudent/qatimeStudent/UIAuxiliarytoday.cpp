#include "UIAuxiliarytoday.h"

UIAuxiliaryToday::UIAuxiliaryToday(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.enter_pushButton, SIGNAL(clicked()), this, SLOT(clickedEnter()));

	QFont font = ui.time_label->font();
	font.setPixelSize(13);
	ui.time_label->setFont(font);

	font = ui.course_label->font();
	font.setPixelSize(13);
	ui.course_label->setFont(font);

	font = ui.lesson_label->font();
	font.setPixelSize(13);
	ui.lesson_label->setFont(font);

	font = ui.status_label->font();
	font.setPixelSize(13);
	ui.status_label->setFont(font);
}

UIAuxiliaryToday::~UIAuxiliaryToday()
{

}

void UIAuxiliaryToday::AddLesson(QString lessonName, QString courseID, QString courseName, QString time, QString status,bool b1v1)
{
	m_courseID = courseID;
	m_b1v1 = b1v1;

	ui.course_label->setText(courseName);
	ui.lesson_label->setText("课程:" + lessonName);
	ui.time_label->setText(time);
	ui.status_label->setText(status);
	if (status == "直播中")
		ui.status_label->setStyleSheet("color:rgb(255,0,0)");

	int fontSize = ui.course_label->fontMetrics().width(courseName);//获取之前设置的字符串的像素大小
	if (fontSize >= 170) //与label自身相比较
	{
		QString  tmpStr = ui.course_label->fontMetrics().elidedText(courseName, Qt::ElideRight, 170);
		ui.course_label->setText(tmpStr);
		ui.course_label->setToolTip(courseName);
	}

	fontSize = ui.lesson_label->fontMetrics().width("课程:" + lessonName);//获取之前设置的字符串的像素大小
	if (fontSize >= 170) //与label自身相比较
	{
		QString  tmpStr = ui.lesson_label->fontMetrics().elidedText("课程:" + lessonName, Qt::ElideRight, 170);
		ui.lesson_label->setText(tmpStr);
		ui.lesson_label->setToolTip("课程:" + lessonName);
	}
}

QString UIAuxiliaryToday::GetCourseID()
{
	return m_courseID;
}

void UIAuxiliaryToday::clickedEnter()
{
	emit clickAuxiliaryToday(this);
}

bool UIAuxiliaryToday::Is1v1()
{
	return m_b1v1;
}