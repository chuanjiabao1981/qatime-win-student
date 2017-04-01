#include "UIAuxiliarylist.h"
#include <QUrl>
#include <QNetworkAccessManager>
#include <QObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QMouseEvent>

UIAuxiliaryList::UIAuxiliaryList(QWidget *parent)
	: QWidget(parent)
	, m_iMsgCount(0)
	, m_pWorker(NULL)
{
	ui.setupUi(this);
	setCursor(Qt::PointingHandCursor);
	setMouseTracking(true);
	ui.number_label->setText("");
	ui.pic_label->setVisible(false);

	m_pWorker = new Worker();
	connect(this, SIGNAL(sig_StartLoading()), m_pWorker, SLOT(slot_StartLoading()));

	QFont font = ui.name_label->font();
	font.setPixelSize(13);
	ui.name_label->setFont(font);

	font = ui.grade_label->font();
	font.setPixelSize(12);
	ui.grade_label->setFont(font);

	font = ui.number_label->font();
	font.setPixelSize(12);
	ui.number_label->setFont(font);
}

UIAuxiliaryList::~UIAuxiliaryList()
{

}


QLabel* UIAuxiliaryList::AddCourse(QString picUrl, QString courseName, QString grade, QString teacherName, QString chatID, QString courseID, 
	QString teacherID, QString token, QString studentName, std::string AudioPath, QString status)
{
	m_picUrl = picUrl;
	m_courseName = courseName;
	m_teacherName = teacherName;
	m_chatID = chatID;
	m_courseID = courseID;
	m_teacherID = teacherID;
	m_token = token;
	m_studentName = studentName;
	m_AudioPath = AudioPath;
	m_status = status;

	setNetworkPic(picUrl);

	ui.name_label->setText(courseName);
	ui.grade_label->setText(grade + "|" + teacherName);


	int fontSize = ui.name_label->fontMetrics().width(courseName);//��ȡ֮ǰ���õ��ַ��������ش�С
	if (fontSize >= ui.name_label->width()) //��label������Ƚ�
	{
		QString  tmpStr = ui.name_label->fontMetrics().elidedText(courseName, Qt::ElideRight, ui.name_label->width());
		ui.name_label->setText(tmpStr);
		ui.name_label->setToolTip(courseName);
	}

	fontSize = ui.grade_label->fontMetrics().width(grade + "|" + teacherName);//��ȡ֮ǰ���õ��ַ��������ش�С
	if (fontSize >= ui.grade_label->width()) //��label������Ƚ�
	{
		QString  tmpStr = ui.grade_label->fontMetrics().elidedText(grade + "|" + teacherName, Qt::ElideRight, ui.name_label->width());
		ui.grade_label->setText(tmpStr);
		ui.grade_label->setToolTip(grade + "|" + teacherName);
	}

// 	if (status == "teaching")
// 		ui.teaching_label->setVisible(true);
// 	else
	ui.teaching_label->setVisible(false);

	return ui.label;
}

void UIAuxiliaryList::setNetworkPic(const QString &szUrl)
{
// 	m_pWorker->SetUrl(ui.pic_label, szUrl);
// 	emit sig_StartLoading();
// 	QUrl url(szUrl);
// 	QNetworkAccessManager manager;
// 	QEventLoop loop;
// 
// 	QNetworkReply *reply = manager.get(QNetworkRequest(url));
// 	//���������������ɺ��˳����¼�ѭ�� 
// 	QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
// 	//�������¼�ѭ�� 
// 	loop.exec();
// 	QByteArray jpegData = reply->readAll();
// 	QPixmap pixmap;
// 	QSize pixSize(ui.label->width(), ui.label->height());
// 	// ���سɹ�����ʾ
// 	if (pixmap.loadFromData(jpegData))
// 	{
// 		QPixmap scaledPixmap = pixmap.scaled(pixSize, Qt::IgnoreAspectRatio);
// 		ui.label->setPixmap(scaledPixmap);
// 	}
// 	else // ������ʾ����ͼƬ
// 	{
// 		QString sUrl = "./images/teacherPhoto.png";
// 		pixmap = QPixmap(sUrl);
// 		QPixmap scaledPixmap = pixmap.scaled(pixSize, Qt::IgnoreAspectRatio);
// 		ui.label->setPixmap(scaledPixmap);
// 	}
}

void UIAuxiliaryList::mousePressEvent(QMouseEvent *e)
{
	if (Qt::LeftButton == e->button())
	{
		emit clickAuxiliary(this);
		return;
	}
}

void UIAuxiliaryList::enterEvent(QEvent *e)
{
	setStyleSheet("background-color: rgb(251, 239, 193)");
}

void UIAuxiliaryList::leaveEvent(QEvent* e)
{
	setStyleSheet("background-color: rgb(255, 255, 255)");
}

QString	UIAuxiliaryList::PicUrl()
{
	return m_picUrl;
}
QString	UIAuxiliaryList::CourseName()
{
	return m_courseName;
}
QString	UIAuxiliaryList::CourseID()
{
	return m_courseID;
}
QString	UIAuxiliaryList::ChatID()
{
	return m_chatID;
}
QString	UIAuxiliaryList::TeacherID()
{
	return m_teacherID;
}
QString	UIAuxiliaryList::TeacherName()
{
	return m_teacherName;
}
QString	UIAuxiliaryList::Token()
{
	return m_token;
}
std::string	UIAuxiliaryList::AudioPath()
{
	return m_AudioPath;
}
QString UIAuxiliaryList::StudentName()
{
	return m_studentName;
}

QString UIAuxiliaryList::Status()
{
	return m_status;
}

void UIAuxiliaryList::AddMsgCount()
{
	m_iMsgCount++;
	QString strNum = QString::number(m_iMsgCount);
	ui.number_label->setText(strNum);
	ui.pic_label->setVisible(true);
}

void UIAuxiliaryList::ClearMsgNumber()
{
	m_iMsgCount = 0;
	ui.number_label->setText("");
	ui.pic_label->setVisible(false);
}

int UIAuxiliaryList::UnreadMsgCount()
{
	return m_iMsgCount;
}