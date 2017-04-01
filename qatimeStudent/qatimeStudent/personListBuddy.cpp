#include "personlistbuddy.h"
#include <QPainter>
#include <QSize>

#pragma execution_character_set("utf-8")
personListBuddy::personListBuddy(QWidget *parent) :
    QWidget(parent)
	, button(NULL)
	, head(NULL)
	, secLinEdit(NULL)
	, secFindButton(NULL)
	, m_bZoom(false)
{
 //   initUi();
	this->resize(QSize(320, 30));
}

personListBuddy::~personListBuddy()
{
}

void personListBuddy::initFirst()
{
	peopleNum = new QLabel(this);	
	peopleNum->move(95, 5);
	peopleNum->setText("��ǰ�ۿ����� 0");
}
void personListBuddy::initFindPeople(int iWidth)
{
	secLinEdit = new QLineEdit(this);
	connect(secLinEdit, SIGNAL(textChanged(const QString)), this, SLOT(findName(const QString)));
	secFindButton = new QPushButton(this);
	connect(secFindButton, SIGNAL(clicked()), this, SLOT(findName()));

	secLinEdit->move(1, 1);
	secLinEdit->setFixedWidth(iWidth);
	secLinEdit->setFixedHeight(28);
	secLinEdit->setPlaceholderText("������Ҫ�������");
	secLinEdit->setAlignment(Qt::AlignLeft);//�������	
	secLinEdit->setStyleSheet("border-image: url(:/LoginWindow/images/AuxiliaryPanelBack.png);");
	secFindButton->setStyleSheet("border-image: url(:/LoginWindow/images/find.png)");
	secFindButton->resize(33, 28);
	secFindButton->move(iWidth, 1);
	secFindButton->setCursor(Qt::PointingHandCursor);
}
void personListBuddy::initNotFind(int iWidth)
{
	notFind = new QLabel(this);
	notFind->setBaseSize(QSize(200, 25));
	notFind->move(95, 5);
	notFind->setText("û���ҵ��ú���");
	notFind->setAlignment(Qt::AlignCenter);//���ж���;//���ж���	
	notFind->setStyleSheet("color:red;");
	notFind->hide();
	firstButton = new QCheckBox(this);
	connect(firstButton, SIGNAL(clicked(bool)), this, SLOT(stopAllTalk(bool)));
	firstButton->move(iWidth, 7);
	firstButton->setText("ȫ��");
}

//��ʼ��Ui
QLabel* personListBuddy::initUi(const QString &szUrl, QString strName, QString ID, int iWidth)
{
    //��ʼ��
	head = new QLabel(this);
    name=new QLabel(this);
	m_ID = ID;
	button = new QCheckBox(this);
	connect(button, SIGNAL(stateChanged(int)), this, SLOT(radioChange(int)));
	button->installEventFilter(this);
    //����ͷ���С
    head->setFixedSize(32,32);
    //���ø���ǩ������Ϊ��ɫ
    QPalette color;
    color.setColor(QPalette::Text,Qt::gray);
    //����
    head->move(0,0);
    name->move(7+16+10,8);
	button->move(iWidth, 7);
	button->setText("����");
	setNetworkPic(szUrl);
	name->setText(strName);
	return head;
}

bool personListBuddy::eventFilter(QObject *target, QEvent *event)
{
	if (target == button) {
		if (event->type() == QEvent::MouseButtonRelease)
		{
			bool b = button->isChecked();
			emit emitRadioChange(!b, m_ID, name->text());
		}
	}
	return QWidget::eventFilter(target, event);
}

void personListBuddy::setMute(bool bMute)
{
	if (button->isChecked() == bMute)
		return;

	button->setChecked(bMute);
	emit emitRadioChange(bMute, m_ID, name->text());
}

void personListBuddy::setOlineNum(int olineNum,int allNum)
{
	peopleNum->setText("��ǰ�ۿ����� "+QString::number(olineNum));
}

void personListBuddy::setCheckBox(bool bCheck)
{
	if (bCheck)
		button->setChecked(true);
	else
		button->setCheckState(Qt::CheckState::Unchecked);
}

// �¼�����������Ҫ��Ϊ����ͼƬ�ܹ�ȫ�������head����
// bool personListBuddy::eventFilter(QObject *obj, QEvent *event)
// {
//     if(obj == head)
//     {
//         if(event->type() == QEvent::Paint)
//         {
//             QPainter painter(head);			
// 			painter.drawPixmap(head->rect(), headPath);
//         }
//     }
//     return QWidget::eventFilter(obj, event);
// }

void personListBuddy::radioChange(int b)
{
//	emit emitRadioChange(b, m_ID, name->text());
}

void personListBuddy::stopAllTalk(bool b)
{
	emit signalStopAllTalk(b,"0");
}
//TODO ������Ӵ˹���
void personListBuddy::onlyOnline(bool b)
{

}

void personListBuddy::findName()
{
	QString name = secLinEdit->text();
	emit signalFindName(name);
}
//������ѧ���༭��ΪNULLʱ����ʾ����ѧ������
void personListBuddy::findName(const QString name)
{
	if (name.isEmpty())
	{
		emit signalFindNameNULL(name);
	}	
}

void personListBuddy::setNetworkPic(const QString &szUrl)
{
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
// 	QSize pixSize(24,24);
// 	// ���سɹ�����ʾ
// 	if (pixmap.loadFromData(jpegData))
// 	{
// 		QPixmap scaledPixmap = pixmap.scaled(pixSize, Qt::KeepAspectRatio);
// 		head->setPixmap(scaledPixmap);
// 		head->setAccessibleDescription(szUrl);
// 	}
// 	else // ������ʾ����ͼƬ
// 	{
		QPixmap pixmap;
		QString sUrl = "./images/teacherPhoto.png";
		pixmap = QPixmap(sUrl);
		QPixmap scaledPixmap = pixmap.scaled(QSize(24, 24), Qt::IgnoreAspectRatio);
		head->setPixmap(scaledPixmap);
//	}
}

void  personListBuddy::SetZoom(bool bZoom)
{
	m_bZoom = bZoom;
}

bool personListBuddy::IsZoom()
{
	return m_bZoom;
}